#include "Threading.h"

static DWORD ToDword() {
	return 0;
}

template<typename T, typename ... Types>
static DWORD ToDword(T t, const Types& ... types) {
	return static_cast<DWORD>(t) | ToDword(types...);
}

Luxko::Threading::SynchronizationAccessRight Luxko::Threading::operator|(SynchronizationAccessRight a, SynchronizationAccessRight b)
{
	return static_cast<SynchronizationAccessRight>(ToDword(a, b));
}

Luxko::Threading::SynchronizationAccessRight Luxko::Threading::operator&(SynchronizationAccessRight a, SynchronizationAccessRight b)
{
	return static_cast<SynchronizationAccessRight>(ToDword(a)&ToDword(b));
}


Luxko::Threading::Event Luxko::Threading::Event::OpenEvent(const std::wstring& eventName, EventAccessRight ear /*= EventAccessRight::ModifyState*/, SynchronizationAccessRight sar /*= SynchronizationAccessRight::Synchronize*/, bool inheritable/* = false*/)
{
	Event e;
	e._hEvent = ::OpenEvent(ToDword(ear, sar), inheritable, eventName.c_str());
	return e;
}

Luxko::Threading::EventNotifyOne Luxko::Threading::EventNotifyOne::Open(std::wstring& eventName, EventAccessRight ear /*= EventAccessRight::ModifyState*/, SynchronizationAccessRight sar /*= SynchronizationAccessRight::Synchronize*/, bool inheritable /*= false*/)
{
	auto e = Event::OpenEvent(eventName, ear, sar, inheritable);
	EventNotifyOne se;
	se._hEvent = e.Handle();
	return se;
}

Luxko::Threading::EventNotifyOne Luxko::Threading::EventNotifyOne::CreateNameLess(bool initiallySet, SECURITY_ATTRIBUTES* sa /*= nullptr*/)
{
	EventNotifyOne se;
	se._hEvent = CreateEvent(sa, FALSE, initiallySet, nullptr);
	return se;
}

Luxko::Threading::EventNotifyOne Luxko::Threading::EventNotifyOne::CreateNamed(bool initiallySet, const std::wstring& name, SECURITY_ATTRIBUTES* sa /*= nullptr*/)
{
	EventNotifyOne se;
	se._hEvent = CreateEvent(sa, FALSE, initiallySet, name.c_str());
	return se;
}

Luxko::Threading::EventNotifyAll Luxko::Threading::EventNotifyAll::Open(std::wstring& eventName, EventAccessRight ear /*= EventAccessRight::ModifyState*/, SynchronizationAccessRight sar /*= SynchronizationAccessRight::Synchronize*/, bool inheritable /*= false*/)
{
	auto e = Event::OpenEvent(eventName, ear, sar, inheritable);
	EventNotifyAll se;
	se._hEvent = e.Handle();
	return se;
}

Luxko::Threading::EventNotifyAll Luxko::Threading::EventNotifyAll::CreateNameLess(bool initiallySet, SECURITY_ATTRIBUTES* sa /*= nullptr*/)
{
	EventNotifyAll ena;
	ena._hEvent = CreateEvent(sa, TRUE, initiallySet, nullptr);
	return ena;
}

Luxko::Threading::EventNotifyAll Luxko::Threading::EventNotifyAll::CreateNamed(bool initiallySet, const std::wstring& name, SECURITY_ATTRIBUTES* sa /*= nullptr*/)
{
	EventNotifyAll ena;
	ena._hEvent = CreateEvent(sa, TRUE, initiallySet, name.c_str());
	return ena;
}

Luxko::Threading::Overlap::Overlap(long long offset, const Event& e)
{
	LARGE_INTEGER li;
	li.QuadPart = offset;
	_ov.Offset = li.LowPart;
	_ov.OffsetHigh = li.HighPart;
	_ov.hEvent = e.Handle();
}

long long Luxko::Threading::Overlap::Offset() const
{
	LARGE_INTEGER li;
	li.LowPart = _ov.Offset;
	li.HighPart = _ov.OffsetHigh;
	return li.QuadPart;
}

void Luxko::Threading::Overlap::SetEvent(const Event& e)
{
	_ov.hEvent = e.Handle();
}

void Luxko::Threading::Overlap::Offset(long long offset)
{
	LARGE_INTEGER li;
	li.QuadPart = offset;
	_ov.Offset = li.LowPart;
	_ov.OffsetHigh = li.HighPart;
}

