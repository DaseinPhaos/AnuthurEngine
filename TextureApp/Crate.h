//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************
#pragma once
#include "D3D12App.h"
#include "HelperMethods.h"
#include "Matrix4x4f.h"
#include "PerspecCamera.h"
using namespace Luxko;
using namespace Luxko::Anuthur;

constexpr auto ShaderPath = LR"(..\Src\HLSLShaders\crate.hlsl)";
constexpr auto TextureDir = LR"(..\Asset\Textures\)";

struct BasicLight
{
	Vector3f	Pos;
	float		FallStart;
	Vector3f	Irradiance;
	float		FallEnd;
	Vector3f	Direction;
	float		Power;
};

struct LightPack
{
	BasicLight Directional;
	BasicLight PointLinear;
	BasicLight PointQuadra;
	BasicLight Spotlight;
	Vector4f AmbientLight;
};
//constexpr auto sve3 = sizeof(Vector3f);
//constexpr auto ssb = sizeof(BasicLight);
//constexpr auto ss = sizeof(LightPack);

struct CameraData {
	Matrix4x4f WCTransform;
	Matrix4x4f WHTransform;
	Vector4f Position;
};

struct ComponentTransformationData {
	Matrix4x4f CrateLW;
	Matrix4x4f NormalLW;
};

struct Vertex {
	Vertex(const Vector4f& pos, const Vector3f& normal, float x, float y)
		:Position(pos), Normal(normal), tx(x), ty(y) {}
	Vector4f Position;
	Vector3f Normal;
	float tx;
	float ty;
};

struct FRDirtyCounts
{
	UINT Camera;
	UINT Light;
	UINT MeshTransfrom;
	UINT MeshMaterial;
};

struct BlinnPhongMaterial
{
	Vector3f	R0;
	float		Roughness;
	Vector3f	DiffuseAlbedo;
	//float offset;
};



//class CrateFrameResource : public FrameResource {
//public:
//	explicit CrateFrameResource(ID3D12Device* pDevice)
//		: FrameResource(pDevice), _cameraData(pDevice),
//		_crateLWTransform(pDevice), _lightPack(pDevice),
//		_material(pDevice) {
//		ThrowIfFailed(pDevice->CreateDescriptorHeap(
//			&D3D12Helper::DescriptorHeapDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
//				5u, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE),
//			IID_PPV_ARGS(_CBVheap.GetAddressOf())));
//	}
//	
//	CrateFrameResource(CrateFrameResource&& fr)
//		:FrameResource(std::move(fr)) {
//		_cameraData = std::move(fr._cameraData);
//		_crateLWTransform = std::move(fr._crateLWTransform);
//		_lightPack = std::move(fr._lightPack);
//		_material = std::move(fr._material);
//	}
//
//	CrateFrameResource& operator=(CrateFrameResource&& fr) {
//		FrameResource::operator =(std::move(fr));
//		_cameraData = std::move(fr._cameraData);
//		_crateLWTransform = std::move(fr._crateLWTransform);
//		_lightPack = std::move(fr._lightPack);
//		_material = std::move(fr._material);
//	}
//
//	virtual ~CrateFrameResource() {	}
//
//	void UpdateCamera(const PerspecCamera& pc) {
//		CameraData _cameraDataCPU;
//		_cameraDataCPU.Position = pc.GetPosition().AsVector4f();
//		_cameraDataCPU.WCTransform = pc.TransformWtoO().AsMatrix4x4();
//		_cameraDataCPU.WHTransform = pc.TransformWtoH().AsMatrix4x4();
//		_cameraData.Update(_cameraDataCPU);
//	}
//
//	void UpdateCrateTransform(const Matrix4x4f& t) {
//		ComponentTransformationData ctd;
//		ctd.CrateLW = t;
//		ctd.NormalLW = t.Inverse().Transpose();
//		_crateLWTransform.Update(ctd);
//	}
//
//	void UpdateLightPack(const LightPack& l) {
//		_lightPack.Update(l);
//	}
//
//	void UpdateMaterial(const BlinnPhongMaterial& v) {
//		_material.Update(v);
//	}
//
//	D3D12Helper::UpdateBuffer<CameraData>					_cameraData;
//	D3D12Helper::UpdateBuffer<LightPack>					_lightPack;
//	D3D12Helper::UpdateBuffer<ComponentTransformationData>	_crateLWTransform;
//	D3D12Helper::UpdateBuffer<BlinnPhongMaterial>			_material;
//
//	// contain a CBV for camera data, a CBV for lights,
//	// a CBV for meshLW transform, a CBV for mesh material(part of).
//	ComPtr<ID3D12DescriptorHeap>									_CBVheap;
//};

