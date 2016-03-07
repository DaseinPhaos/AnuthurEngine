//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#define _LUXKO
//#define _GLYPH
#ifdef _LUXKO
#include "Vector3f.h"
#include "Matrix3x3f.h"
#include "Vector4f.h"
#include "Matrix4x4f.h"
#include "MathIO.h"
#endif // _LUXKO
#ifdef _GLYPH
#include "GVector3f.h"
#include "GMatrix3f.h"
#include "GVector4f.h"
#include "GMatrix4f.h"
#endif // _GLYPH
#include "Timer.h"
#include <iostream>
#include <sstream>

float GetAve(const float* f, int n) {
	double sum = 0.;
	for (int i = 0; i < n; ++i) {
		sum += f[i];
	}
	return static_cast<float>(sum / n);
}

#pragma region VectorPerformanceTest

//int main() {
//#define TEST_SIZE 1000000
//#define TEST_TIME 100
//	float addT[TEST_TIME];
//	float subT[TEST_TIME];
//	float scaT[TEST_TIME];
//	float dotT[TEST_TIME];
//	float crossT[TEST_TIME];
//	float normalT[TEST_TIME];
//	float magT[TEST_TIME];
//	//float floatPoor[TEST_SIZE];
//#ifdef _GLYPH
//	using namespace Glyph3;
//#endif // _GLYPH
//	using namespace Luxko;
//
//	auto floatPoor = new float[TEST_SIZE];
//	//Vector3f vPoor[TEST_SIZE];
//	auto vPoor = new Vector3f[TEST_SIZE];
//	auto rPoor = new Vector3f[TEST_SIZE];
//	//int pPoor[TEST_SIZE];
//	auto pPoor = new int[TEST_SIZE];
//	for (int index = 0; index < TEST_TIME; ++index) {
//		HANDLE hFile = CreateFile(L"Fuck.txt", GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
//		if (hFile == INVALID_HANDLE_VALUE) {
//			return 1;
//		}
//		std::default_random_engine dre;
//		std::uniform_real_distribution<float> df(-1000000.f, 1000000.f);
//
//		for (int i = 0; i < TEST_SIZE; ++i) {
//			floatPoor[i] = df(dre);
//		}
//		std::uniform_int_distribution<int> di(0, (TEST_SIZE)-1);
//
//
//		for (int i = 0; i < TEST_SIZE; ++i) {
//			vPoor[i].x = df(dre);
//			vPoor[i].y = df(dre);
//			vPoor[i].z = df(dre);
//		}
//
//		for (int i = 0; i < TEST_SIZE; ++i) {
//			pPoor[i] = di(dre);
//		}
//		Timer t;
//		for (int i = 0; i < TEST_SIZE; ++i) {
//			auto r = vPoor[i] + vPoor[pPoor[i]];
//			rPoor[i] = r;
//		}
//		t.Elapse();
//		addT[index] = t.GetLastMs();
//		std::cout << "Addition using " << addT[index] << " miniseconds." << std::endl;
//
//		DWORD written;
//		WriteFile(hFile, &rPoor, sizeof(Vector3f)*TEST_SIZE, &written, nullptr);
//
//		for (int i = 0; i < TEST_SIZE; ++i) {
//			pPoor[i] = di(dre);
//		}
//		t.Reset();
//		for (int i = 0; i < TEST_SIZE; ++i) {
//			auto r = vPoor[i] - vPoor[pPoor[i]];
//			rPoor[i] = r;
//		}
//		t.Elapse();
//
//		WriteFile(hFile, &rPoor, sizeof(Vector3f)*TEST_SIZE, &written, nullptr);
//		subT[index] = t.GetLastMs();
//		std::cout << "Subtraction using " << subT[index] << " miniseconds." << std::endl;
//		/*for (int i = 0; i < TEST_SIZE; ++i) {
//		pPoor[i] = di(dre);
//		}*/
//		t.Reset();
//		for (int i = 0; i < TEST_SIZE; ++i) {
//			auto r = vPoor[i] * floatPoor[i];
//			rPoor[i] = r;
//		}
//		t.Elapse();
//		scaT[index] = t.GetLastMs();
//		WriteFile(hFile, &rPoor, sizeof(Vector3f)*TEST_SIZE, &written, nullptr);
//		std::cout << "Scalar multiplication using " << scaT[index] << " miniseconds." << std::endl;
//		for (int i = 0; i < TEST_SIZE; ++i) {
//			pPoor[i] = di(dre);
//		}
//		t.Reset();
//		for (int i = 0; i < TEST_SIZE; ++i) {
//			auto r = vPoor[i].Dot(vPoor[pPoor[i]]);
//			floatPoor[i] = r;
//		}
//		t.Elapse();
//		dotT[index] = t.GetLastMs();
//		WriteFile(hFile, &floatPoor, sizeof(float)*TEST_SIZE, &written, nullptr);
//		std::cout << "Dot multiplication using " << dotT[index] << " miniseconds." << std::endl;
//		for (int i = 0; i < TEST_SIZE; ++i) {
//			pPoor[i] = di(dre);
//		}
//		t.Reset();
//		for (int i = 0; i < TEST_SIZE; ++i) {
//			auto r = vPoor[i].Cross(vPoor[pPoor[i]]);
//			rPoor[i] = r;
//		}
//		t.Elapse();
//		crossT[index] = t.GetLastMs();
//		WriteFile(hFile, &rPoor, sizeof(Vector3f)*TEST_SIZE, &written, nullptr);
//		std::cout << "Cross multiplication using " << crossT[index] << " miniseconds." << std::endl;
//
//
//		t.Reset();
//		for (int i = 0; i < TEST_SIZE; ++i) {
//#ifdef _LUXKO
//			rPoor[i] = vPoor[i].GetNormal();
//#endif // _LUXKO
//#ifdef _GLYPH
//			rPoor[i] = Vector3f::Normalize(vPoor[i]);
//#endif // _GLYPH
//		}
//		t.Elapse();
//		normalT[index] = t.GetLastMs();
//		WriteFile(hFile, &rPoor, sizeof(Vector3f)*TEST_SIZE, &written, nullptr);
//		std::cout << "Normalization using " << normalT[index] << " miniseconds." << std::endl;
//
//		t.Reset();
//		for (int i = 0; i < TEST_SIZE; ++i) {
//			floatPoor[i] = rPoor[i].Magnitude();
//		}
//		t.Elapse();
//		magT[index] = t.GetLastMs();
//		WriteFile(hFile, &floatPoor, sizeof(float)*TEST_SIZE, &written, nullptr);
//		std::cout << "Get magnitude using " << magT[index] << " miniseconds." << std::endl;
//
//		CloseHandle(hFile);
//	}
//	delete[] floatPoor;
//	delete[] pPoor;
//	delete[] vPoor;
//	delete[] rPoor;
//	std::wstringstream wss;
//	wss << L"Test Results:\r\n" << L"Average addition takes " << GetAve(addT, TEST_TIME) << " ms.\r\n"
//		<< L"Average subtraction takes " << GetAve(subT, TEST_TIME) << " ms.\r\n"
//		<< L"Average scalar multiplication takes " << GetAve(scaT, TEST_TIME) << " ms.\r\n"
//		<< L"Average dot production takes " << GetAve(dotT, TEST_TIME) << " ms.\r\n"
//		<< L"Average cross production takes " << GetAve(crossT, TEST_TIME) << " ms.\r\n"
//		<< L"Average normalization takes " << GetAve(normalT, TEST_TIME) << " ms.\r\n"
//		<< L"Average magnitude takes " << GetAve(magT, TEST_TIME) << " ms.\r\n"<<std::flush;
//
//
//	auto wsst = wss.str();
//	HANDLE hFile = CreateFile(L"TestResultsV3F.txt", GENERIC_WRITE,0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
//	DWORD sizeWritten;
//	WriteFile(hFile, wsst.c_str(), wsst.length()*sizeof(wchar_t), &sizeWritten, nullptr);
//	CloseHandle(hFile);
//	std::getchar();
//}
#pragma endregion VectorPerformanceTest
#pragma region Matrix3fPerformanceTest

