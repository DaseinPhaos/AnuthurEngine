#include "DRPipeline.h"
#include <iostream>
#include "MathIO.h"

int main() {
	using namespace Luxko::Anuthur::DRP::LightPass::NaiveLights;
	LightParams lp;
	lp.posW = Luxko::Vector4f(1.f, 1.f, 1.f, 1.f);
	lp.angleY = std::sqrt(2.f) / 2.f;
	lp.direction = Luxko::Vector4f(1.f, 0.f, 0.f, 0.f);
	lp.range = Luxko::Vector4f(2.f, 0.f, 0.f, 0.f);
	SpotLight::generateOtoWMatrix(lp);
	std::cout << lp.mOtoW * Luxko::Vector4f(0.f, 1.f, 1.f, 1.f) << std::endl;
	std::cout << lp.mOtoW * Luxko::Vector4f(0.f, 0.f, 1.f, 1.f) << std::endl;
	std::cout << lp.mOtoW * Luxko::Vector4f(1.f, 0.f, 1.f, 1.f) << std::endl;
	getchar();
}