//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************
#include "DRPipeline.h"

void Luxko::Anuthur::DRP::GBPass::NaiveBlinnPhong::initialize(ID3D12Device* pDevice, 
	DXGI_FORMAT dsvFormat /*= DXGI_FORMAT_D24_UNORM_S8_UINT*/)
{
	using namespace D3D12Helper;

	// initialize root signature
	{
		RootSignatureDescriptor rsd;
		rsd.PushRPCBV(0u); // for camera attributes
		rsd.PushRPCBV(1u); // for material attributes
		RootDescriptorTable rdt;
		rdt.Push(RootDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
			static_cast<UINT>(2))); // for diffuse and normal map
		rsd.PushRPDescriptorTable(rdt.Get());
		rsd.PushStaticSamplerDescriptor(0u);

		ComPtr<ID3DBlob> rootSigBlob;
		ThrowIfFailed(rsd.SerializeRootSignature(rootSigBlob.GetAddressOf(),
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT));
		ThrowIfFailed(pDevice->CreateRootSignature(0u, rootSigBlob->GetBufferPointer(),
			rootSigBlob->GetBufferSize(), IID_PPV_ARGS(_rootSignature.ReleaseAndGetAddressOf())));
	}

	// initialize shader bytecode
	{
		_vertexShader.CompileFromFile(vsPath, "VSMain", "vs_5_0");
		_pixelShader.CompileFromFile(psPath, "PSMain", "ps_5_0");
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

//D3D12_SHADER_BYTECODE Luxko::Anuthur::DRP::GBPass::NaiveBinnPhong::getVS() const
//{
//	return _vertexShader.Get();
//}
//
//D3D12_SHADER_BYTECODE Luxko::Anuthur::DRP::GBPass::NaiveBinnPhong::getPS() const
//{
//	return _pixelShader.Get();
//}
//
//ID3D12PipelineState* Luxko::Anuthur::DRP::GBPass::NaiveBinnPhong::getPSO() const
//{
//	return _normalState.Get();
//}
//
//ID3D12PipelineState* Luxko::Anuthur::DRP::GBPass::NaiveBinnPhong::getPSOWireframe() const
//{
//	return _wireframeState.Get();
//}
//
//ID3D12RootSignature* Luxko::Anuthur::DRP::GBPass::NaiveBinnPhong::getRootSignature() const
//{
//	return _rootSignature.Get();
//}
//
//void Luxko::Anuthur::DRP::GBPass::NaiveBinnPhong::recordRp0Camera(
//	ID3D12GraphicsCommandList* cmdlist, D3D12_GPU_VIRTUAL_ADDRESS cameraGpuAddress)
//{
//	cmdlist->SetGraphicsRootConstantBufferView(static_cast<UINT>(0), cameraGpuAddress);
//}
//
//void Luxko::Anuthur::DRP::GBPass::NaiveBinnPhong::recordRp1Material(
//	ID3D12GraphicsCommandList* cmdlist, D3D12_GPU_VIRTUAL_ADDRESS materialGpuAddress)
//{
//	cmdlist->SetGraphicsRootConstantBufferView(static_cast<UINT>(1), materialGpuAddress);
//}
//
//void Luxko::Anuthur::DRP::GBPass::NaiveBinnPhong::recordRp2Textures(
//	ID3D12GraphicsCommandList* cmdlist, D3D12_GPU_DESCRIPTOR_HANDLE texsSrvGpuHandle)
//{
//	cmdlist->SetGraphicsRootDescriptorTable(static_cast<UINT>(2), texsSrvGpuHandle);
//}
//
//void Luxko::Anuthur::DRP::GBPass::NaiveBinnPhong::recordStateSettings(
//	ID3D12GraphicsCommandList* cmdlist,
//	D3D_PRIMITIVE_TOPOLOGY primitiveTopology)
//{
//	cmdlist->OMSetStencilRef(0x80);
//	cmdlist->IASetPrimitiveTopology(primitiveTopology);
//	cmdlist->SetPipelineState(_normalState.Get());
//	cmdlist->SetGraphicsRootSignature(_rootSignature.Get());
//}
//
//void Luxko::Anuthur::DRP::GBPass::NaiveBinnPhong::recordStateSettingsWireframe(
//	ID3D12GraphicsCommandList* cmdlist, D3D_PRIMITIVE_TOPOLOGY primitiveTopology)
//{
//	cmdlist->OMSetStencilRef(0x80);
//	cmdlist->IASetPrimitiveTopology(primitiveTopology);
//	cmdlist->SetPipelineState(_normalState.Get());
//	cmdlist->SetGraphicsRootSignature(_rootSignature.Get());
//}
//
//void Luxko::Anuthur::DRP::GBPass::NaiveBinnPhong::recordGBTransitionFrom(
//	ID3D12GraphicsCommandList* cmdlist, ID3D12Resource** gBuffers,
//	D3D12_RESOURCE_STATES from /*= D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE*/)
//{
//	D3D12Helper::ResourceBarrier barriers[] = {
//		D3D12Helper::ResourceBarrier::TransitionBarrier(
//			gBuffers[0], from, D3D12_RESOURCE_STATE_RENDER_TARGET),
//		D3D12Helper::ResourceBarrier::TransitionBarrier(
//			gBuffers[1], from, D3D12_RESOURCE_STATE_RENDER_TARGET),
//		D3D12Helper::ResourceBarrier::TransitionBarrier(
//			gBuffers[2], from, D3D12_RESOURCE_STATE_RENDER_TARGET),
//	};
//	cmdlist->ResourceBarrier(3u, barriers);
//}
//
//void Luxko::Anuthur::DRP::GBPass::NaiveBinnPhong::recordGBTransitionTo(
//	ID3D12GraphicsCommandList* cmdlist, ID3D12Resource** gBuffers,
//	D3D12_RESOURCE_STATES to /*= D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE*/)
//{
//	D3D12Helper::ResourceBarrier barriers[] = {
//		D3D12Helper::ResourceBarrier::TransitionBarrier(
//			gBuffers[0], D3D12_RESOURCE_STATE_RENDER_TARGET, to),
//		D3D12Helper::ResourceBarrier::TransitionBarrier(
//			gBuffers[1], D3D12_RESOURCE_STATE_RENDER_TARGET, to),
//		D3D12Helper::ResourceBarrier::TransitionBarrier(
//			gBuffers[2], D3D12_RESOURCE_STATE_RENDER_TARGET, to),
//	};
//	cmdlist->ResourceBarrier(3u, barriers);
//}
//
//void Luxko::Anuthur::DRP::GBPass::NaiveBinnPhong::recordClearAndSetRtvDsv(
//	ID3D12GraphicsCommandList* cmdlist, D3D12_CPU_DESCRIPTOR_HANDLE* rtvHandles,
//	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle, UINT numRects,const D3D12_RECT* pRects)
//{
//	cmdlist->ClearDepthStencilView(dsvHandle,
//		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.f,
//		static_cast<UINT8>(0), numRects, pRects);
//	static FLOAT bbg[] = { 0.f, 0.f, 0.f, 0.f };
//	cmdlist->ClearRenderTargetView(rtvHandles[0], bbg, numRects, pRects);
//	cmdlist->ClearRenderTargetView(rtvHandles[1], bbg, numRects, pRects);
//	cmdlist->ClearRenderTargetView(rtvHandles[2], bbg, numRects, pRects);
//	cmdlist->OMSetRenderTargets(static_cast<UINT>(3), rtvHandles, TRUE, &dsvHandle);
//
//}

D3D12_INPUT_LAYOUT_DESC Luxko::Anuthur::DRP::GBPass::NaiveBlinnPhong::getInputLayout()
{
	static D3D12_INPUT_ELEMENT_DESC ieds[] = {
		D3D12Helper::InputElementDescriptor("POSITION", DXGI_FORMAT_R32G32B32A32_FLOAT, 0u),
		D3D12Helper::InputElementDescriptor("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT, 16u),
		D3D12Helper::InputElementDescriptor("TANGENT", DXGI_FORMAT_R32G32B32_FLOAT, 28u),
		D3D12Helper::InputElementDescriptor("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT, 40u),
	};
	return D3D12_INPUT_LAYOUT_DESC{ ieds, static_cast<UINT>(4) };
}

//void Luxko::Anuthur::DRP::LightPass::NaiveLights::recordRp1CameraAndGBuffer(
//	ID3D12GraphicsCommandList* cmdlist,
//	D3D12_GPU_DESCRIPTOR_HANDLE cameraGpuHandleAddress)
//{
//	cmdlist->SetGraphicsRootDescriptorTable(static_cast<UINT>(1), cameraGpuHandleAddress);
//}
//
//void Luxko::Anuthur::DRP::LightPass::NaiveLights::recordRp0Light(
//	ID3D12GraphicsCommandList* cmdlist, D3D12_GPU_VIRTUAL_ADDRESS lightCBGpuAddress,
//	UINT& stencilRef)
//{
//	assert(cmdlist);
//	++stencilRef;
//	assert(stencilRef < 0xff);
//	cmdlist->OMSetStencilRef(stencilRef);
//	cmdlist->SetGraphicsRootConstantBufferView(static_cast<UINT>(0), lightCBGpuAddress);
//}

ID3D12RootSignature* Luxko::Anuthur::DRP::LightPass::NaiveLights::getRootSignature(ID3D12Device* creationDevice /*= nullptr /* Used first time*/)
{

	static ComPtr<ID3D12RootSignature> naiveRootSig = nullptr;
	if (naiveRootSig.Get() == nullptr) {
		assert(creationDevice);
		using namespace D3D12Helper;
		RootSignatureDescriptor rsd;
		rsd.PushRPCBV(0u);
		RootDescriptorTable rdt;
		rdt.Push(RootDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1u, 1u));
		rdt.Push(RootDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3u));
		rsd.PushRPDescriptorTable(rdt.Get());
		ComPtr<ID3DBlob> rootSigBlob;
		ThrowIfFailed(rsd.SerializeRootSignature(rootSigBlob.GetAddressOf(),
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT));
		ThrowIfFailed(creationDevice->CreateRootSignature(0u,
			rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
			IID_PPV_ARGS(naiveRootSig.ReleaseAndGetAddressOf())));
	}
	return naiveRootSig.Get();
}


//void Luxko::Anuthur::DRP::LightPass::NaiveLights::recordSettings(
//	ID3D12GraphicsCommandList* cmdlist, ID3D12Device* creationDevice /*= nullptr /* Used first time*/)
//{
//
//	assert(cmdlist);
//	cmdlist->SetGraphicsRootSignature(getRootSignature(creationDevice));
//	cmdlist->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//}
//
//
//void Luxko::Anuthur::DRP::LightPass::NaiveLights::resetLightingRecords(
//	ID3D12GraphicsCommandList* cmdlist, UINT& stencilRef,
//	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle,
//	UINT numRects, D3D12_RECT* pRects)
//{
//	stencilRef = 0x80u;
//	cmdlist->ClearDepthStencilView(dsvHandle,
//		D3D12_CLEAR_FLAG_STENCIL, 0.f, static_cast<UINT8>(stencilRef),
//		numRects, pRects);
//}
//
//void Luxko::Anuthur::DRP::LightPass::NaiveLights::recordGBTransitionFrom(
//	ID3D12GraphicsCommandList* cmdlist, ID3D12Resource** gBuffers,
//	D3D12_RESOURCE_STATES from /*= D3D12_RESOURCE_STATE_RENDER_TARGET*/)
//{
//	D3D12Helper::ResourceBarrier barriers[] = {
//		D3D12Helper::ResourceBarrier::TransitionBarrier(
//			gBuffers[0], from, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE),
//		D3D12Helper::ResourceBarrier::TransitionBarrier(
//			gBuffers[1], from, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE),
//		D3D12Helper::ResourceBarrier::TransitionBarrier(
//			gBuffers[2], from, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE),
//	};
//	cmdlist->ResourceBarrier(3u, barriers);
//}
//
//void Luxko::Anuthur::DRP::LightPass::NaiveLights::recordGBTransitionTo(
//	ID3D12GraphicsCommandList* cmdlist, ID3D12Resource** gBuffers,
//	D3D12_RESOURCE_STATES to /*= D3D12_RESOURCE_STATE_RENDER_TARGET*/)
//{
//	D3D12Helper::ResourceBarrier barriers[] = {
//		D3D12Helper::ResourceBarrier::TransitionBarrier(
//			gBuffers[0], D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, to),
//		D3D12Helper::ResourceBarrier::TransitionBarrier(
//			gBuffers[1], D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, to),
//		D3D12Helper::ResourceBarrier::TransitionBarrier(
//			gBuffers[2], D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, to),
//	};
//	cmdlist->ResourceBarrier(3u, barriers);
//}
//
//void Luxko::Anuthur::DRP::LightPass::NaiveLights::recordClearAndSetRtvDsv(
//	ID3D12GraphicsCommandList* cmdlist, D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle,
//	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle, UINT numRects, D3D12_RECT* pRects)
//{
//	static FLOAT bbg[] = { 0.f, 0.f, 0.f, 0.f };
//	cmdlist->ClearRenderTargetView(rtvHandle, bbg, numRects, pRects);
//	cmdlist->OMSetRenderTargets(static_cast<UINT>(1), &rtvHandle, TRUE, &dsvHandle);
//}

D3D12_INPUT_LAYOUT_DESC Luxko::Anuthur::DRP::LightPass::NaiveLights::getInputLayout()
{
	static D3D12_INPUT_ELEMENT_DESC ieds[] = {
		D3D12Helper::InputElementDescriptor("POSITION", DXGI_FORMAT_R32G32B32_FLOAT, 0u),
	};
	return D3D12_INPUT_LAYOUT_DESC{ ieds, static_cast<UINT>(1) };
}

void Luxko::Anuthur::DRP::LightPass::NaiveLights::PointLight::initialize(
	ID3D12Device* pDevice,
	DXGI_FORMAT rtvFormat /*= DXGI_FORMAT_R8G8B8A8_UNORM*/,
	DXGI_FORMAT dsvFormat /*= DXGI_FORMAT_D24_UNORM_S8_UINT*/)
{
	using namespace D3D12Helper;

	// initialize shader bytecode
	{
		D3D_SHADER_MACRO macro[] = {
			"POINTLIGHT", "1", nullptr, nullptr
		};
		_vertexShader.CompileFromFile(vsPath, "VSMain", "vs_5_0", macro);
		_pixelShader.CompileFromFile(psPath, "PSMain", "ps_5_0", macro);
	}

	// initialize PSOs
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psd = {};
		psd.InputLayout = getInputLayout();
		psd.pRootSignature = getRootSignature(pDevice);
		psd.VS = _vertexShader.Get();
		psd.PS = _pixelShader.Get();
		psd.BlendState = BlendDescriptor::Default();
		psd.BlendState.RenderTarget[0].BlendEnable = TRUE;
		psd.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
		psd.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
		psd.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		psd.SampleMask = UINT_MAX;
		psd.RasterizerState = RasterizerDescriptor(D3D12_FILL_MODE_SOLID,
			D3D12_CULL_MODE_FRONT);
		psd.DepthStencilState = DepthStencilStateDescriptor::Default();
		psd.DepthStencilState.DepthEnable = FALSE;
		psd.DepthStencilState.StencilEnable = TRUE;
		psd.DepthStencilState.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_GREATER;
		psd.DepthStencilState.BackFace.StencilPassOp = D3D12_STENCIL_OP_REPLACE;
		psd.DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_GREATER;
		psd.DepthStencilState.BackFace.StencilPassOp = D3D12_STENCIL_OP_REPLACE;

		psd.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psd.NumRenderTargets = static_cast<UINT>(1);
		psd.RTVFormats[0] = rtvFormat;
		psd.DSVFormat = dsvFormat;
		psd.SampleDesc.Count = 1;
		psd.SampleDesc.Quality = 0;
		ThrowIfFailed(pDevice->CreateGraphicsPipelineState(&psd,
			IID_PPV_ARGS(_normalState.ReleaseAndGetAddressOf())));
	}
}

