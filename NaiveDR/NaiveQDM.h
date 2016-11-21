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
#include "DRPipeline.h"
#include "NaiveQuadtreeDisplacement.h"

using namespace Luxko;
using namespace Luxko::Anuthur;

class MouseKeyTracker {
public:
	MouseKeyTracker() {
		pKeyboard = nullptr;
		pMouse = nullptr;
	}

	void update() {
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

class LightControl {
public:
	using LightParams = DRP::LightPass::NaiveLights::LightParams;
	LightControl()noexcept {
		pMKTracker = nullptr;
		pLight = nullptr;
		hSpeed = 0.01f;
		vSpeed = 0.01f;
		forwardSpeed = 1.f;
		backwardSpeed = 1.f;
		leftSpeed = 1.f;
		rightSpeed = 1.f;
		upSpeed = 1.f;
		downSpeed = 1.f;

		using namespace DirectX;
		forwardKey = Keyboard::Up;
		backwardKey = Keyboard::Down;
		leftKey = Keyboard::Left;
		rightKey = Keyboard::Right;
		upKey = Keyboard::LeftControl;
		downKey = Keyboard::LeftShift;

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
		assert(pLight != nullptr);
		assert(pMKTracker);
		using namespace DirectX;
		bool updated = false;
		// update camera position
		auto& keyState = pMKTracker->keyboardState;
		auto translationDelta = Vector3DH(0.f, 0.f, 0.f);
		bool deltaChanged = false;
		if (keyState.IsKeyDown(forwardKey)) {
			translationDelta[0] += forwardSpeed;
			deltaChanged = true;
		}
		if (keyState.IsKeyDown(backwardKey)) {
			translationDelta[0] -= backwardSpeed;
			deltaChanged = true;
		}
		if (keyState.IsKeyDown(leftKey)) {
			translationDelta[2] += leftSpeed;
			deltaChanged = true;
		}
		if (keyState.IsKeyDown(rightKey)) {
			translationDelta[2] -= rightSpeed;
			deltaChanged = true;
		}
		if (keyState.IsKeyDown(upKey)) {
			translationDelta[1] += upSpeed;
			deltaChanged = true;
		}
		if (keyState.IsKeyDown(downKey)) {
			translationDelta[1] -= downSpeed;
			deltaChanged = true;
		}

		if (deltaChanged) {
			pLight->posW += translationDelta.AsVector4f();
			updated = true;
		}

		return updated;
	}


public:
	float hSpeed;
	float vSpeed;
	float forwardSpeed;
	float backwardSpeed;
	float leftSpeed;
	float rightSpeed;
	float upSpeed;
	float downSpeed;
	LightParams* pLight;
	MouseKeyTracker* pMKTracker;

	DirectX::Keyboard::Keys forwardKey;
	DirectX::Keyboard::Keys backwardKey;
	DirectX::Keyboard::Keys leftKey;
	DirectX::Keyboard::Keys rightKey;
	DirectX::Keyboard::Keys upKey;
	DirectX::Keyboard::Keys downKey;
	float lastTimeMs;
};

class NaiveQDMApp : public D3D12App {
public:
	struct RenderObject {
		MeshGeometry mesh;
		ComPtr<ID3D12Resource> materialCB;
		ComPtr<ID3D12Resource> diffuseMap;
		ComPtr<ID3D12Resource> normalMap;
		ComPtr<ID3D12Resource> qdMap;
		ComPtr<ID3D12DescriptorHeap> srvHeap;
	};
	
	NaiveQDMApp(UINT width, UINT height, const std::wstring& name, BOOL windowed = TRUE)
		: D3D12App(width, height, name, windowed) {

	}

	virtual ~NaiveQDMApp() = default;

protected:
	// overridden methods
	virtual void OnInit() override;

	virtual void OnDestroy() override;

	virtual void OnUpdate() override;

	virtual void OnRender() override;
private:
	PerspecCamera _mainCam;
	MouseKeyTracker _mkTracker;
	CameraControl _camControl;
	WTF::NaiveBpqdmGenerator _qdmGenerator;
	WTF::NBPQDM _qdm;
	int _sampleMode;
	DRP::LightPass::NaiveLights::PointLight _pointLight;
	DRP::LightPass::NaiveLights::DirectionalLight _directionalLight;
	DRP::LightPass::NaiveLights::SpotLight _spotLight;

	DRP::Common::CameraAttr _cameraAttr_cpu;
	D3D12Helper::UpdateBuffer<DRP::Common::CameraAttr> _cameraAttr_gpu;


	UINT64 _gBuffers[3];
	UINT64 _gDepthStencil;

	UINT64 _lightPassDH;
	UINT64 _dsvDH;
	UINT64 _rtvDH;

	using GBInput = WTF::NBPQDM::VSI;
	using MaterialsCB = WTF::NBPQDM::MaterialAttr;

	std::vector<RenderObject> _renderObjects; // vbd, ibd, material, diffuse map, normal map

	using LightParamsCB = DRP::LightPass::NaiveLights::LightParams;

	std::vector<std::pair<LightParamsCB, Microsoft::WRL::ComPtr<ID3D12Resource>>> _pointLights;
	std::vector<std::pair<LightParamsCB, Microsoft::WRL::ComPtr<ID3D12Resource>>> _spotLights;
	std::vector<std::pair<LightParamsCB, Microsoft::WRL::ComPtr<ID3D12Resource>>> _directionalLights;

	std::vector<std::pair<ID3D12GraphicsCommandList*, ID3D12CommandAllocator*>> _cmdObjs;

	LightControl _movingLight_control;
	LightParamsCB _movingLight_cpu;
	D3D12Helper::UpdateBuffer<LightParamsCB> _movingLight_gpu;
private:
	void RecordCmds(ID3D12GraphicsCommandList* pCmdlist, ID3D12CommandAllocator* pCmdAlloc);
	void ReadBackTo(ID3D12Resource* src, const wchar_t* filename,
		UINT subResource = 0);
	DirectX::Keyboard::KeyboardStateTracker _kbdst;
};