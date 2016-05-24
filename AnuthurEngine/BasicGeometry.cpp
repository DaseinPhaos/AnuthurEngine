//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************
#include "BasicGeometry.h"

Luxko::Anuthur::BasicGeometry::Vertex::Vertex(const Point3DH& position, const Vector3DH& normal, const Vector3DH& tangentU, float tx, float ty, float tz /*= 0.f*/, float tw /*= 0.f*/)
	:Pos(position), Norm(normal), TangentU(tangentU), Texture(tx, ty, tz, tw) {}

Luxko::Anuthur::BasicGeometry::Vertex::Vertex(float px, float py, float pz, float nx, float ny, float nz, float tux, float tuy, float tuz, float tx, float ty, float tz /*= 0.f*/, float tw /*= 0.f*/)
	: Pos(px, py, pz), Norm(nx, ny, nz), TangentU(tux, tuy, tuz), Texture(tx, ty, tz, tw) { }

const Luxko::Anuthur::BasicGeometry::Mesh Luxko::Anuthur::BasicGeometry::Box(float width, float height, float length, const float* textureCoordinates /*= nullptr*/)
{
	auto x = width / 2.f;
	auto y = height / 2.f;
	auto z = length / 2.f;

	static float defualtTextureCoordinates[] = {
		// Side A, clockwise
		0.f, 0.4f, 0.f, 0.f, 0.3f, 0.f, 0.3f, 0.4f,
		// Side B, clockwise
		0.35f, 0.4f, 0.35f, 0.f, 0.65f, 0.f, 0.65f, 0.4f,
		// Side C, clockwise
		0.7f, 0.4f, 0.7f, 0.f, 1.0f, 0.f, 1.0f, 0.4f,
		// Side D, clockwise
		0.f, 1.0f, 0.f, 0.6f, 0.3f, 0.6f, 0.3f, 1.0f,
		// Side E, clockwise
		0.35f, 1.0f, 0.35f, 0.6f, 0.65f, 0.6f, 0.65f, 1.0f,
		// Side F, clockwise
		0.7f, 1.0f, 0.7f, 0.6f, 1.0f, 0.6f, 1.0f, 1.0f,
	};
	if (textureCoordinates == nullptr) {
		textureCoordinates = defualtTextureCoordinates;
	}

	auto result = Mesh();
	result.Vertices = {
		// Side A, clockwise
		Vertex(-x, -y, z, 0.f, 0.f, 1.f, -1.f, 0.f, 0.f, textureCoordinates[0], textureCoordinates[1]),
		Vertex(-x, y, z, 0.f, 0.f, 1.f, -1.f, 0.f, 0.f, textureCoordinates[2], textureCoordinates[3]),
		Vertex(x, y, z, 0.f, 0.f, 1.f, -1.f, 0.f, 0.f, textureCoordinates[4], textureCoordinates[5]),
		Vertex(x, -y, z, 0.f, 0.f, 1.f, -1.f, 0.f, 0.f, textureCoordinates[6], textureCoordinates[7]),

		// Side B, clockwise
		Vertex(x, -y, z, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, textureCoordinates[8], textureCoordinates[9]),
		Vertex(x, y, z, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, textureCoordinates[10], textureCoordinates[11]),
		Vertex(x, y, -z, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, textureCoordinates[12], textureCoordinates[13]),
		Vertex(x, -y, -z, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, textureCoordinates[14], textureCoordinates[15]),

		// Side C, clockwise
		Vertex(x, -y, -z, 0.f, 0.f, -1.f, 1.f, 0.f, 0.f, textureCoordinates[16], textureCoordinates[17]),
		Vertex(x, y, -z, 0.f, 0.f, -1.f, 1.f, 0.f, 0.f, textureCoordinates[18], textureCoordinates[19]),
		Vertex(-x, y, -z, 0.f, 0.f, -1.f, 1.f, 0.f, 0.f, textureCoordinates[20], textureCoordinates[21]),
		Vertex(-x, -y, -z, 0.f, 0.f, -1.f, 1.f, 0.f, 0.f, textureCoordinates[22], textureCoordinates[23]),

		// Side D, clockwise
		Vertex(-x, -y, -z, -1.f, 0.f, 0.f, 0.f, 0.f, -1.f, textureCoordinates[24], textureCoordinates[25]),
		Vertex(-x, y, -z, -1.f, 0.f, 0.f, 0.f, 0.f, -1.f, textureCoordinates[26], textureCoordinates[27]),
		Vertex(-x, y, z, -1.f, 0.f, 0.f, 0.f, 0.f, -1.f, textureCoordinates[28], textureCoordinates[29]),
		Vertex(-x, -y, z, -1.f, 0.f, 0.f, 0.f, 0.f, -1.f, textureCoordinates[30], textureCoordinates[31]),

		// Side E, clockwise
		Vertex(x, y, z, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, textureCoordinates[32], textureCoordinates[33]),
		Vertex(-x, y, z, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, textureCoordinates[34], textureCoordinates[35]),
		Vertex(-x, y, -z, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, textureCoordinates[36], textureCoordinates[37]),
		Vertex(x, y, -z, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, textureCoordinates[38], textureCoordinates[39]),

		// Side F, clockwise
		Vertex(-x, -y, z, 0.f, -1.f, 0.f, -1.f, 0.f, 0.f, textureCoordinates[40], textureCoordinates[41]),
		Vertex(x, -y, z, 0.f, -1.f, 0.f, -1.f, 0.f, 0.f, textureCoordinates[42], textureCoordinates[43]),
		Vertex(x, -y, -z, 0.f, -1.f, 0.f, -1.f, 0.f, 0.f, textureCoordinates[44], textureCoordinates[45]),
		Vertex(-x, -y, -z, 0.f, -1.f, 0.f, -1.f, 0.f, 0.f, textureCoordinates[46], textureCoordinates[47])
	};
	result.Indices = {
		// Side A, offset = 0
		0, 1, 2, 0, 2, 3,
		// Side B, offset = 4
		4, 5, 6, 4, 6, 7,
		// Side C, offset = 8
		8, 9, 10, 8, 10, 11,
		// Side D, offset = 12
		12, 13, 14, 12, 14, 15,
		// Side E, offset = 16
		16, 17, 18, 16, 18, 19,
		// Side F, offset = 20
		20, 21, 22, 20, 22, 23
	};

	return result;
}

