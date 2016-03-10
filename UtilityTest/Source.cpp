#include "FileSystem.h"
#include <iostream>
#include <cassert>

int main() {
	std::wcout.imbue(std::locale("chs"));
	std::wcin.imbue(std::locale("chs"));
	std::wcerr.imbue(std::locale("chs"));
	using namespace Luxko::FileSystem;
	auto currentDirectory = Directory::GetCurrent();
	std::wcout << L"��ǰ·����\n\t" <<currentDirectory<< std::endl;
	std::wcout << L"�˶�������·��..." << std::endl;
	if (Directory::SetCurrent(L"..\\")) {
		currentDirectory = Directory::GetCurrent();
		std::wcout << L"��ǰ·����\n\t" << currentDirectory << std::endl;
	}
	getchar();
	std::wcout << L"ö�ٵ�ǰ·�����ļ�..." << std::endl;
	Searching search(Directory::GetCurrent()+L"\\*");
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
		if (!WriteFile(file.Handle(), fileName.c_str(), fileName.size()*sizeof(wchar_t), &bytesWritten, nullptr)) {
			std::wcout << L"д��ʧ��.\r\n";
		}
		if (bytesWritten != fileName.size()*sizeof(wchar_t)) {
			std::wcout << L"д��û����ȫ�ɹ�.\r\n";
		}
		outerfile = std::move(file);
		assert(!file.Valid());
	}
	assert(outerfile.Valid());
	std::wcout << L"��ӡ�ļ�����..." << std::endl;
	wchar_t content[256] = {L'\0'};
	DWORD bytesRead;
	OVERLAPPED ov = {0};
	ov.Offset = 0x0;
	ov.OffsetHigh = 0x0;
	if (!ReadFile(outerfile.Handle(), &content, fileName.size()*sizeof(wchar_t), &bytesRead,&ov)
		|| bytesRead != fileName.size()*sizeof(wchar_t)) {
		std::wcout << L"��ȡ�ļ�����ʧ��.." << std::endl;
	}
	else {
		std::wcout << content<<std::endl;
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
	if (!File::Move(L"AnuthurEngine\\"+fileName, currentDirectory+L"\\MovedFile.txt")) {
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
}