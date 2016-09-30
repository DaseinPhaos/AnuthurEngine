//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************
#include "Crate.h"
#include "BasicGeometry.h"

constexpr auto FrameResourceCount = 3u;
void CrateApp::OnInit()
{
	//BasicD3D12ElementsInitialization();
	D3D12App::OnInit();
	_d3d12Manager.GetMainCmdList()->Reset(_d3d12Manager.GetMainCmdAllocator(), nullptr);
	InitializeShaders();
	InitializeRootSignature();
	InitializePSOs();
	InitializeSceneComponents();
	InitializeFrameResources();
	InitializeTextures();
	InitializeDescriptors();
}

void CrateApp::OnUpdate()
{
	LogFPSToTitle();


	auto& cfr = _frameResources[_currentFRindex];
	if (_frDirtyCounts.Camera > 0) {

		cfr.UpdateCamera(_mainCamera);
		_frDirtyCounts.Camera--;
	}
	if (_d3d12Manager.GetMainFence()->GetCompletedValue() < cfr._fenceCount) {
		auto hEvent = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
		ThrowIfFailed(_d3d12Manager.GetMainFence()->SetEventOnCompletion(cfr._fenceCount, hEvent));
		WaitForSingleObject(hEvent, INFINITE);
		CloseHandle(hEvent);
	}

	static LONGLONG lastTick = _mainTimer.PeekCurrentTick();
	static LONGLONG lastTickl = lastTick;
	auto currentTick = _mainTimer.PeekCurrentTick();
	if (_mainTimer.TicksToMs(currentTick - lastTick) > 16.f) {
		lastTick = currentTick;

		// Update mouse
		{
			auto mouseState = _mouse->GetState();
			_mouseStateTracker.Update(mouseState);
			if (_mouseStateTracker.leftButton == DirectX::Mouse::ButtonStateTracker::PRESSED) {
				_mouse->SetMode(DirectX::Mouse::Mode::MODE_RELATIVE);
			}
			else if (_mouseStateTracker.leftButton == DirectX::Mouse::ButtonStateTracker::RELEASED) {
				_mouse->SetMode(DirectX::Mouse::Mode::MODE_ABSOLUTE);
			}

			static float f_pi = static_cast<float>(M_PI)*360.f;
			if (mouseState.positionMode == DirectX::Mouse::MODE_RELATIVE) {
				// FPS-like transform
				auto deltaH = 2.f*static_cast<float>(M_PI) - (mouseState.x) / f_pi;
				auto deltaV = 2.f*static_cast<float>(M_PI) - (mouseState.y) / f_pi;
				auto rotation = Transform3DH::RotationN(_mainCamera.GetRightDirection(), deltaV);
				rotation = Transform3DH::RotationN(_mainCamera.GetUpDirection(), deltaH) * rotation;
				_mainCamera._orientation.ApplyTransformOnOrientation(rotation);
				_frDirtyCounts.Camera = FrameResourceCount;
			}
		}

		{
			auto keysState = _keyboard->GetState();
			auto translateDelta = Vector3DH(0.f, 0.f, 0.f);
			if (keysState.W) {
				translateDelta += _mainCamera.GetLookDirection()*0.5f;
				_frDirtyCounts.Camera = FrameResourceCount;
			}
			if (keysState.S) {
				translateDelta -= _mainCamera.GetLookDirection()*0.5f;
				_frDirtyCounts.Camera = FrameResourceCount;
			}
			if (keysState.A) {
				translateDelta -= _mainCamera.GetRightDirection()*0.18f;
				_frDirtyCounts.Camera = FrameResourceCount;
			}
			if (keysState.D) {
				translateDelta += _mainCamera.GetRightDirection()*0.18f;
				_frDirtyCounts.Camera = FrameResourceCount;
			}
			if (_frDirtyCounts.Camera == FrameResourceCount) {
				_mainCamera.ApplyTransform(Transform3DH::Translation(translateDelta));
			}

			_keyboardStateTracker.Update(keysState);
			if (_keyboardStateTracker.IsKeyPressed(DirectX::Keyboard::Space)) {
				if (_currentPSO == _d3d12Manager.FindPSO("wireframe").Get())
					_currentPSO = _d3d12Manager.FindPSO("normal").Get();
				else _currentPSO = _d3d12Manager.FindPSO("wireframe").Get();
			}

		}

	}

}

