//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************
#include "BoxApp.h"
#include "Matrix4x4f.h"
#include "Frame3DH.h"

void BoxApp::OnInit()
{
	D3D12App::BasicD3D12ElementsInitialization();
	InitializeSceneComponents();
	InitializeShaders();
	InitializeRootSignature();
	InitializePipelineState();
	InitializeHeaps();
	//BindResourceViews();
	_mainCmdList->Close();
	ID3D12CommandList* cmdlsts[] = { _mainCmdList.Get() };
	_cmdQueue->ExecuteCommandLists(1u, cmdlsts);
	FlushCommandQueue();
}

void BoxApp::OnDestroy()
{
	D3D12App::OnDestroy();
	if (_cbBuffer.Get() != nullptr)
		_cbBuffer->Unmap(0, nullptr);
}

void BoxApp::OnMouseDown(WPARAM wParam, int x, int y)
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
	_mouseLastX = x;
	_mouseLastY = y;
}

void BoxApp::OnMouseMove(WPARAM wParam, int x, int y)
{
	static float f_pi = static_cast<float>(M_PI)*100.f;
	auto deltaH = (x - _mouseLastX) / f_pi;
	auto deltaV = (y - _mouseLastY) / f_pi;
	if (_lMouseDown) {
		auto rotation = Luxko::Transform3DH::RotationN(_mainCamera.GetRightDirection(), deltaV);
		auto formerUp = _mainCamera.GetUpDirection();
		rotation *= Luxko::Transform3DH::RotationN(formerUp, deltaH);
		//auto look = _mainCamera.GetLookDirection();
		//auto formerPos = _mainCamera.GetPosition();
		//look = rotation*look;
		//_mainCamera._Orientation = Luxko::Frame3DH(look, formerUp, formerPos);


		_mainCamera.ApplyTransform(rotation);
		_mouseLastX = x;
		_mouseLastY = y;
	}
	if (_rMouseDown) {
		auto translate = Luxko::Transform3DH::Translation(-deltaH*_mainCamera.GetRightDirection());
		translate *= Luxko::Transform3DH::Translation(deltaV*_mainCamera.GetUpDirection());
		_mainCamera.ApplyTransform(translate);
		_mouseLastX = x;
		_mouseLastY = y;
	}
	if (_mMouseDown) {
		auto translate = Luxko::Transform3DH::Translation(deltaV*_mainCamera.GetLookDirection());
		_mainCamera.ApplyTransform(translate);
		_mouseLastX = x;
		_mouseLastY = y;
	}

}

void BoxApp::OnMouseUp(WPARAM wParam, int x, int y)
{
	_lMouseDown = false;
	_rMouseDown = false;
	_mMouseDown = false;
}

void BoxApp::OnUpdate()
{
	//D3D12_RANGE range;
	//range.Begin = 0u;
	//range.End = _cbBufferSize;
	//void* pUpdate;
	//_cbBuffer->Map(0u, &range, &pUpdate);
	//auto vhmatrix = _mainCamera.TransformVtoH().AsMatrix4x4();
	//auto wvmatrix =  _mainCamera.TransformWtoV().AsMatrix4x4();
	//auto rhmatrix = _mainCamera._Orientation.GetTransform().AsMatrix4x4();
	auto whmatrix = _mainCamera.TransformWtoH().AsMatrix4x4()/*.Transpose()*/;
	std::memcpy(_cbBufferMappedAddress, &whmatrix, sizeof(Luxko::Matrix4x4f));
	//_cbBuffer->Unmap(0u, &range);

	static auto deltaC = 0.01f;
	_mainTimer.Elapse();
	if (_mainTimer.DeltaInMs() > 30.f) {
		_mainTimer.Reset(true);
		_bgColor[0] += deltaC;
		if (_bgColor[0] > 1.f
			|| _bgColor[0] < 0.f) deltaC = -deltaC;
	}
}

