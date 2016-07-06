//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

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




Luxko::QuarticRoots Luxko::SolveQuarticPolynomial(float a, float b, float c, float d)
{
	auto p = (-3.f / 8.f)*a*a + b;
	auto q = a*a*a / 8.f - (a * b) / 2.f + c;
	auto r = (-3.f / 256.f)*a*a*a*a + a*a*b / 16.f - a*c / 4.f + d;
	auto result = SolveQuarticPolynomial(p, q, r);
	for (auto i = 0U; i < result.count; ++i) {
		result[i] -= a / 4.f;
	}
	return result;
}

Luxko::QuarticRoots Luxko::SolveQuarticPolynomial(float p, float q, float r)
{
	QuarticRoots result;
	result.count = 0;
	auto ys = SolveCubicPolynomials(-p / 2.f, -r, (4.f*r*p - q*q) / 8.f);
	if (ys.count == 0) {
		
		return result;
	}
	float y=-1.f;
	for (auto i = 0U; i < ys.count; ++i) {
		if (ys[i] > 0.f
			&& 2.f*ys[i]-p>=0.f
			&& ys[i]*ys[i]-r>=0.f) {
			y = ys[i];
			break;
		}
	}
	if (y < 0.f) return result;
	assert(2.f*y - p >= 0.f);
	assert(y*y - r >= 0.f);
	auto m = std::sqrtf(2.f*y - p);
	auto n = std::sqrtf(y*y - r);
	if (q >= 0.f) {
		auto r1 = SolveQuadraticPolynomials(m, y - n);
		auto r2 = SolveQuadraticPolynomials(-m, y + n);
		return ValResultMerge(r1, r2);
	}
	else {
		assert(q < 0.f);
		auto r1 = SolveQuadraticPolynomials(m, y + n);
		auto r2 = SolveQuadraticPolynomials(-m, y - n);
		return ValResultMerge(r1, r2);
	}
}
