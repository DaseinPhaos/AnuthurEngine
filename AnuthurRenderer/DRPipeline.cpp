//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************
#include "DRPipeline.h"

void Luxko::Anuthur::DRP::GBPass::NaiveBinnPhong::initialize(ID3D12Device* pDevice, 
	DXGI_FORMAT dsFormat /*= DXGI_FORMAT_D24_UNORM_S8_UINT*/)
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
		psd.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psd.NumRenderTargets = static_cast<UINT>(3);
		psd.RTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		psd.RTVFormats[1] = DXGI_FORMAT_R10G10B10A2_UNORM;
		psd.RTVFormats[2] = DXGI_FORMAT_R10G10B10A2_UNORM;
		psd.DSVFormat = dsFormat;
		psd.SampleDesc.Count = 1;
		psd.SampleDesc.Quality = 0;
		ThrowIfFailed(pDevice->CreateGraphicsPipelineState(&psd,
			IID_PPV_ARGS(_normalState.ReleaseAndGetAddressOf())));
		psd.RasterizerState = RasterizerDescriptor(D3D12_FILL_MODE_WIREFRAME);
		ThrowIfFailed(pDevice->CreateGraphicsPipelineState(&psd,
			IID_PPV_ARGS(_wireframeState.ReleaseAndGetAddressOf())));
	}
}

D3D12_SHADER_BYTECODE Luxko::Anuthur::DRP::GBPass::NaiveBinnPhong::getVS() const
{
	return _vertexShader.Get();
}

D3D12_SHADER_BYTECODE Luxko::Anuthur::DRP::GBPass::NaiveBinnPhong::getPS() const
{
	return _pixelShader.Get();
}

ID3D12PipelineState* Luxko::Anuthur::DRP::GBPass::NaiveBinnPhong::getPSO() const
{
	return _normalState.Get();
}

ID3D12PipelineState* Luxko::Anuthur::DRP::GBPass::NaiveBinnPhong::getPSOWireframe() const
{
	return _wireframeState.Get();
}

ID3D12RootSignature* Luxko::Anuthur::DRP::GBPass::NaiveBinnPhong::getRootSignature() const
{
	return _rootSignature.Get();
}

void Luxko::Anuthur::DRP::GBPass::NaiveBinnPhong::recordRp0Camera(
	ID3D12GraphicsCommandList* cmdlist, D3D12_GPU_VIRTUAL_ADDRESS cameraGpuAddress)
{
	cmdlist->SetGraphicsRootConstantBufferView(static_cast<UINT>(0), cameraGpuAddress);
}

void Luxko::Anuthur::DRP::GBPass::NaiveBinnPhong::recordRp1Material(
	ID3D12GraphicsCommandList* cmdlist, D3D12_GPU_VIRTUAL_ADDRESS materialGpuAddress)
{
	cmdlist->SetGraphicsRootConstantBufferView(static_cast<UINT>(1), materialGpuAddress);
}

void Luxko::Anuthur::DRP::GBPass::NaiveBinnPhong::recordRp2Textures(
	ID3D12GraphicsCommandList* cmdlist, D3D12_GPU_DESCRIPTOR_HANDLE texsSrvGpuHandle)
{
	cmdlist->SetGraphicsRootDescriptorTable(static_cast<UINT>(2), texsSrvGpuHandle);
}

void Luxko::Anuthur::DRP::GBPass::NaiveBinnPhong::recordStateSettings(
	ID3D12GraphicsCommandList* cmdlist, D3D_PRIMITIVE_TOPOLOGY primitiveTopology)
{
	cmdlist->IASetPrimitiveTopology(primitiveTopology);
	cmdlist->SetPipelineState(_normalState.Get());
	cmdlist->SetGraphicsRootSignature(_rootSignature.Get());
}