void BoxApp::OnRender()
{
	using namespace Luxko::Anuthur;
	using namespace Luxko::Anuthur::D3D12Helper;
	/*FlushCommandQueue();*/
	LogFPSToTitle();
	// ThrowIfFailed(_mainCmdAllocator->Reset());
	_mainCmdList->Reset(_mainCmdAllocator.Get(), _pipelineState.Get());
	_mainCmdList->RSSetViewports(1, &_mainViewport);
	_mainCmdList->RSSetScissorRects(1, &_mainScissorRect);
	_mainCmdList->SetGraphicsRootSignature(_rootSignature.Get());
	_mainCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//BindResourceViews();

	using namespace Luxko::Anuthur;
	using namespace Luxko::Anuthur::D3D12Helper;
	//_mainCmdList->SetGraphicsRootSignature(_rootSignature.Get());
	VertexBuffersDescriptor vbd;
	vbd.Push(_verticeBuffer->GetGPUVirtualAddress(), sizeof(Vertex) * _countof(_Box), sizeof(Vertex));
	vbd.Apply(_mainCmdList.Get(), 0u);

	IndexBufferDescriptor ibd(_IndiceBuffer->GetGPUVirtualAddress(), sizeof(UINT16) * _countof(_Indice));
	ibd.Apply(_mainCmdList.Get());


	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Transition.pResource = _swapChainBuffer[_currentBackBufferIndex].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	_mainCmdList->ResourceBarrier(1u, &barrier);

	FLOAT color[4] = { 0.4f, .4f, .4f, .4f };
	_mainCmdList->ClearRenderTargetView(GetCurrentBackBufferView(),
		color, 0, nullptr);

	_mainCmdList->ClearDepthStencilView(_mainDSVHeap->GetCPUDescriptorHandleForHeapStart(),
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.f, 0, 0, nullptr);
	_mainCmdList->OMSetRenderTargets(1, &GetCurrentBackBufferView(), TRUE,
		&_mainDSVHeap->GetCPUDescriptorHandleForHeapStart());

	//ID3D12DescriptorHeap* dhs[] = { _cbvHeap.Get() };
	//_mainCmdList->SetDescriptorHeaps(_countof(dhs), dhs);
	//_mainCmdList->SetGraphicsRootDescriptorTable(0, _cbvHeap->GetGPUDescriptorHandleForHeapStart());

	_mainCmdList->SetGraphicsRootConstantBufferView(0u, _cbBuffer->GetGPUVirtualAddress());

	_mainCmdList->DrawIndexedInstanced(36u, 1u, 0u, 0u, 0u);
	D3D12_RESOURCE_BARRIER barrier1 = {};
	barrier1.Transition.pResource = _swapChainBuffer[_currentBackBufferIndex].Get();
	barrier1.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	barrier1.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	_mainCmdList->ResourceBarrier(1, &barrier1);
	ThrowIfFailed(_mainCmdList->Close());

	ID3D12CommandList* cmdlsts[] = { _mainCmdList.Get() };
	_cmdQueue->ExecuteCommandLists(1u, cmdlsts);

	ThrowIfFailed(_swapChain->Present(0, 0));
	_currentBackBufferIndex = (_currentBackBufferIndex + 1) % SwapChainBufferCount;
	FlushCommandQueue();
}

void BoxApp::InitializePipelineState()
{
	using namespace Luxko::Anuthur;
	using namespace Luxko::Anuthur::D3D12Helper;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psDesc;
	ZeroMemory(&psDesc, sizeof(psDesc));

	InputLayoutDescriptor ild;
	ild.PushElementDescription("POSITION", DXGI_FORMAT_R32G32B32A32_FLOAT);
	ild.PushElementDescription("COLOR", DXGI_FORMAT_R32G32B32A32_FLOAT);
	psDesc.pRootSignature = _rootSignature.Get();
	psDesc.VS = _vertexShader.Get();
	psDesc.PS = _pixelShader.Get();
	psDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psDesc.SampleMask = UINT_MAX;
	psDesc.RasterizerState = RasterizerDescriptor(/*D3D12_FILL_MODE_WIREFRAME, D3D12_CULL_MODE_NONE*/);
	psDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psDesc.InputLayout = ild.Get();
	psDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psDesc.NumRenderTargets = 1;
	psDesc.RTVFormats[0] = _backBufferFormat;
	psDesc.DSVFormat = _depthStencilFormat;
	psDesc.SampleDesc.Count = 1;
	psDesc.SampleDesc.Quality = 0;

	ThrowIfFailed(_d3d12Device->CreateGraphicsPipelineState(&psDesc, IID_PPV_ARGS(_pipelineState.GetAddressOf())));

}