//int main() {
//#define TEST_SIZE 1000000
//#define TEST_TIME 100
//	float addT[TEST_TIME];
//	float subT[TEST_TIME];
//	float scaT[TEST_TIME];
//	float crossT[TEST_TIME];
//	float detT[TEST_TIME];
//	float inverseT[TEST_TIME];
//	float vecT[TEST_TIME];
//	//float floatPoor[TEST_SIZE];
//#ifdef _GLYPH
//	using namespace Glyph3;
//#endif // _GLYPH
//#ifdef _LUXKO
//	using namespace Luxko;
//#endif // _LUXKO
//
//	auto floatPoor = new float[TEST_SIZE];
//	//Vector3f vPoor[TEST_SIZE];
//	auto mPoor = new Matrix3x3f[TEST_SIZE];
//	auto rPoor = new Matrix3x3f[TEST_SIZE];
//	auto vPoor = new Vector3f[TEST_SIZE];
//	//int pPoor[TEST_SIZE];
//	auto pPoor = new int[TEST_SIZE];
//	for (int index = 0; index < TEST_TIME; ++index) {
//		HANDLE hFile = CreateFile(L"Fuck.txt", GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
//		if (hFile == INVALID_HANDLE_VALUE) {
//			return 1;
//		}
//		std::default_random_engine dre;
//		std::uniform_real_distribution<float> df(-1000000.f, 1000000.f);
//
//		for (int i = 0; i < TEST_SIZE; ++i) {
//			floatPoor[i] = df(dre);
//		}
//		std::uniform_int_distribution<int> di(0, (TEST_SIZE)-1);
//
//
//		for (int i = 0; i < TEST_SIZE; ++i) {
//			float f[12];
//			for (int k = 0; k < 12; ++k) {
//				f[k] = df(dre);
//			}
//			mPoor[i] = Matrix3x3f(f);
//		}
//
//		for (int i = 0; i < TEST_SIZE; ++i) {
//			pPoor[i] = di(dre);
//		}
//		Luxko::Timer t;
//		for (int i = 0; i < TEST_SIZE; ++i) {
//			auto r = mPoor[i] + mPoor[pPoor[i]];
//			rPoor[i] = r;
//		}
//		t.Elapse();
//		addT[index] = t.GetLastMs();
//		std::cout << "Addition using " << addT[index] << " miniseconds." << std::endl;
//
//		DWORD written;
//		WriteFile(hFile, &rPoor, sizeof(Matrix3x3f)*TEST_SIZE, &written, nullptr);
//
//		for (int i = 0; i < TEST_SIZE; ++i) {
//			pPoor[i] = di(dre);
//		}
//		t.Reset();
//		for (int i = 0; i < TEST_SIZE; ++i) {
//			auto r = mPoor[i] - mPoor[pPoor[i]];
//			rPoor[i] = r;
//		}
//		t.Elapse();
//
//		WriteFile(hFile, &rPoor, sizeof(Matrix3x3f)*TEST_SIZE, &written, nullptr);
//		subT[index] = t.GetLastMs();
//		std::cout << "Subtraction using " << subT[index] << " miniseconds." << std::endl;
//		/*for (int i = 0; i < TEST_SIZE; ++i) {
//		pPoor[i] = di(dre);
//		}*/
//		t.Reset();
//		for (int i = 0; i < TEST_SIZE; ++i) {
//			auto r = mPoor[i] * floatPoor[i];
//			rPoor[i] = r;
//		}
//		t.Elapse();
//		scaT[index] = t.GetLastMs();
//		WriteFile(hFile, &rPoor, sizeof(Matrix3x3f)*TEST_SIZE, &written, nullptr);
//		std::cout << "Scalar multiplication using " << scaT[index] << " miniseconds." << std::endl;
//
//		for (int i = 0; i < TEST_SIZE; ++i) {
//			pPoor[i] = di(dre);
//		}
//		t.Reset();
//		for (int i = 0; i < TEST_SIZE; ++i) {
//			auto r = mPoor[i]*mPoor[pPoor[i]];
//			rPoor[i] = r;
//		}
//		t.Elapse();
//		crossT[index] = t.GetLastMs();
//		WriteFile(hFile, &rPoor, sizeof(Matrix3x3f)*TEST_SIZE, &written, nullptr);
//		std::cout << "Matrix multiplication using " << crossT[index] << " miniseconds." << std::endl;
//
//		t.Reset();
//		for (int i = 0; i < TEST_SIZE; ++i) {
//			auto r = mPoor[i].Determinant();
//			floatPoor[i] = r;
//		}
//		t.Elapse();
//		detT[index] = t.GetLastMs();
//		WriteFile(hFile, &floatPoor, sizeof(float)*TEST_SIZE, &written, nullptr);
//		std::cout << "Determinate using " << detT[index] << " ms." << std::endl;
//
//
//		t.Reset();
//		for (int i = 0; i < TEST_SIZE; ++i) {
//			auto r = mPoor[i].Inverse();
//			rPoor[i] = r;
//		}
//		t.Elapse();
//		inverseT[index] = t.GetLastMs();
//		WriteFile(hFile, &rPoor, sizeof(Matrix3x3f)*TEST_SIZE, &written, nullptr);
//		std::cout << "Inverse using " << inverseT[index] << " ms." << std::endl;
//
//		for (int i = 0; i < TEST_SIZE; ++i) {
//			vPoor[i] = { df(dre),df(dre),df(dre) };
//		}
//		t.Reset();
//		for (int i = 0; i < TEST_SIZE; ++i) {
//			auto r = mPoor[i] * vPoor[i];
//			vPoor[i] = r;
//		}
//		t.Elapse();
//		vecT[index] = t.GetLastMs();
//		WriteFile(hFile, &vPoor, sizeof(Vector3f)*TEST_SIZE, &written, nullptr);
//		std::cout << "Vector multiplication using " << vecT[index] << " ms." << std::endl;
//		CloseHandle(hFile);
//	}
//	delete[] floatPoor;
//	delete[] pPoor;
//	delete[] mPoor;
//	delete[] rPoor;
//	std::wstringstream wss;
//	wss << L"Test Results:\r\n" << L"Average addition takes " << GetAve(addT, TEST_TIME) << " ms.\r\n"
//		<< L"Average subtraction takes " << GetAve(subT, TEST_TIME) << " ms.\r\n"
//		<< L"Average scalar multiplication takes " << GetAve(scaT, TEST_TIME) << " ms.\r\n"
//		<< L"Average matrix multiplication takes " << GetAve(crossT, TEST_TIME) << " ms.\r\n"
//		<<L"Average determinant takes "<<GetAve(detT,TEST_TIME)<<" ms.\r\n"
//		<<L"Average inversion takes "<<GetAve(inverseT,TEST_TIME)<<" ms.\r\n"
//		<< L"Average vector mult. takes " << GetAve(vecT, TEST_TIME) << " ms." <<std::flush;
//
//
//	auto wsst = wss.str();
//	HANDLE hFile = CreateFile(L"TestResults.txt", GENERIC_WRITE,0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
//	DWORD sizeWritten;
//	WriteFile(hFile, wsst.c_str(), wsst.length()*sizeof(wchar_t), &sizeWritten, nullptr);
//	CloseHandle(hFile);
//	std::getchar();
//}
#pragma endregion MatrixPerformanceTest

