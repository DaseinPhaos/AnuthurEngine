//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#include "FileSystem.h"
#include <sstream>
#include <cstring>

static DWORD ToDword() { return 0x0UL; }
template<typename T, typename ... Types>
static DWORD ToDword(const T& t, const Types& ... types);


template<typename T, typename ... Types>
DWORD ToDword(const T& t, const Types& ... types)
{
	return static_cast<DWORD>(t) | ToDword(types ...);
}

Luxko::FileSystem::File Luxko::FileSystem::File::Create(const std::wstring& fileName, FileAccess access /*= FileAccess::ReadWrite*/, FileShareMode shareMode /*= FileShareMode::NoSharing*/, FileCreationOption creationOp /*= FileCreationOption::CreateAlways*/, FileAttribute attribute /*= FileAttribute::Normal*/, FileFlag flags /*= FileFlag::NoFlags*/, SECURITY_ATTRIBUTES* pSA /*= nullptr*/)
{
	File f;
	f._hFile = CreateFile(fileName.c_str(), ToDword(access), ToDword(shareMode),pSA, ToDword(creationOp),
		ToDword(attribute, flags), NULL);
	f._isValid = (f._hFile != INVALID_HANDLE_VALUE);
	if (!f._isValid) {
		std::wstringstream wss;
		wss << L"File Creation Failed. Error code: " << GetLastError() << std::flush;
		throw wss.str().c_str();
	}
	return std::move(f);
}

Luxko::FileSystem::File Luxko::FileSystem::File::CreateAccordingTo(const std::wstring& fileName, const File& f, FileAccess access /*= FileAccess::ReadWrite*/, FileShareMode shareMode /*= FileShareMode::NoSharing*/, SECURITY_ATTRIBUTES* pSA /*= nullptr*/)
{
	File file;
	file._hFile = CreateFile(fileName.c_str(), ToDword(access), ToDword(shareMode), pSA, CREATE_ALWAYS,
		0, f._hFile);
	file._isValid = (f._hFile != INVALID_HANDLE_VALUE);
	if (!f._isValid) {
		std::wstringstream wss;
		wss << L"File Creation Failed. Error code: " << GetLastError() << std::flush;
		throw wss.str().c_str();
	}
	return std::move(file);
}

Luxko::FileSystem::File::~File()
{
	Close();
}

Luxko::FileSystem::File::File(File&& f)
{
	//Close();
	_hFile = f._hFile;
	_isValid = f._isValid;
	f._isValid = false;
}

Luxko::FileSystem::File& Luxko::FileSystem::File::operator=(File&& f)
{
	Close();
	_hFile = f._hFile;
	_isValid = f._isValid;
	f._isValid = false;
	return *this;
}

void Luxko::FileSystem::File::swap(File& f)
{
	auto tHFile = _hFile;
	auto tIsValid = _isValid;
	_hFile = f._hFile;
	_isValid = f._isValid;
	f._hFile = tHFile;
	f._isValid = tIsValid;
}

void Luxko::FileSystem::File::Close()
{
	if (_isValid) {
		CloseHandle(_hFile);
	}
	_isValid = false;
}

bool Luxko::FileSystem::File::SetPos(long long distance, FilePosition fp, long long * newPos /*= nullptr*/)
{
	LARGE_INTEGER det;
	det.QuadPart = distance;
	
	if (!SetFilePointerEx(_hFile, det, &det, ToDword(fp)))
	{
		return false;
	}
	if (newPos != nullptr) {
		*newPos = det.QuadPart;
	}
	return true;
}

long long Luxko::FileSystem::File::Size() const
{
	LARGE_INTEGER r;
	GetFileSizeEx(_hFile, &r);
	return r.QuadPart;
}



std::wstring Luxko::FileSystem::File::GetFullName(bool includeVolume) const
{
	wchar_t temp[1000];
	DWORD mask = VOLUME_NAME_NONE;
	if (includeVolume) {
		mask = VOLUME_NAME_DOS;
	}
	auto length = GetFinalPathNameByHandle(_hFile, temp, 1000, mask);
	if (length == 0) {
		std::wstringstream wss;
		wss << L"File Creation Failed. Error code: " << GetLastError() << std::flush;
		throw wss.str().c_str();
	}
	temp[length] = L'\0';
	return std::wstring(temp);
}


