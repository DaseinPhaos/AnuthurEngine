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
		//// Side A, clockwise
		//0.f, 0.4f, 0.f, 0.f, 0.3f, 0.f, 0.3f, 0.4f,
		//// Side B, clockwise
		//0.35f, 0.4f, 0.35f, 0.f, 0.65f, 0.f, 0.65f, 0.4f,
		//// Side C, clockwise
		//0.7f, 0.4f, 0.7f, 0.f, 1.0f, 0.f, 1.0f, 0.4f,
		//// Side D, clockwise
		//0.f, 1.0f, 0.f, 0.6f, 0.3f, 0.6f, 0.3f, 1.0f,
		//// Side E, clockwise
		//0.35f, 1.0f, 0.35f, 0.6f, 0.65f, 0.6f, 0.65f, 1.0f,
		//// Side F, clockwise
		//0.7f, 1.0f, 0.7f, 0.6f, 1.0f, 0.6f, 1.0f, 1.0f,

		// Side A, clockwise
		0.f, 0.f, 0.f, 1.f, 1.f, 1.f, 1.f, 0.f,
		// Side B, clockwise
		0.f, 0.f, 0.f, 1.f, 1.f, 1.f, 1.f, 0.f,
		// Side C, clockwise
		0.f, 0.f, 0.f, 1.f, 1.f, 1.f, 1.f, 0.f,
		// Side D, clockwise
		0.f, 0.f, 0.f, 1.f, 1.f, 1.f, 1.f, 0.f,
		// Side E, clockwise
		0.f, 0.f, 0.f, 1.f, 1.f, 1.f, 1.f, 0.f,
		// Side F, clockwise
		0.f, 0.f, 0.f, 1.f, 1.f, 1.f, 1.f, 0.f,
	};
	if (textureCoordinates == nullptr) {
		textureCoordinates = defualtTextureCoordinates;
	}

	auto result = Mesh();
	result.Vertices = {
		// Side A, clockwise
		Vertex(-x, -y, z, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, textureCoordinates[0], textureCoordinates[1]),
		Vertex(-x, y, z, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, textureCoordinates[2], textureCoordinates[3]),
		Vertex(x, y, z, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, textureCoordinates[4], textureCoordinates[5]),
		Vertex(x, -y, z, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, textureCoordinates[6], textureCoordinates[7]),

		// Side B, clockwise
		Vertex(x, -y, z, 1.f, 0.f, 0.f, 0.f, 0.f, -1.f, textureCoordinates[8], textureCoordinates[9]),
		Vertex(x, y, z, 1.f, 0.f, 0.f, 0.f, 0.f, -1.f, textureCoordinates[10], textureCoordinates[11]),
		Vertex(x, y, -z, 1.f, 0.f, 0.f, 0.f, 0.f, -1.f, textureCoordinates[12], textureCoordinates[13]),
		Vertex(x, -y, -z, 1.f, 0.f, 0.f, 0.f, 0.f, -1.f, textureCoordinates[14], textureCoordinates[15]),

		// Side C, clockwise
		Vertex(x, -y, -z, 0.f, 0.f, -1.f, -1.f, 0.f, 0.f, textureCoordinates[16], textureCoordinates[17]),
		Vertex(x, y, -z, 0.f, 0.f, -1.f, -1.f, 0.f, 0.f, textureCoordinates[18], textureCoordinates[19]),
		Vertex(-x, y, -z, 0.f, 0.f, -1.f, -1.f, 0.f, 0.f, textureCoordinates[20], textureCoordinates[21]),
		Vertex(-x, -y, -z, 0.f, 0.f, -1.f, -1.f, 0.f, 0.f, textureCoordinates[22], textureCoordinates[23]),

		// Side D, clockwise
		Vertex(-x, -y, -z, -1.f, 0.f, 0.f, 0.f, 0.f, 1.f, textureCoordinates[24], textureCoordinates[25]),
		Vertex(-x, y, -z, -1.f, 0.f, 0.f, 0.f, 0.f, 1.f, textureCoordinates[26], textureCoordinates[27]),
		Vertex(-x, y, z, -1.f, 0.f, 0.f, 0.f, 0.f, 1.f, textureCoordinates[28], textureCoordinates[29]),
		Vertex(-x, -y, z, -1.f, 0.f, 0.f, 0.f, 0.f, 1.f, textureCoordinates[30], textureCoordinates[31]),

		// Side E, clockwise
		Vertex(x, y, z, 0.f, 1.f, 0.f, 0.f, 0.f, -1.f, textureCoordinates[32], textureCoordinates[33]),
		Vertex(-x, y, z, 0.f, 1.f, 0.f, 0.f, 0.f, -1.f, textureCoordinates[34], textureCoordinates[35]),
		Vertex(-x, y, -z, 0.f, 1.f, 0.f, 0.f, 0.f, -1.f, textureCoordinates[36], textureCoordinates[37]),
		Vertex(x, y, -z, 0.f, 1.f, 0.f, 0.f, 0.f, -1.f, textureCoordinates[38], textureCoordinates[39]),

		// Side F, clockwise
		Vertex(-x, -y, z, 0.f, -1.f, 0.f, 0.f, 0.f, -1.f, textureCoordinates[40], textureCoordinates[41]),
		Vertex(x, -y, z, 0.f, -1.f, 0.f,  0.f, 0.f, -1.f, textureCoordinates[42], textureCoordinates[43]),
		Vertex(x, -y, -z, 0.f, -1.f, 0.f,  0.f, 0.f, -1.f, textureCoordinates[44], textureCoordinates[45]),
		Vertex(-x, -y, -z, 0.f, -1.f, 0.f, 0.f, 0.f, -1.f, textureCoordinates[46], textureCoordinates[47])
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

const Luxko::Anuthur::BasicGeometry::Mesh Luxko::Anuthur::BasicGeometry::Cylinder(float bRadius, float tRadius, float height, unsigned int stackCount /*= 8u*/, unsigned int sliceCount /*= 8u*/, const float* textureCoordinates /*= nullptr*/)
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
		result.Indices.push_back(static_cast<unsigned int>(bi));
		result.Indices.push_back(i + 1);
	}

	for (auto i = 0u; i < sliceCount; ++i) {
		result.Indices.push_back(i + stackCount*(sliceCount + 1));
		result.Indices.push_back(i + stackCount*(sliceCount + 1) + 1);
		result.Indices.push_back(static_cast<unsigned int>(ti));

	}

	return result;
}

