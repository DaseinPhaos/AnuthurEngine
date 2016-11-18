//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************
#include "NaiveQuadtreeDisplacement.h"

void WTF::NaiveBpqdmGenerator::initialize(ID3D12Device* pDevice)
{
	using namespace D3D12Helper;

	// initialize root signature
	{
		RootSignatureDescriptor rsd;
		// rsd.PushRPCBV(0u); // for texture attributes
		rsd.PushRPConstant(0u, 1u);
		RootDescriptorTable rdt;
		rdt.Push(RootDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1u));
		rsd.PushRPDescriptorTable(rdt.Get());

		ComPtr<ID3DBlob> rootSigBlob;
		ThrowIfFailed(rsd.SerializeRootSignature(rootSigBlob.GetAddressOf(),
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT));
		ThrowIfFailed(pDevice->CreateRootSignature(0u, rootSigBlob->GetBufferPointer(),
			rootSigBlob->GetBufferSize(), IID_PPV_ARGS(_rootSignature.ReleaseAndGetAddressOf())));
	}

	// initialize shader bytecode
	{
		D3D_SHADER_MACRO macro[] = {
			"PS0", "1", nullptr, nullptr
		};
		ThrowIfFailed(_vertexShader.CompileFromFile(shaderPath, "VSMain", "vs_5_0"));


		ThrowIfFailed(_pixelShader0.CompileFromFile(shaderPath, "PSMain0", "ps_5_0", macro));
		macro[0].Name = "PS1";
		ThrowIfFailed(_pixelShader1.CompileFromFile(shaderPath, "PSMain1", "ps_5_0", macro));
	}

	// initialize PSOs
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psd = {};
		psd.InputLayout = getInputLayout();
		psd.pRootSignature = _rootSignature.Get();
		psd.VS = _vertexShader.Get();
		psd.PS = _pixelShader0.Get();
		psd.BlendState = BlendDescriptor::Default();
		psd.SampleMask = UINT_MAX;
		psd.RasterizerState = RasterizerDescriptor();
		psd.DepthStencilState = DepthStencilStateDescriptor::Default();
		psd.DepthStencilState.DepthEnable = FALSE;
		psd.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psd.NumRenderTargets = static_cast<UINT>(3);
		psd.RTVFormats[0] = DXGI_FORMAT_R32_FLOAT;
		psd.DSVFormat =DXGI_FORMAT_D24_UNORM_S8_UINT;
		psd.SampleDesc.Count = 1;
		psd.SampleDesc.Quality = 0;
		ThrowIfFailed(pDevice->CreateGraphicsPipelineState(&psd,
			IID_PPV_ARGS(_normalState0.ReleaseAndGetAddressOf())));

		psd.PS = _pixelShader1.Get();
		ThrowIfFailed(pDevice->CreateGraphicsPipelineState(&psd,
			IID_PPV_ARGS(_normalState1.ReleaseAndGetAddressOf())));
	}

	// initialize descriptor heaps
	{
		ThrowIfFailed(pDevice->CreateDescriptorHeap(
			&D3D12Helper::DescriptorHeapDescriptor(
				D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 16u,
				D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE),
			IID_PPV_ARGS(_srvHeap.GetAddressOf())));

		ThrowIfFailed(pDevice->CreateDescriptorHeap(
			&D3D12Helper::DescriptorHeapDescriptor(
				D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 16u),
			IID_PPV_ARGS(_rtvHeap.GetAddressOf())));
	}
}

D3D12_INPUT_LAYOUT_DESC WTF::NaiveBpqdmGenerator::getInputLayout()
{
	static D3D12_INPUT_ELEMENT_DESC ieds = D3D12Helper::InputElementDescriptor(
		"POSITION_ND", DXGI_FORMAT_R32G32B32_FLOAT, 0u);
	//D3D12_INPUT_LAYOUT_DESC ild;
	//ild.NumElements = 1u;
	//ild.pInputElementDescs = &ieds;
	return D3D12_INPUT_LAYOUT_DESC{ &ieds, 1u };
}

const Luxko::Anuthur::MeshGeometry& WTF::NaiveBpqdmGenerator::getInputBuffer(
	ID3D12Device* pDevice /*= nullptr*/,
	ID3D12GraphicsCommandList* pCmdlist /*= nullptr*/)
{
	static MeshGeometry mesh;
	static bool initialized = false;
	if (!initialized) {
		assert(pDevice);
		assert(pCmdlist);
		std::vector<VSI> vertice;
		std::vector<UINT> indice;
		vertice.reserve(4);
		vertice.push_back(VSI{ Vector3f(-1.f, -1.f, 0.5f) });
		vertice.push_back(VSI{ Vector3f(-1.f, 1.f, 0.5f) });
		vertice.push_back(VSI{ Vector3f(1.f, 1.f, 0.5f) });
		vertice.push_back(VSI{ Vector3f(1.f, -1.f, 0.5f) });

		indice.reserve(6);
		indice.push_back(0u);
		indice.push_back(1u);
		indice.push_back(2u);
		indice.push_back(0u);
		indice.push_back(2u);
		indice.push_back(3u);


		mesh.InitializeCPUResource(DXGI_FORMAT_R32_UINT,
			static_cast<UINT>(indice.size()), indice.data(),
			static_cast<UINT>(sizeof(VSI)), static_cast<UINT>(vertice.size()),
			vertice.data());
		mesh.RecordUpdateFromCPUtoGPU(pDevice, pCmdlist);
		initialized = true;
	}
	return mesh;
}