#pragma region MatrixFunctionalityTest

//int main() {
//	using namespace Luxko;
//	Matrix3x3f m(1.f, 2.f, 3.f,
//		-4.f, 5.f, 6.f,
//		-2.f, -3.f, 2.f);
//	Matrix3x3f n(0.f, 3.f, 8.f,
//		2.f, 6.f, 0.f,
//		1.f, -1.f, 1.f);
//	Vector3f vs[3] = {
//		{8.f,7.f,6.f},
//		{4.f,3.f,1.f},
//		{0.f,1.f,2.f}
//	};
//	Matrix3x3f p(vs);
//	std::cout << "P = " << p;
//	p = Matrix3x3f(vs[0], vs[1], vs[2]);
//	std::cout << "New P = " << p << "P^T = " << p.Transpose();
//	std::cout << "M = " << m << "N = " << n
//		<< "M + N = " << (m + n)
//		<< "M - N = " << (m - n)
//		<< "M * 2 = " << (m * 2.f)
//		<< "M * N = " << (m*n);
//
//	std::cout << "Zero = " << Matrix3x3f::Zero() << "Identity = " << Matrix3x3f::Identity()
//		<< "Det(I) = " << Matrix3x3f::Identity().Determinant() << std::endl
//		<< "Det(M) = " << m.Determinant() << std::endl;
//	std::cout << "M^-1 = " << m.Inverse()
//		<< "M^-1 * M = " << m*m.Inverse();
//	std::cout << "m,row(0) = " << m.Row(0) << std::endl;
//	std::cout << "N * M_row_0 = " << n*m.Row(0) << std::endl;
//	getchar();
//}
#pragma endregion MatrixFunctionalityTest