const Luxko::Anuthur::BasicGeometry::Mesh Luxko::Anuthur::BasicGeometry::Cylinder(float bRadius, float tRadius, float height, size_t stackCount /*= 8u*/, size_t sliceCount /*= 8u*/, const float* textureCoordinates /*= nullptr*/)
{
	auto result = Mesh();
	result.Vertices.reserve((1 + stackCount)*(sliceCount + 1) + 2);
	result.Indices.reserve(6 * sliceCount*(1 + stackCount));

	auto dTheta = static_cast<float>(M_PI)*2.f / sliceCount;
	auto theta = 0.f;

	auto dHeight = height / stackCount;
	auto y = 0.f;

	auto dRadius = (bRadius - tRadius) / stackCount;
	auto r = bRadius;

	auto vi = 0u;
	// We insert vertice bottom up
	for (auto i = 0u; i <= stackCount; ++i) {
		y = i*dHeight;
		for (auto j = 0u; j <= sliceCount; ++j) {
			theta = j * dTheta;
			auto cosTheta = std::cos(theta);
			auto sinTheta = std::sin(theta);
			auto tangent = Vector3DH(-sinTheta, 0.f, cosTheta);
			auto biTangent = Vector3DH((bRadius - tRadius)* cosTheta, -height, (bRadius - tRadius) * sinTheta);
			auto normal = tangent.Cross(biTangent);
			auto position = Point3DH(cosTheta * r, y, sinTheta * r);
			result.Vertices.emplace_back(position, normal, tangent, static_cast<float>(j) / sliceCount, 1.f - static_cast<float>(i) / stackCount);
		}
		r -= dRadius;
	}

	for (auto i = 0u; i < stackCount; ++i) {
		for (auto j = 0u; j < sliceCount; ++j) {
			result.Indices.push_back(i*(sliceCount + 1) + j);

			result.Indices.push_back((i + 1)*(sliceCount + 1) + j + 1);
			result.Indices.push_back((i + 1)*(sliceCount + 1) + j);
			result.Indices.push_back(i*(sliceCount + 1) + j);

			result.Indices.push_back(i*(sliceCount + 1) + j + 1);
			result.Indices.push_back((i + 1)*(sliceCount + 1) + j + 1);
		}
	}

	// Top center
	result.Vertices.emplace_back(0.f, height, 0.f, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f);
	auto bi = result.Vertices.size();
	auto ti = bi - 1;
	// Bottom center
	result.Vertices.emplace_back(0.f, 0.f, 0.f, 0.f, -1.f, 0.f, -1.f, 0.f, 0.f, 0.f, 0.f);

	for (auto i = 0u; i < sliceCount; ++i) {
		result.Indices.push_back(i);
		result.Indices.push_back(bi);
		result.Indices.push_back(i + 1);
	}

	for (auto i = 0u; i < sliceCount; ++i) {
		result.Indices.push_back(i + stackCount*(sliceCount + 1));
		result.Indices.push_back(i + stackCount*(sliceCount + 1) + 1);
		result.Indices.push_back(ti);

	}

	return result;
}

