//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************
#pragma once
#include "HelperMethods.h"
#include "D3D12App.h"
#include "Matrix4x4f.h"
#include "PerspecCamera.h"
#include "MeshResource.h"
#include "LightandMaterial.h"

using namespace Luxko;
using namespace Luxko::Anuthur;

struct Vertex {
	Vertex(const Point3DH& p, const Vector3DH& v)
	{
		Position = p;
		normal = v.AsVector4f().xyz();
	}

	Vertex(const Point3DH& p, const Vector4f& v) {
		Position = p;
		normal = v.xyz();
	}
	Point3DH Position;
	Vector3f normal;
};
struct LightPack {
	struct BasicLight
	{
		Vector4f	PosAndFallStart;
		Vector4f	IrradianceAndFallEnd;
		Vector4f	DirectionAndPower;
	};
	BasicLight directional;
	BasicLight pointLinear;
	BasicLight pointQuadra;
	BasicLight spotlight;
};

class ShapeFrameResource : public FrameResource {
public:
	ShapeFrameResource(ID3D12Device* device);

	virtual ~ShapeFrameResource();
	void UpdateCam(const PerspecCamera& newCamera);

	// Matrix4x4f _whTransform;
	Microsoft::WRL::ComPtr<ID3D12Resource>	_whTransformGPU;
	BYTE*									_pwhTransformGPU;
	D3D12Helper::UpdateBuffer<LightPack>	_lightsGPU;
};

class ShapeApp : public D3D12App {
public:


	ShapeApp(UINT width, UINT height, const std::wstring& name) :
		D3D12App(width, height, name) {}
	ShapeApp(const ShapeApp&) = delete;
	ShapeApp& operator=(const ShapeApp&) = delete;
	virtual ~ShapeApp() {}

protected:
	virtual void OnInit() override;
	virtual void OnRender() override;
	virtual void OnUpdate() override;
	virtual void OnMouseDown(WPARAM wParam, int x, int y) override;
	virtual void OnMouseMove(WPARAM wParam, int x, int y) override;
	virtual void OnMouseUp(WPARAM wParam, int x, int y) override;
private:
	void InitializeSceneComponents();
	void InitializeShaders();
	void InitializePSOs();
	void InitializeFrameResources();
	void InitializeRootSignatures();
	void InitializeDescriptors();

	PerspecCamera																	_mainCamera;
	std::unordered_map<std::string, Matrix4x4f>										_sceneTransforms;
	//std::unordered_map<std::string, D3D12Helper::UpdateBuffer<Matrix4x4f>>			_scTransformsGPU;
	std::unordered_map<std::string, ComPtr<ID3D12Resource>>							_scTransformsGPU;
	std::unordered_map<std::string, MeshGeometry>									_geometrys;
	std::unordered_map<std::string, BlinnPhongMaterial>								_geometryMaterials;
	//std::unordered_map<std::string, D3D12Helper::UpdateBuffer<BlinnPhongMaterial>>	_geometryMaterialsGPU;
	std::unordered_map<std::string, ComPtr<ID3D12Resource>>							_geometryMaterialsGPU;
	// packed light resource, 1 directional, 1 pointLinear, 1 pointQuadra, 1 spotlight
	LightPack																		_lights;
	//D3D12Helper::UpdateBuffer<LightPack>											_lightsGPU;

	std::unordered_map<std::string, D3D12Helper::ShaderByteCode>					_shaders;
	std::unordered_map<std::string, ComPtr<ID3D12PipelineState>>					_PSOs;
	ID3D12PipelineState*															_currentPSO;
	//std::vector<ComPtr<ID3DBlob>>													_rootSignaturesRaw;
	ComPtr<ID3D12RootSignature>														_rootSignature;
	std::vector<std::unique_ptr<ShapeFrameResource>>								_frameResources;
	UINT																			_currentFRIndex = 0u;
	UINT																			_frDirtyCount;
	bool																			_lMouseDown = false;
	bool																			_rMouseDown = false;
	bool																			_mMouseDown = false;
	int																				_mouseLastX = 0;
	int																				_mouseLastY = 0;
};