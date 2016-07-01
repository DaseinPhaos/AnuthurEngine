#include "Threading.h"
#include <cassert>
static DWORD ToDword() {
	return 0;
}

template<typename T, typename ... Types>
static DWORD ToDword(T t, const Types& ... types) {
	return static_cast<DWORD>(t) | ToDword(types...);
}

Luxko::Threading::EventSynchronizationAccessRight Luxko::Threading::operator|(EventSynchronizationAccessRight a, EventSynchronizationAccessRight b)
{
	return static_cast<EventSynchronizationAccessRight>(ToDword(a, b));
}

Luxko::Threading::EventSynchronizationAccessRight Luxko::Threading::operator&(EventSynchronizationAccessRight a, EventSynchronizationAccessRight b)
{
	return static_cast<EventSynchronizationAccessRight>(ToDword(a)&ToDword(b));
}


LUXKOUTILITY_API Luxko::Threading::EventCreationFlags Luxko::Threading::operator|(EventCreationFlags a, EventCreationFlags b)
{
	return static_cast<EventCreationFlags>(ToDword(a, b));
}

LUXKOUTILITY_API Luxko::Threading::EventCreationFlags Luxko::Threading::operator&(EventCreationFlags a, EventCreationFlags b)
{
	return static_cast<EventCreationFlags>(ToDword(a)&ToDword(b));
}


//#pragma region Old Event Implementation
//Luxko::Threading::Event Luxko::Threading::Event::OpenEvent(const std::wstring& eventName, EventAccessRight ear /*= EventAccessRight::ModifyState*/, SynchronizationAccessRight sar /*= SynchronizationAccessRight::Synchronize*/, bool inheritable/* = false*/)
//{
//	Event e;
//	e._hEvent = ::OpenEvent(ToDword(ear, sar), inheritable, eventName.c_str());
//	return e;
//}
//
//Luxko::Threading::EventNotifyOne Luxko::Threading::EventNotifyOne::Open(std::wstring& eventName, EventAccessRight ear /*= EventAccessRight::ModifyState*/, SynchronizationAccessRight sar /*= SynchronizationAccessRight::Synchronize*/, bool inheritable /*= false*/)
//{
//	auto e = Event::OpenEvent(eventName, ear, sar, inheritable);
//	EventNotifyOne se;
//	se._hEvent = e.Handle();
//	return se;
//}
//
//Luxko::Threading::EventNotifyOne Luxko::Threading::EventNotifyOne::CreateNameLess(bool initiallySet, SECURITY_ATTRIBUTES* sa /*= nullptr*/)
//{
//	EventNotifyOne se;
//	se._hEvent = CreateEvent(sa, FALSE, initiallySet, nullptr);
//	return se;
//}
//
//Luxko::Threading::EventNotifyOne Luxko::Threading::EventNotifyOne::CreateNamed(bool initiallySet, const std::wstring& name, SECURITY_ATTRIBUTES* sa /*= nullptr*/)
//{
//	EventNotifyOne se;
//	se._hEvent = CreateEvent(sa, FALSE, initiallySet, name.c_str());
//	return se;
//}
//
//Luxko::Threading::EventNotifyAll Luxko::Threading::EventNotifyAll::Open(std::wstring& eventName, EventAccessRight ear /*= EventAccessRight::ModifyState*/, SynchronizationAccessRight sar /*= SynchronizationAccessRight::Synchronize*/, bool inheritable /*= false*/)
//{
//	auto e = Event::OpenEvent(eventName, ear, sar, inheritable);
//	EventNotifyAll se;
//	se._hEvent = e.Handle();
//	return se;
//}
//
//Luxko::Threading::EventNotifyAll Luxko::Threading::EventNotifyAll::CreateNameLess(bool initiallySet, SECURITY_ATTRIBUTES* sa /*= nullptr*/)
//{
//	EventNotifyAll ena;
//	ena._hEvent = CreateEvent(sa, TRUE, initiallySet, nullptr);
//	return ena;
//}
//
//Luxko::Threading::EventNotifyAll Luxko::Threading::EventNotifyAll::CreateNamed(bool initiallySet, const std::wstring& name, SECURITY_ATTRIBUTES* sa /*= nullptr*/)
//{
//	EventNotifyAll ena;
//	ena._hEvent = CreateEvent(sa, TRUE, initiallySet, name.c_str());
//	return ena;
//}
//#pragma endregion Old Event Implementation

