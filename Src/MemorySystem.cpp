//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#include "MemorySystem.h"
#include <cassert>

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
	}
	else {
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

//Luxko::Memory::MappingObject::MappingObject(MappingObject&& m)
//{
//	_hMappedFile = m._hMappedFile;
//	_isValid = m._isValid;
//	m._isValid = false;
//}
//
//Luxko::Memory::MappingObject& Luxko::Memory::MappingObject::operator=(MappingObject&& m)
//{
//	Close();
//	_hMappedFile = m._hMappedFile;
//	_isValid = m._isValid;
//	m._isValid = false;
//	return *this;
//}
//
//Luxko::Memory::MappingObject::~MappingObject()
//{
//	Close();
//}
//
//Luxko::Memory::MappingObject Luxko::Memory::MappingObject::CreateFromFile(const FileSystem::File& file, std::wstring& mapObjName, PageProtectionOption ppo, PageProtectionFlags ppf /*= PageProtectionFlags::NoFlags*/, long long maximumSize /*= 0x0LL*/, SECURITY_ATTRIBUTES* psa /*= nullptr*/)
//{
//	MappingObject result;
//	LARGE_INTEGER li;
//	li.QuadPart = maximumSize;
//	result._hMappedFile = CreateFileMapping(file.Handle(), psa, ToDword(ppo, ppf), li.HighPart, li.LowPart, mapObjName.c_str());
//	if (result._hMappedFile == nullptr) {
//		result._isValid = false;
//	}
//	else {
//		result._isValid = true;
//	}
//	return result;
//}
//
//Luxko::Memory::MappingObject Luxko::Memory::MappingObject::CreateFromPage(std::wstring& mapObjName, long long maximumSize, PageProtectionOption ppo, PageProtectionFlags ppf /*= PageProtectionFlags::NoFlags*/, SECURITY_ATTRIBUTES* psa /*= nullptr*/)
//{
//	MappingObject result;
//	LARGE_INTEGER li;
//	li.QuadPart = maximumSize;
//	result._hMappedFile = CreateFileMapping(INVALID_HANDLE_VALUE, psa, ToDword(ppo, ppf), li.HighPart, li.LowPart, mapObjName.c_str());
//	if (result._hMappedFile == nullptr) {
//		result._isValid = false;
//	}
//	else {
//		result._isValid = true;
//	}
//	return result;
//}
//
//Luxko::Memory::MappingObject Luxko::Memory::MappingObject::OpenByName(std::wstring& mapObjName, FileMapAccessRight desiredAccess, bool inheritable)
//{
//	MappingObject result;
//	result._hMappedFile = OpenFileMapping(ToDword(desiredAccess), static_cast<BOOL>(inheritable), mapObjName.c_str());
//	if (result._hMappedFile == nullptr) {
//		result._isValid = false;
//	}
//	else {
//		result._isValid = true;
//	}
//	return result;
//}
//
//void* Luxko::Memory::MappingObject::Map(FileMapAccessRight access, long long offset, size_t bytesToMap)
//{
//	LARGE_INTEGER li;
//	li.QuadPart = offset;
//	return MapViewOfFile(_hMappedFile, ToDword(access), li.HighPart, li.LowPart, bytesToMap);
//}
//
//bool Luxko::Memory::MappingObject::UnMap(void* baseAddress)
//{
//	return UnmapViewOfFile(baseAddress) != 0;
//}
//
//bool Luxko::Memory::MappingObject::Flush(void* baseAddress, size_t bytesToFlush)
//{
//	return FlushViewOfFile(baseAddress, bytesToFlush) != 0;
//}
//
//void Luxko::Memory::MappingObject::Close()
//{
//	if (_isValid) {
//		CloseHandle(_hMappedFile);
//		_isValid = false;
//	}
//	
//}

Luxko::Memory::MappingObject::MappingObject(MappingObject&& m)
{
	_hMappedObject = std::move(m._hMappedObject);
}

Luxko::Memory::MappingObject Luxko::Memory::MappingObject::CreateFromFile(const FileSystem::File& file, long long maximumSize /*= 0ll*/, PageProtectionOption ppo /*= PageProtectionOption::ReadWrite*/, PageProtectionFlags ppf /*= PageProtectionFlags::NoFlags*/, const wchar_t* mapObjName /*= nullptr*/, SECURITY_ATTRIBUTES* psa /*= nullptr*/)
{
	LARGE_INTEGER li;
	li.QuadPart = maximumSize;
	MappingObject mo;
	mo._hMappedObject = static_cast<KernelObjectHandle>(CreateFileMapping(
		file.Handle(), psa, ToDword(ppo, ppf), li.HighPart, li.LowPart, mapObjName));
	return std::move(mo);
}

Luxko::Memory::MappingObject Luxko::Memory::MappingObject::CreateFromPage(const wchar_t* mapObjName, long long maximumSize, PageProtectionOption ppo, PageProtectionFlags ppf /*= PageProtectionFlags::NoFlags*/, SECURITY_ATTRIBUTES* psa /*= nullptr*/)
{
	LARGE_INTEGER li;
	li.QuadPart = maximumSize;
	MappingObject mo;
	mo._hMappedObject = static_cast<KernelObjectHandle>(CreateFileMapping(
		INVALID_HANDLE_VALUE, psa, ToDword(ppo, ppf), li.HighPart, li.LowPart, mapObjName));
	return std::move(mo);
}

Luxko::Memory::MappingObject Luxko::Memory::MappingObject::OpenByName(const wchar_t* mapObjName, FileMapAccessRight desiredAccess, bool inheritable)
{
	MappingObject result;
	result._hMappedObject = static_cast<KernelObjectHandle>(OpenFileMapping(ToDword(desiredAccess), static_cast<BOOL>(inheritable), mapObjName));
	return std::move(result);
}

void* Luxko::Memory::MappingObject::Map(FileMapAccessRight access, long long offset, /* Should be a multiplication of system granity. */ size_t bytesToMap /* If 0, it would try to map from offset to the end. */)
{
	LARGE_INTEGER li;
	li.QuadPart = offset;
	return MapViewOfFile(_hMappedObject.Get(), ToDword(access), li.HighPart, li.LowPart, bytesToMap);
}

bool Luxko::Memory::MappingObject::UnMap(void* baseAddress)
{
	return FALSE != UnmapViewOfFile(baseAddress);
}

bool Luxko::Memory::MappingObject::Flush(void* baseAddress, size_t bytesToFlush)
{
	return FALSE != FlushViewOfFile(baseAddress, bytesToFlush);
}

void Luxko::Memory::MappingObject::Close()
{
	_hMappedObject.Release();
}

Luxko::Memory::MappingObject& Luxko::Memory::MappingObject::operator=(MappingObject&& m)
{
	_hMappedObject = std::move(m._hMappedObject);
	return *this;
}

Luxko::Memory::CommittedMemoryToken::CommittedMemoryToken(void* sa, DWORD sizeInBytes)
{
	StartAddress = sa;
	SizeInBytes = sizeInBytes;
}

Luxko::Memory::VirtualMemory::VirtualMemory()
{
	_basePtr = nullptr;
}

Luxko::Memory::VirtualMemory::~VirtualMemory()
{
	DeCommitAllAndRelease();
}

void Luxko::Memory::VirtualMemory::DeCommitAllAndRelease() noexcept
{
	if (_basePtr) {
		VirtualFree(_basePtr, 0, MEM_RELEASE);
		_basePtr = nullptr;
		_pagesCommitMask.clear();
	}
}

Luxko::Memory::VirtualMemory Luxko::Memory::VirtualMemory::Reserve(size_t bytesToReserve, VirtualMemoryProtectionOptions vpo)
{
	VirtualMemory vm;
	vm._basePtr = VirtualAlloc(nullptr, bytesToReserve, MEM_RESERVE | MEM_TOP_DOWN, ToDword(vpo));
	auto pagesReserved = AlignToPageSize(bytesToReserve);
	vm._pagesCommitMask = std::vector<bool>(pagesReserved, false);
	vm._vpo = vpo;
	return std::move(vm);
}

Luxko::Memory::VirtualMemory Luxko::Memory::VirtualMemory::ReserveAndCommit(size_t bytesToReserve, VirtualMemoryProtectionOptions vpo)
{
	VirtualMemory vm;
	vm._basePtr = VirtualAlloc(nullptr, bytesToReserve, MEM_RESERVE | MEM_TOP_DOWN | MEM_COMMIT, ToDword(vpo));
	static auto pageSize = Win32Basic::SystemInfo::PageSize();
	auto pagesReserved = AlignToPageSize(bytesToReserve);
	vm._pagesCommitMask = std::vector<bool>(pagesReserved, true);
	vm._vpo = vpo;
	return std::move(vm);
}

const Luxko::Memory::CommittedMemoryToken Luxko::Memory::VirtualMemory::Commit(size_t startPageIndex, size_t numberOfPagesToCommit)
{
	assert(startPageIndex + numberOfPagesToCommit <= _pagesCommitMask.size());
	
	auto sizeToCommit = numberOfPagesToCommit * Win32Basic::SystemInfo::PageSize();
	auto startAddress = GetAddressFromPageIndex(startPageIndex);

	auto ptr = VirtualAlloc(startAddress, sizeToCommit, MEM_COMMIT, ToDword(_vpo));

	assert(ptr == startAddress);
	for (auto i = 0u; i < numberOfPagesToCommit; ++i) {
		assert(_pagesCommitMask[i + startPageIndex] == false);
		_pagesCommitMask[i + startPageIndex] = true;
	}
	return CommittedMemoryToken(ptr, sizeToCommit);
}

void Luxko::Memory::VirtualMemory::DeCommit(const CommittedMemoryToken& cmt)
{
	static auto pageSize = Win32Basic::SystemInfo::PageSize();
	BOOL result = VirtualFree(cmt.StartAddress, cmt.SizeInBytes, MEM_DECOMMIT);
	assert(result != FALSE);
	auto startIndex = FindPageIndexFromAddress(cmt.StartAddress);
	auto indexCount = cmt.SizeInBytes / pageSize;
	assert(startIndex + indexCount <= _pagesCommitMask.size());
	assert(cmt.SizeInBytes % pageSize == 0);
	for (auto i = 0u; i < indexCount; ++i) {
		assert(_pagesCommitMask[i + startIndex] == true);
		_pagesCommitMask[i + startIndex] = false;
	}
}

size_t Luxko::Memory::VirtualMemory::FindPageIndexFromAddress(void* address) const noexcept
{
	auto offset = reinterpret_cast<size_t>(address) - reinterpret_cast<size_t>(_basePtr);
	return offset / Win32Basic::SystemInfo::PageSize();
}

void* Luxko::Memory::VirtualMemory::GetAddressFromPageIndex(size_t index) const noexcept
{
	return reinterpret_cast<void*>(reinterpret_cast<size_t>(_basePtr) + index*Win32Basic::SystemInfo::PageSize());
}

size_t Luxko::Memory::VirtualMemory::GetPagesCount() const noexcept
{
	return _pagesCommitMask.size();
}

bool Luxko::Memory::VirtualMemory::IsPageCommitted(size_t pageIndex) const
{
	return _pagesCommitMask.at(pageIndex);
}

size_t Luxko::Memory::VirtualMemory::AlignToPageSize(size_t toBeAligned)
{
	static auto pageSize = static_cast<size_t>(Win32Basic::SystemInfo::PageSize());
	return (toBeAligned + pageSize - 1) / pageSize;
}

Luxko::Memory::VirtualMemory& Luxko::Memory::VirtualMemory::operator=(VirtualMemory&& vm)
{
	if (&vm == this) return *this;
	DeCommitAllAndRelease();
	_basePtr = vm._basePtr;
	vm._basePtr = nullptr;
	_pagesCommitMask = std::move(vm._pagesCommitMask);
	_vpo = vm._vpo;
	return *this;
}

Luxko::Memory::VirtualMemory::VirtualMemory(VirtualMemory&& vm)
{
	_basePtr = vm._basePtr;
	vm._basePtr = nullptr;
	_pagesCommitMask = std::move(vm._pagesCommitMask);
	_vpo = vm._vpo;
}