const Luxko::Anuthur::BasicGeometry::Mesh Luxko::Anuthur::BasicGeometry::Sphere(float radius, unsigned int stackCount /*= 8u*/, unsigned int sliceCount /*= 8u*/, const float* textureCoordinates /*= nullptr*/)
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
			auto tangent = Vector3DH(-z * sinAlpha, 0.f, z*cosAlpha).Normalize();
			result.Vertices.emplace_back(
				pos, norm, tangent, 
				dAlpha*i/(2.f*static_cast<float>(M_PI)), 
				dBeta*j/ static_cast<float>(M_PI));
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
			auto tangent = Vector3DH(-z * sinAlpha, 0.f, z*cosAlpha).Normalize();
			result.Vertices.emplace_back(
				pos, norm, tangent, 
				dAlpha*i / (2.f*static_cast<float>(M_PI)),
				dBeta*j / static_cast<float>(M_PI));


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

const Luxko::Anuthur::BasicGeometry::Mesh Luxko::Anuthur::BasicGeometry::Grid(float width, float length, UINT m, UINT n)
{
	Mesh result;

	result.Vertices.reserve((m + 1)*(n + 1));
	auto halfWidth = 0.5f * width;
	auto halfHeight = 0.5f * length;
	auto dx = width / m;
	auto dz = length / n;
	auto z = -halfHeight;

	for (auto i = 0u; i <= n; ++i) {
		auto x = -halfWidth;
		auto tv = static_cast<float>(i) / n;
		for (auto j = 0u; j <= m; ++j) {
			auto tu = static_cast<float>(j) / m;
			result.Vertices.emplace_back(
				Point3DH{ x, 0.f, z },
				Vector3DH{ 0.f, 1.f, 0.f },
				Vector3DH{ 1.f, 0.f, 0.f },
				tu, tv
			);

			x += dx;
		}
		z += dz;
	}

	// Set indice
	result.Indices.reserve(6 * m*n);
	for (auto i = 0u; i < n; ++i) {
		for (auto j = 0u; j < m; ++j) {
			result.Indices.push_back(j*(m + 1) + i);
			result.Indices.push_back(j * (m + 1) + i + 1);
			result.Indices.push_back((j + 1)*(m + 1) + i);

			result.Indices.push_back((j + 1)*(m + 1) + i);
			result.Indices.push_back(j*(m + 1) + i + 1);
			result.Indices.push_back((j + 1)*(m + 1) + i + 1);

			//result.Indices.push_back(j * (m + 1) + i + 1);
			//result.Indices.push_back(j*(m + 1) + i);

			//result.Indices.push_back((j + 1)*(m + 1) + i);
			//result.Indices.push_back(j*(m + 1) + i + 1);
			//result.Indices.push_back((j + 1)*(m + 1) + i);

			//result.Indices.push_back((j + 1)*(m + 1) + i + 1);
		}
	}

	return result;
}