Luxko::Threading::Overlap::Overlap(long long offset, const Event& e)
{
	LARGE_INTEGER li;
	li.QuadPart = offset;
	_ov.Offset = li.LowPart;
	_ov.OffsetHigh = li.HighPart;
	_ov.hEvent = e.Get().Get();
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
	_ov.hEvent = e.Get().Get();
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
	return FALSE != InitializeCriticalSectionAndSpinCount(&_cs, spinCount);
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
	return *this;
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

Luxko::Threading::CriticalSectionToken::CriticalSectionToken(CriticalSection& cs, bool tryEnter/* = false*/)
{
	if (cs._valid) {
		if (tryEnter) {
			auto pcs = &cs._cs;
			if (FALSE == TryEnterCriticalSection(pcs))
				_ptr = nullptr;
			else _ptr = pcs;
		}
		else {
			_ptr = &cs._cs;
			EnterCriticalSection(_ptr);
		}
	}
	else {
		_ptr = nullptr;
	}
}

void Luxko::Threading::CriticalSectionToken::Release() noexcept
{
	if (_ptr) {
		LeaveCriticalSection(_ptr);
		_ptr = nullptr;
	}
}



bool Luxko::Threading::CriticalSectionToken::SleepOnCV(ConditionVariable& cv, DWORD milliSeconds /*= INFINITE*/) noexcept
{
	if (_ptr == nullptr) return false;
	return FALSE != SleepConditionVariableCS(&cv._cv, _ptr, milliSeconds);
}

bool Luxko::Threading::CriticalSectionToken::IsValid() const noexcept
{
	return _ptr != nullptr;
}

Luxko::Threading::CriticalSectionToken::~CriticalSectionToken() noexcept
{
	if(_ptr) LeaveCriticalSection(_ptr);
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

Luxko::Threading::KernelObjectHandle::KernelObjectHandle(HANDLE handle) noexcept
{
	if (handle == INVALID_HANDLE_VALUE) handle = nullptr;
	_handle = handle;
}



Luxko::Threading::KernelObjectHandle::~KernelObjectHandle() noexcept
{
	Release();
}

void Luxko::Threading::KernelObjectHandle::Release() noexcept
{
	if (_handle == nullptr/* ||
		_handle == INVALID_HANDLE_VALUE*/) return;
	CloseHandle(_handle);
	_handle = nullptr;
}

HANDLE Luxko::Threading::KernelObjectHandle::Get() const noexcept
{
	return _handle;
}

Luxko::Threading::WaitObjectResult Luxko::Threading::KernelObjectHandle::WaitFor(DWORD milliSeconds /*= INFINITE*/) const
{
	return static_cast<WaitObjectResult>(WaitForSingleObject(_handle, milliSeconds));
}

Luxko::Threading::WaitObjectResult Luxko::Threading::KernelObjectHandle::SignalAndWait(const KernelObjectHandle& toSignal, /* Must be a singalable kernal object, i.e. a mutex, a semaphore or a event. */ const KernelObjectHandle& toWaitFor, DWORD milliSeconds /*= INFINITE*/, bool altertable /*= false */)
{
	BOOL aB = FALSE;
	if (altertable) aB = TRUE;
	return static_cast<WaitObjectResult>(SignalObjectAndWait(toSignal.Get(), toWaitFor.Get(), milliSeconds, aB));
}

Luxko::Threading::KernelObjectHandle& Luxko::Threading::KernelObjectHandle::operator=(KernelObjectHandle&& kh) noexcept
{
	Release();
	_handle = kh._handle;
	kh._handle = nullptr;
	return *this;
}

Luxko::Threading::KernelObjectHandle::KernelObjectHandle(KernelObjectHandle&& kh) noexcept
{
	_handle = kh._handle;
	kh._handle = nullptr;
}

Luxko::Threading::KernelObjectHandle::KernelObjectHandle() noexcept
{
	_handle = nullptr;
}

Luxko::Threading::Event::Event(Event&& e) noexcept
	:_hEvent(std::move(e._hEvent)) { }

Luxko::Threading::Event Luxko::Threading::Event::Create(LPTSTR eventName /*= nullptr*/, EventCreationFlags flags /*= EventCreationFlags::Default*/, EventSynchronizationAccessRight access /*= SynchronizationAccessRight::All*/, PSECURITY_ATTRIBUTES sa /*= nullptr*/) noexcept
{
	auto ke = static_cast<KernelObjectHandle>(CreateEventEx(sa,
		eventName, ToDword(flags), ToDword(access)));
	Event e;
	e._hEvent = std::move(ke);
	return std::move(e);
}

Luxko::Threading::Event Luxko::Threading::Event::Open(LPTSTR eventName, EventSynchronizationAccessRight access /*= SynchronizationAccessRight::All*/, bool inherit /*= false*/) noexcept
{
	auto ke = static_cast<KernelObjectHandle>(OpenEvent(ToDword(access),
		static_cast<BOOL>(inherit), eventName));
	Event e;
	e._hEvent = std::move(ke);
	return std::move(e);
}

bool Luxko::Threading::Event::Set() noexcept
{
	return FALSE != SetEvent(_hEvent.Get());
}

bool Luxko::Threading::Event::Reset() noexcept
{
	return FALSE != ResetEvent(_hEvent.Get());
}

const Luxko::Threading::KernelObjectHandle& Luxko::Threading::Event::Get() const noexcept
{
	return _hEvent;
}

Luxko::Threading::Event& Luxko::Threading::Event::operator=(Event&& e)noexcept
{
	_hEvent = std::move(e._hEvent);
	return *this;
}

Luxko::Threading::Semaphore::Semaphore(Semaphore&& e) noexcept
{
	_hSemaphore = std::move(e._hSemaphore);
}

Luxko::Threading::Semaphore Luxko::Threading::Semaphore::Create(LONG maximumCount, LONG initialCount /*= 0l*/, LPTSTR semaphoreName /*= nullptr*/, SemaphoreAccessRight desiredAccess /*= SemaphoreAccessRight::All*/, PSECURITY_ATTRIBUTES sa) noexcept
{
	Semaphore s;
	auto kh = static_cast<KernelObjectHandle>(CreateSemaphoreEx(sa, initialCount, maximumCount, semaphoreName, 0, ToDword(desiredAccess)));
	s._hSemaphore = std::move(kh);
	return std::move(s);
}

Luxko::Threading::Semaphore Luxko::Threading::Semaphore::Open(LPTSTR semaphoreName, SemaphoreAccessRight desiredAccess /*= SemaphoreAccessRight::Modify*/, bool inherit /*= false*/) noexcept
{
	Semaphore s;
	BOOL iB = FALSE;
	if (inherit) iB = TRUE;
	s._hSemaphore = static_cast<KernelObjectHandle>(OpenSemaphore(ToDword(desiredAccess), iB, semaphoreName));
	return std::move(s);
}

bool Luxko::Threading::Semaphore::IncreaseCount(LONG countToIncrease) noexcept
{
	return FALSE != ReleaseSemaphore(_hSemaphore.Get(), countToIncrease, nullptr);
}

const Luxko::Threading::KernelObjectHandle& Luxko::Threading::Semaphore::Get() const noexcept
{
	return _hSemaphore;
}

Luxko::Threading::Semaphore& Luxko::Threading::Semaphore::operator=(Semaphore&& e) noexcept
{
	_hSemaphore = std::move(e._hSemaphore);
	return *this;
}

Luxko::Threading::Mutex::Mutex(Mutex&& e) noexcept
{
	_hMutex = std::move(e._hMutex);
}

Luxko::Threading::Mutex Luxko::Threading::Mutex::Create(LPTSTR mutexName /*= nullptr*/, bool initiallyOwnedByCreator /*= false*/, MutexAccessRight desiredAccess /*= MutexAccessRight::All*/, PSECURITY_ATTRIBUTES sa /*= nullptr*/) noexcept
{
	Mutex m;
	DWORD flag = 0;
	if (initiallyOwnedByCreator) flag = CREATE_MUTEX_INITIAL_OWNER;
	m._hMutex = static_cast<KernelObjectHandle>(CreateMutexEx(sa, mutexName, flag, ToDword(desiredAccess)));
	return std::move(m);
}

Luxko::Threading::Mutex Luxko::Threading::Mutex::Open(LPTSTR mutexName, bool inherit /*= false*/, MutexAccessRight desiredAccess /*= MutexAccessRight::Modify*/) noexcept
{
	Mutex m;
	BOOL iB = FALSE;
	if (inherit) iB = TRUE;
	m._hMutex = static_cast<KernelObjectHandle>(OpenMutex(ToDword(desiredAccess), iB, mutexName));
	return std::move(m);
}

const Luxko::Threading::KernelObjectHandle& Luxko::Threading::Mutex::Get() const noexcept
{
	return _hMutex;
}

bool Luxko::Threading::Mutex::Release()
{
	return FALSE != ReleaseMutex(_hMutex.Get());
}

Luxko::Threading::Mutex& Luxko::Threading::Mutex::operator=(Mutex&& e) noexcept
{
	_hMutex = std::move(e._hMutex);
	return *this;
}

Luxko::Threading::Thread::Thread(PTHREAD_START_ROUTINE functionInvoked, void* param, ThreadCreationFlags flags /*= ThreadCreationFlags::Default*/, DWORD stackSizeInBytes /*= 0*/, PSECURITY_ATTRIBUTES sa /*= nullptr*/)
{
	_functionInvoked = functionInvoked;
	_paramtersPassed = param;
	_creationFlags = flags;
	_stackSizeInBytes = stackSizeInBytes;
	_psa = sa;
}

void Luxko::Threading::Thread::Begin() noexcept
{
	_hThread = static_cast<KernelObjectHandle>(reinterpret_cast<HANDLE>(_beginthreadex(_psa, _stackSizeInBytes, reinterpret_cast<_beginthreadex_proc_type>(_functionInvoked), _paramtersPassed, static_cast<DWORD>(_creationFlags), reinterpret_cast<unsigned int*>(&_tID))));
}

bool Luxko::Threading::Thread::Terminate(DWORD exitCode) noexcept
{
	return FALSE != TerminateThread(_hThread.Get(), exitCode);
}

DWORD Luxko::Threading::Thread::Resume() noexcept
{
	return ResumeThread(_hThread.Get());
}

DWORD Luxko::Threading::Thread::Suspend() noexcept
{
	return SuspendThread(_hThread.Get());
}



Luxko::Threading::Thread::ThreadTimeInfo Luxko::Threading::Thread::GetThreadTimeInfo()
{
	ThreadTimeInfo tti;
	auto gr = GetThreadTimes(_hThread.Get(), reinterpret_cast<LPFILETIME>(&tti.creationTime),
		reinterpret_cast<LPFILETIME>(&tti.exitTime),
		reinterpret_cast<LPFILETIME>(&tti.kernelTime),
		reinterpret_cast<LPFILETIME>(&tti.userTime));
	assert(FALSE != gr);
	return tti;
}

Luxko::Threading::ThreadPriority Luxko::Threading::Thread::Priority()
{
	return static_cast<ThreadPriority>(GetThreadPriority(_hThread.Get()));
}

bool Luxko::Threading::Thread::Priority(ThreadPriority priority)
{
	return FALSE != SetThreadPriority(_hThread.Get(), ToDword(priority));
}

const Luxko::Threading::KernelObjectHandle& Luxko::Threading::Thread::Get() const noexcept
{
	return _hThread;
}

DWORD Luxko::Threading::Thread::StackSize() const noexcept
{
	return _stackSizeInBytes;
}

DWORD Luxko::Threading::Thread::ID() const noexcept
{
	return _tID;
}

PTHREAD_START_ROUTINE Luxko::Threading::Thread::FunctionInvoked() const noexcept
{
	return _functionInvoked;
}

void* Luxko::Threading::Thread::ParametersPassed() const noexcept
{
	return _paramtersPassed;
}

Luxko::Threading::Thread& Luxko::Threading::Thread::operator=(Thread&& e) noexcept
{
	_tID = e._tID;
	_stackSizeInBytes = e._stackSizeInBytes;
	_creationFlags = e._creationFlags;
	_hThread = std::move(e._hThread);
	_functionInvoked = e._functionInvoked;
	_psa = e._psa;
	_paramtersPassed = e._paramtersPassed;
	return *this;
}

Luxko::Threading::Thread::Thread(Thread&& e) noexcept
{
	_tID = e._tID;
	_stackSizeInBytes = e._stackSizeInBytes;
	_creationFlags = e._creationFlags;
	_hThread = std::move(e._hThread);
	_functionInvoked = e._functionInvoked;
	_psa = e._psa;
	_paramtersPassed = e._paramtersPassed;
}