void Luxko::Anuthur::DRP::GBPass::NaiveBinnPhong::recordStateSettingsWireframe(
	ID3D12GraphicsCommandList* cmdlist, D3D_PRIMITIVE_TOPOLOGY primitiveTopology)
{
	cmdlist->IASetPrimitiveTopology(primitiveTopology);
	cmdlist->SetPipelineState(_normalState.Get());
	cmdlist->SetGraphicsRootSignature(_rootSignature.Get());
}

void Luxko::Anuthur::DRP::GBPass::NaiveBinnPhong::recordGBTransitionFrom(
	ID3D12GraphicsCommandList* cmdlist, ID3D12Resource* gBuffers,
	D3D12_RESOURCE_STATES from /*= D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE*/)
{
	D3D12Helper::ResourceBarrier barriers[] = {
		D3D12Helper::ResourceBarrier::TransitionBarrier(
			gBuffers, from, D3D12_RESOURCE_STATE_RENDER_TARGET),
		D3D12Helper::ResourceBarrier::TransitionBarrier(
			gBuffers + 1, from, D3D12_RESOURCE_STATE_RENDER_TARGET),
		D3D12Helper::ResourceBarrier::TransitionBarrier(
			gBuffers + 2, from, D3D12_RESOURCE_STATE_RENDER_TARGET),
	};
	cmdlist->ResourceBarrier(4u, barriers);
}

void Luxko::Anuthur::DRP::GBPass::NaiveBinnPhong::recordGBTransitionTo(
	ID3D12GraphicsCommandList* cmdlist, ID3D12Resource* gBuffers,
	D3D12_RESOURCE_STATES to /*= D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE*/)
{
	D3D12Helper::ResourceBarrier barriers[] = {
		D3D12Helper::ResourceBarrier::TransitionBarrier(
			gBuffers, D3D12_RESOURCE_STATE_RENDER_TARGET, to),
		D3D12Helper::ResourceBarrier::TransitionBarrier(
			gBuffers + 1, D3D12_RESOURCE_STATE_RENDER_TARGET, to),
		D3D12Helper::ResourceBarrier::TransitionBarrier(
			gBuffers + 2, D3D12_RESOURCE_STATE_RENDER_TARGET, to),
	};
	cmdlist->ResourceBarrier(3u, barriers);
}

void Luxko::Anuthur::DRP::GBPass::NaiveBinnPhong::recordClearAndSetRtvDsv(
	ID3D12GraphicsCommandList* cmdlist, D3D12_CPU_DESCRIPTOR_HANDLE* rtvHandles,
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle, UINT numRects, D3D12_RECT* pRects)
{
	cmdlist->ClearDepthStencilView(dsvHandle,
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.f,
		static_cast<UINT8>(0), numRects, pRects);
	static FLOAT bbg[] = { 0.f, 0.f, 0.f, 0.f };
	cmdlist->ClearRenderTargetView(rtvHandles[0], bbg, numRects, pRects);
	cmdlist->ClearRenderTargetView(rtvHandles[1], bbg, numRects, pRects);
	cmdlist->ClearRenderTargetView(rtvHandles[2], bbg, numRects, pRects);
	cmdlist->OMSetRenderTargets(static_cast<UINT>(3), rtvHandles, TRUE, &dsvHandle);

}

D3D12_INPUT_LAYOUT_DESC Luxko::Anuthur::DRP::GBPass::NaiveBinnPhong::getInputLayout()
{
	static D3D12_INPUT_ELEMENT_DESC ieds[] = {
		D3D12Helper::InputElementDescriptor("POSITION", DXGI_FORMAT_R32G32B32A32_FLOAT, 0u),
		D3D12Helper::InputElementDescriptor("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT, 16u),
		D3D12Helper::InputElementDescriptor("TANGENT", DXGI_FORMAT_R32G32B32_FLOAT, 28u),
		D3D12Helper::InputElementDescriptor("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT, 40u),
	};
	return D3D12_INPUT_LAYOUT_DESC{ ieds, static_cast<UINT>(4) };
}
