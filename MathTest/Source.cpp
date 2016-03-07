//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#include "CommonHeader.h"
#include "RootFinding.h"
#include <iostream>
#include "MathIO.h"
#include "FloatsComparision.h"
#include <cassert>
#include <random>
#include "Matrix4x4f.h"
#include "Line3DH.h"
#include "Plane3DH.h"

float CheckCubicResult(float a, float b, float c, const Luxko::CubicRoots& cr) {
	for (auto i = 0U; i < cr.count; ++i) {
		auto r = cr[i] * cr[i] * cr[i] + a*cr[i] * cr[i] + b*cr[i] + c;
		return std::fabsf(r);
	}
}

int main() {
	constexpr size_t testTime = 100000;
	//double epsilon[testTime];
	//double sum = 0.;
	//double max = 0.;
	//double min = 0.;
	//auto result = Luxko::SolveQuadraticPolynomials(2.f, 1.f);
	//std::cout << result << std::endl;
	//auto result2 = Luxko::SolveCubicPolynomials(1, 2, 3);
	//std::cout << result2 << std::endl;


	//using namespace std;
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	std::default_random_engine dre(li.LowPart);
	//uniform_real_distribution<float> urd(-1000.f,1000.f);
	//for (int i = 0; i < testTime; ++i) {
	//	float ps[3] = { urd(dre),urd(dre),urd(dre) };
	//	auto r = Luxko::SolveCubicPolynomials(ps[0], ps[1], ps[2]);
	//	epsilon[i] = CheckCubicResult(ps[0], ps[1], ps[2], r);
	//	if(Luxko::IsValidFloat(epsilon[i]))
	//		sum += epsilon[i];
	//	else {
	//		cout << "Result NaN." << endl;
	//		cout << "a = " << ps[0] << ", b = " << ps[1] << ", c = " << ps[2] << endl;
	//		cout << r << endl;
	//	}
	//	if (max < epsilon[i]) {
	//		max = epsilon[i];
	//		cout << "New max: " << max<<endl;
	//		cout << "a = " << ps[0] << ", b = " << ps[1] << ", c = " << ps[2] << endl;
	//		cout << r << endl;
	//	}
	//}
	//cout << "average = "<<sum / testTime << endl;
	//getchar();

	std::uniform_real_distribution<double> urd;
	//std::uniform_int_distribution<int> urd(0);
	using namespace Luxko;
	auto Identity = Matrix4x4f::Identity();
	int ieqCount = 0;
	int rejectCount = 0;
	for (int i = 0; i < testTime; ++i) {
		//auto m = Matrix4x4f(urd(dre), urd(dre), urd(dre), urd(dre),
		//	urd(dre), urd(dre), urd(dre), urd(dre),
		//	urd(dre), urd(dre), urd(dre), urd(dre),
		//	urd(dre), urd(dre), urd(dre), urd(dre));
		//if (AlmostEqualRelativeAndAbs(m.Determinant(), 0.f))
		//{
		//	//std::cout << "Zero determinant meet: " << m << std::endl<< "determinant = " << m.Determinant() << std::endl;
		//	--i;
		//	++rejectCount;
		//	continue;
		//}
		//auto im = m.Inverse();
		//auto id = im*m;
		//if (id != Identity) {
		//	//std::cout << "Equality failed.\nM = " << m << "\nM * M^-1 = " << id << std::endl;
		//	++ieqCount;
		//}
		auto plane = Plane3DH(Point3DH(urd(dre), urd(dre), urd(dre)), Vector3DH(urd(dre), urd(dre), urd(dre)));
		auto norm = plane.GetNormal();
		auto nLine = Line3DH(Point3DH(0.f, 0.f, 0.f), norm);
		auto S = nLine.Intersect(plane);
		auto line = Line3DH(Point3DH(urd(dre), urd(dre), urd(dre)), -norm);
		auto p = line(urd(dre));
		
		
		auto SP = p - S;
		auto v = SP.Cross(norm);


		if (!v.Parallel(plane)) {
			++ieqCount;
		}
		
	}
	std::cout << "Total " << rejectCount << " times rejected." << std::endl;

	std::cout << "Total " << ieqCount << " times failed." << std::endl;
	getchar();
}
