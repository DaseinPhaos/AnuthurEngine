#include "FileSystem.h"
#include "Threading.h"
#include "Delegate.h"
#include <iostream>
#include <cassert>
#include <thread>
#include "Allocator.h"
//#include "ApplicationHelper.h"

int FileSystemTest() {
	std::wcout.imbue(std::locale("chs"));
	std::wcin.imbue(std::locale("chs"));
	std::wcerr.imbue(std::locale("chs"));
	using namespace Luxko::FileSystem;
	auto currentDirectory = Directory::GetCurrent();
	std::wcout << L"当前路径：\n\t" << currentDirectory << std::endl;
	std::wcout << L"运动至父级路径..." << std::endl;
	if (Directory::SetCurrent(L"..\\")) {
		currentDirectory = Directory::GetCurrent();
		std::wcout << L"当前路径：\n\t" << currentDirectory << std::endl;
	}
	getchar();
	std::wcout << L"枚举当前路径下文件..." << std::endl;
	Searching search(Directory::GetCurrent() + L"\\*");
	while (search.lastResultValid()) {
		auto lastResult = search.lastResult();
		std::wcout << L"\t" << lastResult.m_fileName << std::endl;
		search.SearchNext();
	}
	std::wcout << L"\r\n请输入一个文件名:" << std::endl;
	std::wstring fileName;
	std::wcin >> fileName;
	File outerfile;
	assert(!outerfile.Valid());
	{
		std::wcout << L"正在创建 \"" << fileName << L"\"..." << std::endl;
		auto file = File::Create(fileName);
		std::wcout << L"正在写入一些数据..";
		DWORD bytesWritten;
		if (!WriteFile(file.Handle(), fileName.c_str(), static_cast<DWORD>(fileName.size() * sizeof(wchar_t)), &bytesWritten, nullptr)) {
			std::wcout << L"写入失败.\r\n";
		}
		if (bytesWritten != fileName.size() * sizeof(wchar_t)) {
			std::wcout << L"写入没有完全成功.\r\n";
		}
		outerfile = std::move(file);
		assert(!file.Valid());
	}
	assert(outerfile.Valid());
	std::wcout << L"打印文件内容..." << std::endl;
	wchar_t content[256] = { L'\0' };
	DWORD bytesRead;
	OVERLAPPED ov = { 0 };
	ov.Offset = 0x0;
	ov.OffsetHigh = 0x0;
	if (!ReadFile(outerfile.Handle(), &content, static_cast<DWORD>(fileName.size() * sizeof(wchar_t)), &bytesRead, &ov)
		|| bytesRead != fileName.size() * sizeof(wchar_t)) {
		std::wcout << L"读取文件内容失败.." << std::endl;
	}
	else {
		std::wcout << content << std::endl;
	}


	std::wcout << L"文件路径：\r\n" << outerfile.GetFullName() << std::endl;
	outerfile.SetFileName(L"草泥马.txt");
	std::wcout << L"文件名称：\r\n" << outerfile.GetFileName() << std::endl;

	getchar();
	std::wcout << L"关闭此文件..\r\n请按回车继续.." << std::endl;
	getchar();
	outerfile.Close();
	assert(!outerfile.Valid());

	std::wcout << L"移动此文件至父目录?" << std::endl;
	auto fileDir = currentDirectory;
	Directory::SetCurrent(L"..\\");
	currentDirectory = Directory::GetCurrent();
	getchar();
	if (!File::Move(L"AnuthurEngine\\" + fileName, currentDirectory + L"\\MovedFile.txt")) {
		std::wcout << L"移动失败！" << std::endl;
	}
	std::wcout << L"删除文件?" << std::endl;
	getchar();

	if (!File::HardLink(std::wstring(L"AnuthurEngine\\FDuCK"), L"\\MovedFile.txt")) {
		std::wcout << L"创建硬链接失败!" << std::endl;
	}

	if (!File::Delete(Directory::GetCurrent() + L"\\MovedFile.txt")) {
		std::wcout << L"删除失败！" << std::endl;
	}
	getchar();
	return 0;
}



