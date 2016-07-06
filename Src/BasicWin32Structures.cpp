//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#include "BasicWin32Structures.h"

Luxko::Win32Basic::FileTime::FileTime(const SystemTime& st)
{
	if (!SystemTimeToFileTime(&st.m_st, reinterpret_cast<LPFILETIME>(&m_ft))) {
		throw L"ST to FT conversion Failed.";
	}
}

Luxko::Win32Basic::FileTime::FileTime(LARGE_INTEGER li)
{
	m_ft.dwLowDateTime = li.LowPart;
	m_ft.dwHighDateTime = li.HighPart;
}

bool Luxko::Win32Basic::FileTime::operator<(const FileTime& ft) const
{
	auto r = CompareFileTime(&m_ft, &ft.m_ft);
	return r < 0;
}

Luxko::Win32Basic::FileTime Luxko::Win32Basic::FileTime::ToLocal() const
{
	FileTime result;
	if (!FileTimeToLocalFileTime(&m_ft, &result.m_ft)) {
		throw L"FT to LFT conversion Failed.";
	}
	return result;
}

Luxko::Win32Basic::FileTime Luxko::Win32Basic::FileTime::ToGlobal() const
{
	FileTime result;
	if (!LocalFileTimeToFileTime(&m_ft, &result.m_ft)) {
		throw L"FT to LFT conversion Failed.";
	}
	return result;
}

LARGE_INTEGER Luxko::Win32Basic::FileTime::toLargeInteger() const
{
	LARGE_INTEGER result;
	result.LowPart = m_ft.dwLowDateTime;
	result.HighPart = m_ft.dwHighDateTime;
	return result;
}

bool Luxko::Win32Basic::FileTime::operator==(const FileTime& ft)const
{
	return std::memcmp(&m_ft, &ft.m_ft, sizeof(FILETIME)) == 0;
}

Luxko::Win32Basic::SystemTime::SystemTime(const FileTime& ft)
{
	if (!FileTimeToSystemTime(&ft.m_ft, &m_st)) {
		throw L"FT to ST conversion failed!";
	}
}

bool Luxko::Win32Basic::SystemTime::operator==(const SystemTime& st)const
{
	return std::memcmp(&m_st, &st.m_st, sizeof(SYSTEMTIME)) == 0;
}

const Luxko::Win32Basic::KernelObjectHandle& Luxko::Win32Basic::KernelObjectHandle::InterpretAs(HANDLE* pHandle) noexcept
{
	auto red = reinterpret_cast<KernelObjectHandle*>(pHandle);
	return *red;
}

Luxko::Win32Basic::KernelObjectHandle Luxko::Win32Basic::KernelObjectHandle::Duplicate(HANDLE sourceProcessHandleRaw, const KernelObjectHandle& sourceHandle, HANDLE targetProcessHandleRaw, DWORD desiredAccess, bool Inheritable)
{
	BOOL iB = FALSE;
	if (Inheritable) iB = TRUE;
	HANDLE target = nullptr;
	if (!DuplicateHandle(sourceProcessHandleRaw, sourceHandle.Get(),
		targetProcessHandleRaw, &target, desiredAccess, iB, 0)) target = nullptr;
	return static_cast<KernelObjectHandle>(target);
}

Luxko::Win32Basic::KernelObjectHandle Luxko::Win32Basic::KernelObjectHandle::DuplicateSameAccess(HANDLE sourceProcessHandleRaw, const KernelObjectHandle& sourceHandle, HANDLE targetProcessHandleRaw, bool Inheritable)
{
	BOOL iB = FALSE;
	if (Inheritable) iB = TRUE;
	HANDLE target = nullptr;
	if (!DuplicateHandle(sourceProcessHandleRaw, sourceHandle.Get(),
		targetProcessHandleRaw, &target, 0, iB, DUPLICATE_SAME_ACCESS)) target = nullptr;
	return static_cast<KernelObjectHandle>(target);
}

Luxko::Win32Basic::KernelObjectHandle::KernelObjectHandle(HANDLE handle) noexcept
{
	if (handle == INVALID_HANDLE_VALUE) handle = nullptr;
	_handle = handle;
}



Luxko::Win32Basic::KernelObjectHandle::~KernelObjectHandle() noexcept
{
	Release();
}

void Luxko::Win32Basic::KernelObjectHandle::Release() noexcept
{
	if (_handle == nullptr/* ||
						  _handle == INVALID_HANDLE_VALUE*/) return;
	CloseHandle(_handle);
	_handle = nullptr;
}

HANDLE Luxko::Win32Basic::KernelObjectHandle::Get() const noexcept
{
	return _handle;
}

bool Luxko::Win32Basic::KernelObjectHandle::Valid() const noexcept
{
	return _handle != nullptr && _handle != INVALID_HANDLE_VALUE;
}

Luxko::Win32Basic::WaitObjectResult Luxko::Win32Basic::KernelObjectHandle::WaitFor(DWORD milliSeconds /*= INFINITE*/) const
{
	return static_cast<WaitObjectResult>(WaitForSingleObject(_handle, milliSeconds));
}

Luxko::Win32Basic::WaitObjectResult Luxko::Win32Basic::KernelObjectHandle::SignalAndWait(const KernelObjectHandle& toSignal, /* Must be a singalable kernal object, i.e. a mutex, a semaphore or a event. */ const KernelObjectHandle& toWaitFor, DWORD milliSeconds /*= INFINITE*/, bool altertable /*= false */)
{
	BOOL aB = FALSE;
	if (altertable) aB = TRUE;
	return static_cast<WaitObjectResult>(SignalObjectAndWait(toSignal.Get(), toWaitFor.Get(), milliSeconds, aB));
}

Luxko::Win32Basic::KernelObjectHandle& Luxko::Win32Basic::KernelObjectHandle::operator=(KernelObjectHandle&& kh) noexcept
{
	if (&kh == this) return *this;
	Release();
	_handle = kh._handle;
	kh._handle = nullptr;
	return *this;
}

Luxko::Win32Basic::KernelObjectHandle::KernelObjectHandle(KernelObjectHandle&& kh) noexcept
{
	_handle = kh._handle;
	kh._handle = nullptr;
}

Luxko::Win32Basic::KernelObjectHandle::KernelObjectHandle() noexcept
{
	_handle = nullptr;
}

const Luxko::Win32Basic::SystemInfo& Luxko::Win32Basic::SystemInfo::Get() noexcept
{
	static SystemInfo si;
	return si;
}

DWORD Luxko::Win32Basic::SystemInfo::PageSize() noexcept
{
	return Get().s_si.dwPageSize;
}

LPVOID Luxko::Win32Basic::SystemInfo::MinimumApplicationAddress() noexcept
{
	return Get().s_si.lpMinimumApplicationAddress;
}

LPVOID Luxko::Win32Basic::SystemInfo::MaximumApplicationAddress() noexcept
{
	return Get().s_si.lpMaximumApplicationAddress;
}

DWORD Luxko::Win32Basic::SystemInfo::NumberOfProcessors() noexcept
{
	return Get().s_si.dwNumberOfProcessors;
}

DWORD Luxko::Win32Basic::SystemInfo::ProcessorType() noexcept
{
	return Get().s_si.dwProcessorType;
}

DWORD Luxko::Win32Basic::SystemInfo::AllocationGranularity() noexcept
{
	return Get().s_si.dwAllocationGranularity;
}

Luxko::Win32Basic::SystemInfo::SystemInfo()
{
	GetSystemInfo(&s_si);
}