std::wstring Luxko::FileSystem::File::GetFileName() const
{
	struct FileName {
		DWORD fileNameLength;
		wchar_t temp[256];
	}temp;
	if (!GetFileInformationByHandleEx(_hFile, FILE_INFO_BY_HANDLE_CLASS::FileNameInfo, &temp, 255)) {

		std::wstringstream wss;
		wss << L"GetFileInfo Failed. Error code: " << GetLastError() << std::flush;
		throw wss.str().c_str();

	}
	temp.temp[temp.fileNameLength/sizeof(wchar_t)] = L'\0';
	return std::wstring(temp.temp);

}

bool Luxko::FileSystem::File::SetFileName(const std::wstring& newName)
{
	struct FileName {
		DWORD fileNameLength;
		wchar_t temp[256];
	}temp;
	temp.fileNameLength = newName.size();
	if (temp.fileNameLength >= 256) {
		return false;
	}
	std::wmemcpy(temp.temp, newName.c_str(), 255);
	if (!SetFileInformationByHandle(_hFile, FILE_INFO_BY_HANDLE_CLASS::FileNameInfo, &temp, sizeof(temp))) {
		return false;
	}
	return true;
}

Luxko::FileSystem::BasicFileInfo Luxko::FileSystem::File::GetBasicInfo() const
{
	FILE_BASIC_INFO fbi;
	if (!GetFileInformationByHandleEx(_hFile, FILE_INFO_BY_HANDLE_CLASS::FileBasicInfo, &fbi,sizeof(fbi))) {

		std::wstringstream wss;
		wss << L"GetFileInfo Failed. Error code: " << GetLastError() << std::flush;
		throw wss.str().c_str();

	}
	return fbi;
}

bool Luxko::FileSystem::File::SetBasicInfo(BasicFileInfo& bfi)
{
	if (!SetFileInformationByHandle(_hFile, FILE_INFO_BY_HANDLE_CLASS::FileBasicInfo, &bfi, sizeof(bfi))) {
		return false;
	}
	return true;
}

bool Luxko::FileSystem::File::Read(void* data, DWORD bytesToRead, DWORD& bytesRead)
{
	return ReadFile(_hFile, data, bytesToRead, &bytesRead, nullptr) != 0;
}

bool Luxko::FileSystem::File::Write(void* data, DWORD bytesToWrite, DWORD& bytesWritten)
{
	return WriteFile(_hFile, data, bytesToWrite, &bytesWritten, nullptr) != 0;
}

long long Luxko::FileSystem::File::GetPos() const
{
	LARGE_INTEGER r;
	r.QuadPart = 0x0;
	SetFilePointerEx(_hFile, r, &r, FILE_CURRENT);
	return r.QuadPart;
}

Luxko::FileSystem::FileAttribute Luxko::FileSystem::operator|(FileAttribute a, FileAttribute b)
{
	return static_cast<FileAttribute>(ToDword(a, b));
}

Luxko::FileSystem::MoveFileFlag Luxko::FileSystem::operator&(MoveFileFlag a, MoveFileFlag b)
{
	auto da = static_cast<DWORD>(a);
	auto db = static_cast<DWORD>(b);
	return static_cast<MoveFileFlag>(da&db);
}

Luxko::FileSystem::FileFlag Luxko::FileSystem::operator&(FileFlag a, FileFlag b)
{
	auto da = static_cast<DWORD>(a);
	auto db = static_cast<DWORD>(b);
	return static_cast<FileFlag>(da&db);
}

Luxko::FileSystem::FileAttribute Luxko::FileSystem::operator&(FileAttribute a, FileAttribute b)
{
	auto da = static_cast<DWORD>(a);
	auto db = static_cast<DWORD>(b);
	return static_cast<FileAttribute>(da&db);

}

