////**********************************************************************
//// This file is part of the Anuthur Engine. It is distributed under
//// the MIT license(https://opensource.org/licenses/MIT).
////
//// Copyright (c) Dasein Phaos aka. Luxko.
////**********************************************************************
//#include "PerspecCamera.h"
//#include "CommonHeader.h"
//#include "RootFinding.h"
//#include <iostream>
//#include "MathIO.h"
//#include "Timer.h"
//#include "FloatsComparision.h"
//#include <cassert>
//#include <random>
//#include "Matrix4x4f.h"
//#include "Line3DH.h"
//#include "Plane3DH.h"
//#include "Frame3DH.h"
//
//float CheckCubicResult(float a, float b, float c, const Luxko::CubicRoots& cr) {
//	double sum=0.;
//	for (auto i = 0U; i < cr.count; ++i) {
//		auto r = cr[i] * cr[i] * cr[i] + a*cr[i] * cr[i] + b*cr[i] + c;
//		sum += r;
//	}
//	return static_cast<float>(sum / cr.count);
//}
//float CheckQuarticResult(float a, float b, float c, float d, const Luxko::QuarticRoots& qr) {
//	double sum = 0.;
//	for (auto i = 0U; i < qr.count; ++i) {
//		auto r = qr[i] * qr[i] * qr[i] * qr[i] + a*qr[i] * qr[i] * qr[i] + b*qr[i] * qr[i] + c*qr[i] + d;
//		sum += r;
//	}
//	return static_cast<float>(sum / qr.count);
//}
//
//int TestMatrix4x4f() {
//	constexpr size_t testTime = 100000;
//	//double epsilon[testTime];
//	//double sum = 0.;
//	//double max = 0.;
//	//double min = 0.;
//	//auto result = Luxko::SolveQuadraticPolynomials(2.f, 1.f);
//	//std::cout << result << std::endl;
//	//auto result2 = Luxko::SolveCubicPolynomials(1, 2, 3);
//	//std::cout << result2 << std::endl;
//
//
//	//using namespace std;
//	LARGE_INTEGER li;
//	QueryPerformanceCounter(&li);
//	std::default_random_engine dre(li.LowPart);
//	//uniform_real_distribution<float> urd(-1000.f,1000.f);
//	//for (int i = 0; i < testTime; ++i) {
//	//	float ps[3] = { urd(dre),urd(dre),urd(dre) };
//	//	auto r = Luxko::SolveCubicPolynomials(ps[0], ps[1], ps[2]);
//	//	epsilon[i] = CheckCubicResult(ps[0], ps[1], ps[2], r);
//	//	if(Luxko::IsValidFloat(epsilon[i]))
//	//		sum += epsilon[i];
//	//	else {
//	//		cout << "Result NaN." << endl;
//	//		cout << "a = " << ps[0] << ", b = " << ps[1] << ", c = " << ps[2] << endl;
//	//		cout << r << endl;
//	//	}
//	//	if (max < epsilon[i]) {
//	//		max = epsilon[i];
//	//		cout << "New max: " << max<<endl;
//	//		cout << "a = " << ps[0] << ", b = " << ps[1] << ", c = " << ps[2] << endl;
//	//		cout << r << endl;
//	//	}
//	//}
//	//cout << "average = "<<sum / testTime << endl;
//	//getchar();
//
//	std::uniform_real_distribution<float> urd(-100,100);
//	//std::uniform_int_distribution<int> urd(0);
//	using namespace Luxko;
//	auto Identity = Matrix4x4f::Identity();
//	int ieqCount = 0;
//	int rejectCount = 0;
//	for (int i = 0; i < testTime; ++i) {
//		auto m = Matrix4x4f(urd(dre), urd(dre), urd(dre), urd(dre),
//			urd(dre), urd(dre), urd(dre), urd(dre),
//			urd(dre), urd(dre), urd(dre), urd(dre),
//			urd(dre), urd(dre), urd(dre), urd(dre));
//		if (AlmostEqualRelativeAndAbs(m.Determinant(), 0.f))
//		{
//			//std::cout << "Zero determinant meet: " << m << std::endl<< "determinant = " << m.Determinant() << std::endl;
//			--i;
//			++rejectCount;
//			continue;
//		}
//		auto im = m.Inverse();
//		auto id = im*m;
//		if (id != Identity) {
//			//std::cout << "Equality failed.\nM = " << m << "\nM * M^-1 = " << id << std::endl;
//			++ieqCount;
//		}
//		//auto plane = Plane3DH(Point3DH(urd(dre), urd(dre), urd(dre)), Vector3DH(urd(dre), urd(dre), urd(dre)));
//		//auto norm = plane.GetNormal();
//		//auto nLine = Line3DH(Point3DH(0.f, 0.f, 0.f), norm);
//		//auto S = nLine.Intersect(plane);
//		//auto line = Line3DH(Point3DH(urd(dre), urd(dre), urd(dre)), -norm);
//		//auto p = line(urd(dre));
//		//
//		//
//		//auto SP = p - S;
//		//auto v = SP.Cross(norm);
//
//
//		//if (!v.Parallel(plane)) {
//		//	++ieqCount;
//		//}
//		
//	}
//	std::cout << "Total " << rejectCount << " times rejected." << std::endl;
//
//	std::cout << "Total " << ieqCount << " times failed." << std::endl;
//	
//	//float sum = 0.f;
//	//float max = 0.f;
//	//size_t count = 0;
//	//float epsilon[testTime];
//	//using namespace std;
//	//Luxko::Timer timer = Luxko::Timer();
//	//for (int i = 0; i < testTime; ++i) {
//	//	float ps[4] = { urd(dre),urd(dre),urd(dre),urd(dre) };
//	//	auto r = Luxko::SolveQuarticPolynomial(ps[0], ps[1], ps[2], ps[3]);
//	//	epsilon[i] = CheckQuarticResult(ps[0], ps[1], ps[2], ps[3], r);
//	//	if (Luxko::IsValidFloat(epsilon[i])) {
//	//		sum += epsilon[i];
//	//		++count;
//	//	}
//	//	else {
//	//		//cout << "Result NaN." << endl;
//	//		//cout << "a = " << ps[0] << ", b = " << ps[1] << ", c = " << ps[2] <<", d = "<<ps[3]<< endl;
//	//		//cout << r << endl;
//	//	}
//	//	if (max < epsilon[i]) {
//	//		max = epsilon[i];
//	//		cout << "New max: " << max<<endl;
//	//		cout << "a = " << ps[0] << ", b = " << ps[1] << ", c = " << ps[2] << ", d = " << ps[3] << endl;
//	//		cout << r << endl;
//	//	}
//	//}
//	//timer.Elapse();
//
//	//cout << "average = "<<sum / count << endl<<"Time used = "<<timer.GetLastMs()<<endl;
//	getchar();
//	return 0;
//}
//
//int main() {
//	using namespace Luxko;
//	Vector3DH up = { 0.f,1.f,0.f };
//	Vector3DH look = { 10.f,0.f,30.f };
//	Point3DH point = { -20.f,35.f,-50.f };
//	Frame3DH f(look, up, point);
//	auto right = f.Right();
//	auto leftHand = f.GetTransformLH().Inverse();
//	auto rightHand = f.GetTransform().Inverse();
//	std::cout << "right direc = " << right.AsVector4f() <<std::endl
//		<< "Left hand = " << leftHand.AsMatrix4x4() << std::endl
//		<< "right hand = " << rightHand.AsMatrix4x4() << std::endl;
//
//	auto camera = Anuthur::PerspecCamera::FromHFOVAndAspectRatio(1.f, 11.f, 4.f / 3.f, static_cast<float>(M_PI_4),
//		f);
//	camera.ApplyTransform(f.GetTransform());
//	std::cout << "View Transform= " << camera.TransformVtoH().AsMatrix4x4();
//
//	std::default_random_engine dre(88);
//	std::uniform_real_distribution<float> urd(-100.f, 100.f);
//	//Point3DH rp = { urd(dre),urd(dre),urd(dre) };
//	Point3DH rp = { 1,2,3 };
//	float temp;
//	auto i = 0;
//	while (std::cin>>temp) {
//		if (i < 3) {
//			rp[i] = temp;
//			i++;
//		}
//		else {
//			i = 0;
//			std::cout << "rp = " << rp.AsVector4f() << std::endl
//				<< "rh*rp = " << (rightHand*rp).AsVector4f() << std::endl
//				<< "lh*rp = " << (leftHand*rp).AsVector4f() << std::endl;
//		}
//	}
//
//	//assert(rp == rightHand*rp);
//	//assert(AlmostEqualRelativeAndAbs(-rp.z(),(leftHand*rp).z()));
//
//
//
//	getchar();
//}


#include <memory>

#include <iostream>

using namespace std;

class Bar {
public:
	int fuck = 10;
	int Value()const noexcept { return fuck; }
};

class Foo {
public:
	auto operator->()const throw() { return _ptr.operator ->(); }
	shared_ptr<Bar> _ptr;
};

int main()
{
	Foo foo;
	foo._ptr = make_shared<Bar>();
	cout << foo->Value();
	getchar();
}