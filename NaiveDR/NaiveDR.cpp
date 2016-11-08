//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************
#include "NaiveDR.h"
#include "d3dx12.h"
#include "BasicGeometry.h"
// #include "DirectXTKInc/DDSTextureLoader.h"
#include "DDSTextureLoader.h"
#include "WICReadback.h"
#include <iostream>
void DeferredRenderingRuntime::initialize(Anuthur::D3D12Manager& manager, DXGI_FORMAT targetRTF)
{
	using namespace D3D12Helper;

	// initialize the root signature
	{
		RootSignatureDescriptor gbRsd;

		gbRsd.PushRPCBV(0u); // for transforms
		gbRsd.PushRPCBV(1u); // for materials
		//gbRsd.PushRPSRV(0u); // for diffuse map
		//gbRsd.PushRPSRV(1u); // for normal map
		RootDescriptorTable gbRdt;
		gbRdt.Push(RootDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, static_cast<UINT>(2)));
		gbRsd.PushRPDescriptorTable(gbRdt.Get());
		gbRsd.PushStaticSamplerDescriptor(0u); // for static sampler
		_gbRootSignature = manager.AddRootSignature(
			gbRsd,
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT, 0u,
			gbRSStr);
		// _pGBRootSignature = manager.FindRootSignature(id);
	}
	{
		RootSignatureDescriptor lpRsd;
		lpRsd.PushRPCBV(0u); // for light params
		// lpRsd.PushRPCBV(1u); // for camera
		//lpRsd.PushRPSRV(0u); // for gb normal
		//lpRsd.PushRPSRV(1u); // for gb diffuse albedo
		//lpRsd.PushRPSRV(2u); // for gb specular albedo
		//lpRsd.PushRPSRV(3u); // for gb positon
		RootDescriptorTable lpRdt;
		lpRdt.Push(RootDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1u, 1u, 0u, 0u));
		lpRdt.Push(RootDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 4u, 0u, 0u, 1u));
		lpRsd.PushRPDescriptorTable(lpRdt.Get());
		_lpRootSignature = manager.AddRootSignature(lpRsd,
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT,
			0u, lpRSStr);
		// _pLightPassRootSignature = manager.FindRootSignature(id);
	}


	// initialize shaders
	{
		_gbVS = manager.AddShader(gbShaderPath, "VS", "vs_5_0");
		_gbPS = manager.AddShader(gbShaderPath, "PS", "ps_5_0");

		_lpVS = manager.AddShader(lpShaderPath, "VS", "vs_5_0");
		D3D_SHADER_MACRO macro[] = {
			"POINTLIGHT", "1", nullptr, nullptr
		};
		_lpPointlightPS = manager.AddShader(lpShaderPath, "PS", "ps_5_0", nullptr, macro);
		macro[0].Name = "SPOTLIGHT";
		_lpSpotlightPS = manager.AddShader(lpShaderPath, "PS", "ps_5_0", nullptr, macro);
		macro[0].Name = "DIRCTIONALLIGHT";
		_lpDirectionalPS = manager.AddShader(lpShaderPath, "PS", "ps_5_0", nullptr, macro);
	}

	// initialize g-buffers for GPU

	// initialize PSOs
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC gbGpsd = {};
		InputLayoutDescriptor gbIld;
		gbIld.PushElementDescription("POSITION", DXGI_FORMAT_R32G32B32A32_FLOAT);
		gbIld.PushElementDescription("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT);
		gbIld.PushElementDescription("TANGENT", DXGI_FORMAT_R32G32B32_FLOAT);
		gbIld.PushElementDescription("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT);
		gbGpsd.InputLayout = gbIld.Get();
		gbGpsd.pRootSignature = manager.FindRootSignature(_gbRootSignature).Get();
		gbGpsd.VS = manager.FindShader(_gbVS).Get();
		gbGpsd.PS = manager.FindShader(_gbPS).Get();
		gbGpsd.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		gbGpsd.SampleMask = UINT_MAX;
		gbGpsd.RasterizerState = RasterizerDescriptor();
		gbGpsd.DepthStencilState = DepthStencilStateDescriptor::Default();
		gbGpsd.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		gbGpsd.NumRenderTargets = 4u;
		gbGpsd.RTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		gbGpsd.RTVFormats[1] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		gbGpsd.RTVFormats[2] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		gbGpsd.RTVFormats[3] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		gbGpsd.DSVFormat = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
		gbGpsd.SampleDesc.Count = 1;
		gbGpsd.SampleDesc.Quality = 0;
		_gbNormalPSO = manager.AddPSO(gbGpsd, gbNormalPSOStr);
		gbGpsd.RasterizerState = RasterizerDescriptor(D3D12_FILL_MODE_WIREFRAME);
		_gbWireframePSO = manager.AddPSO(gbGpsd, gbWireframePSOStr);
	}

	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC lpGpsd = {};
		InputLayoutDescriptor lpIld;
		lpIld.PushElementDescription("POSITION", DXGI_FORMAT_R32G32B32_FLOAT);
		lpGpsd.InputLayout = lpIld.Get();
		lpGpsd.pRootSignature = manager.FindRootSignature(_lpRootSignature).Get();
		auto bsd = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		//bsd.RenderTarget[0].BlendEnable = TRUE;
		//bsd.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		//bsd.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
		//bsd.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
		//bsd.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		//bsd.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ONE;
		//bsd.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		//bsd.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		//lpGpsd.BlendState = bsd;
		lpGpsd.BlendState.AlphaToCoverageEnable = FALSE;
		lpGpsd.BlendState.IndependentBlendEnable = FALSE;
		for (auto i = 0u; i < 8u; ++i) {
			lpGpsd.BlendState.RenderTarget[i].BlendEnable = TRUE;
			lpGpsd.BlendState.RenderTarget[i].BlendOp = D3D12_BLEND_OP_ADD;
			lpGpsd.BlendState.RenderTarget[i].SrcBlend = D3D12_BLEND_ONE;
			lpGpsd.BlendState.RenderTarget[i].DestBlend = D3D12_BLEND_ONE;
			lpGpsd.BlendState.RenderTarget[i].SrcBlendAlpha = D3D12_BLEND_ONE;
			lpGpsd.BlendState.RenderTarget[i].DestBlendAlpha = D3D12_BLEND_ZERO;
			lpGpsd.BlendState.RenderTarget[i].BlendOpAlpha = D3D12_BLEND_OP_ADD;
			lpGpsd.BlendState.RenderTarget[i].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		}
		lpGpsd.SampleMask = UINT_MAX;
		lpGpsd.RasterizerState = RasterizerDescriptor();
		// lpGpsd.DepthStencilState = DepthStencilStateDescriptor::Default();
		lpGpsd.DepthStencilState = DepthStencilStateDescriptor::Default();
		lpGpsd.DepthStencilState.DepthEnable = FALSE;
		lpGpsd.DepthStencilState.StencilEnable = FALSE;
		lpGpsd.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		lpGpsd.NumRenderTargets = 1u;
		lpGpsd.RTVFormats[0] = targetRTF;
		// lpGpsd.DSVFormat = targetDSF;
		lpGpsd.SampleDesc.Count = 1;
		lpGpsd.SampleDesc.Quality = 0;
		lpGpsd.VS = manager.FindShader(_lpVS).Get();
		lpGpsd.PS = manager.FindShader(_lpSpotlightPS).Get();
		_lpSpotlightPSO = manager.AddPSO(lpGpsd, lpSpotlightPSOStr);
		lpGpsd.PS = manager.FindShader(_lpPointlightPS).Get();
		_lpPointlightPSO = manager.AddPSO(lpGpsd, lpPointlightPSOStr);
		lpGpsd.PS = manager.FindShader(_lpDirectionalPS).Get();
		_lpDirectionalPSO = manager.AddPSO(lpGpsd, lpDirectonalPSOStr);
	}

	_pManager = &manager;
}