#pragma region Vector4fFuntionalityTest
//int main() {
//	using namespace Luxko;
//	Vector4f p(1.f, 2.f, 3.f, 4.f);
//	Vector4f q(0.f, -4.f, 1.f, 5.f);
//	std::cout << "P = " << p << std::endl;
//	std::cout << "Q = " << q << std::endl;
//	std::cout << "P.Proj = " << p.Projection() << std::endl;
//	std::cout << "PQ = " << p.Dot(q) << std::endl;
//	std::cout << "P + Q = " << p+q << std::endl;
//	std::cout << "P - Q = " << p-q << std::endl;
//	std::cout << "P * 2 = " << p*2.f << std::endl;
//	std::cout << "Q / 2 = " << q/2.f << std::endl;
//	std::cout << "-Q = " << -q << std::endl;
//	std::cout << "P.mag() = " << p.Magnitude() << std::endl;
//	std::cout << "Q.norm = " << q.GetNormal() << std::endl;
//	getchar();
//}
#pragma endregion Vector4fFuntionalityTest

#pragma region Vector4fPerformanceTest
//
//int main() {
//#define TEST_SIZE 1000000
//#define TEST_TIME 100
//	float addT[TEST_TIME];
//	float subT[TEST_TIME];
//	float scaT[TEST_TIME];
//	float dotT[TEST_TIME];
//	float crossT[TEST_TIME];
//	float normalT[TEST_TIME];
//	float magT[TEST_TIME];
//	//float floatPoor[TEST_SIZE];
//#ifdef _GLYPH
//	using namespace Glyph3;
//#endif // _GLYPH
//	using namespace Luxko;
//
//	auto floatPoor = new float[TEST_SIZE];
//	//Vector4f vPoor[TEST_SIZE];
//	auto vPoor = new Vector4f[TEST_SIZE];
//	auto rPoor = new Vector4f[TEST_SIZE];
//	//int pPoor[TEST_SIZE];
//	auto pPoor = new int[TEST_SIZE];
//	for (int index = 0; index < TEST_TIME; ++index) {
//		HANDLE hFile = CreateFile(L"Fuck.txt", GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
//		if (hFile == INVALID_HANDLE_VALUE) {
//			return 1;
//		}
//		std::default_random_engine dre;
//		std::uniform_real_distribution<float> df(-1000000.f, 1000000.f);
//
//		for (int i = 0; i < TEST_SIZE; ++i) {
//			floatPoor[i] = df(dre);
//		}
//		std::uniform_int_distribution<int> di(0, (TEST_SIZE)-1);
//
//
//		for (int i = 0; i < TEST_SIZE; ++i) {
//			vPoor[i].x = df(dre);
//			vPoor[i].y = df(dre);
//			vPoor[i].z = df(dre);
//			vPoor[i].w = df(dre);
//		}
//
//		for (int i = 0; i < TEST_SIZE; ++i) {
//			pPoor[i] = di(dre);
//		}
//		Timer t;
//		for (int i = 0; i < TEST_SIZE; ++i) {
//			auto r = vPoor[i] + vPoor[pPoor[i]];
//			rPoor[i] = r;
//		}
//		t.Elapse();
//		addT[index] = t.GetLastMs();
//		std::cout << "Addition using " << addT[index] << " miniseconds." << std::endl;
//
//		DWORD written;
//		WriteFile(hFile, &rPoor, sizeof(Vector4f)*TEST_SIZE, &written, nullptr);
//
//		for (int i = 0; i < TEST_SIZE; ++i) {
//			pPoor[i] = di(dre);
//		}
//		t.Reset();
//		for (int i = 0; i < TEST_SIZE; ++i) {
//			auto r = vPoor[i] - vPoor[pPoor[i]];
//			rPoor[i] = r;
//		}
//		t.Elapse();
//
//		WriteFile(hFile, &rPoor, sizeof(Vector4f)*TEST_SIZE, &written, nullptr);
//		subT[index] = t.GetLastMs();
//		std::cout << "Subtraction using " << subT[index] << " miniseconds." << std::endl;
//		/*for (int i = 0; i < TEST_SIZE; ++i) {
//		pPoor[i] = di(dre);
//		}*/
//		t.Reset();
//		for (int i = 0; i < TEST_SIZE; ++i) {
//			auto r = vPoor[i] * floatPoor[i];
//			rPoor[i] = r;
//		}
//		t.Elapse();
//		scaT[index] = t.GetLastMs();
//		WriteFile(hFile, &rPoor, sizeof(Vector4f)*TEST_SIZE, &written, nullptr);
//		std::cout << "Scalar multiplication using " << scaT[index] << " miniseconds." << std::endl;
//		for (int i = 0; i < TEST_SIZE; ++i) {
//			pPoor[i] = di(dre);
//		}
//		t.Reset();
//		for (int i = 0; i < TEST_SIZE; ++i) {
//			auto r = vPoor[i].Dot(vPoor[pPoor[i]]);
//			floatPoor[i] = r;
//		}
//		t.Elapse();
//		dotT[index] = t.GetLastMs();
//		WriteFile(hFile, &floatPoor, sizeof(float)*TEST_SIZE, &written, nullptr);
//		std::cout << "Dot multiplication using " << dotT[index] << " miniseconds." << std::endl;
//
//		
//		t.Reset();
//		for (int i = 0; i < TEST_SIZE; ++i) {
//			auto r = vPoor[i].Projection();
//			rPoor[i] = Vector4f(r, 1.f);
//		}
//		t.Elapse();
//		crossT[index] = t.GetLastMs();
//		WriteFile(hFile, &rPoor, sizeof(Vector4f)*TEST_SIZE, &written, nullptr);
//		std::cout << "Projection using " << dotT[index] << " miniseconds." << std::endl;
//		t.Reset();
//		for (int i = 0; i < TEST_SIZE; ++i) {
//#ifdef _LUXKO
//			rPoor[i] = vPoor[i].GetNormal();
//#endif // _LUXKO
//#ifdef _GLYPH
//			rPoor[i] = Vector4f::Normalize(vPoor[i]);
//#endif // _GLYPH
//		}
//		t.Elapse();
//		normalT[index] = t.GetLastMs();
//		WriteFile(hFile, &rPoor, sizeof(Vector4f)*TEST_SIZE, &written, nullptr);
//		std::cout << "Normalization using " << normalT[index] << " miniseconds." << std::endl;
//
//		t.Reset();
//		for (int i = 0; i < TEST_SIZE; ++i) {
//			floatPoor[i] = rPoor[i].Magnitude();
//		}
//		t.Elapse();
//		magT[index] = t.GetLastMs();
//		WriteFile(hFile, &floatPoor, sizeof(float)*TEST_SIZE, &written, nullptr);
//		std::cout << "Get magnitude using " << magT[index] << " miniseconds." << std::endl << std::endl << std::endl;
//
//		CloseHandle(hFile);
//	}
//	delete[] floatPoor;
//	delete[] pPoor;
//	delete[] vPoor;
//	delete[] rPoor;
//	std::wstringstream wss;
//	wss << L"Test Results:\r\n" << L"Average addition takes " << GetAve(addT, TEST_TIME) << " ms.\r\n"
//		<< L"Average subtraction takes " << GetAve(subT, TEST_TIME) << " ms.\r\n"
//		<< L"Average scalar multiplication takes " << GetAve(scaT, TEST_TIME) << " ms.\r\n"
//		<< L"Average dot production takes " << GetAve(dotT, TEST_TIME) << " ms.\r\n"
//		<< L"Average projection production takes " << GetAve(crossT, TEST_TIME) << " ms.\r\n"
//		<< L"Average normalization takes " << GetAve(normalT, TEST_TIME) << " ms.\r\n"
//		<< L"Average magnitude takes " << GetAve(magT, TEST_TIME) << " ms.\r\n"<<std::flush;
//
//
//	auto wsst = wss.str();
//	std::wstring fileName;
//#ifdef _LUXKO
//	fileName = L"TestResultV4fL.txt";
//#endif
//#ifdef _GLYPH
//	fileName = L"TestResultV4fG.txt";
//#endif
//	HANDLE hFile = CreateFile(fileName.c_str(), GENERIC_WRITE,0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
//	DWORD sizeWritten;
//	WriteFile(hFile, wsst.c_str(), wsst.length()*sizeof(wchar_t), &sizeWritten, nullptr);
//	CloseHandle(hFile);
//	std::getchar();
//}
#pragma endregion Vector4fPerformanceTest

