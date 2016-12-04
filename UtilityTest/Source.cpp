#include "FileSystem.h"
#include "Threading.h"
#include "Delegate.h"
#include <iostream>
#include <cassert>
#include <thread>
#include "Allocator.h"
#include "ObserverPattern.h"
#include "AnyOptional.h"
#include <unordered_set>

int FileSystemTest() {
	std::wcout.imbue(std::locale("chs"));
	std::wcin.imbue(std::locale("chs"));
	std::wcerr.imbue(std::locale("chs"));
	using namespace Luxko::FileSystem;
	auto currentDirectory = Directory::GetCurrent();
	std::wcout << L"��ǰ·����\n\t" << currentDirectory << std::endl;
	std::wcout << L"�˶�������·��..." << std::endl;
	if (Directory::SetCurrent(L"..\\")) {
		currentDirectory = Directory::GetCurrent();
		std::wcout << L"��ǰ·����\n\t" << currentDirectory << std::endl;
	}
	getchar();
	std::wcout << L"ö�ٵ�ǰ·�����ļ�..." << std::endl;
	Searching search(Directory::GetCurrent() + L"\\*");
	while (search.lastResultValid()) {
		auto lastResult = search.lastResult();
		std::wcout << L"\t" << lastResult.m_fileName << std::endl;
		search.SearchNext();
	}
	std::wcout << L"\r\n������һ���ļ���:" << std::endl;
	std::wstring fileName;
	std::wcin >> fileName;
	File outerfile;
	assert(!outerfile.Valid());
	{
		std::wcout << L"���ڴ��� \"" << fileName << L"\"..." << std::endl;
		auto file = File::Create(fileName);
		std::wcout << L"����д��һЩ����..";
		DWORD bytesWritten;
		if (!WriteFile(file.Handle(), fileName.c_str(), static_cast<DWORD>(fileName.size() * sizeof(wchar_t)), &bytesWritten, nullptr)) {
			std::wcout << L"д��ʧ��.\r\n";
		}
		if (bytesWritten != fileName.size() * sizeof(wchar_t)) {
			std::wcout << L"д��û����ȫ�ɹ�.\r\n";
		}
		outerfile = std::move(file);
		assert(!file.Valid());
	}
	assert(outerfile.Valid());
	std::wcout << L"��ӡ�ļ�����..." << std::endl;
	wchar_t content[256] = { L'\0' };
	DWORD bytesRead;
	OVERLAPPED ov = { 0 };
	ov.Offset = 0x0;
	ov.OffsetHigh = 0x0;
	if (!ReadFile(outerfile.Handle(), &content, static_cast<DWORD>(fileName.size() * sizeof(wchar_t)), &bytesRead, &ov)
		|| bytesRead != fileName.size() * sizeof(wchar_t)) {
		std::wcout << L"��ȡ�ļ�����ʧ��.." << std::endl;
	}
	else {
		std::wcout << content << std::endl;
	}


	std::wcout << L"�ļ�·����\r\n" << outerfile.GetFullName() << std::endl;
	outerfile.SetFileName(L"������.txt");
	std::wcout << L"�ļ����ƣ�\r\n" << outerfile.GetFileName() << std::endl;

	getchar();
	std::wcout << L"�رմ��ļ�..\r\n�밴�س�����.." << std::endl;
	getchar();
	outerfile.Close();
	assert(!outerfile.Valid());

	std::wcout << L"�ƶ����ļ�����Ŀ¼?" << std::endl;
	auto fileDir = currentDirectory;
	Directory::SetCurrent(L"..\\");
	currentDirectory = Directory::GetCurrent();
	getchar();
	if (!File::Move(L"AnuthurEngine\\" + fileName, currentDirectory + L"\\MovedFile.txt")) {
		std::wcout << L"�ƶ�ʧ�ܣ�" << std::endl;
	}
	std::wcout << L"ɾ���ļ�?" << std::endl;
	getchar();

	if (!File::HardLink(std::wstring(L"AnuthurEngine\\FDuCK"), L"\\MovedFile.txt")) {
		std::wcout << L"����Ӳ����ʧ��!" << std::endl;
	}

	if (!File::Delete(Directory::GetCurrent() + L"\\MovedFile.txt")) {
		std::wcout << L"ɾ��ʧ�ܣ�" << std::endl;
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

class StringC {
public:
	explicit StringC(const std::string& s) :ptr(s.c_str()) {
		std::cout << "StringC ctor called from " << s << std::endl;
	}
private:
	const char* ptr;
};

class CopyTalk {
public:
	explicit CopyTalk(float t) {
		value = t;
	}
	CopyTalk(const CopyTalk& ct) {
		std::cout << "CopyTalk lv-copy ctor talked! Value : " << ct.value << std::endl;
		value = ct.value;
	}

	CopyTalk(CopyTalk&& ct) {
		std::cout << "CopyTalk rv-copy ctor talked! Value : " << ct.value << std::endl;
		value = ct.value;
	}

	CopyTalk& operator=(const CopyTalk& ct) {
		std::cout << "CopyTalk lv-copy operator= talked! Value : " << ct.value << std::endl;
		value = ct.value;
		return *this;
	}

	CopyTalk& operator=(CopyTalk&& ct) {
		std::cout << "CopyTalk rv-copy operator= talked! Value : " << ct.value << std::endl;
		value = ct.value;
		return *this;
	}

	float value;
};

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
		std::cout << "1 + 2 == " << t2.Invoke(1, 2) << std::endl;
	}
	
	// Fucking unsafe here
	// std::cout << "1 + 2 == " << t2.Invoke(1, 2) << std::endl;
	
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
	
	using DStringC = Luxko::Delegate<StringC, const std::string&>;
	using BStringC = Luxko::Delegate<Base, const std::string&>;
	DStringC ds = DStringC::FromDefault();
	ds.Invoke("fuck");
	// BStringC bs = BStringC::FromDefault();
	// bs.Invoke("fuck bs");
	//ds.Bind<StringC::StringC>();
	
	using CTBD = Luxko::Delegate<Base, CopyTalk>;
	CTBD ctbd;
	auto lamb1 = [](CopyTalk ct) {return Base(ct.value); };
	ctbd.Bind(&lamb1);
	CopyTalk&& ct = CopyTalk(1.f);
	ct.value = 3.f;
	ctbd.Invoke(ct);
	ctbd.Invoke(CopyTalk(2.f));

	using RCTBD = Luxko::Delegate<Base, CopyTalk&&>;
	RCTBD rctbd;
	auto rfunc = [](CopyTalk&& ct) {return Base(ct.value); };
	rfunc(std::move(ct));
	rctbd.Bind(&rfunc);
	rctbd.Invoke(std::move(CopyTalk(4.f)));
	ct.value = 5.f;
	rctbd.Invoke(std::move(ct));
	// UNSAFE :
	// std::cout << "2 - 1 == " << t3.Invoke(1.f);
	getchar();


	using LCTBD = Luxko::Delegate<Base, CopyTalk&>;
	LCTBD lctbd;
	auto lfunc = [](CopyTalk& ct) { return Base(ct.value); ct.value = 0.f; };
	ct = CopyTalk(8.f);
	lctbd.Bind(&lfunc);
	auto base = lctbd.Invoke(ct);
	std::cout << "b.value == " << base._result << std::endl;
	ct.value = 9.f;
	lctbd.Invoke(std::move(ct));


	getchar();
	std::unordered_set<Luxko::Delegate<Base, CopyTalk&>> fuckset;
	
	fuckset.insert(lctbd);

	return 0;
}


