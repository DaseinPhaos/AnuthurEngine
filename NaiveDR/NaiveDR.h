//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************
#pragma once
#include "D3D12App.h"
#include "D3D12HelperMethods.h"
#include "Matrix4x4f.h"
#include "Vector4f.h"
#include "Vector3f.h"
#include "Point3DH.h"
#include "PerspecCamera.h"
#include "Transform3DH.h"
// Sample project using a naive implementation of deferred rendering.
using namespace Luxko;
using namespace Luxko::Anuthur;
struct TransformsCB {
	Matrix4x4f mWorld;
	Matrix4x4f mWorldView;
	Matrix4x4f mWorldViewProj;
};

struct MaterialsCB {
	float specularAlbedo[3];
	float specularPower;
	
};

struct LightParamsCB {
	Vector4f pos;
	Vector4f color;
	Vector4f direction;
	Vector4f range;
	float angle[2];
};

struct CameraCB {
	Vector3f pos;
};

struct GBInput {
	Point3DH pos;
	Vector3f norm;
	Vector3f tangent;
	float u;
	float v;
};

struct OMBindings {
	D3D12_GPU_VIRTUAL_ADDRESS transformsCB;
	D3D12_CPU_DESCRIPTOR_HANDLE* rtvStart;
	D3D12_CPU_DESCRIPTOR_HANDLE* dsv;
	D3D12_RECT* rectStart;
	UINT rectCount;
	D3D12_VIEWPORT* viewportStart;
	UINT viewportCount;
};

struct GBPassBindings {
	// D3D12_GPU_VIRTUAL_ADDRESS transformsCB;
	D3D12_GPU_VIRTUAL_ADDRESS materialsCB;
	D3D12_GPU_DESCRIPTOR_HANDLE mapsDH;
	//D3D12_GPU_VIRTUAL_ADDRESS diffuseMap;
	//D3D12_GPU_VIRTUAL_ADDRESS normalMap;
	D3D12_VERTEX_BUFFER_VIEW* vbvs;
	UINT vbv_count;
	D3D12_INDEX_BUFFER_VIEW* ibv;
	UINT index_count;
};

struct LightPassBindings
{
	D3D12_GPU_VIRTUAL_ADDRESS lights;
};

struct LightPassMaskVB
{
	Vector3f poses[6];
};

class DeferredRenderingRuntime {
public:
	DeferredRenderingRuntime() = default;

	~DeferredRenderingRuntime() = default;

	void initialize(Anuthur::D3D12Manager& manager, DXGI_FORMAT targetRTF);

	inline ID3D12PipelineState* getSpotlightPSO()const {
		return _pManager->FindPSO(_lpSpotlightPSO).Get();
	}

	inline ID3D12PipelineState* getPointlightPSO()const {
		return _pManager->FindPSO(_lpPointlightPSO).Get();
	}

	inline ID3D12PipelineState* getDirectionallightPSO()const {
		return _pManager->FindPSO(_lpDirectionalPSO).Get();
	}

	inline ID3D12PipelineState* getGBNormalPSO()const {
		return _pManager->FindPSO(_gbNormalPSO).Get();
	}

	inline ID3D12PipelineState* getGBWireframePSO()const {
		return _pManager->FindPSO(_gbWireframePSO).Get();
	}

	inline ID3D12RootSignature* getGBRootSignature()const {
		return _pManager->FindRootSignature(_gbRootSignature).Get();
	}

	inline ID3D12RootSignature* getLPRootSignature()const {
		return _pManager->FindRootSignature(_lpRootSignature).Get();
	}


	// resource barriers should be set before calling this method.
	void recordGBPass(ID3D12GraphicsCommandList* pCmdlist,
		const OMBindings& gboms,
		GBPassBindings* bindingStart,
		unsigned int bindingCount = 1u)const;