class CrateFrameResource : public FrameResource {
public:
	explicit CrateFrameResource(ID3D12Device* pDevice)
		: FrameResource(pDevice), _cameraData(pDevice),_lightPack(pDevice)
	{
		ThrowIfFailed(pDevice->CreateDescriptorHeap(
			&D3D12Helper::DescriptorHeapDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
				3u, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE),
			IID_PPV_ARGS(_CBVheap.GetAddressOf())));
	}

	CrateFrameResource(CrateFrameResource&& fr)
		:FrameResource(std::move(fr)) {
		_cameraData = std::move(fr._cameraData);
		_lightPack = std::move(fr._lightPack);
	}

	CrateFrameResource& operator=(CrateFrameResource&& fr) {
		FrameResource::operator =(std::move(fr));
		_cameraData = std::move(fr._cameraData);
		_lightPack = std::move(fr._lightPack);
	}

	virtual ~CrateFrameResource() {	}

	void UpdateCamera(const PerspecCamera& pc) {
		CameraData _cameraDataCPU;
		_cameraDataCPU.Position = pc.GetPosition().AsVector4f();
		_cameraDataCPU.WCTransform = pc.TransformWtoO().AsMatrix4x4();
		_cameraDataCPU.WHTransform = pc.TransformWtoH().AsMatrix4x4();
		_cameraData.Update(_cameraDataCPU);
	}

	void UpdateLightPack(const LightPack& l) {
		_lightPack.Update(l);
	}

	D3D12Helper::UpdateBuffer<CameraData>					_cameraData;
	D3D12Helper::UpdateBuffer<LightPack>					_lightPack;


	// contain a CBV for camera data, a CBV for lights,
	// a SRV
	ComPtr<ID3D12DescriptorHeap>									_CBVheap;
};

class CrateApp : public D3D12App {
public:

	CrateApp(UINT width, UINT height, const std::wstring& name) :
		D3D12App(width, height, name),
		_crateLWTransform(Transform3DH::Translation(0.f, 5.f, 0.f)),
		_gridLWTransform(Transform3DH::Translation(0.f, -5.f, 0.f))	{}
	virtual ~CrateApp() {}


protected:

	virtual void OnInit() override;
	virtual void OnUpdate() override;
	virtual void OnRender() override;
	virtual void OnMouseDown(WPARAM wParam, int x, int y) override;
	virtual void OnMouseUp(WPARAM wParam, int x, int y) override;
	virtual void OnMouseMove(WPARAM wParam, int x, int y) override;

private:
	void InitializeSceneComponents();
	void InitializeFrameResources();
	void InitializeShaders();
	void InitializeRootSignature();
	void InitializePSOs();
	void InitializeDescriptors();
	void InitializeTextures();

	// Scene components, may be updated in response to the user input per frame.
	PerspecCamera													_mainCamera;
	LightPack														_lights;
	Transform3DH													_crateLWTransform;
	Transform3DH													_gridLWTransform;
	
	// crate mesh data
	BlinnPhongMaterial												_crateMaterial;
	D3D12Helper::MeshGeometry										_crateMesh;
	D3D12_INDEX_BUFFER_VIEW											_crateIBV;
	D3D12_VERTEX_BUFFER_VIEW										_crateVBV;
	// grid mesh data
	BlinnPhongMaterial												_gridMaterial;
	D3D12Helper::MeshGeometry										_gridMesh;
	D3D12_INDEX_BUFFER_VIEW											_gridIBV;
	D3D12_VERTEX_BUFFER_VIEW										_gridVBV;

	// crate texture resource
	ComPtr<ID3D12Resource>											_crateTextureGPU;
	ComPtr<ID3D12DescriptorHeap>									_crateSRVUAVheap;
	ComPtr<ID3D12Resource>											_gridTextureGPU;
	ComPtr<ID3D12DescriptorHeap>									_gridSRVUAVheap;
	

	D3D12Helper::UpdateBuffer<ComponentTransformationData>			_crateTGPU;
	D3D12Helper::UpdateBuffer<BlinnPhongMaterial>					_crateMGPU;
	D3D12Helper::UpdateBuffer<ComponentTransformationData>			_gridTGPU;
	D3D12Helper::UpdateBuffer<BlinnPhongMaterial>					_gridMGPU;
	
	// pipeline resources
	ComPtr<ID3D12RootSignature>										_rootSignature;
	std::unordered_map<std::string, ComPtr<ID3D12PipelineState>>	_PSOs;
	ID3D12PipelineState*											_currentPSO;
	std::unordered_map<std::string, D3D12Helper::ShaderByteCode>	_shaders;
	// frame resources
	std::vector<CrateFrameResource>									_frameResources;
	FRDirtyCounts													_frDirtyCounts;
	size_t															_currentFRindex;

	// mouse control
	//Point3DH _pivot;
	int _lastMousePosX;
	int _lastMousePosY;
	bool _lbDown = false;

};