int ThreadingTest() {
	//using namespace Luxko::FileSystem;
	//FileFlag ff = FileFlag::BackupSemantics;
	//FileFlag fff = FileFlag::DeleteOnClose;
	//auto ffff = ff | fff;
	//std::cout << static_cast<DWORD>(ffff) << std::endl;
	//getchar();

	//using namespace Luxko;
	//Threading::ConditionVariable cv;
	//Threading::CriticalSection cs;
	//auto e = Threading::Event::Create(L"Luxko_What'sup_Event");
	//auto m = Threading::Mutex::Create(L"Luxko_Mutex_sup");
	//auto s = Threading::Semaphore::Create(5l);
	//Threading::SlimRWLock srwLock;

	SYSTEM_INFO si;
	GetSystemInfo(&si);
	std::cout << "System Info:\n"
		<< "\tProcessor Architecture: " << si.wProcessorArchitecture << std::endl
		<< "\tPage Size: " << si.dwPageSize << std::endl
		<< "\tNumber of Processors: " << si.dwNumberOfProcessors << std::endl
		<< "\tProcessor Level: " << si.wProcessorLevel << std::endl
		<< "\tAllocation Granularity: " << si.dwAllocationGranularity << std::endl << std::endl;

	MEMORYSTATUSEX ms;
	ms.dwLength = sizeof(ms);
	if (FALSE == GlobalMemoryStatusEx(&ms)) {
		auto errorCode = GetLastError();
		std::cout << "Can't fetch memory status. Error code: " << errorCode << std::endl;
	}
	else {
		
		std::cout << "Memory Status:\n"
			<< "\tMemory Load:" << ms.dwMemoryLoad << std::endl
			<< "\tTotal Physical Memory: " << ms.ullTotalPhys << std::endl
			<< "\tAvaiable Physical Memory: " << ms.ullAvailPhys << std::endl
			<< "\tTotal Page File: " << ms.ullTotalPageFile << std::endl
			<< "\tAvaiable Page File: " << ms.ullAvailPageFile << std::endl
			<< "\tTotal Virtual Memory: " << ms.ullTotalVirtual << std::endl
			<< "\tAvaiable Virtual Memory: " << ms.ullAvailVirtual << std::endl;
	}

	getchar();
	return 0;
}


class Base {
public:
	Base(float r = 0.f) :_result(r) {}
	void Method(int a) {
		std::cout << "CLass Method invoked, a == " << a << std::endl;
	}

	float operator()(float x) { return _result - x; }

	~Base() {
		std::cout << "~Base() called at" << _result << std::endl; 
		_result = 0.f;
	}
	float _result;
};

void Function(int a) {
	std::cout << "Function invoked, a == " << a << std::endl;
}


int DelegateTest() {
	auto MethodSig = &Base::Method;
	Base b;
	(b.*MethodSig)(0);

	using TestDel = Luxko::Delegate<void, int>;
	TestDel test;
	test.Bind<Function>();
	test.Invoke(1);


	test.Bind<Base, &Base::Method>(&b);
	test.Invoke(2);

	using TestDelA = Luxko::Delegate<int, int, int>;
	TestDelA t2;
	{
		auto another_free_func = [test](int x, int y) {test.Invoke(x + y); return x + y; };
		t2.Bind(&another_free_func);
	}
	
	std::cout << "1 + 2 == " << t2.Invoke(1, 2) << std::endl;
	
	using TestDelC = Luxko::Delegate<float, float>;
	TestDelC t3;
	{
		Base b1(1.0f);
		//t3.Bind<Base, &Base::operator()>(&b1);
		t3.Bind<Base>(&b1);
		std::cout << "1 - 20 == " << t3.Invoke(20.f) << std::endl;
		b1._result = 2.f;
		std::cout << "2 - 20 == " << t3.Invoke(20.f) << std::endl;
	}
	// UNSAFE :
	// std::cout << "2 - 1 == " << t3.Invoke(1.f);
	getchar();
	return 0;

}


int AllocatorTest() {
	using Luxko::MemAlloc::Allocator;
	
	std::list<int, Allocator<int>> ints = { 1, 3, 5, 7, 9 };


	return 0;
}

int main() {
	return AllocatorTest();
}