static UINT is2exp(UINT src) {
	if (src > 0) {
		for (int i = 0; i < 32; ++i) {
			if (src == (1 << i)) {
				return i + 1;
			}
		}
	}
	return 0;
}

std::pair<UINT, Luxko::Anuthur::ComPtr<ID3D12Resource>> 
WTF::NaiveBpqdmGenerator::generateQDM(
	ID3D12Resource* srcNormalTexture,
	ID3D12Device* device, ID3D12GraphicsCommandList* cmdlist)
{
	auto srcTextureDesc = srcNormalTexture->GetDesc();
	assert(srcTextureDesc.Width == srcTextureDesc.Height);
	auto texAttr = static_cast<UINT>(srcTextureDesc.Width);

	auto mipLevelCount = is2exp(texAttr);
	assert(mipLevelCount != 0 && mipLevelCount <= 16);

	D3D12_RECT rect = {
		0, 0, static_cast<LONG>(srcTextureDesc.Width),
		static_cast<LONG>(srcTextureDesc.Height)
	};
	cmdlist->RSSetScissorRects(1u, &rect);

	D3D12_VIEWPORT viewport = {
		0.f, 0.f, static_cast<FLOAT>(srcTextureDesc.Width),
		static_cast<FLOAT>(srcTextureDesc.Height),
		0.f, 1.f };
	cmdlist->RSSetViewports(1u, &viewport);

	auto& mesh = getInputBuffer();
	cmdlist->IASetIndexBuffer(&mesh.IndexBufferView());
	cmdlist->IASetVertexBuffers(0u, 1u, &mesh.VertexBufferView());
	ID3D12DescriptorHeap* dhs[] = {
		_srvHeap.Get()/*, _rtvHeap.Get()*/
	};
	cmdlist->SetDescriptorHeaps(1u, dhs);
	recordStateSettings0(cmdlist);

	recordRp0TexAttr(cmdlist, texAttr);

	auto currSrvCpu = D3D12Helper::DescriptorHandleCPU(_srvHeap->GetCPUDescriptorHandleForHeapStart());
	device->CreateShaderResourceView(srcNormalTexture, nullptr, currSrvCpu);
	auto currSrvGpu = D3D12Helper::DescriptorHandleGPU(_srvHeap->GetGPUDescriptorHandleForHeapStart());
	
	
	recordRp1BaseTex(cmdlist, currSrvGpu);
	auto const srvDSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);



	ComPtr<ID3D12Resource> result;
	ThrowIfFailed(device->CreateCommittedResource(
		&D3D12Helper::ResourceHeapProperties(),
		D3D12_HEAP_FLAG_NONE, &D3D12Helper::ResourceDescriptor::Tex2D(
			srcTextureDesc.Width, texAttr, DXGI_FORMAT_R32_FLOAT,
			1u, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET,
			0u, static_cast<UINT16>(mipLevelCount)),
		D3D12_RESOURCE_STATE_COMMON,
		nullptr, IID_PPV_ARGS(result.GetAddressOf())));
	D3D12Helper::ResourceBarrier beginBarriers[] = {
		D3D12Helper::ResourceBarrier::TransitionBarrier(
			result.Get(), D3D12_RESOURCE_STATE_COMMON,
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_BARRIER_FLAG_NONE, 0u)
	};
	cmdlist->ResourceBarrier(1u, beginBarriers);
	auto currRtvCpu = D3D12Helper::DescriptorHandleCPU(_rtvHeap->GetCPUDescriptorHandleForHeapStart());
	auto const rtvDSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	device->CreateRenderTargetView(result.Get(),
		&D3D12Helper::RTVDescriptor::Texture2DDesc(DXGI_FORMAT_UNKNOWN, 0u, 0u),
		currRtvCpu);


	recordClearAndSetRtvDsv(cmdlist, currRtvCpu, 1u, &rect);
	cmdlist->DrawIndexedInstanced(mesh.GetTotoalIndexCount(), 1u, 0u, 0u, 0u);

	recordStateSettings1(cmdlist);
	auto currentLevel = 1u;
	while (currentLevel < mipLevelCount) {
		
		texAttr /= 2;
		recordResourcesTransitionFrom(cmdlist, result.Get(), result.Get(),
			currentLevel - 1, currentLevel);
		currSrvCpu.Offset(srvDSize);
		currSrvGpu.Offset(srvDSize);
		device->CreateShaderResourceView(result.Get(),
			&D3D12Helper::SRVDescriptor::Texture2DDesc(
				DXGI_FORMAT_UNKNOWN, currentLevel - 1, 0u, 1u), currSrvCpu);
		currRtvCpu.Offset(rtvDSize);
		device->CreateRenderTargetView(result.Get(),
			&D3D12Helper::RTVDescriptor::Texture2DDesc(
				DXGI_FORMAT_UNKNOWN, currentLevel, 0u),
			currRtvCpu);


		rect.bottom = static_cast<LONG>(texAttr);
		rect.right = static_cast<LONG>(texAttr);
		viewport.Height = static_cast<FLOAT>(texAttr);
		viewport.Width = static_cast<FLOAT>(texAttr);
		cmdlist->RSSetScissorRects(1u, &rect);
		cmdlist->RSSetViewports(1u, &viewport);
		recordRp0TexAttr(cmdlist, texAttr);
		recordRp1BaseTex(cmdlist, currSrvGpu);
		recordClearAndSetRtvDsv(cmdlist, currRtvCpu, 1u, &rect);
		cmdlist->DrawIndexedInstanced(mesh.GetTotoalIndexCount(), 1u, 0u, 0u, 0u);
		currentLevel += 1;

	}
	//cmdlist->Close();
	D3D12Helper::ResourceBarrier lastBarriers[] = {
		D3D12Helper::ResourceBarrier::TransitionBarrier(result.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_BARRIER_FLAG_NONE,
		mipLevelCount - 1) };
	cmdlist->ResourceBarrier(1u, lastBarriers);
	return std::make_pair(mipLevelCount, result);
}