void CrateApp::OnRender()
{
	auto _mainCmdList = _d3d12Manager.GetMainCmdList();
	auto& cfr = _frameResources[_currentFRindex];
	ThrowIfFailed(cfr._cmdAllocator->Reset());
	ThrowIfFailed(_mainCmdList->Reset(cfr._cmdAllocator.Get(), _currentPSO));

	_mainCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_mainCmdList->SetGraphicsRootSignature(_d3d12Manager.FindRootSignature(_rootSignature).Get());

	//ID3D12DescriptorHeap* dhs[] = { cfr._CBVheap.Get()/*, _mainSRVUAVheap.Get()*/ };
	//_mainCmdList->SetGraphicsRootShaderResourceView(1u, _crateTextureGPU->GetGPUVirtualAddress());
	//_mainCmdList->SetDescriptorHeaps(_countof(dhs), dhs);
	//_mainCmdList->SetGraphicsRootDescriptorTable(0u, cfr._CBVheap->GetGPUDescriptorHandleForHeapStart());
	//_mainCmdList->SetGraphicsRootDescriptorTable(1u, _mainSRVUAVheap->GetGPUDescriptorHandleForHeapStart());
	_mainCmdList->RSSetScissorRects(1u, &_d3d12Manager.FindWindowResource(_wndResourceID).GetMainScissor());
	_mainCmdList->RSSetViewports(1u, &_d3d12Manager.FindWindowResource(_wndResourceID).GetMainViewport());
	_mainCmdList->SetGraphicsRootConstantBufferView(1u, cfr._cameraData.Get()->GetGPUVirtualAddress());
	_mainCmdList->SetGraphicsRootConstantBufferView(2u, cfr._lightPack.Get()->GetGPUVirtualAddress());


	_mainCmdList->ResourceBarrier(1u, &D3D12Helper::ResourceBarrier::TransitionBarrier(
		_d3d12Manager.FindWindowResource(_wndResourceID).GetCurrentBackBufferResource(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	_mainCmdList->ClearDepthStencilView(_d3d12Manager.FindWindowResource(_wndResourceID).GetCPUDSV(),
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.f, 0, 0u, nullptr);
	static FLOAT bgColor[] = { 0.8f, 0.8f, 0.8f, 1.f };
	_mainCmdList->ClearRenderTargetView(_d3d12Manager.FindWindowResource(_wndResourceID).GetCurrentCPURTV(),
		bgColor, 0u, nullptr);
	_mainCmdList->OMSetRenderTargets(1u, &_d3d12Manager.FindWindowResource(_wndResourceID).GetCurrentCPURTV(),
		TRUE, &_d3d12Manager.FindWindowResource(_wndResourceID).GetCPUDSV());

	_mainCmdList->IASetIndexBuffer(&_crateIBV);
	_mainCmdList->IASetVertexBuffers(0u, 1u, &_crateVBV);
	//_mainCmdList->IASetIndexBuffer(&_d3d12Manager.FindIBD(_crateIBV));	
	//_d3d12Manager.FindVBD(_crateVBV).Apply(_mainCmdList, 0u);
	ID3D12DescriptorHeap* dhs[] = { _d3d12Manager.FindDescriptorHeap(_crateSRVUAVheap).Get()/*, _mainSRVUAVheap.Get()*/ };
	_mainCmdList->SetDescriptorHeaps(_countof(dhs), dhs);
	_mainCmdList->SetGraphicsRootDescriptorTable(0u, _d3d12Manager.FindDescriptorHeap(_crateSRVUAVheap)->GetGPUDescriptorHandleForHeapStart());
	_mainCmdList->DrawIndexedInstanced(_crateMesh.GetTotoalIndexCount(), 1u, 0u, 0u, 0u);

	_mainCmdList->IASetIndexBuffer(&_gridIBV);
	_mainCmdList->IASetVertexBuffers(0u, 1u, &_gridVBV);
	//_mainCmdList->IASetIndexBuffer(&_d3d12Manager.FindIBD(_gridIBV));
	//_d3d12Manager.FindVBD(_gridVBV).Apply(_mainCmdList, 0u);
	ID3D12DescriptorHeap* dhs1[] = { _d3d12Manager.FindDescriptorHeap(_gridSRVUAVheap).Get()/*, _mainSRVUAVheap.Get()*/ };
	_mainCmdList->SetDescriptorHeaps(_countof(dhs1), dhs1);
	_mainCmdList->SetGraphicsRootDescriptorTable(0u, _d3d12Manager.FindDescriptorHeap(_gridSRVUAVheap)->GetGPUDescriptorHandleForHeapStart());
	_mainCmdList->DrawIndexedInstanced(_gridMesh.GetTotoalIndexCount(), 1u, 0u, 0u, 0u);

	_mainCmdList->ResourceBarrier(1u, &D3D12Helper::ResourceBarrier::TransitionBarrier(
		_d3d12Manager.FindWindowResource(_wndResourceID).GetCurrentBackBufferResource(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	_mainCmdList->Close();
	
	ID3D12CommandList* cmdlsts[] = { _mainCmdList };
	_d3d12Manager.GetCmdQueue()->ExecuteCommandLists(1u, cmdlsts);
	cfr._fenceCount = _d3d12Manager.IncrementMainFenceCount();
	_d3d12Manager.GetCmdQueue()->Signal(_d3d12Manager.GetMainFence(), 
		_d3d12Manager.GetMainFenceCount());
	_d3d12Manager.FindWindowResource(_wndResourceID).GetSwapChain()->Present(0u, 0u);
	_d3d12Manager.FindWindowResource(_wndResourceID).AdvanceBackBufferIndex();
	_currentFRindex = (_currentFRindex + 1) % FrameResourceCount;

}

void CrateApp::OnDestroy()
{
	if(_d3d12Manager.GetD3D12Device() != nullptr) _d3d12Manager.FlushCommandQueue();
	_crateTextureGPU = nullptr;
	_gridTextureGPU = nullptr;
	_crateTGPU.Release();
	_crateMGPU.Release();
	_gridTGPU.Release();
	_gridMGPU.Release();
	for (auto& e : _frameResources) {
		e._cameraData.Release();
		e._lightPack.Release();
		e._CBVheap = nullptr;
	}
}

void CrateApp::InitializeSceneComponents()
{
	// Initialize main camera
	{
		auto cameraFrame = Frame3DH(Vector3DH(-1.f, -1.f, -1.f), Vector3DH::E2(), Point3DH(20.f, 20.f, 20.f));
		_mainCamera = PerspecCamera::FromHFOVAndAspectRatio(1.f, 1000.f, 16.f / 9.f,
			3.f*static_cast<float>(M_PI) / 4.f, cameraFrame);
	}

	// Initialize crate
	{
		//_crateLWTransform = Transform3DH::Translation(0.f, -1.f, 0.f);
		

		_crateMaterial = BlinnPhongMaterial{ Vector3f(10.02f, 10.12f, 10.12f), 10.1f, Vector3f(1.5f, 1.5f, 1.5f) };
		_crateTGPU = D3D12Helper::UpdateBuffer<ComponentTransformationData>(_d3d12Manager.GetD3D12Device());
		_crateMGPU = D3D12Helper::UpdateBuffer<BlinnPhongMaterial>(_d3d12Manager.GetD3D12Device());

		ComponentTransformationData ctd;
		ctd.CrateLW = _crateLWTransform.AsMatrix4x4();
		ctd.NormalLW = ctd.CrateLW.Inverse().Transpose();
		_crateTGPU.Update(ctd);
		_crateMGPU.Update(_crateMaterial);


		auto crateMeshData = BasicGeometry::Box(5.f, 5.f, 5.f);
		//crateMeshData.ReviseNormal();
		std::vector<Vertex> crateVertexData;
		crateVertexData.reserve(crateMeshData.Vertices.size());
		constexpr auto fourMask = 0x3u;
		static float textures[] = {
			0.f, 1.f,
			0.f, 0.f,
			1.f, 0.f,
			1.f, 1.f
		};
		for (auto i = 0u; i < crateMeshData.Vertices.size(); ++i) {
			auto j = fourMask&i;
			auto& v = crateMeshData.Vertices[i];
			crateVertexData.emplace_back(v.Pos.AsVector4f(), v.Norm.AsVector4f().xyz(),
				textures[2 * j], textures[2 * j + 1]);
		}

		auto& crateIndiceData = crateMeshData.GetIndices16Bit();
		_crateMesh.InitializeCPUResource(DXGI_FORMAT_R16_UINT,
			static_cast<UINT>(crateIndiceData.size()), crateIndiceData.data(),
			sizeof(Vertex), static_cast<UINT>(crateVertexData.size()),
			crateVertexData.data());
		_crateMesh.RecordUpdateFromCPUtoGPU(_d3d12Manager.GetD3D12Device(), _d3d12Manager.GetMainCmdList());
	}

	// Initialize grid
	{
		//_gridLWTransform = Transform3DH::RotationN(Vector3DH::E1(), static_cast<float>(M_PI_4)/2.f) * _gridLWTransform;
		//_gridMaterial = BlinnPhongMaterial{ Vector3f(0.95f, 0.64f, 0.54f), 0.f, Vector3f(1.f, 1.f, 1.f) };
		_gridMaterial = BlinnPhongMaterial{ Vector3f(0.95f, 0.64f, 0.54f), 10.f, Vector3f(1.f, 1.f, 1.f) };
		_gridTGPU = D3D12Helper::UpdateBuffer<ComponentTransformationData>(_d3d12Manager.GetD3D12Device());
		_gridMGPU = D3D12Helper::UpdateBuffer<BlinnPhongMaterial>(_d3d12Manager.GetD3D12Device());

		ComponentTransformationData ctd;
		ctd.CrateLW = _gridLWTransform.AsMatrix4x4();
		ctd.NormalLW = ctd.CrateLW.Inverse().Transpose();
		_gridTGPU.Update(ctd);
		_gridMGPU.Update(_gridMaterial);


		//auto gridMD = BasicGeometry::Grid(20.f, 30.f, 10, 20);
		auto gridMD = BasicGeometry::Terran(40.f, 40.f, 30, 20);
		std::vector<Vertex> gridVD;
		gridVD.reserve(gridMD.Vertices.size());
		for (auto i = 0u; i < gridMD.Vertices.size(); ++i) {
			auto& v = gridMD.Vertices[i];
			gridVD.emplace_back(v.Pos.AsVector4f(), /*v.Norm.AsVector4f().xyz()*/ Vector3f(0.f, 1.f, 0.f),
				v.Texture._x*2.5f, v.Texture._y*4.f);
		}
		auto& gridID = gridMD.GetIndices16Bit();
		_gridMesh.InitializeCPUResource(DXGI_FORMAT_R16_UINT, static_cast<UINT>(gridID.size()),
			gridID.data(), sizeof(Vertex), static_cast<UINT>(gridVD.size()), gridVD.data());
		_gridMesh.RecordUpdateFromCPUtoGPU(_d3d12Manager.GetD3D12Device(), _d3d12Manager.GetMainCmdList());
	}

	// Initialize lights
	{
		_lights.Directional.Irradiance = { 0.13f, 0.13f, 0.13f };
		_lights.Directional.Direction = Vector3f(1.f, -1.f, -1.f).Normalize();
		
		_lights.PointLinear.Pos = { 5.f, 5.f, -5.f };
		_lights.PointLinear.FallStart = 0.f;
		_lights.PointLinear.Irradiance = { 0.15f, 0.16f, 0.37f };
		_lights.PointLinear.FallEnd = 30.f;
		
		_lights.PointQuadra.Pos = { -5.f, 5.f, 5.f };
		_lights.PointQuadra.Irradiance = { 0.f, 0.f, 0.f };
		_lights.PointQuadra.FallEnd = 1.f;

		_lights.Spotlight.Pos = { 3.f, 6.f, 6.f };
		_lights.Spotlight.FallStart = 0.f;
		_lights.Spotlight.Irradiance = { 0.80f, 0.25f, 0.35f };
		_lights.Spotlight.FallEnd = 120.f;
		_lights.Spotlight.Direction = { 0.f, -1.f, 0.f };
		_lights.Spotlight.Power = 10.f;

		_lights.AmbientLight = { 0.1f, 0.1f, 0.1f, 1.f };
	}
}

void CrateApp::InitializeFrameResources()
{
	using namespace D3D12Helper;
	_frameResources.reserve(FrameResourceCount);
	for (auto i = 0u; i < FrameResourceCount; ++i) {
		_frameResources.emplace_back(_d3d12Manager.GetD3D12Device());
		auto& fr = _frameResources.back();
		fr.UpdateCamera(_mainCamera);
		//fr.UpdateCrateTransform(_crateLWTransform.AsMatrix4x4());
		fr.UpdateLightPack(_lights);
		//fr.UpdateMaterial(_crateMaterial);
		auto cpuHandle = static_cast<DescriptorHandleCPU>(fr._CBVheap->GetCPUDescriptorHandleForHeapStart());

		_d3d12Manager.GetD3D12Device()->CreateConstantBufferView(&CBVDescriptor(fr._cameraData.Get(), 0u, fr._cameraData.Size()),
			cpuHandle);
		_d3d12Manager.GetD3D12Device()->CreateConstantBufferView(&CBVDescriptor(fr._lightPack.Get(), 0u, fr._lightPack.Size()),
			cpuHandle.Offset(static_cast<SIZE_T>(_d3d12Manager.GetCbvSrvUavDescriptorSize())));
		//_d3d12Device->CreateConstantBufferView(
		//	&CBVDescriptor(fr._crateLWTransform.Get(), 0u, fr._crateLWTransform.Size()),
		//	cpuHandle.Offset(static_cast<SIZE_T>(_cbvSrvUavDescriptorSize)));
		//_d3d12Device->CreateConstantBufferView(
		//	&CBVDescriptor(fr._material.Get(), 0u, fr._material.Size()),
		//	cpuHandle.Offset(static_cast<SIZE_T>(_cbvSrvUavDescriptorSize)));
	}
	_frDirtyCounts.Camera = FrameResourceCount;
	_frDirtyCounts.Light = FrameResourceCount;
	_frDirtyCounts.MeshMaterial = FrameResourceCount;
	_frDirtyCounts.MeshTransfrom = FrameResourceCount;
	_currentFRindex = 0u;
}

void CrateApp::InitializeShaders()
{
	using namespace D3D12Helper;
	_d3d12Manager.AddShader(ShaderPath, "VS", "vs_5_0", "VS");
	_d3d12Manager.AddShader(ShaderPath, "PS", "ps_5_0", "PS");
// 	auto& vs = _shaders["VS"];
// 	ThrowIfFailed(vs.CompileFromFile(ShaderPath, "VS", "vs_5_0"));

// 	_shaders["PS"] = ShaderByteCode();
// 	auto& ps = _shaders["PS"];
// 	ThrowIfFailed(ps.CompileFromFile(ShaderPath, "PS", "ps_5_0"));

}

void CrateApp::InitializeRootSignature()
{
	using namespace D3D12Helper;
	RootSignatureDescriptor rsd;
	RootDescriptorTable rdt;
	//rdt.Push(RootDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 4u, 0u));
	rdt.Push(RootDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 2u, 2u));
	rdt.Push(RootDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1u, 0u));
	rsd.PushRPDescriptorTable(rdt.Get(), D3D12_SHADER_VISIBILITY_ALL);
	rsd.PushRPCBV(0u);
	rsd.PushRPCBV(1u);
	//RootDescriptorTable rdt2;
	//rdt2.Push(RootDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1u, 0u));
	//rsd.PushRPDescriptorTable(rdt2.Get(), D3D12_SHADER_VISIBILITY_PIXEL);
	//rsd.PushRPSRV(0u, 0u, D3D12_SHADER_VISIBILITY_PIXEL);
	// describe static samplers
	{
		// 0u : Anisotropic wrap
		rsd.PushStaticSamplerDescriptor(0u, D3D12_FILTER_ANISOTROPIC,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP);

		// 1u : Point wrap
		rsd.PushStaticSamplerDescriptor(1u, D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP);

		// 2u : Linear wrap
		rsd.PushStaticSamplerDescriptor(2u, D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP);

	}
	_rootSignature = _d3d12Manager.AddRootSignature(rsd, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
// 	ComPtr<ID3DBlob> serializedRootSignature;
// 	ThrowIfFailed(rsd.SerializeRootSignature(serializedRootSignature.GetAddressOf(),
// 		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT));
// 	ThrowIfFailed(_d3d12Manager.GetD3D12Device()->CreateRootSignature(0u, serializedRootSignature->GetBufferPointer(),
// 		serializedRootSignature->GetBufferSize(), IID_PPV_ARGS(_rootSignature.GetAddressOf())));

}

void CrateApp::InitializePSOs()
{
	using namespace D3D12Helper;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsd = { };
	InputLayoutDescriptor ild;
	ild.PushElementDescription("POSITION", DXGI_FORMAT_R32G32B32A32_FLOAT);
	ild.PushElementDescription("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT);
	ild.PushElementDescription("TEXTURE", DXGI_FORMAT_R32G32_FLOAT);
	gpsd.InputLayout = ild.Get();
	gpsd.pRootSignature = _d3d12Manager.FindRootSignature(_rootSignature).Get();
	gpsd.VS = _d3d12Manager.FindShader("VS").Get();
	gpsd.PS = _d3d12Manager.FindShader("PS").Get();
	gpsd.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	gpsd.SampleMask = UINT_MAX;
	gpsd.RasterizerState = RasterizerDescriptor();
	gpsd.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	gpsd.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	gpsd.NumRenderTargets = 1u;
	gpsd.RTVFormats[0] = _d3d12Manager.FindWindowResource(_wndResourceID).GetBackBufferFormat();
	gpsd.DSVFormat = _d3d12Manager.FindWindowResource(_wndResourceID).GetDepthStencilFormat();
	gpsd.SampleDesc.Count = 1;
	gpsd.SampleDesc.Quality = 0;
	_d3d12Manager.AddPSO(gpsd, "normal");
// 	_PSOs["normal"] = ComPtr<ID3D12PipelineState>();
// 	ThrowIfFailed(_d3d12Device->CreateGraphicsPipelineState(&gpsd, IID_PPV_ARGS(_PSOs["normal"].GetAddressOf())));
	gpsd.RasterizerState = D3D12Helper::RasterizerDescriptor(D3D12_FILL_MODE_WIREFRAME, D3D12_CULL_MODE_NONE);
	_d3d12Manager.AddPSO(gpsd, "wireframe");
	_currentPSO = _d3d12Manager.FindPSO("wireframe").Get();
// 	_PSOs["wireframe"] = ComPtr<ID3D12PipelineState>();
// 	ThrowIfFailed(_d3d12Device->CreateGraphicsPipelineState(&gpsd, IID_PPV_ARGS(_PSOs["wireframe"].GetAddressOf())));
// 	_currentPSO = _PSOs["wireframe"].Get();
}

void CrateApp::InitializeDescriptors()
{
	_crateMesh.DisposeUploaders();
	_crateIBV = _crateMesh.IndexBufferView();
	_crateVBV = _crateMesh.VertexBufferView();

	_gridMesh.DisposeUploaders();
	_gridIBV = _gridMesh.IndexBufferView();
	_gridVBV = _gridMesh.VertexBufferView();

	
	auto pHeap = _d3d12Manager.FindDescriptorHeap(_crateSRVUAVheap).Get();
	_d3d12Manager.CreateCBVOnHeap(&D3D12Helper::CBVDescriptor(_crateTGPU.Get(), 0u, _crateTGPU.Size()), pHeap, 0u);
	_d3d12Manager.CreateCBVOnHeap(&D3D12Helper::CBVDescriptor(_crateMGPU.Get(), 0u, _crateMGPU.Size()), pHeap, 1u);
	_d3d12Manager.CreateSRVOnHeap(_crateTextureGPU.Get(), nullptr, pHeap, 2u);


	pHeap = _d3d12Manager.FindDescriptorHeap(_gridSRVUAVheap).Get();
	_d3d12Manager.CreateCBVOnHeap(&D3D12Helper::CBVDescriptor(_gridTGPU.Get(), 0u, _gridTGPU.Size()), pHeap, 0u);
	_d3d12Manager.CreateCBVOnHeap(&D3D12Helper::CBVDescriptor(_gridMGPU.Get(), 0u, _gridMGPU.Size()), pHeap, 1u);
	_d3d12Manager.CreateSRVOnHeap(_gridTextureGPU.Get(), nullptr, pHeap, 2u);
	
}

void CrateApp::InitializeTextures()
{
	auto _d3d12Device = _d3d12Manager.GetD3D12Device();
	auto _mainCmdList = _d3d12Manager.GetMainCmdList();
	ComPtr<ID3D12Resource> uploader = nullptr;
	std::wstring textureName = TextureDir;
	textureName += L"bricks.dds";
	ThrowIfFailed(D3D12Helper::ReadDDSTextureFromFile(_d3d12Device, _mainCmdList,
		textureName.c_str(), _crateTextureGPU, uploader));
	//ThrowIfFailed(_mainCmdList->Close());
	//ID3D12CommandList* cmdlsts[] = { _mainCmdList.Get() };
	//_cmdQueue->ExecuteCommandLists(1u, cmdlsts);

	ComPtr<ID3D12Resource> uploader2;
	textureName = TextureDir;
	textureName += L"ice.dds";
	ThrowIfFailed(D3D12Helper::ReadDDSTextureFromFile(_d3d12Device, _mainCmdList,
		textureName.c_str(), _gridTextureGPU, uploader2));
	ThrowIfFailed(_mainCmdList->Close());
	ID3D12CommandList* cmdlsts[] = { _mainCmdList };
	_d3d12Manager.GetCmdQueue()->ExecuteCommandLists(1u, cmdlsts);
	
	_crateSRVUAVheap = _d3d12Manager.AddDescriptorHeap(D3D12Helper::DescriptorHeapDescriptor(
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 3u, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE));
	_gridSRVUAVheap = _d3d12Manager.AddDescriptorHeap(D3D12Helper::DescriptorHeapDescriptor(
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 3u, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE));
	_d3d12Manager.FlushCommandQueue();
}