Luxko::FileSystem::FileFlag Luxko::FileSystem::operator|(FileFlag a, FileFlag b)
{
	return static_cast<FileFlag>(ToDword(a, b));
}

Luxko::FileSystem::MoveFileFlag Luxko::FileSystem::operator|(MoveFileFlag a, MoveFileFlag b)
{
	return static_cast<MoveFileFlag>(ToDword(a, b));
}

void Luxko::FileSystem::swap(File& a, File& b)
{
	a.swap(b);
}

bool Luxko::FileSystem::File::Delete(const std::wstring& absFileName)
{
	return DeleteFile(absFileName.c_str()) != 0;
}

bool Luxko::FileSystem::File::Copy(const std::wstring& absSourceFileName, const std::wstring& absTargetName, bool replaceExists)
{
	return CopyFile(absSourceFileName.c_str(), absTargetName.c_str(), static_cast<BOOL>(replaceExists)) != 0;
}

bool Luxko::FileSystem::File::HardLink(const std::wstring& linkName, const std::wstring& sourceFileName)
{
	return CreateHardLink(linkName.c_str(), sourceFileName.c_str(), nullptr) != 0;
}

bool Luxko::FileSystem::File::Move(const std::wstring& absOldFileName, const std::wstring& absNewFileName, MoveFileFlag moveFlags)
{
	return MoveFileEx(absOldFileName.c_str(), absNewFileName.c_str(), ToDword(moveFlags)) != 0;
}

bool Luxko::FileSystem::Directory::Create(const std::wstring& absDirectoryName)
{
	return CreateDirectory(absDirectoryName.c_str(), nullptr) != 0;
}

bool Luxko::FileSystem::Directory::Remove(const std::wstring& absDirectoryName)
{
	return RemoveDirectory(absDirectoryName.c_str()) != 0;
}

bool Luxko::FileSystem::Directory::SetCurrent(const std::wstring& absDirctoryName)
{
	return SetCurrentDirectory(absDirctoryName.c_str()) != 0;
}

std::wstring Luxko::FileSystem::Directory::GetExePath()
{
	wchar_t temp[1000];
	auto size = GetModuleFileName(nullptr, temp, 1000);
	if (size == 0 || size == 1000) {
		throw;
	}
	temp[size] = L'\0';
	return std::wstring(temp);
}

std::wstring Luxko::FileSystem::Directory::GetCurrent()
{
	wchar_t temp[1000];
	if (::GetCurrentDirectory(1000, temp) == 0) {
		throw L"Directory too Long!";
	}
	return std::wstring(temp);
}

std::wstring Luxko::FileSystem::Directory::GetParentUnlessRoot(const std::wstring& dirPath)
{
	auto pos = dirPath.find_last_of(L'\\');
	auto possibleResult = dirPath.substr(0U, pos);
	if (possibleResult.find(L'\\') == possibleResult.size()) {
		return dirPath;
	}
	return possibleResult;
}


std::wstring Luxko::FileSystem::Directory::GetChild(const std::wstring& dirPath, const std::wstring& childPath)
{
	if (childPath[0] == L'\\') {
		return dirPath + childPath;
	}
	return dirPath + L'\\' + childPath;
}

Luxko::FileSystem::FileTime::FileTime(const SystemTime& st)
{
	if (!SystemTimeToFileTime(&st.m_st, reinterpret_cast<LPFILETIME>(&m_ft))) {
		throw L"ST to FT conversion Failed.";
	}
}

Luxko::FileSystem::FileTime::FileTime(LARGE_INTEGER li)
{
	m_ft.dwLowDateTime = li.LowPart;
	m_ft.dwHighDateTime = li.HighPart;
}

bool Luxko::FileSystem::FileTime::operator<(const FileTime& ft) const
{
	auto r = CompareFileTime(&m_ft, &ft.m_ft);
	return r < 0;
}

