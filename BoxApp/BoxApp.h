//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************
#pragma once
#include "D3D12HelperMethods.h"
#include "D3D12App.h"
#include "PerspecCamera.h"
#include "Point3DH.h"
#include "Vector4f.h"
#include "MeshResource.h"
#include "BasicGeometry.h"

//struct Vertex {
//	Luxko::Point3DH	Position;
//	Luxko::Vector4f	Color;
//
//	Vertex() {}
//	Vertex(float x, float y, float z, float r, float g, float b) :
//		Position(x, y, z), Color(r, g, b, 1.f) {}
//};

//struct Vertex {
//	Luxko::Point3DH position;
//	Vertex() { }
//	Vertex(float x, float y, float z) :position(x, y, z) {}
//};
using namespace Luxko;
struct Vertex {
	
	Vertex(const Point3DH& position,
		const Vector3DH& normal,
		const Vector3DH& tangentU,
		float tx, float ty, float tz = 0.f, float tw = 0.f);
	Vertex(float px, float py, float pz,
		float nx, float ny, float nz,
		float tux, float tuy, float tuz,
		float tx, float ty, float tz = 0.f, float tw = 0.f);
	Vertex() {}
	Vertex(const Vertex&) = default;
	Vertex& operator=(const Vertex&) = default;
	~Vertex() {}


	Point3DH	Position;
	Vector3DH	Normal;
	Vector3DH	TangentU;
	Vector4f	Texture;
};

constexpr auto sliceCount = 4u;
constexpr auto stackCount = 2u;

class BoxApp : public Luxko::Anuthur::D3D12App {
public:
	BoxApp(UINT width, UINT height, const std::wstring& name) :
		D3D12App(width, height, name) {}
	virtual ~BoxApp() {}
protected:
	virtual void OnInit() override;


	virtual void OnDestroy() override;


	virtual void OnMouseDown(WPARAM wParam, int x, int y) override;

	virtual void OnMouseUp(WPARAM wParam, int x, int y) override;


	virtual void OnMouseMove(WPARAM wParam, int x, int y) override;


	virtual void OnUpdate() override;


	virtual void OnRender() override;

	virtual bool OnEvent(MSG msg) override;

private:
	void InitializeSceneComponents();
	void InitializeRootSignature();
	void InitializePipelineState();
	void InitializeShaders();
	void InitializeBuffers();
	void BindResourceViews();

	Luxko::Anuthur::PerspecCamera							_mainCamera;
	Luxko::Anuthur::D3D12Helper::ShaderByteCode				_vertexShader;
	Luxko::Anuthur::D3D12Helper::ShaderByteCode				_pixelShader;
	Microsoft::WRL::ComPtr<ID3D12PipelineState>				_pipelineStateWireFrame;
	Microsoft::WRL::ComPtr<ID3D12PipelineState>				_pipelineStateNormal;
	Microsoft::WRL::ComPtr<ID3D12RootSignature>				_rootSignature;
	Microsoft::WRL::ComPtr<ID3DBlob>						_rootSignatureRaw;
	Microsoft::WRL::ComPtr<ID3D12Resource>					_verticePositionBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource>					_verticeAttributeBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource>					_IndiceBuffer;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>		_uploadBuffers;
	Microsoft::WRL::ComPtr<ID3D12Resource>					_cbBuffer;
	void*													_cbBufferMappedAddress;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>			_cbvHeap;
	FLOAT													_bgColor[4] = { 0.f, 0.3f, 0.1f, 1.f };
	Vertex													_Box[2 * (1 + sliceCount)*(stackCount + 1)];
	Luxko::Vector4f											_BoxColors[13];
	size_t													_cbBufferSize;
	UINT16													_Indice[12u * sliceCount*stackCount]; // 54
	bool													_lMouseDown = false;
	bool													_rMouseDown = false;
	bool													_mMouseDown = false;
	int														_mouseLastX;
	int														_mouseLastY;
	ID3D12PipelineState*									_pCurrentPS = nullptr;
	

};