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
			PerspecCamera() :OrientedObject(),_n(1.f),_f(10.f),_r(1.6f),_t(.9f) {}
			PerspecCamera(float nearDistance, float farDistance, float rightDistance,
				float topDistance, const Vector3DH& lookDirection,
				const Vector3DH& upDirection, const Point3DH& position) :
				_n(nearDistance), _f(farDistance), _r(rightDistance), _t(topDistance),
				OrientedObject(Frame3DH(lookDirection, upDirection, position)) {}

			PerspecCamera(float nearDistance, float farDistance,
				float rightDistance, float topDistance,
				const Frame3DH& frame = Frame3DH::StandardRightHandFrame()) :
				_n(nearDistance), _f(farDistance), _r(rightDistance),
				_t(topDistance), OrientedObject(frame) {}

			static PerspecCamera FromHFOVAndAspectRatio(
				float nearDistance, float farDistance, float aspectRatio,
				float angle, const Vector3DH& lookDirection,
				const Vector3DH& upDirection, const Point3DH& position);

			static PerspecCamera FromHFOVAndAspectRatio(
				float nearDistance, float farDistance, float aspectRatio,
				float angle, const Frame3DH& frame = Frame3DH::StandardRightHandFrame());

			PerspecCamera(const PerspecCamera&) = default;
			PerspecCamera& operator=(const PerspecCamera&) = default;
			virtual ~PerspecCamera() {}

			// As this is a DirectX style camera, the view transform
			// returns coordinates in a left-handed frame.
			Transform3DH TransformWtoV()const;
			Transform3DH TransformVtoH()const;
			Transform3DH TransformWtoH()const;
			Point3DH ApplyHomoClipTransform(const Point3DH& p) {
				auto r = TransformWtoH()*p;
				return r.HomogenizeInPlace();
			}

			float NearPlaneDistance()const noexcept { return _n; }
			float FarPlaneDistance()const noexcept { return _f; }
			float HorizontialFOV()const noexcept { return 2 * std::atan(_r / _n); }
			float VerticalFOV()const noexcept { return 2 * std::atan(_t / _n); }
			float AspectRatio()const noexcept { return _r / _t; }

		private:
			// Data members
			float _n; // Distance from the near plane.
			float _f; // Distance from the far plane.
			float _r; // Distance from the right edge of the near plane to the yz-plane
			float _t; // Distance from the top edge of the near plane to the xz-plane


		};
	}
}