Luxko::FileSystem::FileTime Luxko::FileSystem::FileTime::ToLocal() const
{
	FileTime result;
	if (!FileTimeToLocalFileTime(&m_ft, &result.m_ft)) {
		throw L"FT to LFT conversion Failed.";
	}
	return result;
}

Luxko::FileSystem::FileTime Luxko::FileSystem::FileTime::ToGlobal() const
{
	FileTime result;
	if (!LocalFileTimeToFileTime(&m_ft, &result.m_ft)) {
		throw L"FT to LFT conversion Failed.";
	}
	return result;
}

LARGE_INTEGER Luxko::FileSystem::FileTime::toLargeInteger() const
{
	LARGE_INTEGER result;
	result.LowPart = m_ft.dwLowDateTime;
	result.HighPart = m_ft.dwHighDateTime;
	return result;
}

bool Luxko::FileSystem::FileTime::operator==(const FileTime& ft)const
{
	return std::memcmp(&m_ft, &ft.m_ft, sizeof(FILETIME)) == 0;
}

Luxko::FileSystem::SystemTime::SystemTime(const FileTime& ft)
{
	if (!FileTimeToSystemTime(&ft.m_ft, &m_st)) {
		throw L"FT to ST conversion failed!";
	}
}

bool Luxko::FileSystem::SystemTime::operator==(const SystemTime& st)const
{
	return std::memcmp(&m_st, &st.m_st, sizeof(SYSTEMTIME)) == 0;
}

Luxko::FileSystem::BasicFileInfo::BasicFileInfo(const FILE_BASIC_INFO& fbi)
{
	std::memcpy(this, &fbi, sizeof(BasicFileInfo));
}

Luxko::FileSystem::SearchResult::SearchResult(const WIN32_FIND_DATA& data)
{
	m_basicInfo.m_Attribute = static_cast<decltype(m_basicInfo.m_Attribute)>(data.dwFileAttributes);
	m_basicInfo.m_CreationTime = data.ftCreationTime;
	m_basicInfo.m_ChangeTime = data.ftLastWriteTime;
	m_basicInfo.m_LastAccessTime = data.ftLastAccessTime;
	m_basicInfo.m_LastWriteTime = data.ftLastWriteTime;

	auto pLI = reinterpret_cast<LARGE_INTEGER*>(&m_fileSize);
	pLI->LowPart = data.nFileSizeLow;
	pLI->HighPart = data.nFileSizeHigh;

	m_fileName = data.cFileName;
}

Luxko::FileSystem::Searching::Searching(const std::wstring& fileName)
{
	WIN32_FIND_DATA wfd;
	_sHandle = FindFirstFile(fileName.c_str(), &wfd);
	if (_sHandle == INVALID_HANDLE_VALUE) {
		_isValid = false;
	}
	else {
		_lastSearchResult = wfd;
		_isValid = true;
	}
}

Luxko::FileSystem::Searching::~Searching()
{
	Close();
}



bool Luxko::FileSystem::Searching::lastResultValid() const
{
	return _isValid;
}

Luxko::FileSystem::SearchResult Luxko::FileSystem::Searching::lastResult() const
{
	return _lastSearchResult;
}

bool Luxko::FileSystem::Searching::SearchNext()
{
	if (!_isValid) {
		return false;
	}
	WIN32_FIND_DATA wfd;
	_isValid = (FindNextFile(_sHandle, &wfd) != 0);
	if (!_isValid) {
		return false;
	}
	_lastSearchResult = wfd;
	return true;
}

void Luxko::FileSystem::Searching::Close()
{
	FindClose(_sHandle);
	_isValid = false;
}

Luxko::FileSystem::Searching::Searching(Searching&& s)
{
	_sHandle = s._sHandle;
	_isValid = s._isValid;
	_lastSearchResult = s._lastSearchResult;
	s._isValid = false;
}

Luxko::FileSystem::Searching& Luxko::FileSystem::Searching::operator=(Searching&& s)
{
	Close();
	_sHandle = s._sHandle;
	_isValid = s._isValid;
	_lastSearchResult = s._lastSearchResult;
	s._isValid = false;
	return *this;
}