void WTF::NBPQDM::initialize(ID3D12Device* pDevice, DXGI_FORMAT dsvFormat /*= DXGI_FORMAT_D24_UNORM_S8_UINT*/)
{
	using namespace D3D12Helper;

	// initialize root signature
	{
		RootSignatureDescriptor rsd;
		rsd.PushRPCBV(0u); // for camera attributes
		rsd.PushRPCBV(1u); // for material attributes
		RootDescriptorTable rdt;
		rdt.Push(RootDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
			static_cast<UINT>(3))); // for diffuse and normal map
		rsd.PushRPDescriptorTable(rdt.Get());
		rsd.PushStaticSamplerDescriptor(0u);
		rsd.PushStaticSamplerDescriptor(1u,
			D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR);
		ComPtr<ID3DBlob> rootSigBlob;
		ThrowIfFailed(rsd.SerializeRootSignature(rootSigBlob.GetAddressOf(),
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT));
		ThrowIfFailed(pDevice->CreateRootSignature(0u, rootSigBlob->GetBufferPointer(),
			rootSigBlob->GetBufferSize(), IID_PPV_ARGS(_rootSignature.ReleaseAndGetAddressOf())));
	}

	// initialize shader bytecode
	{
		ThrowIfFailed(_vertexShader.CompileFromFile(shaderPath, "VSMain", "vs_5_0"));
		ThrowIfFailed(_pixelShader.CompileFromFile(shaderPath, "PSMain", "ps_5_0"));
	}

	// initialize PSOs
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psd = {};
		psd.InputLayout = getInputLayout();
		psd.pRootSignature = _rootSignature.Get();
		psd.VS = _vertexShader.Get();
		psd.PS = _pixelShader.Get();
		psd.BlendState = BlendDescriptor::Default();
		psd.SampleMask = UINT_MAX;
		psd.RasterizerState = RasterizerDescriptor();
		psd.DepthStencilState = DepthStencilStateDescriptor::Default();
		psd.DepthStencilState.StencilEnable = TRUE;
		psd.DepthStencilState.FrontFace.StencilPassOp = D3D12_STENCIL_OP_REPLACE;

		psd.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psd.NumRenderTargets = static_cast<UINT>(3);
		psd.RTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		psd.RTVFormats[1] = DXGI_FORMAT_R10G10B10A2_UNORM;
		psd.RTVFormats[2] = DXGI_FORMAT_R10G10B10A2_UNORM;
		psd.DSVFormat = dsvFormat;
		psd.SampleDesc.Count = 1;
		psd.SampleDesc.Quality = 0;
		ThrowIfFailed(pDevice->CreateGraphicsPipelineState(&psd,
			IID_PPV_ARGS(_normalState.ReleaseAndGetAddressOf())));
		psd.RasterizerState = RasterizerDescriptor(D3D12_FILL_MODE_WIREFRAME);
		ThrowIfFailed(pDevice->CreateGraphicsPipelineState(&psd,
			IID_PPV_ARGS(_wireframeState.ReleaseAndGetAddressOf())));
	}
}

D3D12_INPUT_LAYOUT_DESC WTF::NBPQDM::getInputLayout()
{
	static D3D12_INPUT_ELEMENT_DESC ieds[] = {
		D3D12Helper::InputElementDescriptor("POSITION", DXGI_FORMAT_R32G32B32A32_FLOAT, 0u),
		D3D12Helper::InputElementDescriptor("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT, 16u),
		D3D12Helper::InputElementDescriptor("TANGENT", DXGI_FORMAT_R32G32B32_FLOAT, 28u),
		D3D12Helper::InputElementDescriptor("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT, 40u),
	};
	return D3D12_INPUT_LAYOUT_DESC{ ieds, static_cast<UINT>(4) };
}