const Luxko::Anuthur::BasicGeometry::Mesh Luxko::Anuthur::BasicGeometry::Sphere(float radius, size_t stackCount /*= 8u*/, size_t sliceCount /*= 8u*/, const float* textureCoordinates /*= nullptr*/)
{
	auto result = Mesh();
	result.Vertices.reserve(2 * (1 + sliceCount)*(stackCount + 1));
	auto dAlpha = static_cast<float>(M_PI) * 2 / sliceCount;
	auto dBeta = static_cast<float>(M_PI) / stackCount;

	// Center circle first, then advance into up and down stacks.
	for (auto i = 0u; i <= sliceCount; ++i) {
		auto cosAlpha = std::cos(dAlpha*i);
		auto sinAlpha = std::sin(dAlpha*i);
		//auto pos = Point3DH(radius*cosAlpha, 0.f, radius*sinAlpha);
		//auto norm = Vector3DH(radius*cosAlpha, 0.f, radius*sinAlpha).Normalize();
		//auto tangent = Vector3DH::E2().Cross(norm);
		//result.Vertices.push_back(Vertex(pos, norm, tangent, static_cast<float>(i) / sliceCount, 0.5f));

		for (auto j = 0u; j <= stackCount; ++j) {
			auto cosBeta = std::cos(dBeta*j);
			auto sinBeta = std::sin(dBeta*j);
			auto z = radius * sinBeta;
			auto r = radius * cosBeta;

			auto pos = Point3DH(r*cosAlpha, z, r*sinAlpha);
			auto norm = Vector3DH(r*cosAlpha, z, r*sinAlpha).Normalize();
			auto tangent = Vector3DH::E2().Cross(norm).Normalize();
			result.Vertices.emplace_back(pos, norm, tangent, static_cast<float>(i) / sliceCount, 0.5f - static_cast<float>(i / 2.f) / stackCount);
		}
	}

	for (auto i = 0u; i <= sliceCount; ++i) {
		auto cosAlpha = std::cos(dAlpha*i);
		auto sinAlpha = std::sin(dAlpha*i);
		//auto pos = Point3DH(radius*cosAlpha, 0.f, radius*sinAlpha);
		//auto norm = Vector3DH(radius*cosAlpha, 0.f, radius*sinAlpha).Normalize();
		//auto tangent = Vector3DH::E2().Cross(norm);
		//result.Vertices.push_back(Vertex(pos, norm, tangent, static_cast<float>(i) / sliceCount, 0.5f));

		for (auto j = 0u; j <= stackCount; ++j) {
			auto cosBeta = std::cos(dBeta*j);
			auto sinBeta = std::sin(dBeta*j);
			auto z = radius * sinBeta;
			auto r = radius * cosBeta;
			auto pos = Point3DH(r*cosAlpha, -z, r*sinAlpha);
			auto norm = Vector3DH(r*cosAlpha, -z, r*sinAlpha).Normalize();
			auto tangent = (-Vector3DH::E2()).Cross(norm).Normalize();
			result.Vertices.emplace_back(pos, norm, tangent, static_cast<float>(i) / sliceCount, 0.5f + static_cast<float>(i / 2.f) / stackCount);


		}
	}

	result.Indices.reserve(12u * sliceCount*stackCount);

	auto half = (stackCount + 1)*(sliceCount + 1);

	for (auto i = 0u; i < sliceCount; ++i) {
		for (auto j = 0u; j < stackCount; ++j) {
			result.Indices.push_back(i*(stackCount + 1) + j);
			result.Indices.push_back(i*(stackCount + 1) + j + 1);
			result.Indices.push_back((i + 1)*(stackCount + 1) + j + 1);
			result.Indices.push_back(i*(stackCount + 1) + j);
			result.Indices.push_back((i + 1)*(stackCount + 1) + j + 1);
			result.Indices.push_back((i + 1)*(stackCount + 1) + j);


			result.Indices.push_back(i*(stackCount + 1) + j + half);

			result.Indices.push_back((i + 1)*(stackCount + 1) + j + 1 + half);
			result.Indices.push_back(i*(stackCount + 1) + j + 1 + half);
			result.Indices.push_back(i*(stackCount + 1) + j + half);

			result.Indices.push_back((i + 1)*(stackCount + 1) + j + half);
			result.Indices.push_back((i + 1)*(stackCount + 1) + j + 1 + half);
		}
	}

	return result;
}

const Luxko::Anuthur::BasicGeometry::Mesh Luxko::Anuthur::BasicGeometry::Grid(float width, float height, UINT m, UINT n)
{
	Mesh result;
	
	// Set vertice
	result.Vertices.reserve((m+1)*(n+1));
	auto halfWidth = 0.5f * width;
	auto halfHeight = 0.5f * height;
	auto dx = width / m;
	auto dz = height / n;
	auto x = -halfWidth;
	
	for (auto i = 0u; i <= m; ++i) {
		auto z = -halfHeight;
		auto tx = static_cast<float>(i) / m;
		for (auto j = 0u; j <= n; ++j) {
			auto ty = static_cast<float>(j) / n;
			result.Vertices.emplace_back(
				x, 0.f, z,
				0.f, 1.f, 0.f,
				1.f, 0.f, 0.f,
				tx, ty
			);

			z += dz;
		}
		x += dx;
	}

	// Set indice
	result.Indices.reserve(6 * m*n);
	for (auto i = 0u; i < m; ++i) {
		for (auto j = 0u; j < n; ++j) {
			result.Indices.push_back(i*(m + 1) + j + 1);
			result.Indices.push_back(i*(m + 1) + j);
			result.Indices.push_back((i + 1)*(m + 1) + j);

			result.Indices.push_back(i*(m + 1) + j + 1);
			result.Indices.push_back((i + 1)*(m + 1) + j);
			result.Indices.push_back((i + 1)*(m + 1) + j + 1);
		}
	}

	return result;
}