const Luxko::Anuthur::BasicGeometry::Mesh Luxko::Anuthur::BasicGeometry::Terran(float width, float length, UINT m, UINT n, float heightCoefficient)
{
	Mesh mesh = Grid(width, length, m, n);
	auto hw = width*0.5f;
	auto hl = length*0.5f;
	for (auto& v : mesh.Vertices) {
		v.Pos[1] = BasicGeometry::GetTerranHeight(v.Pos.x()/width, v.Pos.z()/length,
			m/13.f,n/13.f) * heightCoefficient;
	}
	
	BasicGeometry::ReviceMeshNormal(mesh);

	for (auto& v : mesh.Vertices) {
		v.TangentU = (v.TangentU - v.Norm).Normalize();
	}

	return mesh;
}

void Luxko::Anuthur::BasicGeometry::ReviceMeshNormal(Mesh& m)
{
	//static constexpr auto PrimitiveVerticeCount = 3u;
	auto vc = m.Vertices.size();
	auto ic = m.Indices.size();
	auto tc = ic / 3u;
	std::vector<std::vector<Vector3DH>> verticeNorms(vc);
	for (auto t = 0u; t < tc; ++t) {
		auto ia = m.Indices[3u * t];
		auto ib = m.Indices[3u * t + 1];
		auto ic = m.Indices[3u * t + 2];
		auto a = m.Vertices[ib].Pos - m.Vertices[ia].Pos;
		auto b = m.Vertices[ic].Pos - m.Vertices[ia].Pos;
		auto n = a.Cross(b);
		n = n.Normalize();
		verticeNorms[ia].push_back(n);
		verticeNorms[ib].push_back(n);
		verticeNorms[ic].push_back(n);
	}

	for (auto i = 0u; i < vc; ++i) {
		auto n = verticeNorms[i].size();
		if(n==0u) continue;
		Vector3DH norm(0.f, 0.f, 0.f);
		for (auto j = 0u; j < n; ++j) {
			auto temp = verticeNorms[i][j];
			norm += temp;
		}
		//norm /= n;
		//m.Vertices[i].Norm = norm;
		m.Vertices[i].Norm = norm/static_cast<float>(n);
	}
}

float Luxko::Anuthur::BasicGeometry::GetTerranHeight(float x, float z, float m, float n)
{
	
	return z*n * std::sinf(x* static_cast<float>(M_PI) *2*m) + x * m * std::cosf(z* static_cast<float>(M_PI) *2*n);
}

void Luxko::Anuthur::BasicGeometry::Mesh::ReviseNormal()
{
	//std::vector<std::vector<Vector3DH>> verticeNorms(Vertices.size());

	//auto triangleCount = Indices.size() / 3u;
	//for (auto i = 0u; i < triangleCount; ++i) {
	//	auto a = Indices[3 * i];
	//	auto b = Indices[3 * i + 1];
	//	auto c = Indices[3 * i + 2];
	//	auto v0 = Vertices[b].Pos - Vertices[a].Pos;
	//	auto v1 = Vertices[c].Pos - Vertices[a].Pos;
	//	auto norm = (v0.Cross(v1)).Normalize();
	//	verticeNorms[a].push_back(norm);
	//	verticeNorms[b].push_back(norm);
	//	verticeNorms[c].push_back(norm);
	//}
	//for (auto i = 0u; i < verticeNorms.size(); ++i) {
	//	auto n = verticeNorms[i].size();
	//	auto norm = Vector3DH(0.f, 0.f, 0.f);
	//	for (auto j = 0u; j < n; ++j) {
	//		norm += verticeNorms[i][j];
	//	}
	//	Vertices[i].Norm = norm / n;

	//}

	BasicGeometry::ReviceMeshNormal(*this);

}