void BoxApp::InitializeRootSignature()
{
	using namespace Luxko::Anuthur;
	using namespace Luxko::Anuthur::D3D12Helper;
	//RootDescriptorTable rdt;
	//rdt.Push(RootDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1u));
	//RootSignatureDescriptor rsd;
	//rsd.PushRootParameter(RootParameter::DescriptorTablef(rdt.Get(), D3D12_SHADER_VISIBILITY_ALL));
	//ThrowIfFailed(rsd.SerializeRootSignature(_rootSignatureRaw.GetAddressOf(),
	//	D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT));
	//ThrowIfFailed(_d3d12Device->CreateRootSignature(0u, _rootSignatureRaw->GetBufferPointer(),
	//	_rootSignatureRaw->GetBufferSize(), IID_PPV_ARGS(_rootSignature.GetAddressOf())));

	RootSignatureDescriptor rsd;
	rsd.PushRPCBV(0u);
	ThrowIfFailed(rsd.SerializeRootSignature(_rootSignatureRaw.GetAddressOf(),
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT));

	ThrowIfFailed(_d3d12Device->CreateRootSignature(0u, _rootSignatureRaw->GetBufferPointer(),
			_rootSignatureRaw->GetBufferSize(), IID_PPV_ARGS(_rootSignature.GetAddressOf())));
}

void BoxApp::InitializeSceneComponents()
{
	//_Box[0] = Vertex(-1.0f, -1.0f, -1.0f, 0.f, 0.f, 0.f);
	//_Box[1] = Vertex(-1.0f, +1.0f, -1.0f, 1.f, 0.f, 0.f);
	//_Box[2] = Vertex(+1.0f, +1.0f, -1.0f, 1.f, 0.f, 1.f);
	//_Box[3] = Vertex(+1.0f, -1.0f, -1.0f, 0.f, 0.f, 1.f);
	//_Box[4] = Vertex(-1.0f, -1.0f, +1.0f, 0.f, 1.f, 0.f);
	//_Box[5] = Vertex(-1.0f, +1.0f, +1.0f, 1.f, 1.f, 0.f);
	//_Box[6] = Vertex(+1.0f, +1.0f, +1.0f, 1.f, 1.f, 1.f);
	//_Box[7] = Vertex(+1.0f, -1.0f, +1.0f, 0.f, 1.f, 1.f);

	// UINT16 indices[] = {
	// 		// front face
	// 		0, 1, 2,
	// 		0, 2, 3,
	// 
	// 		// back face
	// 		4, 6, 5,
	// 		4, 7, 6,
	// 
	// 		// left face
	// 		4, 5, 1,
	// 		4, 1, 0,
	// 
	// 		// right face
	// 		3, 2, 6,
	// 		3, 6, 7,
	// 
	// 		// top face
	// 		1, 5, 6,
	// 		1, 6, 2,
	// 
	// 		// bottom face
	// 		4, 0, 3,
	// 		4, 3, 7
	// 	};

	_Box[0] = Vertex(0.f, 0.f, 0.f, 0.f, 0.f, 0.f);
	_Box[1] = Vertex(1.f, 0.f, 0.f, 1.f, 0.f, 0.f);
	_Box[2] = Vertex(1.f, 0.f, 1.f, 1.f, 0.f, 1.f);
	_Box[3] = Vertex(0.f, 0.f, 1.f, 0.f, 0.f, 1.f);
	_Box[4] = Vertex(0.f, 1.f, 0.f, 0.f, 1.f, 0.f);
	_Box[5] = Vertex(1.f, 1.f, 0.f, 1.f, 1.f, 0.f);
	_Box[6] = Vertex(1.f, 1.f, 1.f, 1.f, 1.f, 1.f);
	_Box[7] = Vertex(0.f, 1.f, 1.f, 0.f, 1.f, 1.f);

	UINT16 indices[] = { 1,2,5,
						2,6,5,
						6,2,3,
						3,7,6,
						0,7,3,
						0,4,7,
						1,5,4,
						1,4,0,
						7,4,5,
						5,6,7,
						2,1,0,
						3,2,0};

	std::memcpy(_Indice, indices, sizeof(UINT16)*_countof(_Indice));

	;

	_mainCamera = Luxko::Anuthur::PerspecCamera::FromHFOVAndAspectRatio(1.f, 1000.f, 4.f / 3.f, 2.f*M_PI / 3.f, Luxko::Frame3DH(Luxko::Vector3DH(-1.f, -2.f, 1.f), Luxko::Vector3DH(0.f, 1.f, 0.f),
		Luxko::Point3DH(2.f, 4.f, -2.f)));

}

