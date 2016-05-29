//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************
#pragma once
#include "RendererPCH.h"
#include "HelperMethods.h"
#include "Vector3DH.h"



namespace Luxko {
	namespace Anuthur {
		class ANUTHURRENDERER_API BasicGeometry {
		public:
			struct ANUTHURRENDERER_API Vertex {
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


				Point3DH	Pos;
				Vector3DH	Norm;
				Vector3DH	TangentU;
				Vector4f	Texture;
			};

			struct ANUTHURRENDERER_API Mesh {
				std::vector<Vertex>	Vertices;
				std::vector<UINT>	Indices;
				const std::vector<UINT16>& GetIndices16Bit()const {
					_indices16.reserve(Indices.size());
					for (auto i = 0u; i < Indices.size(); ++i) {
						_indices16.push_back(static_cast<UINT16>(Indices[i]));
					}
					return _indices16;
				}

				void ReviseNormal();

			private:
				mutable std::vector<UINT16> _indices16;
			};

			// The following methods creates and returns Meshes as const objects.
			static const Mesh Box(float width, float height, float length, const float* textureCoordinates = nullptr);
			static const Mesh Cylinder(float bRadius, float tRadius, float height, size_t stackCount = 8u, size_t sliceCount = 8u, const float* textureCoordinates = nullptr);
			static const Mesh Sphere(float radius, size_t stackCount = 8u, size_t sliceCount = 8u, const float* textureCoordinates = nullptr);

			static const Mesh Grid(float width, float length, UINT m, UINT n);
			static const Mesh Terran(float width, float length, UINT m, UINT n, float heightCoefficient = 2.f);
			
			static void ReviceMeshNormal(Mesh& m);

			// nx, ny \in [-1, 1]
			// m, n marks rounds
			static float GetTerranHeight(float nx, float nz, float m, float n);
		};
	}
}