#pragma region Matrix4x4f FuntionalityTest
//int main() {
//	using namespace Luxko;
//	Matrix4x4f m(1.f, 2.f, 3.f, 4.f,
//		-4.f, 5.f, 6.f, 0.f,
//		-2.f, -3.f, 2.f, -3.f,
//		3.f, 3.f, -4.f, -1.f);
//	Matrix4x4f n(0.f, 3.f, 8.f,5.f,
//		2.f, 6.f, 0.f,3.f,
//		1.f, -1.f, 1.f,2.f,
//		0.f, -3.f, 1.f, 2.f);
//	Vector4f vs[4] = {
//		{8.f,7.f,6.f,1.f},
//		{4.f,3.f,1.f,2.f},
//		{0.f,1.f,2.f,3.f},
//		{1.f,1.f,4.f,3.f}
//	};
//	Matrix4x4f p(vs);
//	std::cout << "P(vs) = " << p;
//	p = Matrix4x4f(vs[0], vs[1], vs[2],vs[3]);
//	std::cout << "P(vs) = " << p << "P^T = " << p.Transpose();
//	Matrix4x4f mm = m;
//	std::cout << "M = " << mm<<std::endl;
//	for (int i = 0; i < 4; ++i) {
//		std::cout << "P.row[" << i << "] = " << p.Row(i) << std::endl;
//		std::cout << "P.column[" << i << "] = " << p.Column(i) << std::endl;
//		std::cout << " M * vs[i] = " << mm*vs[i] << std::endl;
//	}
//	std::cout << "M*M = " << m*m << "N = " << n
//		<< "M + N = " << (m + n)
//		<< "M - N = " << (m - n)
//		<< "M * 2 = " << (m * 2.f)
//		<< "M * N = " << (m*n);
//
//	std::cout << "Zero = " << Matrix4x4f::Zero() << "Identity = " << Matrix4x4f::Identity()
//		<< "Det(I) = " << Matrix4x4f::Identity().Determinant() << std::endl
//		<< "Det(M) = " << m.Determinant() << std::endl;
//	std::cout << "M^-1 = " << m.Inverse()
//		<< "M^-1 * M = " << m*m.Inverse();
//	std::cout << "m,row(0) = " << m.Row(0) << std::endl;
//	std::cout << "N * M_row_0 = " << n*m.Row(0) << std::endl;
//	getchar();
//}
#pragma endregion Matrix4x4f FuntionalityTest