//D3D12_SHADER_BYTECODE Luxko::Anuthur::DRP::LightPass::NaiveLights::PointLight::getVS() const
//{
//	return _vertexShader.Get();
//}
//
//D3D12_SHADER_BYTECODE Luxko::Anuthur::DRP::LightPass::NaiveLights::PointLight::getPS() const
//{
//	return _pixelShader.Get();
//}
//
//ID3D12PipelineState* Luxko::Anuthur::DRP::LightPass::NaiveLights::PointLight::getPSO() const
//{
//	return _normalState.Get();
//}

const Luxko::Anuthur::MeshGeometry& Luxko::Anuthur::DRP::LightPass::NaiveLights
	::PointLight::getInputBuffer(ID3D12Device* pDevice /*= nullptr*/, 
		ID3D12GraphicsCommandList* pCmdlist /*= nullptr*/)
{
	static MeshGeometry mesh;
	static bool initialized = false;
	if (!initialized) {
		assert(pDevice);
		assert(pCmdlist);
		std::vector<VSI> vertice;
		std::vector<UINT> indice;
		vertice.reserve(2 * 17 * 17);
		auto dAlpha = static_cast<float>(M_PI) * 2 / 16;
		auto dBeta = static_cast<float>(M_PI) / 16;

		// Center circle first, then advance into up and down stacks.
		for (auto i = 0u; i <= 16u; ++i) {
			auto cosAlpha = std::cos(dAlpha*i);
			auto sinAlpha = std::sin(dAlpha*i);
			for (auto j = 0u; j <= 16u; ++j) {
				auto cosBeta = std::cos(dBeta*j);
				auto sinBeta = std::sin(dBeta*j);
				auto z = sinBeta;
				auto r = cosBeta;

				VSI vsi;
				vsi.pos = Vector3f(r*cosAlpha, z, r*sinAlpha);
				vertice.push_back(vsi);
			}
		}

		for (auto i = 0u; i <= 16u; ++i) {
			auto cosAlpha = std::cos(dAlpha*i);
			auto sinAlpha = std::sin(dAlpha*i);
			for (auto j = 0u; j <= 16u; ++j) {
				auto cosBeta = std::cos(dBeta*j);
				auto sinBeta = std::sin(dBeta*j);
				auto z = sinBeta;
				auto r = cosBeta;
				VSI vsi;
				vsi.pos = Vector3f(r*cosAlpha, -z, r*sinAlpha);
				vertice.push_back(vsi);
			}
		}

		indice.reserve(12u * 16u * 16u);

		auto half = 17u * 17u;

		for (auto i = 0u; i < 16u; ++i) {
			for (auto j = 0u; j < 16u; ++j) {
				indice.push_back(i*(16u + 1) + j);
				indice.push_back(i*(16u + 1) + j + 1);
				indice.push_back((i + 1)*(16u + 1) + j + 1);
				indice.push_back(i*(16u + 1) + j);
				indice.push_back((i + 1)*(16u + 1) + j + 1);
				indice.push_back((i + 1)*(16u + 1) + j);


				indice.push_back(i*(16u + 1) + j + half);

				indice.push_back((i + 1)*(16u + 1) + j + 1 + half);
				indice.push_back(i*(16u + 1) + j + 1 + half);
				indice.push_back(i*(16u + 1) + j + half);

				indice.push_back((i + 1)*(16u + 1) + j + half);
				indice.push_back((i + 1)*(16u + 1) + j + 1 + half);
			}
		}

		mesh.InitializeCPUResource(DXGI_FORMAT_R32_UINT,
			static_cast<UINT>(indice.size()), indice.data(),
			static_cast<UINT>(sizeof(VSI)), static_cast<UINT>(vertice.size()),
			vertice.data());
		mesh.RecordUpdateFromCPUtoGPU(pDevice, pCmdlist);
		initialized = true;
	}

	return mesh;
}



