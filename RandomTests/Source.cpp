#include "DRPipeline.h"
#include <iostream>
#include "MathIO.h"
#include "Transform3DH.h"
#include "Point3DH.h"
#include "Vector3DH.h"

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
	using namespace Luxko;
	auto t = Transform3DH::Translation(1.f, 2.f, 3.f);
	t = Transform3DH::Rotation(Vector3DH(0.f, 1.f, 0.f), 1.7f) * t;
	auto ti = t.Inverse();
	auto p = Point3DH(1.f, 2.f, 2.f);
	auto pt = t*p;
	auto ptt = ti * pt;
	std::cout << "P == " << p.AsVector4f() << std::endl;
	std::cout << "T * P == " << pt.AsVector4f() << std::endl;
	std::cout << "TI * T * P == " << ptt.AsVector4f() << std::endl;
	getchar();
}