	// resource barriers should be set before calling this method.
	void recordLightPass(ID3D12GraphicsCommandList* pCmdlist,
		const OMBindings& lpoms,
		D3D12_VERTEX_BUFFER_VIEW* maskVbv,
		ID3D12DescriptorHeap* gbvHeap,
		LightPassBindings* pointlights,
		unsigned int plCount = 1u,
		LightPassBindings* spotlights = nullptr,
		unsigned int slCount = 0u,
		LightPassBindings* directionalLights = nullptr,
		unsigned int dlCount = 0u
		)const;
public:
	static constexpr auto gbRSStr = "gbRSStr";
	static constexpr auto lpRSStr = "lpRSStr";
	static constexpr auto gbNormalPSOStr = "gbNormalPSOStr";
	static constexpr auto gbWireframePSOStr = "gbWireframePSOStr";
	static constexpr auto lpSpotlightPSOStr = "lpSpotlightPSOStr";
	static constexpr auto lpPointlightPSOStr = "lpPointlightPSOStr";
	static constexpr auto lpDirectonalPSOStr = "lpDirectionalPSOStr";


private:
	UINT64 _gbRootSignature;
	UINT64 _lpRootSignature;


	UINT64 _gbNormalPSO;
	UINT64 _gbWireframePSO;

	UINT64 _lpSpotlightPSO;
	UINT64 _lpPointlightPSO;
	UINT64 _lpDirectionalPSO;


	UINT64 _gbVS;
	UINT64 _gbPS;

	UINT64 _lpVS;
	UINT64 _lpSpotlightPS;
	UINT64 _lpPointlightPS;
	UINT64 _lpDirectionalPS;

	Anuthur::D3D12Manager* _pManager;

	static constexpr auto gbShaderPath = LR"(F:\Projects\Anuthur\AnuthurEngine\NaiveDR\dsn.hlsl)";
	static constexpr auto lpShaderPath = LR"(F:\Projects\Anuthur\AnuthurEngine\NaiveDR\dsl.hlsl)";
};

class MouseKeyTracker {
public:
	MouseKeyTracker() {
		pKeyboard = nullptr;
		pMouse = nullptr;
	}

	void update(){
		assert(pKeyboard);
		assert(pMouse);
		keyboardState = pKeyboard->GetState();
		mouseState = pMouse->GetState();
		keyboardStateTracker.Update(keyboardState);
		mouseStateTracker.Update(mouseState);
	}

public:
	DirectX::Keyboard* pKeyboard;
	DirectX::Mouse* pMouse;
	DirectX::Keyboard::State keyboardState;
	DirectX::Mouse::State mouseState;
	DirectX::Keyboard::KeyboardStateTracker keyboardStateTracker;
	DirectX::Mouse::ButtonStateTracker mouseStateTracker;
};

class CameraControl {
public:
	CameraControl()noexcept {
		pMKTracker = nullptr;
		pCamera = nullptr;
		hSpeed = 0.01f;
		vSpeed = 0.01f;
		forwardSpeed = 1.f;
		backwardSpeed = 1.f;
		leftSpeed = 1.f;
		rightSpeed = 1.f;

		using namespace DirectX;
		forwardKey = Keyboard::W;
		backwardKey = Keyboard::S;
		leftKey = Keyboard::A;
		rightKey = Keyboard::D;

		lastTimeMs = 0.f;
	}