//void Luxko::Anuthur::DRP::LightPass::NaiveLights::PointLight::recordSettings(
//	ID3D12GraphicsCommandList* cmdList)
//{
//	assert(cmdList);
//	cmdList->SetPipelineState(_normalState.Get());
//	auto& mesh = getInputBuffer();
//	cmdList->IASetVertexBuffers(0u, 1u, &mesh.VertexBufferView());
//	cmdList->IASetIndexBuffer(&mesh.IndexBufferView());
//}
//
//void Luxko::Anuthur::DRP::LightPass::NaiveLights::PointLight::recordDraw(
//	ID3D12GraphicsCommandList* pCmdlist)
//{
//	assert(pCmdlist);
//	pCmdlist->DrawIndexedInstanced(getInputBuffer().GetTotoalIndexCount(),
//		1u, 0u, 0u, 0u);
//}

void Luxko::Anuthur::DRP::LightPass::NaiveLights::PointLight::generateOtoWMatrix(
	LightParams& params)
{
	auto m = Matrix4x4f::Translation(params.posW._x, params.posW._y, params.posW._z) 
		* Matrix4x4f::ScaleN(params.range._x);
	params.mOtoW = m;
}

void Luxko::Anuthur::DRP::LightPass::NaiveLights::SpotLight::initialize(
	ID3D12Device* pDevice, 
	DXGI_FORMAT rtvFormat /*= DXGI_FORMAT_R8G8B8A8_UNORM*/, 
	DXGI_FORMAT dsvFormat /*= DXGI_FORMAT_D24_UNORM_S8_UINT*/)
{
	using namespace D3D12Helper;

	// initialize shader bytecode
	{
		D3D_SHADER_MACRO macro[] = {
			"SPOTLIGHT", "1", nullptr, nullptr
		};
		_vertexShader.CompileFromFile(vsPath, "VSMain", "vs_5_0", macro);
		_pixelShader.CompileFromFile(psPath, "PSMain", "ps_5_0", macro);
	}

	// initialize PSOs
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psd = {};
		psd.InputLayout = getInputLayout();
		psd.pRootSignature = getRootSignature(pDevice);
		psd.VS = _vertexShader.Get();
		psd.PS = _pixelShader.Get();
		psd.BlendState = BlendDescriptor::Default();
		psd.BlendState.RenderTarget[0].BlendEnable = TRUE;
		psd.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
		psd.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
		psd.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		psd.SampleMask = UINT_MAX;
		psd.RasterizerState = RasterizerDescriptor(D3D12_FILL_MODE_SOLID,
			D3D12_CULL_MODE_FRONT);
		psd.DepthStencilState = DepthStencilStateDescriptor::Default();
		psd.DepthStencilState.DepthEnable = FALSE;
		psd.DepthStencilState.StencilEnable = TRUE;
		psd.DepthStencilState.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_GREATER;
		psd.DepthStencilState.BackFace.StencilPassOp = D3D12_STENCIL_OP_REPLACE;
		psd.DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_GREATER;
		psd.DepthStencilState.BackFace.StencilPassOp = D3D12_STENCIL_OP_REPLACE;

		psd.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psd.NumRenderTargets = static_cast<UINT>(1);
		psd.RTVFormats[0] = rtvFormat;
		psd.DSVFormat = dsvFormat;
		psd.SampleDesc.Count = 1;
		psd.SampleDesc.Quality = 0;
		ThrowIfFailed(pDevice->CreateGraphicsPipelineState(&psd,
			IID_PPV_ARGS(_normalState.ReleaseAndGetAddressOf())));
	}
}

