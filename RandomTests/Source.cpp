#include "DRPipeline.h"
#include <iostream>
#include "MathIO.h"
#include "Transform3DH.h"
#include "Point3DH.h"
#include "Vector3DH.h"
#include "PerspecCamera.h"

void PPTests() {
	using namespace Luxko::Anuthur;
	using namespace Luxko;
	PerspecCamera pc = PerspecCamera::FromHFOVAndAspectRatio(
		1.f, 1000.f, 16.f / 9.f, 3.f*static_cast<float>(M_PI) / 4.f,
		Vector3DH(-1.f, -1.f, -1.f).Normalize(),
		Vector3DH(0.f, 1.f, 0.f), Point3DH(10.f, 10.f, 10.f));

	auto point = Point3DH(1.f, 2.f, 3.f);
	auto pV = (pc.TransformWtoV() * point).AsVector4f();
	auto pH = (pc.TransformWtoH() * point).AsVector4f();
	auto pD = pH / pH._w;
	std::cout << "point == " << point.AsVector4f() << std::endl;
	std::cout << "pV == " << pV << std::endl;
	std::cout << "pH == " << pH << std::endl;
	std::cout << "pD == " << pD << std::endl;
}

int main() {
	//using namespace Luxko::Anuthur::DRP::LightPass::NaiveLights;
	//LightParams lp;
	//lp.posW = Luxko::Vector4f(1.f, 1.f, 1.f, 1.f);
	//lp.angleY = std::sqrt(2.f) / 2.f;
	//lp.direction = Luxko::Vector4f(1.f, 0.f, 0.f, 0.f);
	//lp.range = Luxko::Vector4f(2.f, 0.f, 0.f, 0.f);
	//SpotLight::generateOtoWMatrix(lp);
	//std::cout << lp.mOtoW * Luxko::Vector4f(0.f, 1.f, 1.f, 1.f) << std::endl;
	//std::cout << lp.mOtoW * Luxko::Vector4f(0.f, 0.f, 1.f, 1.f) << std::endl;
	//std::cout << lp.mOtoW * Luxko::Vector4f(1.f, 0.f, 1.f, 1.f) << std::endl;
	//getchar();
	
	//using namespace Luxko;
	//auto t = Transform3DH::Translation(1.f, 2.f, 3.f);
	//t = Transform3DH::Rotation(Vector3DH(0.f, 1.f, 0.f), 1.7f) * t;
	//auto ti = t.Inverse();
	//auto p = Point3DH(1.f, 2.f, 2.f);
	//auto pt = t*p;
	//auto ptt = ti * pt;
	//std::cout << "P == " << p.AsVector4f() << std::endl;
	//std::cout << "T * P == " << pt.AsVector4f() << std::endl;
	//std::cout << "TI * T * P == " << ptt.AsVector4f() << std::endl;
	//getchar();

	Luxko::Vector3f normal(0.f, 1.f, 0.f);
	auto tangent = Luxko::Vector3f(1.f, 0.f, 0.f);
	auto bitangent = normal.Cross(tangent);
	std::cout << bitangent << std::endl;
	//PPTests();
	getchar();
}