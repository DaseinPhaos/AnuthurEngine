//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************
#include "NaiveQDM.h"
#include "BasicGeometry.h"
#include "WICReadback.h"
using GBVSI = WTF::NBPQDM::VSI;
static GBVSI convert(const BasicGeometry::Vertex& v) {
	GBVSI r;
	r.posO = v.Pos.AsVector4f();
	r.normal = v.Norm.AsVector4f().xyz();
	r.tangent = v.TangentU.AsVector4f().xyz();
	r.u = v.Texture._x;
	r.v = v.Texture._y;
	return r;
}
void NaiveQDMApp::OnInit()
{
	D3D12App::OnInit();
	auto pDevice = _d3d12Manager.GetD3D12Device();
	auto pCmdList = _d3d12Manager.GetMainCmdList();
	auto pCmdAlloc = _d3d12Manager.GetMainCmdAllocator();
	ThrowIfFailed(pCmdList->Reset(pCmdAlloc, nullptr));
	WTF::NaiveBpqdmGenerator::getInputBuffer(pDevice, pCmdList);
	_mainCam = PerspecCamera::FromHFOVAndAspectRatio(
		1.f, 1000.f, 16.f / 9.f, 3.f*static_cast<float>(M_PI) / 4.f,
		Vector3DH(-1.f, -1.f, -1.f).Normalize(),
		Vector3DH(0.f, 1.f, 0.f), Point3DH(10.f, 10.f, 10.f));
	_cameraAttr_gpu = decltype(_cameraAttr_gpu)(pDevice);

	_mkTracker.pMouse = _mouse.get();
	_mkTracker.pKeyboard = _keyboard.get();

	_camControl.pCamera = &_mainCam;
	_camControl.pMKTracker = &_mkTracker;

	_movingLight_control.pMKTracker = &_mkTracker;
	_movingLight_control.pLight = &_movingLight_cpu;
	_movingLight_cpu.posW = Vector4f(0.f, 5.f, 0.f, 1.f);
	_movingLight_cpu.color = Vector4f(1.f, 1.f, 1.f, 1.f);
	_movingLight_cpu.range = Vector4f(6.6f, 0.f, 0.f, 0.f);

	_movingLight_gpu = decltype(_movingLight_gpu)(pDevice);
	//auto tptr = _movingLight_gpu.Get();

	_qdmGenerator.initialize(pDevice);
	_qdm.initialize(pDevice);
	auto rtvFormat = _d3d12Manager.GetMainWindowResource().GetBackBufferFormat();
	auto dsvFormat = _d3d12Manager.GetMainWindowResource().GetDepthStencilFormat();
	_pointLight.initialize(pDevice, rtvFormat, dsvFormat);
	_spotLight.initialize(pDevice, rtvFormat, dsvFormat);
	_directionalLight.initialize(pDevice, rtvFormat, dsvFormat);

	_pointLight.getInputBuffer(pDevice, pCmdList);
	_spotLight.getInputBuffer(pDevice, pCmdList);
	_directionalLight.getInputBuffer(pDevice, pCmdList);

	// Initialize scene components
	//auto terranMesh = BasicGeometry::Terran(10, 10, 10, 10);
	//auto terranMesh = BasicGeometry::Cylinder(5.f, 2.f, 3.f, 16, 16);
	//auto terranMesh = BasicGeometry::Box(4.f, 5.f, 6.f);
	//auto terranMesh = BasicGeometry::Sphere(5.f);
	auto terranMesh = BasicGeometry::Grid(10, 10, 10, 10);
	std::vector<GBInput> terranVertices;
	for (auto& v : terranMesh.Vertices)
	{
		terranVertices.push_back(convert(v));
	}

	RenderObject terranObject;
	terranObject.mesh.InitializeCPUResource(DXGI_FORMAT_R32_UINT,
		static_cast<UINT>(terranMesh.Indices.size()),
		terranMesh.Indices.data(),
		static_cast<UINT>(sizeof(GBInput)),
		static_cast<UINT>(terranVertices.size()),
		terranVertices.data());
	terranObject.mesh.RecordUpdateFromCPUtoGPU(
		pDevice, pCmdList);

	Microsoft::WRL::ComPtr<ID3D12Resource> diffusemap;
	Microsoft::WRL::ComPtr<ID3D12Resource> normalmap;
	Microsoft::WRL::ComPtr<ID3D12Resource> diffusemapU;
	Microsoft::WRL::ComPtr<ID3D12Resource> normalmapU;

	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(pDevice, pCmdList,
		LR"(..\Asset\Textures\tile.dds)", diffusemap, diffusemapU));
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(pDevice, pCmdList,
		LR"(..\Asset\Textures\tile_nmap.dds)", normalmap, normalmapU));

	auto p = _qdmGenerator.generateQDM(normalmap.Get(), pDevice, pCmdList);
	ID3D12CommandQueue* pCmdQueue = _d3d12Manager.GetCmdQueue();
	ThrowIfFailed(pCmdList->Close());
	ID3D12CommandList* lists[] = { pCmdList };
	pCmdQueue->ExecuteCommandLists(1u, lists);
	//pCmdQueue->Signal(_d3d12Manager.GetMainFence(), _d3d12Manager.GetMainFenceCount() + 1);
	_d3d12Manager.FlushCommandQueue();
	ThrowIfFailed(pCmdList->Reset(pCmdAlloc, nullptr));
	Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer2;
	auto tmCpu = MaterialsCB{
		Matrix4x4f::Identity(),
		Vector3f(0.1f, 0.2f, 0.1f),
		10.f, p.first - 1
	};
	auto mb_gpu = D3D12Helper::CreateDefaultBuffer(pDevice, pCmdList,
		&tmCpu, D3D12Helper::GetCBSizeAligned(sizeof(tmCpu)), uploadBuffer2);


	// lights initialization
	Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer3;
	Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer4;
	Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer5;

	_pointLights.push_back({ _movingLight_cpu,
		_movingLight_gpu.Get() });
	_pointLights.push_back({ {},{} });
	auto& pb = _pointLights.back();
	pb.first.mOtoW = Matrix4x4f::Identity();
	// pb.first.color = Vector4f(0.f, 0.f, 4.f, 1.0f);
	pb.first.color = Vector4f(0.8f, 0.8f, 0.7f, 1.0f);
	// pb.first.color = Vector4f(0.f, 0.f, 0.f, 1.0f);
	pb.first.posW = Vector4f(-5.f, 5.f, 5.f, 1.f);
	pb.first.range = Vector4f(10.f, 0.f, 0.f, 0.f);
	DRP::LightPass::NaiveLights::PointLight::generateOtoWMatrix(pb.first);
	auto pl_gpu = D3D12Helper::CreateDefaultBuffer(
		pDevice, pCmdList, &pb.first, D3D12Helper::GetCBSizeAligned(sizeof(pb.first)),
		uploadBuffer3);

	_spotLights.push_back({ {},{} });
	auto& sb = _spotLights.back();
	// sb.first.mOtoW = Matrix4x4f::Identity();
	//sb.first.color = Vector4f(2.5f, 0.f, 0.f, 1.0f);
	sb.first.color = Vector4f(0.7f, 0.7f, 0.9f, 1.0f);
	// sb.first.color = Vector4f(0.f, 0.f, 0.f, 1.0f);
	sb.first.posW = Vector4f(5.f, 5.f, 5.f, 1.f);
	sb.first.direction = Vector4f(-1.f, -1.f, -1.f, 0.f).Normalize();
	sb.first.range = Vector4f(8.f, 0.f, 0.f, 0.f);
	sb.first.angleX = 0.9f;
	sb.first.angleY = 0.7f;
	DRP::LightPass::NaiveLights::SpotLight::generateOtoWMatrix(sb.first);
	auto sl_gpu = D3D12Helper::CreateDefaultBuffer(
		pDevice, pCmdList, &sb.first, D3D12Helper::GetCBSizeAligned(sizeof(sb.first)),
		uploadBuffer4);

	_directionalLights.push_back({ {},{} });
	auto& db = _directionalLights.back();
	db.first.mOtoW = Matrix4x4f::Identity();
	db.first.color = Vector4f(0.6f, 0.6f, 0.6f, 1.0f);
	db.first.posW = Vector4f(5.f, 5.f, 5.f, 1.f);
	db.first.range = Vector4f(1000.f, 5.f, 5.f, 1.f);
	db.first.direction = Vector4f(5.f, -5.f, 5.f, 0.f).Normalize();
	DRP::LightPass::NaiveLights::DirectionalLight::generateOtoWMatrix(db.first);
	auto dl_gpu = D3D12Helper::CreateDefaultBuffer(
		pDevice, pCmdList, &db.first, D3D12Helper::GetCBSizeAligned(sizeof(db.first)),
		uploadBuffer5);

	// ID3D12CommandQueue* pCmdQueue = _d3d12Manager.GetCmdQueue();
	ThrowIfFailed(pCmdList->Close());
	//ID3D12CommandList* lists[] = { pCmdList };
	pCmdQueue->ExecuteCommandLists(1u, lists);
	//ThrowIfFailed(pCmdQueue->Signal(_d3d12Manager.GetMainFence(),
	//	_d3d12Manager.GetMainFenceCount() + 1));
	_d3d12Manager.FlushCommandQueue();
	//ThrowIfFailed(pCmdAlloc->Reset());
	//ThrowIfFailed(pCmdList->Reset(pCmdAlloc, nullptr));
	ComPtr<ID3D12DescriptorHeap> mdHeap;
	pDevice->CreateDescriptorHeap(
		&D3D12Helper::DescriptorHeapDescriptor(
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 3u,
			D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE), IID_PPV_ARGS(mdHeap.GetAddressOf()));

	_d3d12Manager.CreateSRVOnHeap(diffusemap.Get(), nullptr, mdHeap.Get(), 0u);
	_d3d12Manager.CreateSRVOnHeap(normalmap.Get(), nullptr, mdHeap.Get(), 1u);
	_d3d12Manager.CreateSRVOnHeap(p.second.Get(), nullptr, mdHeap.Get(), 2u);
	terranObject.materialCB = mb_gpu;
	terranObject.diffuseMap = diffusemap;
	terranObject.normalMap = normalmap;
	terranObject.qdMap = p.second;
	terranObject.srvHeap = mdHeap;
	_renderObjects.push_back(terranObject);
	//_meshes_gpu.emplace_back(vb_gpu, ib_gpu, mb_gpu, diffusemap, normalmap, mdHeap);
	pb.second = pl_gpu;
	sb.second = sl_gpu;
	db.second = dl_gpu;

	// g buffers initialization
	D3D12_CLEAR_VALUE rtvcv = {};
	rtvcv.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	_gBuffers[0] = _d3d12Manager.AddTexture2D(D3D12Helper::ResourceHeapProperties(
		D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
		D3D12Helper::ResourceDescriptor::Tex2D(_width, _height,
			rtvcv.Format, 1u, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET),
		D3D12_RESOURCE_STATE_COMMON, &rtvcv);

	rtvcv.Format = DXGI_FORMAT_R10G10B10A2_UNORM;
	_gBuffers[1] = _d3d12Manager.AddTexture2D(D3D12Helper::ResourceHeapProperties(
		D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
		D3D12Helper::ResourceDescriptor::Tex2D(_width, _height,
			rtvcv.Format, 1u, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET),
		D3D12_RESOURCE_STATE_COMMON, &rtvcv);

	_gBuffers[2] = _d3d12Manager.AddTexture2D(D3D12Helper::ResourceHeapProperties(
		D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
		D3D12Helper::ResourceDescriptor::Tex2D(_width, _height,
			rtvcv.Format, 1u, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET),
		D3D12_RESOURCE_STATE_COMMON, &rtvcv);


	rtvcv.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	rtvcv.DepthStencil.Depth = 1.f;
	rtvcv.DepthStencil.Stencil = 0x0;
	_gDepthStencil = _d3d12Manager.AddTexture2D(
		D3D12Helper::ResourceHeapProperties(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		D3D12Helper::ResourceDescriptor::Tex2D(_width, _height,
			rtvcv.Format, 1u, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
		D3D12_RESOURCE_STATE_DEPTH_WRITE, &rtvcv);


	_lightPassDH = _d3d12Manager.AddDescriptorHeap(
		D3D12Helper::DescriptorHeapDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
			4u, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE));

	auto pLpDH = _d3d12Manager.FindDescriptorHeap(_lightPassDH).Get();
	D3D12Helper::CBVDescriptor cameraCBDesc(
		_cameraAttr_gpu.Get()->GetGPUVirtualAddress(), _cameraAttr_gpu.Size());
	_d3d12Manager.CreateCBVOnHeap(&cameraCBDesc, pLpDH, 0u);
	_dsvDH = _d3d12Manager.AddDescriptorHeap(D3D12Helper::DescriptorHeapDescriptor(
		D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1u, D3D12_DESCRIPTOR_HEAP_FLAG_NONE));
	_d3d12Manager.CreateDSVOnHeap(_d3d12Manager.FindTexture2D(_gDepthStencil).Get(),
		nullptr, _d3d12Manager.FindDescriptorHeap(_dsvDH).Get(), 0u);


	_rtvDH = _d3d12Manager.AddDescriptorHeap(D3D12Helper::DescriptorHeapDescriptor(
		D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 3u));
	auto rtvHandle = _d3d12Manager.FindDescriptorHeap(_rtvDH).Get();
	for (auto i = 0u; i < 3u; ++i) {
		auto resrc = _d3d12Manager.FindTexture2D(_gBuffers[i]).Get();
		_d3d12Manager.CreateSRVOnHeap(resrc, nullptr, pLpDH, i + 1u);
		_d3d12Manager.CreateRTVOnHeap(resrc, nullptr, rtvHandle, i);
	}

	_cameraAttr_cpu.farClipD = _mainCam.FarPlaneDistance();
	_cameraAttr_cpu.mVtoW = _mainCam.TransformWtoV().Inverse().AsMatrix4x4();
	_cameraAttr_cpu.mWtoH = _mainCam.TransformWtoH().AsMatrix4x4();
	_cameraAttr_cpu.mWtoV = _mainCam.TransformWtoV().AsMatrix4x4();
	_cameraAttr_cpu.posW = _mainCam.GetPosition().AsVector4f().xyz();
	_cameraAttr_gpu.Update(_cameraAttr_cpu);
	DRP::LightPass::NaiveLights::PointLight::generateOtoWMatrix(_movingLight_cpu);
	_movingLight_gpu.Update(_movingLight_cpu);
}

void NaiveQDMApp::OnDestroy()
{
	_d3d12Manager.FlushCommandQueue();
	D3D12App::OnDestroy();
}

void NaiveQDMApp::OnUpdate()
{
	_d3d12Manager.FlushCommandQueue();
	static auto lastTick = _mainTimer.PeekCurrentTick();
	auto cTick = _mainTimer.PeekCurrentTick();
	auto deltaMs = _mainTimer.TicksToMs(cTick - lastTick);
	lastTick = cTick;
	auto camUpdated = _camControl.Update(deltaMs);
	auto lightUpdated = _movingLight_control.Update(deltaMs);

	_d3d12Manager.FlushCommandQueue();
	_kbdst.Update(_keyboard->GetState());
	if (_kbdst.IsKeyReleased(DirectX::Keyboard::Enter)) {
		//ReadBackTo(_renderObjects[0].qdMap.Get(), L"./qdm9.png", 9);
		ReadBackTo(_d3d12Manager.FindTexture2D(_gBuffers[0]).Get(), L"./gb0 ddrp.png");
		ReadBackTo(_renderObjects[0].qdMap.Get(), L"./qdm0.png", 0);
		ReadBackTo(_renderObjects[0].qdMap.Get(), L"./qdm1.png", 1);
		ReadBackTo(_renderObjects[0].qdMap.Get(), L"./qdm2.png", 2);
		ReadBackTo(_renderObjects[0].qdMap.Get(), L"./qdm3.png", 3);
		//ReadBackTo(_renderObjects[0].qdMap.Get(), L"./qdm4.png", 4);
		//ReadBackTo(_renderObjects[0].qdMap.Get(), L"./qdm5.png", 5);
		//ReadBackTo(_renderObjects[0].qdMap.Get(), L"./qdm6.png", 6);
		//ReadBackTo(_renderObjects[0].qdMap.Get(), L"./qdm7.png", 7);
		//ReadBackTo(_renderObjects[0].qdMap.Get(), L"./qdm8.png", 8);
		//ReadBackTo(_renderObjects[0].qdMap.Get(), L"./qdm9.png", 9);
		//ReadBackTo(_d3d12Manager.FindTexture2D(_gBuffers[1]).Get(), L"./gb1 drp.png");
		//ReadBackTo(_d3d12Manager.FindTexture2D(_gBuffers[2]).Get(), L"./gb2 drp.png");
	}

	if (camUpdated) {
		_cameraAttr_cpu.farClipD = _mainCam.FarPlaneDistance();
		_cameraAttr_cpu.mVtoW = _mainCam.TransformWtoV().Inverse().AsMatrix4x4();
		_cameraAttr_cpu.mWtoH = _mainCam.TransformWtoH().AsMatrix4x4();
		_cameraAttr_cpu.mWtoV = _mainCam.TransformWtoV().AsMatrix4x4();
		_cameraAttr_cpu.posW = _mainCam.GetPosition().AsVector4f().xyz();
		_cameraAttr_gpu.Update(_cameraAttr_cpu);
	}


	if (lightUpdated) {
		DRP::LightPass::NaiveLights::PointLight::generateOtoWMatrix(_movingLight_cpu);
		_movingLight_gpu.Update(_movingLight_cpu);
	}

}

void NaiveQDMApp::OnRender()
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

void NaiveQDMApp::RecordCmds(ID3D12GraphicsCommandList* pCmdlist, ID3D12CommandAllocator* pCmdAlloc)
{
#pragma region recordGBPass
	ID3D12DescriptorHeap* dsvHeap = _d3d12Manager.FindDescriptorHeap(_dsvDH).Get();
	ID3D12DescriptorHeap* rtvHeap = _d3d12Manager.FindDescriptorHeap(_rtvDH).Get();
	D3D12WindowResource& windowResrc = _d3d12Manager.GetMainWindowResource();
	auto sRect = &windowResrc.GetMainScissor();
	pCmdlist->RSSetScissorRects(1u, sRect);
	pCmdlist->RSSetViewports(1u, &windowResrc.GetMainViewport());


	_qdm.recordStateSettings(pCmdlist);
	ID3D12Resource* gbs[] = {
		_d3d12Manager.FindTexture2D(_gBuffers[0]).Get(),
		_d3d12Manager.FindTexture2D(_gBuffers[1]).Get(),
		_d3d12Manager.FindTexture2D(_gBuffers[2]).Get(),
	};
	WTF::NBPQDM::recordGBTransitionFrom(pCmdlist, gbs);


	auto rtvHandle = D3D12Helper::DescriptorHandleCPU(
		_d3d12Manager.FindDescriptorHeap(_rtvDH)->GetCPUDescriptorHandleForHeapStart());

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[3] = {};
	rtvHandles[0] = rtvHandle;
	rtvHandles[1] = rtvHandle.Offset(_d3d12Manager.GetRTVDescriptorSize());
	rtvHandles[2] = rtvHandle.Offset(_d3d12Manager.GetRTVDescriptorSize());
	WTF::NBPQDM::recordClearAndSetRtvDsv(pCmdlist, rtvHandles,
		dsvHeap->GetCPUDescriptorHandleForHeapStart(), 1u, sRect);

	WTF::NBPQDM::recordRp0Camera(pCmdlist, _cameraAttr_gpu.Get()->GetGPUVirtualAddress());
	for (auto i = 0u; i <_renderObjects.size(); ++i) {
		auto& ro = _renderObjects[i];
		WTF::NBPQDM::recordRp1Material(pCmdlist, ro.materialCB->GetGPUVirtualAddress());

		// DRP::GBPass::NaiveBlinnPhong::recordRp1Material(pCmdlist, std::get<2>(mesh)->GetGPUVirtualAddress());
		ID3D12DescriptorHeap* dhs[] = { ro.srvHeap.Get() };
		pCmdlist->SetDescriptorHeaps(1u, dhs);
		WTF::NBPQDM::recordRp2Textures(
			pCmdlist, dhs[0]->GetGPUDescriptorHandleForHeapStart());
		pCmdlist->IASetVertexBuffers(0u, 1u, &ro.mesh.VertexBufferView());
		pCmdlist->IASetIndexBuffer(&ro.mesh.IndexBufferView());
		pCmdlist->DrawIndexedInstanced(
			ro.mesh.GetTotoalIndexCount(), 1u, 0u, 0u, 0u);
	}
#pragma endregion recordGBPass

#pragma region recordLightPass
	DRP::GBPass::NaiveBlinnPhong::recordGBTransitionTo(pCmdlist, gbs);
	DRP::LightPass::NaiveLights::recordSettings(pCmdlist, _d3d12Manager.GetD3D12Device());

	D3D12_RESOURCE_BARRIER bbarrier[] = {
		D3D12Helper::ResourceBarrier::TransitionBarrier(
			windowResrc.GetCurrentBackBufferResource(),
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_BARRIER_FLAG_NONE
		),
	};
	pCmdlist->ResourceBarrier(1u, bbarrier);

	pCmdlist->OMSetRenderTargets(1u, &windowResrc.GetCurrentCPURTV(), TRUE,
		&dsvHeap->GetCPUDescriptorHandleForHeapStart());
	static float bbg[] = { 0.f, 0.f, 0.f, 0.f };
	pCmdlist->ClearRenderTargetView(windowResrc.GetCurrentCPURTV(), bbg, 1u, sRect);

	ID3D12DescriptorHeap* dhs[] = { _d3d12Manager.FindDescriptorHeap(_lightPassDH).Get() };
	pCmdlist->SetDescriptorHeaps(1u, dhs);
	DRP::LightPass::NaiveLights::recordRp1CameraAndGBuffer(pCmdlist,
		dhs[0]->GetGPUDescriptorHandleForHeapStart());
	// pCmdlist->SetGraphicsRootDescriptorTable(1u, dhs[0]->GetGPUDescriptorHandleForHeapStart());
	UINT stencilRef = 0x80;

	_pointLight.recordSettings(pCmdlist);
	// pCmdlist->SetPipelineState(_drr.getPointlightPSO());
	for (auto i = 0u; i < _pointLights.size(); ++i) {
		auto& light = _pointLights[i];
		DRP::LightPass::NaiveLights::recordRp0Light(pCmdlist,
			light.second->GetGPUVirtualAddress(), stencilRef);
		DRP::LightPass::NaiveLights::PointLight::recordDraw(pCmdlist);
		//pCmdlist->SetGraphicsRootConstantBufferView(0u, light.second->GetGPUVirtualAddress());
		//pCmdlist->DrawInstanced(6u, 1u, 0u, 0u);
	}

	_directionalLight.recordSettings(pCmdlist);
	// pCmdlist->SetPipelineState(_drr.getDirectionallightPSO());
	for (auto i = 0u; i < _directionalLights.size(); ++i) {
		auto& light = _directionalLights[i];
		DRP::LightPass::NaiveLights::recordRp0Light(pCmdlist,
			light.second->GetGPUVirtualAddress(), stencilRef);
		DRP::LightPass::NaiveLights::DirectionalLight::recordDraw(pCmdlist);
		//pCmdlist->SetGraphicsRootConstantBufferView(0u, light.second->GetGPUVirtualAddress());
		//pCmdlist->DrawInstanced(6u, 1u, 0u, 0u);
	}

	_spotLight.recordSettings(pCmdlist);
	// pCmdlist->SetPipelineState(_drr.getSpotlightPSO());
	for (auto i = 0u; i < _spotLights.size(); ++i) {
		auto& light = _spotLights[i];
		DRP::LightPass::NaiveLights::recordRp0Light(pCmdlist,
			light.second->GetGPUVirtualAddress(), stencilRef);
		DRP::LightPass::NaiveLights::SpotLight::recordDraw(pCmdlist);
		//pCmdlist->SetGraphicsRootConstantBufferView(0u, light.second->GetGPUVirtualAddress());
		//pCmdlist->DrawInstanced(6u, 1u, 0u, 0u);
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

void NaiveQDMApp::ReadBackTo(ID3D12Resource* src, const wchar_t* filename, UINT subResource /*= 0*/)
{
	auto srcDesc = src->GetDesc();
	//assert(srcDesc.Format == DXGI_FORMAT_R32G32B32A32_FLOAT);
	ID3D12GraphicsCommandList* cmdList = _d3d12Manager.GetMainCmdList();
	ID3D12CommandAllocator* cmdAlloc = _d3d12Manager.GetMainCmdAllocator();
	ID3D12CommandQueue* cmdQueue = _d3d12Manager.GetCmdQueue();
	ID3D12Device* device = _d3d12Manager.GetD3D12Device();

	ComPtr<ID3D12Resource> readbackHeap;
	auto heapProperties = D3D12Helper::ResourceHeapProperties(D3D12_HEAP_TYPE_READBACK);
	auto resDesc = D3D12Helper::ResourceDescriptor::Buffer(srcDesc.Width*srcDesc.Height * (D3D12Helper::DxgiFormatBitSize(srcDesc.Format)/8));
	device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE,
		&resDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(readbackHeap.GetAddressOf()));

	D3D12_TEXTURE_COPY_LOCATION dstl = {};
	dstl.pResource = readbackHeap.Get();
	dstl.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	dstl.PlacedFootprint.Footprint.Depth = 1;
	dstl.PlacedFootprint.Footprint.Format = srcDesc.Format;
	dstl.PlacedFootprint.Footprint.Width = static_cast<UINT>(srcDesc.Width/(1<<subResource));
	dstl.PlacedFootprint.Footprint.Height = static_cast<UINT>(srcDesc.Height/(1<<subResource));
	dstl.PlacedFootprint.Footprint.RowPitch = static_cast<UINT>(dstl.PlacedFootprint.Footprint.Width
		* (D3D12Helper::DxgiFormatBitSize(srcDesc.Format)/8));

	D3D12_TEXTURE_COPY_LOCATION srcl = {};
	srcl.pResource = src;
	srcl.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	srcl.SubresourceIndex = subResource;

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

	Luxko::Anuthur::SaveTexture2DAsPNG(readbackHeap.Get(), filename, 0u,
		static_cast<unsigned int>(srcDesc.Width / (1 << subResource)),
		static_cast<unsigned int>(srcDesc.Height / (1 << subResource)),
		srcDesc.Format);
}