//D3D12_SHADER_BYTECODE Luxko::Anuthur::DRP::LightPass::NaiveLights::SpotLight::getVS() const
//{
//	return _vertexShader.Get();
//}
//
//D3D12_SHADER_BYTECODE Luxko::Anuthur::DRP::LightPass::NaiveLights::SpotLight::getPS() const
//{
//	return _pixelShader.Get();
//}
//
//ID3D12PipelineState* Luxko::Anuthur::DRP::LightPass::NaiveLights::SpotLight::getPSO() const
//{
//	return _normalState.Get();
//}

const Luxko::Anuthur::MeshGeometry& 
	Luxko::Anuthur::DRP::LightPass::NaiveLights::SpotLight::getInputBuffer(
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
		vertice.reserve(18);
		VSI tmpvsi;
		tmpvsi.pos = Vector3f(0.f, 0.f, 0.f);
		vertice.push_back(tmpvsi);
		tmpvsi.pos = Vector3f(0.f, 0.f, 1.f);
		vertice.push_back(tmpvsi);

		auto dAlpha = static_cast<float>(M_PI) * 2 / 16;

		// Center circle first, then advance into up and down stacks.
		for (auto i = 0u; i <= 16u; ++i) {
			auto cosAlpha = std::cos(dAlpha*i);
			auto sinAlpha = std::sin(dAlpha*i);
			VSI vsi;
			vsi.pos = Vector3f(cosAlpha, sinAlpha, 1.f);
			vertice.push_back(vsi);
		}

		indice.reserve(16 * 6);

		for (auto i = 0u; i < 16u; ++i) {
			indice.push_back(i + 2);
			indice.push_back(0);
			indice.push_back(i + 3);
			indice.push_back(i + 2);
			indice.push_back(i + 3);
			indice.push_back(1);
		}
		

		mesh.InitializeCPUResource(DXGI_FORMAT_R32_UINT,
			static_cast<UINT>(indice.size()), indice.data(),
			static_cast<UINT>(sizeof(VSI)), static_cast<UINT>(vertice.size()),
			vertice.data());
		mesh.RecordUpdateFromCPUtoGPU(pDevice, pCmdlist);
		initialized = true;
	}

	return mesh;
}