	bool Update(float timeDeltaMs) {
		lastTimeMs += timeDeltaMs;
		if (lastTimeMs > 20.f) {
			lastTimeMs = 0.f;
			pMKTracker->update();
			return DoUpdate();
			//return DoUpdate(timeDeltaMs / 20.f);
		}
		return false;

	}

private:
	bool DoUpdate() {
		assert(pCamera != nullptr);
		assert(pMKTracker);
		using namespace DirectX;
		bool cameraUpdated = false;

		// update view direction
		auto& mouseState = pMKTracker->mouseState;
		if (mouseState.positionMode == DirectX::Mouse::MODE_RELATIVE) {
			auto deltaH = -hSpeed * mouseState.x;
			auto deltaV = -vSpeed * mouseState.y;

			auto rotation = Transform3DH::RotationN(pCamera->GetRightDirection(), deltaV)
				* Transform3DH::RotationN(pCamera->GetUpDirection(), deltaH);
			pCamera->_orientation.ApplyTransformOnOrientation(rotation);
			cameraUpdated = true;
		}

		// update camera position
		auto& keyState = pMKTracker->keyboardState;
		auto translationDelta = Vector3DH(0.f, 0.f, 0.f);
		bool deltaChanged = false;
		if (keyState.IsKeyDown(forwardKey)) {
			translationDelta += pCamera->GetLookDirection()*forwardSpeed;
			deltaChanged = true;
		}
		if (keyState.IsKeyDown(backwardKey)) {
			translationDelta -= pCamera->GetLookDirection()*backwardSpeed;
			deltaChanged = true;
		}
		if (keyState.IsKeyDown(leftKey)) {
			translationDelta -= pCamera->GetRightDirection()*leftSpeed;
			deltaChanged = true;
		}
		if (keyState.IsKeyDown(rightKey)) {
			translationDelta += pCamera->GetRightDirection()*rightSpeed;
			deltaChanged = true;
		}

		if (deltaChanged) {
			pCamera->ApplyTransform(Transform3DH::Translation(translationDelta));
			cameraUpdated = true;
		}

		
		// update mouse state
		auto lb = pMKTracker->mouseStateTracker.leftButton;
		if (lb == Mouse::ButtonStateTracker::PRESSED) {
			pMKTracker->pMouse->SetMode(Mouse::MODE_RELATIVE);
		}
		else if (lb == Mouse::ButtonStateTracker::RELEASED) {
			pMKTracker->pMouse->SetMode(Mouse::MODE_ABSOLUTE);
		}

		return cameraUpdated;
	}

	bool DoUpdate(float r) {
		assert(pCamera != nullptr);
		assert(pMKTracker);
		using namespace DirectX;
		bool cameraUpdated = false;

		// update view direction
		auto& mouseState = pMKTracker->mouseState;
		if (mouseState.positionMode == DirectX::Mouse::MODE_RELATIVE) {
			auto deltaH = -hSpeed * mouseState.x * r;
			auto deltaV = -vSpeed * mouseState.y * r;

			auto rotation = Transform3DH::RotationN(pCamera->GetRightDirection(), deltaV)
				* Transform3DH::RotationN(pCamera->GetUpDirection(), deltaH);
			pCamera->_orientation.ApplyTransformOnOrientation(rotation);
			cameraUpdated = true;
		}

		// update camera position
		auto& keyState = pMKTracker->keyboardState;
		auto translationDelta = Vector3DH(0.f, 0.f, 0.f);
		bool deltaChanged = false;
		if (keyState.IsKeyDown(forwardKey)) {
			translationDelta += pCamera->GetLookDirection()*(forwardSpeed*r);
			deltaChanged = true;
		}
		if (keyState.IsKeyDown(backwardKey)) {
			translationDelta -= pCamera->GetLookDirection()*(backwardSpeed*r);
			deltaChanged = true;
		}
		if (keyState.IsKeyDown(leftKey)) {
			translationDelta -= pCamera->GetRightDirection()*(leftSpeed*r);
			deltaChanged = true;
		}
		if (keyState.IsKeyDown(rightKey)) {
			translationDelta += pCamera->GetRightDirection()*(rightSpeed*r);
			deltaChanged = true;
		}

		if (deltaChanged) {
			pCamera->ApplyTransform(Transform3DH::Translation(translationDelta));
			cameraUpdated = true;
		}


		// update mouse state
		auto lb = pMKTracker->mouseStateTracker.leftButton;
		if (lb == Mouse::ButtonStateTracker::PRESSED) {
			pMKTracker->pMouse->SetMode(Mouse::MODE_RELATIVE);
		}
		else if (lb == Mouse::ButtonStateTracker::RELEASED) {
			pMKTracker->pMouse->SetMode(Mouse::MODE_ABSOLUTE);
		}

		return cameraUpdated;
	}

public:
	float hSpeed;
	float vSpeed;
	float forwardSpeed;
	float backwardSpeed;
	float leftSpeed;
	float rightSpeed;
	PerspecCamera* pCamera;
	MouseKeyTracker* pMKTracker;