void DeferredRenderingRuntime::recordGBPass(ID3D12GraphicsCommandList* pCmdlist,
	const OMBindings& gboms, GBPassBindings* bindingStart, unsigned int bindingCount /*= 1u*/) const
{
	pCmdlist->SetPipelineState(getGBNormalPSO());
	pCmdlist->SetGraphicsRootSignature(getGBRootSignature());
	pCmdlist->ClearDepthStencilView(*gboms.dsv, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
		1.f, static_cast<UINT8>(0), gboms.rectCount, gboms.rectStart);
	static float bbg[] = { 0.f, 0.f, 0.f, 0.f };
	pCmdlist->ClearRenderTargetView(gboms.rtvStart[0], bbg, gboms.rectCount, gboms.rectStart);
	pCmdlist->ClearRenderTargetView(gboms.rtvStart[1], bbg, gboms.rectCount, gboms.rectStart);
	pCmdlist->ClearRenderTargetView(gboms.rtvStart[2], bbg, gboms.rectCount, gboms.rectStart);
	pCmdlist->ClearRenderTargetView(gboms.rtvStart[3], bbg, gboms.rectCount, gboms.rectStart);

	pCmdlist->OMSetRenderTargets(4u, gboms.rtvStart, TRUE, gboms.dsv);
	pCmdlist->RSSetScissorRects(gboms.rectCount, gboms.rectStart);
	pCmdlist->RSSetViewports(gboms.viewportCount, gboms.viewportStart);
	pCmdlist->SetGraphicsRootConstantBufferView(0u, gboms.transformsCB);
	auto ptr = bindingStart;
	for (auto i = 0u; i < bindingCount; ++i) {
		// pCmdlist->SetGraphicsRootConstantBufferView(0u, ptr->transformsCB);
		pCmdlist->SetGraphicsRootConstantBufferView(1u, ptr->materialsCB);
		pCmdlist->SetGraphicsRootDescriptorTable(2u, ptr->mapsDH);
		//pCmdlist->SetGraphicsRootShaderResourceView(2u, ptr->diffuseMap);
		//pCmdlist->SetGraphicsRootShaderResourceView(3u, ptr->normalMap);
		pCmdlist->IASetVertexBuffers(0u, ptr->vbv_count, ptr->vbvs);
		if (ptr->ibv) {
			pCmdlist->IASetIndexBuffer(ptr->ibv);
			pCmdlist->DrawIndexedInstanced(ptr->index_count, 1u, 0u, 0u, 0u);
		}
		else {
			pCmdlist->DrawInstanced(ptr->index_count, 1u, 0u, 0u);
		}
		ptr += 1;
	}
}