//void Luxko::Anuthur::DRP::LightPass::NaiveLights::SpotLight::recordSettings(
//	ID3D12GraphicsCommandList* cmdList)
//{
//	assert(cmdList);
//	cmdList->SetPipelineState(_normalState.Get());
//	auto& mesh = getInputBuffer();
//	cmdList->IASetVertexBuffers(0u, 1u, &mesh.VertexBufferView());
//	cmdList->IASetIndexBuffer(&mesh.IndexBufferView());
//}
//
//void Luxko::Anuthur::DRP::LightPass::NaiveLights::SpotLight::recordDraw(
//	ID3D12GraphicsCommandList* pCmdlist)
//{
//	assert(pCmdlist);
//	pCmdlist->DrawIndexedInstanced(getInputBuffer().GetTotoalIndexCount(),
//		1u, 0u, 0u, 0u);
//}

void Luxko::Anuthur::DRP::LightPass::NaiveLights::SpotLight::generateOtoWMatrix(
	LightParams& params)
{
	auto length = params.range._x;
	auto scalexy = (std::sqrt(1.f - params.angleY * params.angleY) / params.angleY) * length;
	auto scaling = Matrix4x4f::Scale(scalexy, scalexy, length);
	auto targetFrame = Frame3DH::LookTowards(Vector3DH(-params.direction.xyz()),
		Vector3DH::E2(), Point3DH(params.posW.xyz()));
	auto orienting = targetFrame.GetTransform().Inverse().AsMatrix4x4();
	params.mOtoW = orienting * scaling;
}