#pragma region Matrix4x4fPerformanceTest
int main() {
#define TEST_SIZE 1000000
#define TEST_TIME 100
	float addT[TEST_TIME];
	float subT[TEST_TIME];
	float scaT[TEST_TIME];
	float crossT[TEST_TIME];
	float detT[TEST_TIME];
	float inverseT[TEST_TIME];
	float vecT[TEST_TIME];
	//float floatPoor[TEST_SIZE];
#ifdef _GLYPH
	using namespace Glyph3;
#endif // _GLYPH
#ifdef _LUXKO
	using namespace Luxko;
#endif // _LUXKO

	auto floatPoor = new float[TEST_SIZE];
	//Vector4f vPoor[TEST_SIZE];
	auto mPoor = new Matrix4x4f[TEST_SIZE];
	auto rPoor = new Matrix4x4f[TEST_SIZE];
	auto vPoor = new Vector4f[TEST_SIZE];
	//int pPoor[TEST_SIZE];
	auto pPoor = new int[TEST_SIZE];
	for (int index = 0; index < TEST_TIME; ++index) {
		HANDLE hFile = CreateFile(L"Fuck.txt", GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE) {
			return 1;
		}
		std::default_random_engine dre;
		std::uniform_real_distribution<float> df(-1000000.f, 1000000.f);

		for (int i = 0; i < TEST_SIZE; ++i) {
			floatPoor[i] = df(dre);
		}
		std::uniform_int_distribution<int> di(0, (TEST_SIZE)-1);


		for (int i = 0; i < TEST_SIZE; ++i) {
#ifdef _LUXKO
			float f[16];
			for (int k = 0; k < 12; ++k) {
				f[k] = df(dre);
			}
			mPoor[i] = Matrix4x4f(f);
#endif // _LUXKO
			#ifdef _GLYPH
mPoor[i] = Matrix4x4f(df(dre), df(dre), df(dre), df(dre), df(dre), df(dre), df(dre), df(dre), df(dre), df(dre), df(dre), df(dre), df(dre), df(dre), df(dre), df(dre));
#endif // _GLYPH
		}

		for (int i = 0; i < TEST_SIZE; ++i) {
			pPoor[i] = di(dre);
		}
		Luxko::Timer t;
		for (int i = 0; i < TEST_SIZE; ++i) {
			auto r = mPoor[i] + mPoor[pPoor[i]];
			rPoor[i] = r;
		}
		t.Elapse();
		addT[index] = t.GetLastMs();
		std::cout << "Addition using " << addT[index] << " miniseconds." << std::endl;

		DWORD written;
		WriteFile(hFile, &rPoor, sizeof(Matrix4x4f)*TEST_SIZE, &written, nullptr);

		for (int i = 0; i < TEST_SIZE; ++i) {
			pPoor[i] = di(dre);
		}
		t.Reset();
		for (int i = 0; i < TEST_SIZE; ++i) {
			auto r = mPoor[i] - mPoor[pPoor[i]];
			rPoor[i] = r;
		}
		t.Elapse();

		WriteFile(hFile, &rPoor, sizeof(Matrix4x4f)*TEST_SIZE, &written, nullptr);
		subT[index] = t.GetLastMs();
		std::cout << "Subtraction using " << subT[index] << " miniseconds." << std::endl;
		/*for (int i = 0; i < TEST_SIZE; ++i) {
		pPoor[i] = di(dre);
		}*/
		t.Reset();
		for (int i = 0; i < TEST_SIZE; ++i) {
			auto r = mPoor[i] * floatPoor[i];
			rPoor[i] = r;
		}
		t.Elapse();
		scaT[index] = t.GetLastMs();
		WriteFile(hFile, &rPoor, sizeof(Matrix4x4f)*TEST_SIZE, &written, nullptr);
		std::cout << "Scalar multiplication using " << scaT[index] << " miniseconds." << std::endl;

		for (int i = 0; i < TEST_SIZE; ++i) {
			pPoor[i] = di(dre);
		}
		t.Reset();
		for (int i = 0; i < TEST_SIZE; ++i) {
			auto r = mPoor[i]*mPoor[pPoor[i]];
			rPoor[i] = r;
		}
		t.Elapse();
		crossT[index] = t.GetLastMs();
		WriteFile(hFile, &rPoor, sizeof(Matrix4x4f)*TEST_SIZE, &written, nullptr);
		std::cout << "Matrix multiplication using " << crossT[index] << " miniseconds." << std::endl;

		t.Reset();
		for (int i = 0; i < TEST_SIZE; ++i) {
			auto r = mPoor[i].Determinant();
			floatPoor[i] = r;
		}
		t.Elapse();
		detT[index] = t.GetLastMs();
		WriteFile(hFile, &floatPoor, sizeof(float)*TEST_SIZE, &written, nullptr);
		std::cout << "Determinate using " << detT[index] << " ms." << std::endl;


		t.Reset();
		for (int i = 0; i < TEST_SIZE; ++i) {
			auto r = mPoor[i].Inverse();
			rPoor[i] = r;
		}
		t.Elapse();
		inverseT[index] = t.GetLastMs();
		WriteFile(hFile, &rPoor, sizeof(Matrix4x4f)*TEST_SIZE, &written, nullptr);
		std::cout << "Inverse using " << inverseT[index] << " ms." << std::endl;

		for (int i = 0; i < TEST_SIZE; ++i) {
			vPoor[i] = { df(dre),df(dre),df(dre),df(dre) };
		}
		t.Reset();
		for (int i = 0; i < TEST_SIZE; ++i) {
			auto r = mPoor[i] * vPoor[i];
			vPoor[i] = r;
		}
		t.Elapse();
		vecT[index] = t.GetLastMs();
		WriteFile(hFile, &vPoor, sizeof(Vector4f)*TEST_SIZE, &written, nullptr);
		std::cout << "Vector multiplication using " << vecT[index] << " ms." << std::endl;
		CloseHandle(hFile);
	}
	delete[] floatPoor;
	delete[] pPoor;
	delete[] mPoor;
	delete[] rPoor;
	std::wstringstream wss;
	wss << L"Test Results:\r\n" << L"Average addition takes " << GetAve(addT, TEST_TIME) << " ms.\r\n"
		<< L"Average subtraction takes " << GetAve(subT, TEST_TIME) << " ms.\r\n"
		<< L"Average scalar multiplication takes " << GetAve(scaT, TEST_TIME) << " ms.\r\n"
		<< L"Average matrix multiplication takes " << GetAve(crossT, TEST_TIME) << " ms.\r\n"
		<<L"Average determinant takes "<<GetAve(detT,TEST_TIME)<<" ms.\r\n"
		<<L"Average inversion takes "<<GetAve(inverseT,TEST_TIME)<<" ms.\r\n"
		<< L"Average vector mult. takes " << GetAve(vecT, TEST_TIME) << " ms." <<std::flush;


	auto wsst = wss.str();
	HANDLE hFile = CreateFile(L"TestResultsM4fL OX.txt", GENERIC_WRITE,0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD sizeWritten;
	WriteFile(hFile, wsst.c_str(), wsst.length()*sizeof(wchar_t), &sizeWritten, nullptr);
	CloseHandle(hFile);
	std::getchar();
}
#pragma endregion Matrix4x4fPerformanceTest