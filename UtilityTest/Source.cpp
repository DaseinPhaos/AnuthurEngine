#include "FileSystem.h"
#include <iostream>
#include <cassert>

int main() {
	std::wcout.imbue(std::locale("chs"));
	std::wcin.imbue(std::locale("chs"));
	std::wcerr.imbue(std::locale("chs"));
	using namespace Luxko::FileSystem;
	auto currentDirectory = GetCurrentDir();
	std::wcout << L"当前路径：\n\t" <<currentDirectory<< std::endl;
	if (SetCurrentDir(L"..\\")) {
		currentDirectory = GetCurrentDir();
		std::wcout << L"当前路径：\n\t" << currentDirectory << std::endl;
	}
	getchar();
	std::wcout << L"请输入一个文件名:" << std::endl;
	std::wstring fileName;
	std::wcin >> fileName;
	File outerfile;
	assert(!outerfile.Valid());
	{
		std::wcout << L"正在创建 \"" << fileName << L"\"..." << std::endl;
		auto file = File::Create(fileName);
		std::wcout << L"正在写入一些数据..";
		DWORD bytesWritten;
		if (!WriteFile(file.Handle(), fileName.c_str(), fileName.size()*sizeof(wchar_t), &bytesWritten, nullptr)) {
			std::wcout << L"写入失败.\r\n";
		}
		if (bytesWritten != fileName.size()*sizeof(wchar_t)) {
			std::wcout << L"写入没有完全成功.\r\n";
		}
		outerfile = std::move(file);
		assert(!file.Valid());
	}
	assert(outerfile.Valid());
	std::wcout << L"打印文件内容..." << std::endl;
	wchar_t content[256] = {L'\0'};
	DWORD bytesRead;
	OVERLAPPED ov = {0};
	ov.Offset = 0x0;
	ov.OffsetHigh = 0x0;
	if (!ReadFile(outerfile.Handle(), &content, fileName.size()*sizeof(wchar_t), &bytesRead,&ov)
		|| bytesRead != fileName.size()*sizeof(wchar_t)) {
		std::wcout << L"读取文件内容失败.." << std::endl;
	}
	else {
		std::wcout << content<<std::endl;
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
	SetCurrentDir(L"..\\");
	currentDirectory = GetCurrentDir();
	getchar();
	if (!File::Move(L"AnuthurEngine\\"+fileName, currentDirectory+L"\\MovedFile.txt")) {
		std::wcout << L"移动失败！" << std::endl;
	}
	std::wcout << L"删除文件?" << std::endl;
	getchar();

	if (!File::HardLink(std::wstring(L"AnuthurEngine\\FDuCK"), L"\\MovedFile.txt")) {
		std::wcout << L"创建硬链接失败!" << std::endl;
	}

	if (!File::Delete(GetCurrentDir() + L"\\MovedFile.txt")) {
		std::wcout << L"删除失败！" << std::endl;
	}
	getchar();
}