//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************
#pragma once
#include "D3D12App.h"
#include "D3D12HelperMethods.h"
#include "PerspecCamera.h"
#include <unordered_map>
#include "Transform3DH.h"
#include "Vector3DH.h"
#include "Point3DH.h"


namespace Luxko {
	namespace Anuthur {
		namespace Control {
			class MouseKeyTracker {
			public:
				MouseKeyTracker() {
					pKeyboard = nullptr;
					pMouse = nullptr;
				}

				void initialize(DirectX::Keyboard* pk, DirectX::Mouse* pm) {
					pKeyboard = pk;
					pMouse = pm;
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

				void initialize(MouseKeyTracker* pmk, PerspecCamera* pcam) {
					pMKTracker = pmk;
					pCamera = pcam;
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
		}
	}
}