Luxko::Threading::CriticalSection::CriticalSection() noexcept
{
	_valid = false;
}

bool Luxko::Threading::CriticalSection::Initialize(DWORD spinCount /*= 0*/) noexcept
{
	if (_valid) return false;
	return InitializeCriticalSectionAndSpinCount(&_cs, spinCount);
}

DWORD Luxko::Threading::CriticalSection::SetSpinCount(DWORD spinCount) noexcept
{
	return SetCriticalSectionSpinCount(&_cs, spinCount);
}

Luxko::Threading::CriticalSection& Luxko::Threading::CriticalSection::operator=(CriticalSection&& cs) noexcept
{
	if (_valid) DeleteCriticalSection(&_cs);
	_cs = cs._cs;
	_valid = cs._valid;
	cs._valid = false;
}

Luxko::Threading::CriticalSection::CriticalSection(CriticalSection&& cs) noexcept
{
	_cs = cs._cs;
	_valid = cs._valid;
	cs._valid = false;
}

Luxko::Threading::CriticalSection::~CriticalSection() noexcept
{
	if (_valid) DeleteCriticalSection(&_cs);
}

Luxko::Threading::CriticalSectionToken::CriticalSectionToken(CriticalSection& cs)
{
	_ptr = &cs._cs;
	if(cs._valid) EnterCriticalSection(_ptr);
	_valid = cs._valid;
}

void Luxko::Threading::CriticalSectionToken::Release() noexcept
{
	if (_valid) {
		LeaveCriticalSection(_ptr);
		_valid = false;
	}
}



bool Luxko::Threading::CriticalSectionToken::SleepOnCV(ConditionVariable& cv, DWORD milliSeconds /*= INFINITE*/) noexcept
{
	if (!_valid) return false;
	return FALSE != SleepConditionVariableCS(&cv._cv, _ptr, milliSeconds);
}

Luxko::Threading::CriticalSectionToken::~CriticalSectionToken() noexcept
{
	if(_valid) LeaveCriticalSection(_ptr);
}

Luxko::Threading::SlimRWLock::SlimRWLock() noexcept
{
	InitializeSRWLock(&_lock);
}

Luxko::Threading::SlimRWLockSharedToken::SlimRWLockSharedToken(SlimRWLock& srwLock)
{
	AcquireSRWLockShared(&srwLock._lock);
	_ptr = &srwLock._lock;
}

void Luxko::Threading::SlimRWLockSharedToken::Release() noexcept
{
	if (_ptr) {
		ReleaseSRWLockShared(_ptr);
		_ptr = nullptr;
	}
}

bool Luxko::Threading::SlimRWLockSharedToken::SleepOnCV(ConditionVariable& cv, DWORD milliSeconds /*= INFINITE*/) noexcept
{
	if (_ptr == nullptr) return false;
	return FALSE != SleepConditionVariableSRW(&cv._cv, _ptr, milliSeconds, CONDITION_VARIABLE_LOCKMODE_SHARED);
}

Luxko::Threading::SlimRWLockSharedToken::~SlimRWLockSharedToken() noexcept
{
	if(_ptr) ReleaseSRWLockShared(_ptr);
}

Luxko::Threading::SlimRWLockExclusiveToken::SlimRWLockExclusiveToken(SlimRWLock& srwLock)
{
	AcquireSRWLockExclusive(&srwLock._lock);
	_ptr = &srwLock._lock;
}

void Luxko::Threading::SlimRWLockExclusiveToken::Release() noexcept
{
	if (_ptr) {
		ReleaseSRWLockExclusive(_ptr);
		_ptr = nullptr;
	}
}

bool Luxko::Threading::SlimRWLockExclusiveToken::SleepOnCV(ConditionVariable& cv, DWORD milliSeconds /*= INFINITE*/) noexcept
{
	if (_ptr == nullptr) return false;
	return FALSE != SleepConditionVariableSRW(&cv._cv, _ptr, milliSeconds, 0ul);
}

Luxko::Threading::SlimRWLockExclusiveToken::~SlimRWLockExclusiveToken() noexcept
{
	if (_ptr) ReleaseSRWLockExclusive(_ptr);
}

Luxko::Threading::ConditionVariable::ConditionVariable() noexcept
{
	InitializeConditionVariable(&_cv);
}

void Luxko::Threading::ConditionVariable::WakeOne() noexcept
{
	WakeConditionVariable(&_cv);
}

void Luxko::Threading::ConditionVariable::WakeAll() noexcept
{
	WakeAllConditionVariable(&_cv);
}
