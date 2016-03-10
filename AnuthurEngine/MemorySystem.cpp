//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#include "MemorySystem.h"

static DWORD ToDword() {
	return 0x0;
}

template<typename T, typename ... Types>
static DWORD ToDword(const T& t, const Types& ... types) {
	return static_cast<DWORD>(t) | ToDword(types...);
}

Luxko::Memory::HeapCreationOption Luxko::Memory::operator&(HeapCreationOption a, HeapCreationOption b)
{
	return static_cast<HeapCreationOption>(ToDword(a)&ToDword(b));
}

Luxko::Memory::HeapCreationOption Luxko::Memory::operator|(HeapCreationOption a, HeapCreationOption b)
{
	return static_cast<HeapCreationOption>(ToDword(a, b));
}

Luxko::Memory::FileMapAccessRight Luxko::Memory::operator|(FileMapAccessRight a, FileMapAccessRight b)
{
	return static_cast<FileMapAccessRight>(ToDword(a, b));
}

Luxko::Memory::PageProtectionFlags Luxko::Memory::operator|(PageProtectionFlags a, PageProtectionFlags b)
{
	return static_cast<PageProtectionFlags>(ToDword(a, b));
}

Luxko::Memory::HeapAllocationOption Luxko::Memory::operator&(HeapAllocationOption a, HeapAllocationOption b)
{
	return static_cast<HeapAllocationOption>(ToDword(a)&ToDword(b));
}

Luxko::Memory::HeapAllocationOption Luxko::Memory::operator|(HeapAllocationOption a, HeapAllocationOption b)
{
	return static_cast<HeapAllocationOption>(ToDword(a, b));
}

Luxko::Memory::HeapReallocationOption Luxko::Memory::operator&(HeapReallocationOption a, HeapReallocationOption b)
{
	return static_cast<HeapReallocationOption>(ToDword(a)&ToDword(b));
}

Luxko::Memory::HeapReallocationOption Luxko::Memory::operator|(HeapReallocationOption a, HeapReallocationOption b)
{
	return static_cast<HeapReallocationOption>(ToDword(a, b));
}

Luxko::Memory::Heap::Heap(Heap&& h)
{
	_hHeap = h._hHeap;
	_isValid = h._isValid;
	h._isValid = false;
}

Luxko::Memory::Heap::~Heap()
{
	DestroyThis();
}

Luxko::Memory::Heap Luxko::Memory::Heap::Create(size_t initialSize, size_t maximumSize /*= 0*/, HeapCreationOption options /*= HeapCreationOption::Default*/)
{
	Heap h;
	h._hHeap = HeapCreate(ToDword(options), initialSize, maximumSize);
	if (h._hHeap == nullptr) {
		h._isValid = false;
	}else {
		h._isValid = true;
	}
	return h;
}

void Luxko::Memory::Heap::DestroyThis()
{
	if (_isValid) {
		HeapDestroy(_hHeap);
	}
	_isValid = false;
}


void* Luxko::Memory::Heap::Allocate(size_t bytes, HeapAllocationOption options /*= HeapAllocationOption::Default*/)
{
	if (!_isValid)return nullptr;
	return HeapAlloc(_hHeap, ToDword(options), bytes);
}

bool Luxko::Memory::Heap::DeAllocate(void* ptr, bool isSerialized /*= true*/)
{
	if (!_isValid) {
		return false;
	}
	DWORD flags = 0x0;
	if (!isSerialized) {
		flags = HEAP_NO_SERIALIZE;
	}
	return HeapFree(_hHeap, flags, ptr) != 0;
}

void* Luxko::Memory::Heap::ReAllocate(void* ptr, size_t bytes, HeapReallocationOption options /*= HeapReallocationOption::Default*/)
{
	if (!_isValid) {
		return nullptr;
	}
	return HeapReAlloc(_hHeap, ToDword(options), ptr, bytes);
}

Luxko::Memory::Heap& Luxko::Memory::Heap::operator=(Heap&& h)
{
	DestroyThis();
	_hHeap = h._hHeap;
	_isValid = h._isValid;
	h._isValid = false;
	return *this;
}

Luxko::Memory::MappingObject::MappingObject(MappingObject&& m)
{
	_hMappedFile = m._hMappedFile;
	_isValid = m._isValid;
	m._isValid = false;
}

Luxko::Memory::MappingObject& Luxko::Memory::MappingObject::operator=(MappingObject&& m)
{
	Close();
	_hMappedFile = m._hMappedFile;
	_isValid = m._isValid;
	m._isValid = false;
	return *this;
}

Luxko::Memory::MappingObject::~MappingObject()
{
	Close();
}

Luxko::Memory::MappingObject Luxko::Memory::MappingObject::CreateFromFile(const FileSystem::File& file, std::wstring& mapObjName, PageProtectionOption ppo, PageProtectionFlags ppf /*= PageProtectionFlags::NoFlags*/, long long maximumSize /*= 0x0LL*/, SECURITY_ATTRIBUTES* psa /*= nullptr*/)
{
	MappingObject result;
	LARGE_INTEGER li;
	li.QuadPart = maximumSize;
	result._hMappedFile = CreateFileMapping(file.Handle(), psa, ToDword(ppo, ppf), li.HighPart, li.LowPart, mapObjName.c_str());
	if (result._hMappedFile == nullptr) {
		result._isValid = false;
	}
	else {
		result._isValid = true;
	}
	return result;
}

Luxko::Memory::MappingObject Luxko::Memory::MappingObject::CreateFromPage(std::wstring& mapObjName, long long maximumSize, PageProtectionOption ppo, PageProtectionFlags ppf /*= PageProtectionFlags::NoFlags*/, SECURITY_ATTRIBUTES* psa /*= nullptr*/)
{
	MappingObject result;
	LARGE_INTEGER li;
	li.QuadPart = maximumSize;
	result._hMappedFile = CreateFileMapping(INVALID_HANDLE_VALUE, psa, ToDword(ppo, ppf), li.HighPart, li.LowPart, mapObjName.c_str());
	if (result._hMappedFile == nullptr) {
		result._isValid = false;
	}
	else {
		result._isValid = true;
	}
	return result;
}

Luxko::Memory::MappingObject Luxko::Memory::MappingObject::OpenByName(std::wstring& mapObjName, FileMapAccessRight desiredAccess, bool inheritable)
{
	MappingObject result;
	result._hMappedFile = OpenFileMapping(ToDword(desiredAccess), static_cast<BOOL>(inheritable), mapObjName.c_str());
	if (result._hMappedFile == nullptr) {
		result._isValid = false;
	}
	else {
		result._isValid = true;
	}
	return result;
}

void* Luxko::Memory::MappingObject::Map(FileMapAccessRight access, long long offset, size_t bytesToMap)
{
	LARGE_INTEGER li;
	li.QuadPart = offset;
	return MapViewOfFile(_hMappedFile, ToDword(access), li.HighPart, li.LowPart, bytesToMap);
}

bool Luxko::Memory::MappingObject::UnMap(void* baseAddress)
{
	return UnmapViewOfFile(baseAddress) != 0;
}

bool Luxko::Memory::MappingObject::Flush(void* baseAddress, size_t bytesToFlush)
{
	return FlushViewOfFile(baseAddress, bytesToFlush) != 0;
}

void Luxko::Memory::MappingObject::Close()
{
	if (_isValid) {
		CloseHandle(_hMappedFile);
		_isValid = false;
	}
	
}
