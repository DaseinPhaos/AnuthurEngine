//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************
#pragma once
#include "RendererPCH.h"
#include "OrientedObject.h"


namespace Luxko {
	namespace Anuthur {
		
		/// This class defines a DirectX style camera.
		class ANUTHURRENDERER_API PerspecCamera : public OrientedObject {
		public:
			PerspecCamera() :OrientedObject() {}
			PerspecCamera(float nearDistance, float farDistance, float rightDistance,
				float topDistance, const Vector3DH& lookDirection,
				const Vector3DH& upDirection, const Point3DH& position) :
				_n(nearDistance), _f(farDistance), _r(rightDistance), _t(topDistance),
				OrientedObject(Frame3DH(lookDirection, upDirection, position)) {}

			PerspecCamera(float nearDistance, float farDistance,
				float rightDistance, float topDistance, const Frame3DH& frame) :
				_n(nearDistance), _f(farDistance), _r(rightDistance),
				_t(topDistance), OrientedObject(frame) {}

			static PerspecCamera FromAngleAndAspectRatio(
				float nearDistance, float farDistance, float aspectRatio,
				float angle, const Vector3DH& lookDirection,
				const Vector3DH& upDirection, const Point3DH& position) {
				if (angle >= M_PI_2) {
					angle /= 2.f;
				}
				auto r = nearDistance / std::tan(angle);
				auto t = r / aspectRatio;
				return PerspecCamera(nearDistance, farDistance, r, t,
					lookDirection, upDirection, position);
			}

			static PerspecCamera FromAngleAndAspectRatio(
				float nearDistance, float farDistance, float aspectRatio,
				float angle, const Frame3DH& frame) {
				if (angle >= M_PI_2) {
					angle /= 2.f;
				}
				auto r = nearDistance / std::tan(angle);
				auto t = r / aspectRatio;
				return PerspecCamera(nearDistance, farDistance, r, t, frame);
			}

			PerspecCamera(const PerspecCamera&) = default;
			PerspecCamera& operator=(const PerspecCamera&) = default;
			virtual ~PerspecCamera() {}

			// As this is a DirectX style camera, the view transform
			// returns coordinates in a left-handed frame.
			Transform3DH TransformWtoV()const { return _Orientation.GetTransformLH(); }
			Transform3DH TransformVtoH()const {
				return Transform3DH::HomoClipD3D(_n, _f, _r, _t);
			}
			Transform3DH TransformWtoH()const {
				return TransformVtoH()*TransformWtoV();
			}
			Point3DH ApplyHomoClipTransform(const Point3DH& p) {
				auto r = TransformWtoH()*p;
				return r.HomogenizeInPlace();
			}

			// Data members
			float _n; // Distance from the near plane.
			float _f; // Distance from the far plane.
			float _r; // Distance from the right edge of the near plane to the yz-plane
			float _t; // Distance from the top edge of the near plane to the xz-plane


		};
	}
}