void Luxko::Anuthur::DRP::LightPass::NaiveLights::DirectionalLight::initialize(
	ID3D12Device* pDevice, 
	DXGI_FORMAT rtvFormat /*= DXGI_FORMAT_R8G8B8A8_UNORM*/, 
	DXGI_FORMAT dsvFormat /*= DXGI_FORMAT_D24_UNORM_S8_UINT*/)
{
	using namespace D3D12Helper;

	// initialize shader bytecode
	{
		D3D_SHADER_MACRO macro[] = {
			"DIRCTIONALLIGHT", "1", nullptr, nullptr
		};
		_vertexShader.CompileFromFile(vsPath, "VSMain", "vs_5_0", macro);
		_pixelShader.CompileFromFile(psPath, "PSMain", "ps_5_0", macro);
	}

	// initialize PSOs
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psd = {};
		psd.InputLayout = getInputLayout();
		psd.pRootSignature = getRootSignature(pDevice);
		psd.VS = _vertexShader.Get();
		psd.PS = _pixelShader.Get();
		psd.BlendState = BlendDescriptor::Default();
		psd.BlendState.RenderTarget[0].BlendEnable = TRUE;
		psd.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
		psd.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
		psd.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		psd.SampleMask = UINT_MAX;
		psd.RasterizerState = RasterizerDescriptor(D3D12_FILL_MODE_SOLID,
			D3D12_CULL_MODE_FRONT);
		psd.DepthStencilState = DepthStencilStateDescriptor::Default();
		psd.DepthStencilState.DepthEnable = FALSE;
		psd.DepthStencilState.StencilEnable = TRUE;
		psd.DepthStencilState.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_GREATER;
		psd.DepthStencilState.BackFace.StencilPassOp = D3D12_STENCIL_OP_REPLACE;
		psd.DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_GREATER;
		psd.DepthStencilState.BackFace.StencilPassOp = D3D12_STENCIL_OP_REPLACE;

		psd.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psd.NumRenderTargets = static_cast<UINT>(1);
		psd.RTVFormats[0] = rtvFormat;
		psd.DSVFormat = dsvFormat;
		psd.SampleDesc.Count = 1;
		psd.SampleDesc.Quality = 0;
		ThrowIfFailed(pDevice->CreateGraphicsPipelineState(&psd,
			IID_PPV_ARGS(_normalState.ReleaseAndGetAddressOf())));
	}
}