int AllocatorTest() {
	using Luxko::MemAlloc::Allocator;
	
	std::list<int, Allocator<int>> ints = { 1, 3, 5, 7, 9 };


	return 0;
}

int DEventTest() {
	using Luxko::DEvent;
	using Luxko::Delegate;

	DEvent<int, int, int> dEvent;
	using d_type = decltype(dEvent)::delegate_type;
	
	auto f0 = [](int x, int y) {
		std::cout << "Addition invoked.";
		return x + y;
	};

	auto f1 = [](int x, int y) {
		std::cout << "Subtraction invoked.";
		return x - y;
	};

	d_type d0;
	d0.Bind(&f0);

	d_type d1;
	d1.Bind(&f1);

	dEvent.Add(d0);
	dEvent.Add(d1);
	dEvent.Raise(1, 2);

	dEvent.Remove(d0);
	dEvent.Raise(3, 4);

	return 0;

}

void OptionalTest() {
	Luxko::Optional<Base> ob;
	auto shouldFalse = ob.has_value();
	if (!ob) {
		ob.operator=(3.f);
		std::cout << "ob == " << ob->_result << std::endl;
		std::cout << "ob == " << (*ob)._result << std::endl;

		Luxko::Optional<Base> anotherOb;
		std::cout << "1 == " << anotherOb.value_or(static_cast<float>(1))._result << std::endl;
		try
		{
			auto r = anotherOb.value()._result;
			std::cout << "This should have no effect.." << r;
		}
		catch (Luxko::bad_optional_access& e)
		{
			std::cout << "Exception caught: " << e.what() << std::endl;
		}
		
		anotherOb.swap(ob);
		std::cout << "anotherOb == " << anotherOb.value()._result << std::endl;
	}

	auto od = Luxko::Optional<Luxko::Delegate<Base>>();
	try
	{
		auto v = od.value();
		v.Invoke();
	}
	catch (Luxko::bad_optional_access& e)
	{
		std::cout << "Exception caught: " << e.what() << std::endl;
	}

	auto ot = Luxko::make_optional<Luxko::Delegate<void, int>>();
	std::cout << "ot.has_value(): " << ot.has_value() << std::endl;
	ot->Bind<Base, &Base::Method>(&(*ob));
	ot.value().Invoke(4);
}