	DirectX::Keyboard::Keys forwardKey;
	DirectX::Keyboard::Keys backwardKey;
	DirectX::Keyboard::Keys leftKey;
	DirectX::Keyboard::Keys rightKey;
	float lastTimeMs;
};

class NaiveApp : public D3D12App {
public:
	NaiveApp(UINT width, UINT height, const std::wstring& name, BOOL windowed = TRUE)
		: D3D12App(width, height, name, windowed) {

	}

	virtual ~NaiveApp() = default;

protected:
	// overridden methods
	virtual void OnInit() override;

	virtual void OnDestroy() override;

	virtual bool OnEvent(MSG msg) override;

	virtual void OnUpdate() override;

	virtual void OnRender() override;
private:
	PerspecCamera _mainCam;
	MouseKeyTracker _mkTracker;
	CameraControl _camControl;
	DeferredRenderingRuntime _drr;

	TransformsCB _transformCB_cpu;
	D3D12Helper::UpdateBuffer<TransformsCB> _transformCB_gpu;

	CameraCB _cameraCB;
	D3D12Helper::UpdateBuffer<CameraCB> _cameraCB_gpu;

	LightPassMaskVB _lpmVB;
	D3D12Helper::UpdateBuffer<LightPassMaskVB> _lpmVB_gpu;
	D3D12_VERTEX_BUFFER_VIEW _lpmVBV;

	//ComPtr<ID3D12Resource> _gBuffers[4];
	UINT64 _gBuffers[4];
	// ComPtr<ID3D12Resource> _gDepthStencil;
	UINT64 _gDepthStencil;
	// UINT64 _gbDH;
	UINT64 _lightPassDH;
	UINT64 _dsvDH;
	UINT64 _rtvDH;

	std::vector<std::tuple<
		std::vector<GBInput>,
		std::vector<UINT>, 
		MaterialsCB>> _meshes_cpu;
	std::vector<std::tuple<
		ComPtr<ID3D12Resource>,
		ComPtr<ID3D12Resource>,
		ComPtr<ID3D12Resource>,
		ComPtr<ID3D12Resource>, 
		ComPtr<ID3D12Resource>,
		ComPtr<ID3D12DescriptorHeap>>> _meshes_gpu; // vbd, ibd, material, diffuse map, normal map

	std::vector<std::pair<LightParamsCB, Microsoft::WRL::ComPtr<ID3D12Resource>>> _pointLights;
	std::vector<std::pair<LightParamsCB, Microsoft::WRL::ComPtr<ID3D12Resource>>> _spotLights;
	std::vector<std::pair<LightParamsCB, Microsoft::WRL::ComPtr<ID3D12Resource>>> _directionalLights;

	std::vector<std::pair<ID3D12GraphicsCommandList*, ID3D12CommandAllocator*>> _cmdObjs;

private:
	void RecordCmds(ID3D12GraphicsCommandList* pCmdlist, ID3D12CommandAllocator* pCmdAlloc);
	void ReadBackTo(ID3D12Resource* src, const wchar_t* filename);
	DirectX::Keyboard::KeyboardStateTracker _kbdst;
	//static constexpr auto _cratemRName = "cratem";
	//static constexpr auto _cratetRName = "cratet";
	//static constexpr auto _gridmRName = "gridm";
	//static constexpr auto _gridtRName = "gridt";

	//static constexpr auto _slRName = "spotlight";
	//static constexpr auto _plaRName = "pointlightA";
	//static constexpr auto _plbRName = "pointlightB";
	//static constexpr auto _dlaRName = "DirectionalA";
	//static constexpr auto _dlbRName = "DirectionalB";


};