//D3D12_SHADER_BYTECODE Luxko::Anuthur::DRP::LightPass::NaiveLights::DirectionalLight::getVS() const
//{
//	return _vertexShader.Get();
//}
//
//D3D12_SHADER_BYTECODE Luxko::Anuthur::DRP::LightPass::NaiveLights::DirectionalLight::getPS() const
//{
//	return _pixelShader.Get();
//}
//
//ID3D12PipelineState* Luxko::Anuthur::DRP::LightPass::NaiveLights::DirectionalLight::getPSO() const
//{
//	return _normalState.Get();
//}

const Luxko::Anuthur::MeshGeometry& 
Luxko::Anuthur::DRP::LightPass::NaiveLights::DirectionalLight::getInputBuffer(
	ID3D12Device* pDevice /*= nullptr*/, ID3D12GraphicsCommandList* pCmdlist /*= nullptr*/)
{
	static MeshGeometry mesh;
	static bool initialized = false;
	if (!initialized) {
		assert(pDevice);
		assert(pCmdlist);
		auto x = 0.5f;
		auto y = 0.5f;
		auto z = 0.5f;
		std::vector<VSI> vertice = {
			// Side A, clockwise
			VSI{Vector3f(-x, -y, z)},
			VSI{Vector3f(-x, y, z)},
			VSI{Vector3f(x, y, z)},
			VSI{Vector3f(x, -y, z)},

			// Side B, clockwise
			VSI{Vector3f(x, -y, z)},
			VSI{Vector3f(x, y, z)},
			VSI{Vector3f(x, y, -z)},
			VSI{Vector3f(x, -y, -z)},

			// Side C, clockwise
			VSI{Vector3f(x, -y, -z)},
			VSI{Vector3f(x, y, -z)},
			VSI{Vector3f(-x, y, -z)},
			VSI{Vector3f(-x, -y, -z)},

			// Side D, clockwise
			VSI{Vector3f(-x, -y, -z)},
			VSI{Vector3f(-x, y, -z)},
			VSI{Vector3f(-x, y, z)},
			VSI{Vector3f(-x, -y, z)},

			// Side E, clockwise
			VSI{Vector3f(x, y, z)},
			VSI{Vector3f(-x, y, z)},
			VSI{Vector3f(-x, y, -z)},
			VSI{Vector3f(x, y, -z)},

			// Side F, clockwise
			VSI{Vector3f(-x, -y, z)},
			VSI{Vector3f(x, -y, z)},
			VSI{Vector3f(x, -y, -z)},
			VSI{Vector3f(-x, -y, -z)},
		};
		std::vector<UINT> indice = {
			// Side A, offset = 0
			0, 1, 2, 0, 2, 3,
			// Side B, offset = 4
			4, 5, 6, 4, 6, 7,
			// Side C, offset = 8
			8, 9, 10, 8, 10, 11,
			// Side D, offset = 12
			12, 13, 14, 12, 14, 15,
			// Side E, offset = 16
			16, 17, 18, 16, 18, 19,
			// Side F, offset = 20
			20, 21, 22, 20, 22, 23
		};

		mesh.InitializeCPUResource(DXGI_FORMAT_R32_UINT,
			static_cast<UINT>(indice.size()), indice.data(),
			static_cast<UINT>(sizeof(VSI)), static_cast<UINT>(vertice.size()),
			vertice.data());
		mesh.RecordUpdateFromCPUtoGPU(pDevice, pCmdlist);
		initialized = true;
	}

	return mesh;
}

//void Luxko::Anuthur::DRP::LightPass::NaiveLights::DirectionalLight::recordSettings(
//	ID3D12GraphicsCommandList* cmdList)
//{
//	assert(cmdList);
//	cmdList->SetPipelineState(_normalState.Get());
//	auto& mesh = getInputBuffer();
//	cmdList->IASetVertexBuffers(0u, 1u, &mesh.VertexBufferView());
//	cmdList->IASetIndexBuffer(&mesh.IndexBufferView());
//}
//
//void Luxko::Anuthur::DRP::LightPass::NaiveLights::DirectionalLight::recordDraw(
//	ID3D12GraphicsCommandList* pCmdlist)
//{
//	assert(pCmdlist);
//	pCmdlist->DrawIndexedInstanced(getInputBuffer().GetTotoalIndexCount(),
//		1u, 0u, 0u, 0u);
//}

void Luxko::Anuthur::DRP::LightPass::NaiveLights::DirectionalLight::generateOtoWMatrix(
	LightParams& params)
{
	auto scaling = Matrix4x4f::ScaleN(params.range._x);
	auto translation = Matrix4x4f::Translation(params.posW.xyz());
	params.mOtoW = translation * scaling;
}