void AnyTest() {
	auto a = Luxko::Any();
	std::cout << "typeid(void) == " << a.type().name() << std::endl;
	a.emplace<Base>(3.f);
	std::cout << "typeid(Base) == " << a.type().name() << std::endl;
	std::cout << "3 == " << Luxko::any_cast<Base&>(a)._result << std::endl;
	try {
		auto f = Luxko::any_cast<float>(a);
		std::cout << "This line won't get executed.." << std::endl;
	}
	catch (Luxko::bad_any_cast& e) {
		std::cout << "bad_any_cast caught, what() == " << e.what() << std::endl;
	}
	a.emplace<int>(4);
	std::cout << "typeid(int) == " << a.type().name() << std::endl;
	std::cout << "4 == " << Luxko::any_cast<int>(a) << std::endl;
	std::cout << "nullptr == " << reinterpret_cast<size_t>(Luxko::any_cast<float*>(&a)) << std::endl;
	a.reset();
	std::cout << "typeid(void) == " << a.type().name() << std::endl;
	
}

namespace OBTest {
	class Object {
	public:
		using observer_type = Luxko::Observer<char>;
		void Run() {
			char c;
			std::cout << "Waiting orders..." << std::endl;
			while (std::cin >> c && c != 'q') {
				obs.Notify(c);
				std::cout << "Done!\nWaiting orders..." << std::endl;
			}
			std::cout << "Quit!" << std::endl;
		}
		void AddOB(observer_type* ot) {
			obs.Add(ot);
		}
	private:
		observer_type obs = observer_type::CreateHead();
	};

	void Test() {
		Object object;
		Object::observer_type obs[3];
		//auto obi0 = ;
		obs[0].toBeInvoked.Bind(&([](char c) {std::cout << c << ": ob#0 invoked." << std::endl; }));
		obs[1].toBeInvoked.Bind(&([](char c) {std::cout << c << ": ob#1 invoked." << std::endl; }));
		obs[2].toBeInvoked.Bind(&([](char c) {std::cout << c << ": ob#2 invoked." << std::endl; }));
		object.AddOB(&obs[0]);
		// object.AddOB(&obs[0]); Deadly!
		object.AddOB(&obs[1]);
		object.AddOB(&obs[2]);

		object.Run();

		obs[1].Remove();
		object.Run();
		
	}

}

int main() {
	DelegateTest();
	getchar();
	return 0;
}