void DeferredRenderingRuntime::recordLightPass(ID3D12GraphicsCommandList* pCmdlist, const OMBindings& lpoms, D3D12_VERTEX_BUFFER_VIEW* maskVbv, ID3D12DescriptorHeap* gbvHeap, LightPassBindings* pointlights, unsigned int plCount /*= 1u*/, LightPassBindings* spotlights /*= nullptr*/, unsigned int slCount /*= 0u*/, LightPassBindings* directionalLights /*= nullptr*/, unsigned int dlCount /*= 0u */) const
{
	pCmdlist->SetGraphicsRootSignature(getLPRootSignature());
	static float bbg[] = { 0.f, 0.f, 0.f, 0.f };
	pCmdlist->ClearRenderTargetView(*lpoms.rtvStart, bbg, lpoms.rectCount, lpoms.rectStart);
	pCmdlist->OMSetRenderTargets(1u, lpoms.rtvStart, TRUE, nullptr);
	pCmdlist->RSSetScissorRects(lpoms.rectCount, lpoms.rectStart);
	pCmdlist->RSSetViewports(lpoms.viewportCount, lpoms.viewportStart);
	ID3D12DescriptorHeap* dhs[] = { gbvHeap };
	pCmdlist->SetDescriptorHeaps(1u, dhs);
	pCmdlist->SetGraphicsRootDescriptorTable(1u, gbvHeap->GetGPUDescriptorHandleForHeapStart());

	auto ptr = pointlights;
	pCmdlist->SetPipelineState(getPointlightPSO());
	for (auto i = 0u; i < plCount; ++i, ptr += 1) {
		pCmdlist->SetGraphicsRootConstantBufferView(0u, ptr->lights);
		pCmdlist->DrawInstanced(6u, 1u, 0u, 0u);
	}

	ptr = directionalLights;
	pCmdlist->SetPipelineState(getDirectionallightPSO());
	for (auto i = 0u; i < dlCount; ++i, ptr += 1) {
		pCmdlist->SetGraphicsRootConstantBufferView(0u, ptr->lights);
		pCmdlist->DrawInstanced(6u, 1u, 0u, 0u);
	}

	ptr = spotlights;
	pCmdlist->SetPipelineState(getSpotlightPSO());
	for (auto i = 0u; i < slCount; ++i, ptr += 1) {
		pCmdlist->SetGraphicsRootConstantBufferView(0u, ptr->lights);
		pCmdlist->DrawInstanced(6u, 1u, 0u, 0u);
	}
}

static GBInput convert(const BasicGeometry::Vertex& v) {
	GBInput r;
	r.pos = v.Pos;
	r.norm = v.Norm.AsVector4f().xyz();
	r.tangent = v.TangentU.AsVector4f().xyz();
	r.u = v.Texture._x;
	r.v = v.Texture._y;
	return r;
}

