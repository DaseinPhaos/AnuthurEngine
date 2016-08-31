#include "ShapeApp.h"
#include "BasicGeometry.h"

ShapeFrameResource::ShapeFrameResource(ID3D12Device* device)
:FrameResource(device) {
	auto rd = D3D12Helper::ResourceDescriptor::Buffer(D3D12Helper::GetCBSizeAligned(sizeof(Matrix4x4f)+ sizeof(Point3DH)));

	ThrowIfFailed(device->CreateCommittedResource(&D3D12Helper::ResourceHeapProperties(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE, &rd, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(_whTransformGPU.GetAddressOf())));
	ThrowIfFailed(_whTransformGPU->Map(0, nullptr, reinterpret_cast<void**>(&_pwhTransformGPU)));

}

ShapeFrameResource::~ShapeFrameResource()
{
	_whTransformGPU->Unmap(0, nullptr);
}

void ShapeFrameResource::UpdateCam(const PerspecCamera& newCamera)
{
	std::memcpy(_pwhTransformGPU, &newCamera.TransformWtoH().AsMatrix4x4(), sizeof(Matrix4x4f));
	std::memcpy(_pwhTransformGPU + sizeof(Matrix4x4f), &newCamera.GetPosition(), sizeof(Point3DH));
}

void ShapeApp::OnInit()
{
	BasicD3D12ElementsInitialization();
	InitializeSceneComponents();
	InitializeShaders();
	InitializeFrameResources();
	InitializeRootSignatures();
	InitializePSOs();
	InitializeDescriptors();
}

void ShapeApp::OnRender()
{
	LogFPSToTitle();

	auto& cfr = *_frameResources[_currentFRIndex];
	ThrowIfFailed(cfr._cmdAllocator->Reset());
	_mainCmdList->Reset(cfr._cmdAllocator.Get(), _currentPSO);
	_mainCmdList->SetGraphicsRootSignature(_rootSignature.Get());
	_mainCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_mainCmdList->RSSetViewports(1u, &_mainViewport);
	_mainCmdList->RSSetScissorRects(1u, &_mainScissorRect);

	_mainCmdList->ResourceBarrier(0u, &D3D12Helper::ResourceBarrier::TransitionBarrier(_swapChainBuffer[_currentBackBufferIndex].Get(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	static FLOAT color[4] = { .6f, .6f, .6f, 1.f };
	_mainCmdList->ClearRenderTargetView(GetCurrentBackBufferView(), color, 0, nullptr);
	_mainCmdList->ClearDepthStencilView(_mainDSVHeap->GetCPUDescriptorHandleForHeapStart(),
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.f, 0, 0, nullptr);
	_mainCmdList->OMSetRenderTargets(1, &GetCurrentBackBufferView(), TRUE,
		&_mainDSVHeap->GetCPUDescriptorHandleForHeapStart());
	_mainCmdList->SetGraphicsRootConstantBufferView(2u, cfr._whTransformGPU->GetGPUVirtualAddress());
	_mainCmdList->SetGraphicsRootConstantBufferView(3u, cfr._lightsGPU.Get()->GetGPUVirtualAddress());

	{
		// Draw grid
		auto& gridResource = _geometrys["grid"];
		_mainCmdList->IASetIndexBuffer(&gridResource.IndexBufferView());
		_mainCmdList->IASetVertexBuffers(0u, 1u, &gridResource.VertexBufferView());
		_mainCmdList->SetGraphicsRootConstantBufferView(1u, _geometryMaterialsGPU["grid"]->GetGPUVirtualAddress());
		_mainCmdList->SetGraphicsRootConstantBufferView(0u, _scTransformsGPU["grid"].Get()->GetGPUVirtualAddress());
		_mainCmdList->DrawIndexedInstanced(gridResource.IndexBufferByteSize * 8
			/ D3D12Helper::DxgiFormatBitSize(gridResource.IndexFormat),
			1u, 0u, 0u, 0u);
	}

	{
		// Draw cylinders
		auto& cylinderResource = _geometrys["cylinder"];
		auto cis = cylinderResource.IndexBufferByteSize * 8
			/ D3D12Helper::DxgiFormatBitSize(cylinderResource.IndexFormat);
		_mainCmdList->IASetIndexBuffer(&cylinderResource.IndexBufferView());
		_mainCmdList->IASetVertexBuffers(0u, 1u, &cylinderResource.VertexBufferView());
		_mainCmdList->SetGraphicsRootConstantBufferView(1u, _geometryMaterialsGPU["cylinder"]->GetGPUVirtualAddress());

		_mainCmdList->SetGraphicsRootConstantBufferView(0u, _scTransformsGPU["cylinder0"].Get()->GetGPUVirtualAddress());
		_mainCmdList->DrawIndexedInstanced(cis, 1u, 0u, 0u, 0u);
		_mainCmdList->SetGraphicsRootConstantBufferView(0u, _scTransformsGPU["cylinder1"].Get()->GetGPUVirtualAddress());
		_mainCmdList->DrawIndexedInstanced(cis, 1u, 0u, 0u, 0u);
		_mainCmdList->SetGraphicsRootConstantBufferView(0u, _scTransformsGPU["cylinder2"].Get()->GetGPUVirtualAddress());
		_mainCmdList->DrawIndexedInstanced(cis, 1u, 0u, 0u, 0u);
		_mainCmdList->SetGraphicsRootConstantBufferView(0u, _scTransformsGPU["cylinder3"].Get()->GetGPUVirtualAddress());
		_mainCmdList->DrawIndexedInstanced(cis, 1u, 0u, 0u, 0u);
	}

	{
		// Draw spheres
		auto& sphereResource = _geometrys["sphere"];
		auto sis = sphereResource.IndexBufferByteSize * 8
			/ D3D12Helper::DxgiFormatBitSize(sphereResource.IndexFormat);
		_mainCmdList->IASetIndexBuffer(&sphereResource.IndexBufferView());
		_mainCmdList->IASetVertexBuffers(0u, 1u, &sphereResource.VertexBufferView());
		_mainCmdList->SetGraphicsRootConstantBufferView(1u, _geometryMaterialsGPU["sphere"]->GetGPUVirtualAddress());

		_mainCmdList->SetGraphicsRootConstantBufferView(0u, _scTransformsGPU["sphere0"].Get()->GetGPUVirtualAddress());
		_mainCmdList->DrawIndexedInstanced(sis, 1u, 0u, 0u, 0u);
		_mainCmdList->SetGraphicsRootConstantBufferView(0u, _scTransformsGPU["sphere1"].Get()->GetGPUVirtualAddress());
		_mainCmdList->DrawIndexedInstanced(sis, 1u, 0u, 0u, 0u);
		_mainCmdList->SetGraphicsRootConstantBufferView(0u, _scTransformsGPU["sphere2"].Get()->GetGPUVirtualAddress());
		_mainCmdList->DrawIndexedInstanced(sis, 1u, 0u, 0u, 0u);
		_mainCmdList->SetGraphicsRootConstantBufferView(0u, _scTransformsGPU["sphere3"].Get()->GetGPUVirtualAddress());
		_mainCmdList->DrawIndexedInstanced(sis, 1u, 0u, 0u, 0u);
	}

	// Present it
	_mainCmdList->ResourceBarrier(1u, &D3D12Helper::ResourceBarrier::TransitionBarrier(
		_swapChainBuffer[_currentBackBufferIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT));
	ThrowIfFailed(_mainCmdList->Close());
	ID3D12CommandList* cmdlsts[] = { _mainCmdList.Get() };
	_cmdQueue->ExecuteCommandLists(1u, cmdlsts);
	ThrowIfFailed(_swapChain->Present(0u, 0u));
	_currentBackBufferIndex = (_currentBackBufferIndex + 1) % SwapChainBufferCount;
	cfr._fenceCount = ++_currentMainFenceCount;
	_cmdQueue->Signal(_mainFence.Get(), _currentMainFenceCount);
}

void ShapeApp::OnUpdate()
{
	_currentFRIndex = (_currentFRIndex + 1) % _frameResources.size();
	auto& cfr = (*_frameResources[_currentFRIndex]);
	if (_mainFence->GetCompletedValue() < cfr._fenceCount) {
		auto hEvent = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
		ThrowIfFailed(_mainFence->SetEventOnCompletion(cfr._fenceCount, hEvent));
		WaitForSingleObject(hEvent, INFINITE);
		CloseHandle(hEvent);
	}

	static auto lastTick = 0ll;
	auto cTick = _mainTimer.PeekCurrentTick();
	if (lastTick == 0ll) lastTick = cTick;
	if (_mainTimer.TicksToMs(cTick - lastTick) > 300.f) {
		lastTick = cTick;

		if (GetAsyncKeyState(VK_SPACE)) {
			if (_currentPSO == _PSOs["wireframe"].Get())
				_currentPSO = _PSOs["normal"].Get();
			else _currentPSO = _PSOs["wireframe"].Get();
		}
	}

	static auto rotateZ = Transform3DH::RotationN(Vector3DH::E3(), static_cast<float>(M_PI) / 600.f);
	static auto nagetiveRotateZ = rotateZ.Inverse();
	if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
		auto pos = Point3DH(_lights.pointLinear.PosAndFallStart.xyz());
		_lights.pointLinear.PosAndFallStart = Vector4f((rotateZ*pos).AsVector4f().xyz(),
			_lights.pointLinear.PosAndFallStart._w);
		_frDirtyCount = static_cast<UINT>(_frameResources.size());
	}
	else if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
		auto pos = Point3DH(_lights.pointLinear.PosAndFallStart.xyz());
		_lights.pointLinear.PosAndFallStart = Vector4f((nagetiveRotateZ*pos).AsVector4f().xyz(),
			_lights.pointLinear.PosAndFallStart._w);
		_frDirtyCount = static_cast<UINT>(_frameResources.size());
	}
	static auto rotateX = Transform3DH::RotationN(Vector3DH::E1(), static_cast<float>(M_PI) / 600.f);
	static auto nagetiveRotateX = rotateX.Inverse();
	if (GetAsyncKeyState(VK_UP) & 0x8000) {
		auto pos = Point3DH(_lights.pointLinear.PosAndFallStart.xyz());
		_lights.pointLinear.PosAndFallStart = Vector4f((rotateX*pos).AsVector4f().xyz(),
			_lights.pointLinear.PosAndFallStart._w);
		_frDirtyCount = static_cast<UINT>(_frameResources.size());
	}
	else if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
		auto pos = Point3DH(_lights.pointLinear.PosAndFallStart.xyz());
		_lights.pointLinear.PosAndFallStart = Vector4f((nagetiveRotateX*pos).AsVector4f().xyz(),
			_lights.pointLinear.PosAndFallStart._w);
		_frDirtyCount = static_cast<UINT>(_frameResources.size());
	}

	if (_frDirtyCount > 0u) {
		//CopyMemory(cfr._pwhTransformGPU, &_mainCamera.TransformWtoH().AsMatrix4x4(), sizeof(Matrix4x4f));
		cfr.UpdateCam(_mainCamera);
		cfr._lightsGPU.Update(_lights);
		_frDirtyCount--;
	}
}

void ShapeApp::OnMouseDown(WPARAM wParam, int x, int y)
{
	if (wParam&MK_LBUTTON) {
		_lMouseDown = true;
	}
	if (wParam&MK_RBUTTON) {
		_rMouseDown = true;
	}
	if (wParam&MK_MBUTTON) {
		_mMouseDown = true;
	}
	SetCapture(_hWindow);
	_mouseLastX = x;
	_mouseLastY = y;
}

void ShapeApp::OnMouseMove(WPARAM wParam, int x, int y)
{
	static float f_pi = static_cast<float>(M_PI)*100.f;
	auto deltaH = (x - _mouseLastX) / f_pi;
	auto deltaV = (y - _mouseLastY) / f_pi;
	if (_lMouseDown) {
		auto rotation = Luxko::Transform3DH::RotationN(_mainCamera.GetRightDirection(), deltaV);
		auto formerUp = _mainCamera.GetUpDirection();
		rotation *= Luxko::Transform3DH::RotationN(formerUp, deltaH);


		_mainCamera.ApplyTransform(rotation);
		_mouseLastX = x;
		_mouseLastY = y;
		_frDirtyCount = static_cast<UINT>(_frameResources.size());
	}
	if (_rMouseDown) {
		auto translate = Luxko::Transform3DH::Translation(-deltaH*3.f*_mainCamera.GetRightDirection());
		translate *= Luxko::Transform3DH::Translation(deltaV*_mainCamera.GetUpDirection());
		_mainCamera.ApplyTransform(translate);
		_mouseLastX = x;
		_mouseLastY = y;
		_frDirtyCount = static_cast<UINT>(_frameResources.size());
	}
	if (_mMouseDown) {
		auto translate = Luxko::Transform3DH::Translation(deltaV*3.f*_mainCamera.GetLookDirection());
		_mainCamera.ApplyTransform(translate);
		_mouseLastX = x;
		_mouseLastY = y;
		_frDirtyCount = static_cast<UINT>(_frameResources.size());
	}
}

void ShapeApp::OnMouseUp(WPARAM wParam, int x, int y)
{
	ReleaseCapture();
	_lMouseDown = false;
	_rMouseDown = false;
	_mMouseDown = false;
}

void ShapeApp::InitializeSceneComponents()
{
	ThrowIfFailed(_mainCmdAllocator->Reset());
	_mainCmdList->Reset(_mainCmdAllocator.Get(), nullptr);
	_mainCamera = PerspecCamera::FromHFOVAndAspectRatio(1.f, 1000.f, 16.f / 9.f, 
		2.f*static_cast<float>(M_PI) / 3.f,
		Frame3DH(Vector3DH(-2.f, -1.5f, -2.f), Vector3DH(0.f, 1.f, 0.f), Point3DH(20.f, 20.f, 20.f)));

	// Initialize grid..
	{
		auto gridWorldTransform = Transform3DH::Translation(0.f, 0.f, 0.f);
		_sceneTransforms["grid"] = gridWorldTransform.AsMatrix4x4();


		auto gridMeshData = BasicGeometry::Grid(100.f, 100.f, 50, 50);
		auto gridMeshGeometry = MeshGeometry();
		
		std::vector<Vertex> gridVertices;
		gridVertices.reserve(gridMeshData.Vertices.size());
		for (auto i = 0u; i < gridMeshData.Vertices.size(); ++i) {
			gridVertices.emplace_back(gridMeshData.Vertices[i].Pos,
				gridMeshData.Vertices[i].Norm);
		}
		gridMeshGeometry.InitializeCPUResource(DXGI_FORMAT_R32_UINT,
			static_cast<UINT>(gridMeshData.Indices.size()), gridMeshData.Indices.data(),
			sizeof(Vertex), static_cast<UINT>(gridVertices.size()), gridVertices.data());
		gridMeshGeometry.RecordUpdateFromCPUtoGPU(_d3d12Device.Get(), _mainCmdList.Get());

		_geometrys["grid"] = gridMeshGeometry;
		_geometryMaterials["grid"] = BlinnPhongMaterial(ColorRGBf(1.1f, 1.12f, 2.14f),
			ColorRGBf(0.05f, 0.05f, 0.05f), .2f);
		
	}


	//auto norm2color = [](const Vector3DH& n)->Vector4f {
	//	return Vector4f(0.5f, 0.5f, 0.5f, 1.f) + n.AsVector4f();
	//};

	// Initialize sphere
	{
		auto sphereWorldTransform = Transform3DH::Translation(-15.f, 12.f, -10.f)*Transform3DH::Scale(2.f, 2.2f, 2.f);
		_sceneTransforms["sphere0"] = sphereWorldTransform.AsMatrix4x4();
		_sceneTransforms["sphere1"] = (Transform3DH::Translation(15.f, 0.f, 0.f)*sphereWorldTransform).AsMatrix4x4();
		_sceneTransforms["sphere2"] = (Transform3DH::Translation(15.f, 0.f, 10.f)*sphereWorldTransform).AsMatrix4x4();
		_sceneTransforms["sphere3"] = (Transform3DH::Translation(0.f, 0.f, 10.f)*sphereWorldTransform).AsMatrix4x4();


		auto sphereMeshData = BasicGeometry::Sphere(1.1f, 12u, 24u);

		auto sphereMeshGeometry = MeshGeometry();


		auto sphereVerticeData = std::vector<Vertex>();
		sphereVerticeData.reserve(sphereMeshData.Vertices.size());

		for (auto i = 0u; i < sphereMeshData.Vertices.size(); ++i) {
			sphereVerticeData.emplace_back(sphereMeshData.Vertices[i].Pos,
				sphereMeshData.Vertices[i].Norm);
		}

		sphereMeshGeometry.InitializeCPUResource(DXGI_FORMAT_R32_UINT, static_cast<UINT>(sphereMeshData.Indices.size()),
			sphereMeshData.Indices.data(), sizeof(Vertex), static_cast<UINT>(sphereVerticeData.size()), sphereVerticeData.data());
		sphereMeshGeometry.RecordUpdateFromCPUtoGPU(_d3d12Device.Get(), _mainCmdList.Get());

		_geometrys["sphere"] = sphereMeshGeometry;
		_geometryMaterials["sphere"] = BlinnPhongMaterial(ColorRGBf(1.8f, 0.85f, 0.9f),
			ColorRGBf(0.95f, 0.93f, 0.88f), .1f);
	}

	// Initialize cylinders
	{
		auto cylinderWorldTransform = Transform3DH::Translation(-15.f, 0.f, -10.f);
		_sceneTransforms["cylinder0"] = cylinderWorldTransform.AsMatrix4x4();
		_sceneTransforms["cylinder1"] = (Transform3DH::Translation(15.f, 0.f, 0.f)*cylinderWorldTransform).AsMatrix4x4();
		_sceneTransforms["cylinder2"] = (Transform3DH::Translation(15.f, 0.f, 10.f)*cylinderWorldTransform).AsMatrix4x4();
		_sceneTransforms["cylinder3"] = (Transform3DH::Translation(0.f, 0.f, 10.f)*cylinderWorldTransform).AsMatrix4x4();

		auto cylinderMeshData = BasicGeometry::Cylinder(4.f, 2.f, 10.f);
		auto cylinderVertexData = std::vector<Vertex>();
		cylinderVertexData.reserve(cylinderMeshData.Indices.size());
		for (auto i = 0u; i < cylinderMeshData.Vertices.size(); ++i) {
			cylinderVertexData.emplace_back(cylinderMeshData.Vertices[i].Pos,
				cylinderMeshData.Vertices[i].Norm);
		}
		_geometrys["cylinder"] = MeshGeometry();
		auto& cylinderMeshGeometry = _geometrys["cylinder"];
		cylinderMeshGeometry.InitializeCPUResource(DXGI_FORMAT_R32_UINT,
			static_cast<UINT>(cylinderMeshData.Indices.size()), cylinderMeshData.Indices.data(),
			sizeof(Vertex), static_cast<UINT>(cylinderVertexData.size()), cylinderVertexData.data());
		cylinderMeshGeometry.RecordUpdateFromCPUtoGPU(_d3d12Device.Get(), _mainCmdList.Get());

		_geometryMaterials["cylinder"] = BlinnPhongMaterial(ColorRGBf(2.f, 1.f, 0.3f),
			ColorRGBf(0.02f, 0.02f, 0.02f), 1.f);
	}
	std::vector<ComPtr<ID3D12Resource>> uploaders;
	uploaders.reserve(_sceneTransforms.size() + _geometryMaterials.size());
	std::vector<std::array<Matrix4x4f, 2u>> transforms;
	transforms.reserve(_sceneTransforms.size());
	for (auto& i : _sceneTransforms) {
		std::array<Matrix4x4f, 2u> temp = { i.second, i.second.Transpose().Inverse() };
		transforms.push_back(std::move(temp));
		uploaders.emplace_back(nullptr);
		_scTransformsGPU[i.first] = D3D12Helper::CreateDefaultBuffer(
			_d3d12Device.Get(), _mainCmdList.Get(), transforms.back().data(),
			D3D12Helper::GetCBSizeAligned(2u*sizeof(Matrix4x4f)), uploaders.back());
	}
	for (auto& i : _geometryMaterials) {
		uploaders.emplace_back(nullptr);
		_geometryMaterialsGPU[i.first] = D3D12Helper::CreateDefaultBuffer(
			_d3d12Device.Get(), _mainCmdList.Get(), &i.second.GetData(),
			D3D12Helper::GetCBSizeAligned(sizeof(BlinnPhongMaterialData)), uploaders.back());
	}

	// Execute update commands..
	_mainCmdList->Close();
	ID3D12CommandList* cmds[] = { _mainCmdList.Get() };
	_cmdQueue->ExecuteCommandLists(_countof(cmds), cmds);


	// Initialize lights
	{
		// directional light..
		_lights.directional.DirectionAndPower = Vector4f(1.f, 1.f, 1.f, 0.f).Normalize();
		_lights.directional.IrradianceAndFallEnd = Vector4f(.2f, .2f, .2f, 1.f);

		// linear point light
		_lights.pointLinear.PosAndFallStart = Vector4f(0.f, 20.f, 0.f, 0.f);
		_lights.pointLinear.IrradianceAndFallEnd = Vector4f(0.4f, 0.6f, 0.4f, 40.f);
		//_lights.pointLinear.DirectionAndPower = Vector4f(0.f, 0.f, 0.f, 1.f);

		// quadratic point light
		_lights.pointQuadra.PosAndFallStart = Vector4f(0.f, 20.f, 0.f, 0.f);
		_lights.pointQuadra.IrradianceAndFallEnd = Vector4f(0.4f, 0.6f, 0.4f, .1f);

		// spotlight
		_lights.spotlight.PosAndFallStart = Vector4f(10.f, 10.f, 0.f, 0.f);
		_lights.spotlight.IrradianceAndFallEnd = Vector4f(0.f, 0.7f, 0.7f, 30.f);
		_lights.spotlight.DirectionAndPower = Vector4f(1.f, 0.f, 0.f, 8.f);
	}


	// Flush commands, release resources..
	FlushCommandQueue();
	for (auto& item: _geometrys){
		item.second.DisposeUploaders();
	}
	//for (auto& item : _sceneTransforms) {
	//	_scTransformsGPU[item.first] = D3D12Helper::UpdateBuffer<Matrix4x4f>(_d3d12Device.Get());
	//	_scTransformsGPU[item.first].Update(item.second/*.AsMatrix4x4()*/);
	//}
	//for (auto& item : _geometryMaterials) {
	//	_geometryMaterialsGPU[item.first] = D3D12Helper::UpdateBuffer<BlinnPhongMaterial>(_d3d12Device.Get());
	//	_geometryMaterialsGPU[item.first].Update(item.second);
	//}
}

void ShapeApp::InitializeShaders()
{
	_shaders["vs"] = D3D12Helper::ShaderByteCode();
	auto& vs = _shaders["vs"];
	//ThrowIfFailed(vs.CompileFromFile(L"shaders.hlsl", "VS", "vs_5_0"));
	ThrowIfFailed(vs.CompileFromFile(L"ls.hlsl", "VS", "vs_5_0"));
	_shaders["ps"] = D3D12Helper::ShaderByteCode();
	auto& ps = _shaders["ps"];
	//ThrowIfFailed(ps.CompileFromFile(L"shaders.hlsl", "PS", "ps_5_0"));
	ThrowIfFailed(ps.CompileFromFile(L"ls.hlsl", "PS", "ps_5_0"));
}

void ShapeApp::InitializePSOs()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsd = {};
	D3D12Helper::InputLayoutDescriptor ild;
	ild.PushElementDescription("POSITION", DXGI_FORMAT_R32G32B32A32_FLOAT);
	ild.PushElementDescription("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT);
	gpsd.InputLayout = ild.Get();
	gpsd.pRootSignature = _rootSignature.Get();
	gpsd.VS = _shaders["vs"].Get();
	gpsd.PS = _shaders["ps"].Get();
	gpsd.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	gpsd.SampleMask = UINT_MAX;
	gpsd.RasterizerState = D3D12Helper::RasterizerDescriptor();
	gpsd.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	gpsd.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	gpsd.NumRenderTargets = 1;
	gpsd.RTVFormats[0] = _backBufferFormat;
	gpsd.DSVFormat = _depthStencilFormat;
	gpsd.SampleDesc.Count = 1;
	gpsd.SampleDesc.Quality = 0;

	_PSOs["normal"] = ComPtr<ID3D12PipelineState>();
	ThrowIfFailed(_d3d12Device->CreateGraphicsPipelineState(&gpsd, IID_PPV_ARGS(_PSOs["normal"].GetAddressOf())));
	gpsd.RasterizerState = D3D12Helper::RasterizerDescriptor(D3D12_FILL_MODE_WIREFRAME, D3D12_CULL_MODE_NONE);
	_PSOs["wireframe"] = ComPtr<ID3D12PipelineState>();
	ThrowIfFailed(_d3d12Device->CreateGraphicsPipelineState(&gpsd, IID_PPV_ARGS(_PSOs["wireframe"].GetAddressOf())));
	_currentPSO = _PSOs["wireframe"].Get();
}

void ShapeApp::InitializeFrameResources()
{
	for (auto i = 0u; i < 3u; ++i) {
		_frameResources.push_back(std::make_unique<ShapeFrameResource>(_d3d12Device.Get()));
		_frameResources.back()->_lightsGPU = D3D12Helper::UpdateBuffer<LightPack>(_d3d12Device.Get());
		_frameResources.back()->_lightsGPU.Update(_lights);
	}
	_frDirtyCount = static_cast<UINT>(_frameResources.size());
}

void ShapeApp::InitializeRootSignatures()
{
	D3D12Helper::RootSignatureDescriptor rsd;
	rsd.PushRPCBV(0u); // For mesh-world matrix
	rsd.PushRPCBV(1u); // For mesh material
	rsd.PushRPCBV(2u); // For world-camera matrix
	rsd.PushRPCBV(3u); // For lights
	
	ComPtr<ID3DBlob> serializedRS;
	ThrowIfFailed(rsd.SerializeRootSignature(serializedRS.GetAddressOf(), D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT));
	ThrowIfFailed(_d3d12Device->CreateRootSignature(0u,
		serializedRS->GetBufferPointer(), serializedRS->GetBufferSize(),
		IID_PPV_ARGS(_rootSignature.GetAddressOf())));

}

void ShapeApp::InitializeDescriptors()
{

}