void BoxApp::InitializeShaders()
{
	using namespace Luxko::Anuthur;
	using namespace Luxko::Anuthur::D3D12Helper;

	ThrowIfFailed(_vertexShader.CompileFromFile(L"shaders.hlsl", "VS", "vs_5_0"));
	ThrowIfFailed(_pixelShader.CompileFromFile(L"shaders.hlsl", "PS", "ps_5_0"));
}

void BoxApp::InitializeHeaps()
{
	using namespace Luxko;
	using namespace Luxko::Anuthur;
	using namespace Luxko::Anuthur::D3D12Helper;

	//ThrowIfFailed(_d3d12Device->CreateCommittedResource(&HeapProperties(), D3D12_HEAP_FLAG_NONE,
	//	&ResourceDescriptor(D3D12_RESOURCE_DIMENSION_BUFFER, sizeof(Vertex)*_countof(_Box)),
	//	D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(_verticeHeap.GetAddressOf())));
	_mainCmdList->Reset(_mainCmdAllocator.Get(), _pipelineState.Get());
	_verticeBuffer = CreateDefaultBuffer(_d3d12Device.Get(), _mainCmdList.Get(),
		static_cast<void*>(_Box), static_cast<UINT64>(sizeof(Vertex) * _countof(_Box)), _uploadBuffer);

	// 	ID3D12CommandList* const cmdLists[] = { _mainCmdList.Get() };
	// 	_cmdQueue->ExecuteCommandLists(1u, cmdLists );
	// 	
	// 	FlushCommandQueue();


	_IndiceBuffer = CreateDefaultBuffer(_d3d12Device.Get(), _mainCmdList.Get(),
		static_cast<void*>(_Indice), static_cast<UINT64>(sizeof(UINT16) * _countof(_Indice)),
		_uploadBuffer1);
	_cbBufferSize = D3D12Helper::GetCBSizeAligned(sizeof(Matrix4x4f));
	auto cbHeapDesc = ResourceDescriptor::Buffer(_cbBufferSize);
	ThrowIfFailed(_d3d12Device->CreateCommittedResource(
		&HeapProperties(D3D12_HEAP_TYPE_UPLOAD, D3D12_CPU_PAGE_PROPERTY_UNKNOWN),
		D3D12_HEAP_FLAG_NONE, &cbHeapDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(_cbBuffer.GetAddressOf())));
	ThrowIfFailed(_cbBuffer->Map(0, nullptr, &_cbBufferMappedAddress));


	auto cbdhDesc = DescriptorHeapDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1u,
		D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 0u);
	ThrowIfFailed(_d3d12Device->CreateDescriptorHeap(&cbdhDesc, IID_PPV_ARGS(_cbvHeap.GetAddressOf())));

	_d3d12Device->CreateConstantBufferView(&CBVDescriptor(_cbBuffer->GetGPUVirtualAddress(), _cbBufferSize),
		_cbvHeap->GetCPUDescriptorHandleForHeapStart());



}

void BoxApp::BindResourceViews()
{
	using namespace Luxko::Anuthur;
	using namespace Luxko::Anuthur::D3D12Helper;
	//_mainCmdList->SetGraphicsRootSignature(_rootSignature.Get());
	VertexBuffersDescriptor vbd;
	vbd.Push(_verticeBuffer->GetGPUVirtualAddress(), sizeof(Vertex) * _countof(_Box), sizeof(Vertex));
	vbd.Apply(_mainCmdList.Get(), 0u);

	IndexBufferDescriptor ibd(_IndiceBuffer->GetGPUVirtualAddress(), sizeof(UINT16) * _countof(_Indice));
	ibd.Apply(_mainCmdList.Get());

	//_mainCmdList->SetGraphicsRootConstantBufferView(0u, _cbBuffer->GetGPUVirtualAddress());

}