void NaiveApp::OnInit()
{
	D3D12App::OnInit();
	_mainCam = PerspecCamera::FromHFOVAndAspectRatio(
		1.f, 1000.f, 16.f / 9.f, 3.f*static_cast<float>(M_PI) / 4.f,
		Vector3DH(-1.f, -1.f, -1.f).Normalize(),
		Vector3DH(0.f, 1.f, 0.f), Point3DH(10.f, 10.f, 10.f));
	_transformCB_gpu = D3D12Helper::UpdateBuffer<TransformsCB>(_d3d12Manager.GetD3D12Device());
	_cameraCB_gpu = D3D12Helper::UpdateBuffer<CameraCB>(_d3d12Manager.GetD3D12Device());
	_lpmVB_gpu = D3D12Helper::UpdateBuffer<LightPassMaskVB>(_d3d12Manager.GetD3D12Device());
	_lpmVBV.BufferLocation = _lpmVB_gpu.Get()->GetGPUVirtualAddress();
	_lpmVBV.SizeInBytes = 6u * sizeof(Vector3f);
	_lpmVBV.StrideInBytes = sizeof(Vector3f);
	_mkTracker.pKeyboard = _keyboard.get();
	_mkTracker.pMouse = _mouse.get();

	_camControl.pCamera = &_mainCam;
	_camControl.pMKTracker = &_mkTracker;

	auto A = Vector3f(-1.f, -1.f, .5f);
	auto B = Vector3f(-1.f, 1.f, .5f);
	auto C = Vector3f(1.f, 1.f, .5f);
	auto D = Vector3f(1.f, -1.f, .5f);
	_lpmVB.poses[0] = A;
	_lpmVB.poses[1] = B;
	_lpmVB.poses[2] = C;
	_lpmVB.poses[3] = A;
	_lpmVB.poses[4] = C;
	_lpmVB.poses[5] = D;
	_lpmVB_gpu.Update(_lpmVB);

	_drr.initialize(_d3d12Manager,
		_d3d12Manager.GetMainWindowResource().GetBackBufferFormat());

	// Initialize scene components
	//auto terranMesh = BasicGeometry::Terran(10, 10, 10, 10);
	//auto terranMesh = BasicGeometry::Cylinder(5.f, 2.f, 3.f, 16, 16);
	//auto terranMesh = BasicGeometry::Box(1.f, 2.f, 3.f);
	//auto terranMesh = BasicGeometry::Sphere(5.f);
	auto terranMesh = BasicGeometry::Grid(10, 10, 10, 10);
	std::vector<GBInput> terranVertices;
	for (auto& v : terranMesh.Vertices)
	{
		terranVertices.push_back(convert(v));
	}
	_meshes_cpu.emplace_back(std::move(terranVertices),
		std::move(terranMesh.Indices),
		MaterialsCB{
			{0.3f, 0.13f, 0.63f}, 80.8f
	});

	//Microsoft::WRL::ComPtr<ID3D12Resource> diffusemap;
	//Microsoft::WRL::ComPtr<ID3D12Resource> normalmap;
	//{
	//	std::unique_ptr<uint8_t[]> ddsData;
	//	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	//	ThrowIfFailed(DirectX::LoadDDSTextureFromFile(_d3d12Manager.GetD3D12Device(),
	//		LR"(..\Asset\Textures\bricks.dds)", diffusemap.GetAddressOf(),
	//		ddsData, subresources));
	//}
	//{
	//	std::unique_ptr<uint8_t[]> ddsData;
	//	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	//	ThrowIfFailed(DirectX::LoadDDSTextureFromFile(_d3d12Manager.GetD3D12Device(),
	//		LR"(..\Asset\Textures\bricks_nmap.dds)", normalmap.GetAddressOf(),
	//		ddsData, subresources));
	//}

	Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer0;
	Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer1;
	Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer2;
	auto pDevice = _d3d12Manager.GetD3D12Device();
	auto pCmdList = _d3d12Manager.GetMainCmdList();
	auto pCmdAlloc = _d3d12Manager.GetMainCmdAllocator();
	pCmdList->Reset(pCmdAlloc, nullptr);

	Microsoft::WRL::ComPtr<ID3D12Resource> diffusemap;
	Microsoft::WRL::ComPtr<ID3D12Resource> normalmap;
	Microsoft::WRL::ComPtr<ID3D12Resource> diffusemapU;
	Microsoft::WRL::ComPtr<ID3D12Resource> normalmapU;

	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(pDevice, pCmdList,
		LR"(..\Asset\Textures\bricks2.dds)", diffusemap, diffusemapU));
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(pDevice, pCmdList,
		LR"(..\Asset\Textures\bricks2_nmap.dds)", normalmap, normalmapU));

	auto& tvCpu = std::get<0>(_meshes_cpu.back());
	auto vb_gpu = D3D12Helper::CreateDefaultBuffer(pDevice, pCmdList,
		tvCpu.data(), tvCpu.size() * sizeof(GBInput), uploadBuffer0);
	auto& tiCpu = std::get<1>(_meshes_cpu.back());
	auto ib_gpu = D3D12Helper::CreateDefaultBuffer(pDevice, pCmdList,
		tiCpu.data(), tiCpu.size() * sizeof(UINT), uploadBuffer1);
	auto& tmCpu = std::get<2>(_meshes_cpu.back());
	auto mb_gpu = D3D12Helper::CreateDefaultBuffer(pDevice, pCmdList,
		&tmCpu, D3D12Helper::GetCBSizeAligned(sizeof(tmCpu)), uploadBuffer2);


	// lights initialization
	Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer3;
	Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer4;
	Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer5;
	_pointLights.push_back({ {}, {} });
	auto& pb = _pointLights.back();
	// pb.first.color = Vector4f(0.f, 0.f, 4.f, 1.0f);
	pb.first.color = Vector4f(1.7f, 1.7f, 1.7f, 1.0f);
	// pb.first.color = Vector4f(0.f, 0.f, 0.f, 1.0f);
	pb.first.pos = Vector4f(-5.f, 5.f, 5.f, 1.f);
	pb.first.range = Vector4f(20.f, 0.f, 0.f, 0.f);
	auto pl_gpu = D3D12Helper::CreateDefaultBuffer(
		pDevice, pCmdList, &pb.first, D3D12Helper::GetCBSizeAligned(sizeof(pb.first)),
		uploadBuffer3);
	_spotLights.push_back({ {},{} });
	auto& sb = _spotLights.back();
	//sb.first.color = Vector4f(2.5f, 0.f, 0.f, 1.0f);
	sb.first.color = Vector4f(1.7f, 1.7f, 1.7f, 1.0f);
	// sb.first.color = Vector4f(0.f, 0.f, 0.f, 1.0f);
	sb.first.pos = Vector4f(5.f, 5.f, 5.f, 1.f);
	sb.first.direction = Vector4f(-1.f, -1.f, -1.f, 0.f).Normalize();
	sb.first.range = Vector4f(10.f, 0.f, 0.f, 0.f);
	sb.first.angle[0] = 0.7f;
	sb.first.angle[1] = 0.3f;
	auto sl_gpu = D3D12Helper::CreateDefaultBuffer(
		pDevice, pCmdList, &sb.first, D3D12Helper::GetCBSizeAligned(sizeof(sb.first)),
		uploadBuffer4);

	_directionalLights.push_back({ {},{} });
	auto& db = _directionalLights.back();
	db.first.color = Vector4f(1.7f, 1.7f, 1.7f, 1.0f);
	// sb.first.pos = Vector4f(5.f, 5.f, 5.f, 1.f);
	db.first.direction = Vector4f(-.8f, -1.f, -1.2f, 0.f).Normalize();
	// sb.first.range = Vector4f(15.f, 0.f, 0.f, 0.f);
	// sb.first.angle[0] = 0.7f;
	// sb.first.angle[1] = 0.3f;
	auto dl_gpu = D3D12Helper::CreateDefaultBuffer(
		pDevice, pCmdList, &db.first, D3D12Helper::GetCBSizeAligned(sizeof(db.first)),
		uploadBuffer5);

	ID3D12CommandQueue* pCmdQueue = _d3d12Manager.GetCmdQueue();
	ThrowIfFailed(pCmdList->Close());
	ID3D12CommandList* lists[] = { pCmdList };
	pCmdQueue->ExecuteCommandLists(1u, lists);
	pCmdQueue->Signal(_d3d12Manager.GetMainFence(), _d3d12Manager.GetMainFenceCount() + 1);
	_d3d12Manager.FlushCommandQueue();
	ComPtr<ID3D12DescriptorHeap> mdHeap;
	pDevice->CreateDescriptorHeap(
		&D3D12Helper::DescriptorHeapDescriptor(
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 2u,
			D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE), IID_PPV_ARGS(mdHeap.GetAddressOf()));

	_d3d12Manager.CreateSRVOnHeap(diffusemap.Get(), nullptr, mdHeap.Get(), 0u);
	_d3d12Manager.CreateSRVOnHeap(normalmap.Get(), nullptr, mdHeap.Get(), 1u);
	_meshes_gpu.emplace_back(vb_gpu, ib_gpu, mb_gpu, diffusemap, normalmap, mdHeap);
	pb.second = pl_gpu;
	sb.second = sl_gpu;
	db.second = dl_gpu;

	// g buffers initialization
	D3D12_CLEAR_VALUE rtvcv = {};
	rtvcv.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	_gBuffers[0] = _d3d12Manager.AddTexture2D(D3D12Helper::ResourceHeapProperties(
		D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
		D3D12Helper::ResourceDescriptor::Tex2D(_width, _height,
			DXGI_FORMAT_R32G32B32A32_FLOAT, 1u, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET),
		D3D12_RESOURCE_STATE_COMMON, &rtvcv);
	_gBuffers[1] = _d3d12Manager.AddTexture2D(D3D12Helper::ResourceHeapProperties(
		D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
		D3D12Helper::ResourceDescriptor::Tex2D(_width, _height,
			DXGI_FORMAT_R32G32B32A32_FLOAT, 1u, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET),
		D3D12_RESOURCE_STATE_COMMON, &rtvcv);
	_gBuffers[2] = _d3d12Manager.AddTexture2D(D3D12Helper::ResourceHeapProperties(
		D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
		D3D12Helper::ResourceDescriptor::Tex2D(_width, _height,
			DXGI_FORMAT_R32G32B32A32_FLOAT, 1u, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET),
		D3D12_RESOURCE_STATE_COMMON, &rtvcv);
	_gBuffers[3] = _d3d12Manager.AddTexture2D(D3D12Helper::ResourceHeapProperties(
		D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
		D3D12Helper::ResourceDescriptor::Tex2D(_width, _height,
			DXGI_FORMAT_R32G32B32A32_FLOAT, 1u, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET),
		D3D12_RESOURCE_STATE_COMMON, &rtvcv);

	rtvcv.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	rtvcv.DepthStencil.Depth = 1.f;
	rtvcv.DepthStencil.Stencil = 0x0;
	_gDepthStencil = _d3d12Manager.AddTexture2D(
		D3D12Helper::ResourceHeapProperties(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		D3D12Helper::ResourceDescriptor::Tex2D(_width, _height,
			DXGI_FORMAT_D32_FLOAT_S8X24_UINT, 1u, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
		D3D12_RESOURCE_STATE_DEPTH_WRITE, &rtvcv);

	//_gbDH = _d3d12Manager.AddDescriptorHeap(
	//	D3D12Helper::DescriptorHeapDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
	//		2u, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE));

	//_d3d12Manager.CreateSRVOnHeap(diffusemap.Get(), nullptr,
	//	_d3d12Manager.FindDescriptorHeap(_gbDH).Get(), 0u);
	//_d3d12Manager.CreateSRVOnHeap(normalmap.Get(), nullptr,
	//	_d3d12Manager.FindDescriptorHeap(_gbDH).Get(), 1u);

	_lightPassDH = _d3d12Manager.AddDescriptorHeap(
		D3D12Helper::DescriptorHeapDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
			5u, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE));

	auto pLpDH = _d3d12Manager.FindDescriptorHeap(_lightPassDH).Get();
	D3D12Helper::CBVDescriptor cameraCBDesc(_cameraCB_gpu.Get()->GetGPUVirtualAddress(), _cameraCB_gpu.Size());
	_d3d12Manager.CreateCBVOnHeap(&cameraCBDesc, pLpDH, 0u);
	_dsvDH = _d3d12Manager.AddDescriptorHeap(D3D12Helper::DescriptorHeapDescriptor(
		D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1u, D3D12_DESCRIPTOR_HEAP_FLAG_NONE));
	_d3d12Manager.CreateDSVOnHeap(_d3d12Manager.FindTexture2D(_gDepthStencil).Get(),
		nullptr, _d3d12Manager.FindDescriptorHeap(_dsvDH).Get(), 0u);


	_rtvDH = _d3d12Manager.AddDescriptorHeap(D3D12Helper::DescriptorHeapDescriptor(
		D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 4u));
	auto rtvHandle = _d3d12Manager.FindDescriptorHeap(_rtvDH).Get();
	//auto gbufferDesc = D3D12Helper::SRVDescriptor::Texture2DDesc(
	//	DXGI_FORMAT_UNKNOWN, 0u, 0u);
	// auto gbufferRTVD = D3D12Helper::RTVDescriptor::Texture2DDesc()
	for (auto i = 0u; i < 4; ++i) {
		auto resrc = _d3d12Manager.FindTexture2D(_gBuffers[i]).Get();
		_d3d12Manager.CreateSRVOnHeap(resrc, nullptr, pLpDH, i + 1u);
		_d3d12Manager.CreateRTVOnHeap(resrc, nullptr, rtvHandle, i);
	}

}

void NaiveApp::OnDestroy()
{
	_d3d12Manager.FlushCommandQueue();
	D3D12App::OnDestroy();
}

bool NaiveApp::OnEvent(MSG msg)
{
	return D3D12App::OnEvent(msg);
}

void NaiveApp::OnUpdate()
{
	static auto lastTick = _mainTimer.PeekCurrentTick();
	auto cTick = _mainTimer.PeekCurrentTick();
	auto deltaMs = _mainTimer.TicksToMs(cTick - lastTick);
	lastTick = cTick;
	_camControl.Update(deltaMs);
	_d3d12Manager.FlushCommandQueue();
	_kbdst.Update(_keyboard->GetState());
	if (_kbdst.IsKeyReleased(DirectX::Keyboard::Enter)) {
		ReadBackTo(_d3d12Manager.FindTexture2D(_gBuffers[0]).Get(), L"./gb0.png");
		ReadBackTo(_d3d12Manager.FindTexture2D(_gBuffers[1]).Get(), L"./gb1.png");
		ReadBackTo(_d3d12Manager.FindTexture2D(_gBuffers[2]).Get(), L"./gb2.png");
		ReadBackTo(_d3d12Manager.FindTexture2D(_gBuffers[3]).Get(), L"./gb3.png");
	}
	// _transformCB_cpu.mWorld = _mainCam.TransformWtoO().AsMatrix4x4();
	_transformCB_cpu.mWorld = Matrix4x4f::Identity();
	_transformCB_cpu.mWorldView = _mainCam.TransformWtoV().AsMatrix4x4();
	_transformCB_cpu.mWorldViewProj = _mainCam.TransformWtoH().AsMatrix4x4();
	_transformCB_gpu.Update(_transformCB_cpu);
	_cameraCB.pos = _mainCam.GetPosition().AsVector4f().xyz();
	_cameraCB_gpu.Update(_cameraCB);

}

void NaiveApp::OnRender()
{

	_d3d12Manager.FlushCommandQueue();
	D3D12App::LogFPSToTitle();
	ID3D12GraphicsCommandList* pCmdlist;

	static size_t frameCount = 0;
	static size_t backbufferCount = _d3d12Manager.GetMainWindowResource().GetSwapChainBufferCount();
	if (frameCount < backbufferCount) {
		auto caid = _d3d12Manager.AddCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT);
		auto pca = _d3d12Manager.FindCommandAllocator(caid).Get();
		auto clid = _d3d12Manager.AddCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT, pca);
		auto pcl = _d3d12Manager.FindCommandList(clid).Get();
		RecordCmds(pcl, pca);
		_cmdObjs.emplace_back(pcl, pca);
		pCmdlist = pcl;
		frameCount++;
	}
	else if (frameCount < 2 * backbufferCount) {
		pCmdlist = _cmdObjs[frameCount - backbufferCount].first;
		frameCount++;
		if (frameCount == 2 * backbufferCount) {
			frameCount = backbufferCount;
		}
	}



	ID3D12CommandList* lists[] = { pCmdlist };
	_d3d12Manager.GetCmdQueue()->ExecuteCommandLists(1u, lists);
	_d3d12Manager.GetCmdQueue()->Signal(_d3d12Manager.GetMainFence(),
		_d3d12Manager.GetAdvancedMainFenceCount());
	_d3d12Manager.GetMainWindowResource().GetSwapChain()->Present(0u, 0u);
	_d3d12Manager.GetMainWindowResource().AdvanceBackBufferIndex();

}

void NaiveApp::RecordCmds(ID3D12GraphicsCommandList* pCmdlist, ID3D12CommandAllocator* pCmdAlloc)
{
	static float bbg[] = { 0.f, 0.f, 0.f, 0.f };
	// ThrowIfFailed(pCmdAlloc->Reset());
	// ThrowIfFailed(pCmdlist->Reset(pCmdAlloc, nullptr));
#pragma region recordGBPass
	pCmdlist->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pCmdlist->SetPipelineState(_drr.getGBNormalPSO());
	pCmdlist->SetGraphicsRootSignature(_drr.getGBRootSignature());

	ID3D12DescriptorHeap* dsvHeap = _d3d12Manager.FindDescriptorHeap(_dsvDH).Get();
	D3D12WindowResource& windowResrc = _d3d12Manager.GetMainWindowResource();
	auto sRect = &windowResrc.GetMainScissor();
	pCmdlist->RSSetScissorRects(1u, sRect);
	pCmdlist->RSSetViewports(1u, &windowResrc.GetMainViewport());
	pCmdlist->ClearDepthStencilView(dsvHeap->GetCPUDescriptorHandleForHeapStart(),
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
		1.f, static_cast<UINT8>(0), 1u, sRect);

	D3D12Helper::ResourceBarrier barriers[] = {
		D3D12Helper::ResourceBarrier::TransitionBarrier(
			_d3d12Manager.FindTexture2D(_gBuffers[0]).Get(),
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE/* | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE*/,
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_BARRIER_FLAG_NONE
		),
		D3D12Helper::ResourceBarrier::TransitionBarrier(
			_d3d12Manager.FindTexture2D(_gBuffers[1]).Get(),
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE/* | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE*/,
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_BARRIER_FLAG_NONE
		),
		D3D12Helper::ResourceBarrier::TransitionBarrier(
			_d3d12Manager.FindTexture2D(_gBuffers[2]).Get(),
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE/* | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE*/,
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_BARRIER_FLAG_NONE
		),
		D3D12Helper::ResourceBarrier::TransitionBarrier(
			_d3d12Manager.FindTexture2D(_gBuffers[3]).Get(),
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE/* | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE*/,
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_BARRIER_FLAG_NONE
		),
	};
	pCmdlist->ResourceBarrier(4u, barriers);


	auto rtvHandle = D3D12Helper::DescriptorHandleCPU(
		_d3d12Manager.FindDescriptorHeap(_rtvDH)->GetCPUDescriptorHandleForHeapStart());

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[4] = {};
	pCmdlist->ClearRenderTargetView(rtvHandle, bbg, 1u, sRect);
	rtvHandles[0] = rtvHandle;
	pCmdlist->ClearRenderTargetView(
		rtvHandle.Offset(_d3d12Manager.GetRTVDescriptorSize()), bbg, 1u, sRect);
	rtvHandles[1] = rtvHandle;
	pCmdlist->ClearRenderTargetView(
		rtvHandle.Offset(_d3d12Manager.GetRTVDescriptorSize()), bbg, 1u, sRect);
	rtvHandles[2] = rtvHandle;
	pCmdlist->ClearRenderTargetView(
		rtvHandle.Offset(_d3d12Manager.GetRTVDescriptorSize()), bbg, 1u, sRect);
	rtvHandles[3] = rtvHandle;
	pCmdlist->OMSetRenderTargets(4u, rtvHandles, TRUE,
		&dsvHeap->GetCPUDescriptorHandleForHeapStart());

	pCmdlist->SetGraphicsRootConstantBufferView(0u,
		_transformCB_gpu.Get()->GetGPUVirtualAddress());
	for (auto i = 0u; i < _meshes_gpu.size(); ++i) {
		auto& mesh = _meshes_gpu[i];
		auto& mesh_cpu = _meshes_cpu[i];
		pCmdlist->SetGraphicsRootConstantBufferView(
			1u, std::get<2>(mesh)->GetGPUVirtualAddress());
		ID3D12DescriptorHeap* dhs[] = { std::get<5>(mesh).Get() };
		pCmdlist->SetDescriptorHeaps(1u, dhs);
		pCmdlist->SetGraphicsRootDescriptorTable(
			2u, dhs[0]->GetGPUDescriptorHandleForHeapStart());
		D3D12_VERTEX_BUFFER_VIEW vbv;
		vbv.BufferLocation = std::get<0>(mesh)->GetGPUVirtualAddress();
		vbv.SizeInBytes = std::get<0>(mesh_cpu).size() * sizeof(GBInput);
		vbv.StrideInBytes = sizeof(GBInput);
		pCmdlist->IASetVertexBuffers(0u, 1u, &vbv);
		D3D12_INDEX_BUFFER_VIEW ibv;
		ibv.BufferLocation = std::get<1>(mesh)->GetGPUVirtualAddress();
		ibv.SizeInBytes = std::get<1>(mesh_cpu).size() * sizeof(UINT);
		ibv.Format = DXGI_FORMAT_R32_UINT;
		pCmdlist->IASetIndexBuffer(&ibv);
		pCmdlist->DrawIndexedInstanced(std::get<1>(mesh_cpu).size(), 1u, 0u, 0u, 0u);
	}
#pragma endregion recordGBPass

#pragma region recordLightPass
	D3D12Helper::ResourceBarrier abarriers[] = {
		D3D12Helper::ResourceBarrier::TransitionBarrier(
			_d3d12Manager.FindTexture2D(_gBuffers[0]).Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE/* | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE*/,
			D3D12_RESOURCE_BARRIER_FLAG_NONE
		),
		D3D12Helper::ResourceBarrier::TransitionBarrier(
			_d3d12Manager.FindTexture2D(_gBuffers[1]).Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE/* | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE*/,
			D3D12_RESOURCE_BARRIER_FLAG_NONE
		),
		D3D12Helper::ResourceBarrier::TransitionBarrier(
			_d3d12Manager.FindTexture2D(_gBuffers[2]).Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE/* | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE*/,
			D3D12_RESOURCE_BARRIER_FLAG_NONE
		),
		D3D12Helper::ResourceBarrier::TransitionBarrier(
			_d3d12Manager.FindTexture2D(_gBuffers[3]).Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE/* | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE*/,
			D3D12_RESOURCE_BARRIER_FLAG_NONE
		),
		D3D12Helper::ResourceBarrier::TransitionBarrier(
			windowResrc.GetCurrentBackBufferResource(),
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_BARRIER_FLAG_NONE
		),
	};
	pCmdlist->ResourceBarrier(5u, abarriers);

	pCmdlist->ClearState(nullptr);
	pCmdlist->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// ID3D12GraphicsCommandList* a;
	pCmdlist->IASetVertexBuffers(0u, 1u, &_lpmVBV);
	pCmdlist->SetGraphicsRootSignature(_drr.getLPRootSignature());
	pCmdlist->OMSetRenderTargets(1u, &windowResrc.GetCurrentCPURTV(), TRUE, nullptr);
	pCmdlist->ClearRenderTargetView(windowResrc.GetCurrentCPURTV(), bbg, 1u, sRect);

	pCmdlist->RSSetScissorRects(1u, sRect);
	pCmdlist->RSSetViewports(1u, &_d3d12Manager.GetMainWindowResource().GetMainViewport());
	ID3D12DescriptorHeap* dhs[] = { _d3d12Manager.FindDescriptorHeap(_lightPassDH).Get() };
	pCmdlist->SetDescriptorHeaps(1u, dhs);
	pCmdlist->SetGraphicsRootDescriptorTable(1u, dhs[0]->GetGPUDescriptorHandleForHeapStart());

	pCmdlist->SetPipelineState(_drr.getPointlightPSO());
	for (auto i = 0u; i < _pointLights.size(); ++i) {
		auto& light = _pointLights[i];
		pCmdlist->SetGraphicsRootConstantBufferView(0u, light.second->GetGPUVirtualAddress());
		pCmdlist->DrawInstanced(6u, 1u, 0u, 0u);
	}

	pCmdlist->SetPipelineState(_drr.getDirectionallightPSO());
	for (auto i = 0u; i < _directionalLights.size(); ++i) {
		auto& light = _directionalLights[i];
		pCmdlist->SetGraphicsRootConstantBufferView(0u, light.second->GetGPUVirtualAddress());
		pCmdlist->DrawInstanced(6u, 1u, 0u, 0u);
	}

	pCmdlist->SetPipelineState(_drr.getSpotlightPSO());
	for (auto i = 0u; i < _spotLights.size(); ++i) {
		auto& light = _spotLights[i];
		pCmdlist->SetGraphicsRootConstantBufferView(0u, light.second->GetGPUVirtualAddress());
		pCmdlist->DrawInstanced(6u, 1u, 0u, 0u);
	}

	D3D12_RESOURCE_BARRIER lbarrier[] = {
		D3D12Helper::ResourceBarrier::TransitionBarrier(
			windowResrc.GetCurrentBackBufferResource(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_BARRIER_FLAG_NONE
		),
	};
	pCmdlist->ResourceBarrier(1u, lbarrier);
	ThrowIfFailed(pCmdlist->Close());
#pragma endregion recordLightPass
}

void NaiveApp::ReadBackTo(ID3D12Resource* src, const wchar_t* filename)
{
	auto srcDesc = src->GetDesc();
	assert(srcDesc.Format == DXGI_FORMAT_R32G32B32A32_FLOAT);
	ID3D12GraphicsCommandList* cmdList = _d3d12Manager.GetMainCmdList();
	ID3D12CommandAllocator* cmdAlloc = _d3d12Manager.GetMainCmdAllocator();
	ID3D12CommandQueue* cmdQueue = _d3d12Manager.GetCmdQueue();
	ID3D12Device* device = _d3d12Manager.GetD3D12Device();

	ComPtr<ID3D12Resource> readbackHeap;
	auto heapProperties = D3D12Helper::ResourceHeapProperties(D3D12_HEAP_TYPE_READBACK);
	auto resDesc = D3D12Helper::ResourceDescriptor::Buffer(srcDesc.Width*srcDesc.Height * sizeof(float) * 4);
	device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE,
		&resDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(readbackHeap.GetAddressOf()));

	D3D12_TEXTURE_COPY_LOCATION dstl = {};
	dstl.pResource = readbackHeap.Get();
	dstl.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	dstl.PlacedFootprint.Footprint.Depth = 1;
	dstl.PlacedFootprint.Footprint.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	dstl.PlacedFootprint.Footprint.Width = static_cast<UINT>(srcDesc.Width);
	dstl.PlacedFootprint.Footprint.Height = static_cast<UINT>(srcDesc.Height);
	dstl.PlacedFootprint.Footprint.RowPitch = static_cast<UINT>(srcDesc.Width * sizeof(float) * 4);

	D3D12_TEXTURE_COPY_LOCATION srcl = {};
	srcl.pResource = src;
	srcl.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	srcl.SubresourceIndex = 0u;

	ThrowIfFailed(cmdAlloc->Reset());
	ThrowIfFailed(cmdList->Reset(cmdAlloc, nullptr));
	D3D12Helper::ResourceBarrier barriers[] = {
		D3D12Helper::ResourceBarrier::TransitionBarrier(
			src,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			D3D12_RESOURCE_STATE_COPY_SOURCE,
			D3D12_RESOURCE_BARRIER_FLAG_NONE
		)
	};
	cmdList->ResourceBarrier(1u, barriers);
	cmdList->CopyTextureRegion(&dstl, 0, 0, 0, &srcl, nullptr);
	D3D12Helper::ResourceBarrier abarriers[] = {
		D3D12Helper::ResourceBarrier::TransitionBarrier(
			src,
			D3D12_RESOURCE_STATE_COPY_SOURCE,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			D3D12_RESOURCE_BARRIER_FLAG_NONE
		)
	};
	cmdList->ResourceBarrier(1u, abarriers);
	ThrowIfFailed(cmdList->Close());
	ID3D12CommandList* cmdlsts[] = { cmdList };
	cmdQueue->ExecuteCommandLists(_countof(cmdlsts), cmdlsts);

	_d3d12Manager.FlushCommandQueue();

	Luxko::Anuthur::SaveTexture2DAsPNG(readbackHeap.Get(), 
		static_cast<unsigned int>(srcDesc.Width), 
		static_cast<unsigned int>(srcDesc.Height), filename);
}
