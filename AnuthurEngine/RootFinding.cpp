#include "RootFinding.h"
#include "FloatsComparision.h"
#include <cassert>

Luxko::QuadraticRoots Luxko::SolveQuadraticPolynomials(float b, float c)
{
	Luxko::QuadraticRoots result;
	auto determinant = b*b - 4 * c;
	auto p1 = -b / 2.f;
	if (AlmostEqualRelativeAndAbs(determinant, 0.f)) {
		result.count = 1;
		result.results[0] = p1;
	}
	else if (determinant > 0.f) {
		result.count = 2;
		auto p2 = std::sqrtf(determinant) / 2.f;
		result.results[0] = p1 - p2;
		result.results[1] = p1 + p2;
	}
	else {
		result.count = 0;
	}
	return result;
}

Luxko::CubicRoots Luxko::SolveCubicPolynomials(float a, float b, float c)
{
	auto p = (-(a*a) / 3.f)+b;
	auto q = ((2.f*a*a*a) / 27.f) - (a*b / 3.f) + c;
	auto result = SolveCubicPolynomials(p, q);
	for (auto i = 0U; i < result.count; ++i) {
		result.results[i] -= a / 3.f;
	}
	return result;
}

Luxko::CubicRoots Luxko::SolveCubicPolynomials(float p, float q)
{
	CubicRoots result;

	p = p / 3.f;
	q = q / 2.f;

	auto determinant = p*p*p + q*q;
	if (AlmostEqualRelativeAndAbs(determinant, 0.f)) {
		result.count = 2;
		auto r = std::cbrtf(-q);
		result[0] = 2.f*r;
		result[1] = -r;
	}
	else if (determinant > 0.f) {
		result.count = 1;
		determinant = std::sqrtf(determinant);
		auto r = std::cbrtf(-q + determinant);
		auto s = std::cbrtf(-q - determinant);
		result[0] = r + s;
	}
	else {
		assert(p < 0.f);
		
		result.count = 3;
		auto sqrtmP = std::sqrtf(-p);
		auto theta = (std::acosf(-q / (sqrtmP*sqrtmP*sqrtmP)))/3.f;
		result[0] = 2.f * sqrtmP*std::cosf(theta);
		auto alpha = theta + (static_cast<float>(M_PI)*2.f / 3.f);
		result[1] = 2.f * sqrtmP*std::cosf(alpha);
		auto beta = theta - (static_cast<float>(M_PI)*2.f / 3.f);
		result[2] = 2.f * sqrtmP*std::cosf(beta);
	}
	return result;
}



