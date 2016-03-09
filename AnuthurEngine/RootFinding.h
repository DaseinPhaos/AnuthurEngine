//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#pragma once
#include "CommonHeader.h"
#include "ValResult.h"
namespace Luxko {
	using QuadraticRoots = ValResult<float, 2>;
	// find x s.t. x^2 + b*x + c = 0
	QuadraticRoots SolveQuadraticPolynomials(float b, float c);

	using CubicRoots = ValResult<float, 3>;
	// find x s.t. x^3 + a*x^2 + b*x + c = 0
	CubicRoots SolveCubicPolynomials(float a, float b, float c);
	// find x s.t. x^3 + p*x + q = 0
	CubicRoots SolveCubicPolynomials(float p, float q);

	using QuarticRoots = ValResult<float, 4>;

	//**********************************************************************
	// CAUTION : The following methods don't handle complex numbers
	//			 correctly for the time being. Thus many inputs
	//			 which should have real solutions would yield incorrect no-
	//			 solution output.
	//			 The problem should be managed if these functions are actually
	//			 used.
	// find x s.t. x^4 + a*x^3 + b*x^2 + c*x + d = 0
	QuarticRoots SolveQuarticPolynomial(float a, float b, float c, float d);
	// find x s.t. x^4 + p*x^2 + q*x + r = 0
	QuarticRoots SolveQuarticPolynomial(float p, float q, float r);
	//**********************************************************************

}
