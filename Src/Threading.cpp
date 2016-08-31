#include "Threading.h"
#include <cassert>
static DWORD ToDword() {
	return 0;
}

template<typename T, typename ... Types>
static DWORD ToDword(T t, const Types& ... types) {
	return static_cast<DWORD>(t) | ToDword(types...);
}

void Luxko::Threading::SleepFor(DWORD millieSeconds, bool altertable /*= false*/)
{
	BOOL bA = FALSE;
	if (altertable) bA = TRUE;
	SleepEx(millieSeconds, bA);
}

Luxko::Win32Basic::WaitObjectResult Luxko::Threading::WaitForSignal(const Luxko::Win32Basic::KernelObjectHandle& kh, DWORD milliseconds /*= MathConstants::Infinite_DWORD*/)
{
	return static_cast<WaitObjectResult>(WaitForSingleObject(kh.Get(), milliseconds));
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

Luxko::Threading::Overlapped::Overlapped(long long offset, const Event& e)
{
	LARGE_INTEGER li;
	li.QuadPart = offset;
	_ov.Offset = li.LowPart;
	_ov.OffsetHigh = li.HighPart;
	_ov.hEvent = e.Get().Get();
}

long long Luxko::Threading::Overlapped::Offset() const
{
	LARGE_INTEGER li;
	li.LowPart = _ov.Offset;
	li.HighPart = _ov.OffsetHigh;
	return li.QuadPart;
}

void Luxko::Threading::Overlapped::SetEvent(const Event& e)
{
	_ov.hEvent = e.Get().Get();
}

ULONG_PTR Luxko::Threading::Overlapped::ErrorCode() const
{
	return _ov.Internal;
}

ULONG_PTR Luxko::Threading::Overlapped::BytesTransferred() const
{
	return _ov.InternalHigh;
}

void Luxko::Threading::Overlapped::Offset(long long offset)
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

const Luxko::Win32Basic::KernelObjectHandle& Luxko::Threading::Event::Get() const noexcept
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

const Luxko::Win32Basic::KernelObjectHandle& Luxko::Threading::Semaphore::Get() const noexcept
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

const Luxko::Win32Basic::KernelObjectHandle& Luxko::Threading::Mutex::Get() const noexcept
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

HANDLE Luxko::Threading::Thread::GetCurrentThreadFakeRawHandle()
{
	return GetCurrentThread();
}

DWORD Luxko::Threading::Thread::GetCurrentThreadId()
{
	return ::GetCurrentThreadId();
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

const Luxko::Win32Basic::KernelObjectHandle& Luxko::Threading::Thread::Get() const noexcept
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

Luxko::Threading::IOCPort::IOCPort(IOCPort&& e) noexcept
{
	_hIOCompletionPort = std::move(e._hIOCompletionPort);
}

Luxko::Threading::IOCPort Luxko::Threading::IOCPort::Create(DWORD numberOfConcurrentThreads)
{
	auto kh = static_cast<KernelObjectHandle>(CreateIoCompletionPort(INVALID_HANDLE_VALUE,
		nullptr, static_cast<LONG_PTR>(0), numberOfConcurrentThreads));
	IOCPort result;
	result._hIOCompletionPort = std::move(kh);
	return std::move(result);
}

const Luxko::Win32Basic::KernelObjectHandle& Luxko::Threading::IOCPort::Get() const noexcept
{
	return _hIOCompletionPort;
}

bool Luxko::Threading::IOCPort::AssociateDevice(const Luxko::Win32Basic::KernelObjectHandle& deviceHandle, ULONG_PTR completionKey) noexcept
{
	assert(_hIOCompletionPort.Valid());
	return _hIOCompletionPort.Get() == CreateIoCompletionPort(deviceHandle.Get(), _hIOCompletionPort.Get(),
		completionKey, 0);
}

Luxko::Threading::IOCPort::GetStatusCallResult Luxko::Threading::IOCPort::GetIOCStatusQueued(IOCStatusInfo& statusInfoHolder, DWORD milliSecondsToWait)
{
	auto sOK = GetQueuedCompletionStatus(_hIOCompletionPort.Get(),
		&statusInfoHolder.NumberOfBytes, &statusInfoHolder.CompletionKey,
		reinterpret_cast<LPOVERLAPPED*>(&statusInfoHolder.pOverlapped),
		milliSecondsToWait);
	DWORD lastError = GetLastError();
	if (sOK) {
		return GetStatusCallResult::Success;
	}
	else {
		if (statusInfoHolder.pOverlapped != nullptr) return GetStatusCallResult::FailedIOCRequest;
		else if (lastError == WAIT_TIMEOUT) return GetStatusCallResult::TimeOut;
		else return GetStatusCallResult::BadCall;
	}
}

bool Luxko::Threading::IOCPort::PostIOCStatusToQueue(const IOCStatusInfo& statusInfoToPost)
{
	return FALSE != PostQueuedCompletionStatus(_hIOCompletionPort.Get(), statusInfoToPost.NumberOfBytes,
		statusInfoToPost.CompletionKey, statusInfoToPost.pOverlapped->OvPtr());
}

Luxko::Threading::IOCPort& Luxko::Threading::IOCPort::operator=(IOCPort&& e) noexcept
{
	_hIOCompletionPort = std::move(e._hIOCompletionPort);
	return *this;
}

Luxko::Threading::ThreadPoolEnvironment::ThreadPoolEnvironment()
{
	_pool = nullptr;
	_pEnvironment = nullptr;

}

Luxko::Threading::ThreadPoolEnvironment::~ThreadPoolEnvironment()
{
	Close();
}

PTP_CALLBACK_ENVIRON Luxko::Threading::ThreadPoolEnvironment::Get() const noexcept
{
	return _pEnvironment;
}

PTP_POOL Luxko::Threading::ThreadPoolEnvironment::GetPool() const noexcept
{
	return _pool;
}

Luxko::Threading::ThreadPoolEnvironment Luxko::Threading::ThreadPoolEnvironment::Create()
{
	ThreadPoolEnvironment tpe;
	tpe._pool = CreateThreadpool(nullptr);
	InitializeThreadpoolEnvironment(&tpe._environment);
	tpe._pEnvironment = &tpe._environment;
	SetThreadpoolCallbackPool(tpe._pEnvironment, tpe._pool);
	return std::move(tpe);
}

void Luxko::Threading::ThreadPoolEnvironment::Close() noexcept
{
	if (_pool) {
		CloseThreadpool(_pool);
		_pool = nullptr;
	}
	if (_pEnvironment) {
		DestroyThreadpoolEnvironment(_pEnvironment);
		_pEnvironment = nullptr;
	}
}

bool Luxko::Threading::ThreadPoolEnvironment::SetMinimumThreadCount(DWORD minimum)
{
	return FALSE != SetThreadpoolThreadMinimum(_pool, minimum);
}

void Luxko::Threading::ThreadPoolEnvironment::SetMaximumThreadCount(DWORD maximum)
{
	SetThreadpoolThreadMaximum(_pool, maximum);
}

Luxko::Threading::ThreadPoolEnvironment& Luxko::Threading::ThreadPoolEnvironment::operator=(ThreadPoolEnvironment&& tpe)
{
	if (&tpe == this) return *this;
	Close();
	_pool = tpe._pool;
	tpe._pool = nullptr;

	CopyMemory(&_environment, &tpe._environment, sizeof(TP_CALLBACK_ENVIRON));
	if (tpe._pEnvironment) _pEnvironment = &_environment;
	else _pEnvironment = nullptr;

	tpe._pEnvironment = nullptr;

	return *this;
}

Luxko::Threading::ThreadPoolEnvironment::ThreadPoolEnvironment(ThreadPoolEnvironment&& tpe)
{
	_pool = tpe._pool;
	tpe._pool = nullptr;

	CopyMemory(&_environment, &tpe._environment, sizeof(TP_CALLBACK_ENVIRON));
	if (tpe._pEnvironment) _pEnvironment = &_environment;
	else _pEnvironment = nullptr;

	tpe._pEnvironment = nullptr;
}

Luxko::Threading::ThreadPoolWork::ThreadPoolWork()
{
	_work = nullptr;
}

Luxko::Threading::ThreadPoolWork::~ThreadPoolWork()
{
	Close();
}

Luxko::Threading::ThreadPoolWork& Luxko::Threading::ThreadPoolWork::Interpret(PTP_WORK* pWork) noexcept
{
	auto pied = reinterpret_cast<ThreadPoolWork*>(pWork);
	return *pied;
}

Luxko::Threading::ThreadPoolWork Luxko::Threading::ThreadPoolWork::Create(Callback workHandler, void* workHandlerContext, const ThreadPoolEnvironment& tpe)
{
	ThreadPoolWork tpw;
	tpw._work = CreateThreadpoolWork(workHandler, workHandlerContext, tpe.Get());
	return std::move(tpw);
}

void Luxko::Threading::ThreadPoolWork::Close() noexcept
{
	if (_work) {
		CloseThreadpoolWork(_work);
		_work = nullptr;
	}
}

void Luxko::Threading::ThreadPoolWork::Submit() noexcept
{
	SubmitThreadpoolWork(_work);
}

bool Luxko::Threading::ThreadPoolWork::Valid() const noexcept
{
	return _work != nullptr;
}

void Luxko::Threading::ThreadPoolWork::Wait(bool cancelPending)
{
	BOOL cpB = FALSE;
	if (cancelPending) cpB = TRUE;
	WaitForThreadpoolWorkCallbacks(_work, cpB);
}

PTP_WORK Luxko::Threading::ThreadPoolWork::Get() const noexcept
{
	return _work;
}

Luxko::Threading::ThreadPoolWork& Luxko::Threading::ThreadPoolWork::operator=(ThreadPoolWork&& tg)
{
	if (&tg == this) return *this;
	Close();
	_work = tg._work;
	tg, _work = nullptr;
	return *this;
}

Luxko::Threading::ThreadPoolWork::ThreadPoolWork(ThreadPoolWork&& tg)
{
	_work = tg._work;
	tg, _work = nullptr;
}

Luxko::Threading::ThreadPoolTimer::ThreadPoolTimer()
{
	_timer = nullptr;
}

Luxko::Threading::ThreadPoolTimer::~ThreadPoolTimer()
{
	Close();
}

Luxko::Threading::ThreadPoolTimer& Luxko::Threading::ThreadPoolTimer::Interpret(PTP_TIMER* pTimer) noexcept
{
	auto pied = reinterpret_cast<ThreadPoolTimer*>(pTimer);
	return *pied;
}

Luxko::Threading::ThreadPoolTimer Luxko::Threading::ThreadPoolTimer::Create(Callback timerHandler, void* timerHandlerContext, const ThreadPoolEnvironment& tpe)
{
	ThreadPoolTimer tpt;
	tpt._timer = CreateThreadpoolTimer(timerHandler, timerHandlerContext, tpe.Get());
	return std::move(tpt);
}

void Luxko::Threading::ThreadPoolTimer::Close() noexcept
{
	if (_timer) {
		CloseThreadpoolTimer(_timer);
		_timer = nullptr;
	}
}

void Luxko::Threading::ThreadPoolTimer::Set(const Luxko::Win32Basic::FileTime& firstSetAt, DWORD msPeriod /*= 0*/, DWORD msWindowLength /*= 0*/) noexcept
{
	SetThreadpoolTimer(_timer, const_cast<PFILETIME>(&firstSetAt.m_ft), msPeriod, msWindowLength);
}

bool Luxko::Threading::ThreadPoolTimer::Valid() const noexcept
{
	return _timer != nullptr;
}

bool Luxko::Threading::ThreadPoolTimer::IsSet() noexcept
{
	return FALSE != IsThreadpoolTimerSet(_timer);
}

void Luxko::Threading::ThreadPoolTimer::Wait(bool cancelPending)
{
	BOOL cpB = FALSE;
	if (cancelPending) cpB = TRUE;
	WaitForThreadpoolTimerCallbacks(_timer, cpB);
}

PTP_TIMER Luxko::Threading::ThreadPoolTimer::Get() const noexcept
{
	return _timer;
}

Luxko::Threading::ThreadPoolTimer& Luxko::Threading::ThreadPoolTimer::operator=(ThreadPoolTimer&& tg)
{
	if (&tg == this) return *this;
	Close();
	_timer = tg._timer;
	tg._timer = nullptr;
	return *this;
}

Luxko::Threading::ThreadPoolTimer::ThreadPoolTimer(ThreadPoolTimer&& tg)
{
	_timer = tg._timer;
	tg._timer = nullptr;
}

Luxko::Threading::ThreadPoolWaiter::ThreadPoolWaiter()
{
	_waiter = nullptr;
}

Luxko::Threading::ThreadPoolWaiter::~ThreadPoolWaiter()
{
	Close();
}

Luxko::Threading::ThreadPoolWaiter& Luxko::Threading::ThreadPoolWaiter::Interpret(PTP_TIMER* pTimer) noexcept
{
	auto pied = reinterpret_cast<ThreadPoolWaiter*>(pTimer);
	return *pied;
}

Luxko::Threading::ThreadPoolWaiter Luxko::Threading::ThreadPoolWaiter::Create(Callback timerHandler, void* timerHandlerContext, const ThreadPoolEnvironment& tpe)
{
	ThreadPoolWaiter tpwa;
	tpwa._waiter = CreateThreadpoolWait(timerHandler, timerHandlerContext, tpe.Get());
	return std::move(tpwa);
}

void Luxko::Threading::ThreadPoolWaiter::Close() noexcept
{
	if (_waiter) {
		CloseThreadpoolWait(_waiter);
		_waiter = nullptr;
	}
}

void Luxko::Threading::ThreadPoolWaiter::Set(const Luxko::Win32Basic::KernelObjectHandle& objectToWait, Luxko::Win32Basic::FileTime* waitTime /*= nullptr /* 0 == no waiting, nullptr == always wait */) noexcept
{
	SetThreadpoolWait(_waiter, objectToWait.Get(), reinterpret_cast<PFILETIME>(waitTime));
}

bool Luxko::Threading::ThreadPoolWaiter::Valid() const noexcept
{
	return _waiter != nullptr;
}

void Luxko::Threading::ThreadPoolWaiter::Wait(bool cancelPending)
{
	BOOL cpB = FALSE;
	if (cancelPending) cpB = TRUE;
	WaitForThreadpoolWaitCallbacks(_waiter, cpB);
}

PTP_WAIT Luxko::Threading::ThreadPoolWaiter::Get() const noexcept
{
	return _waiter;
}

Luxko::Threading::ThreadPoolWaiter& Luxko::Threading::ThreadPoolWaiter::operator=(ThreadPoolWaiter&& tg)
{
	if (&tg == this) return *this;
	Close();
	_waiter = tg._waiter;
	tg._waiter = nullptr;
	return *this;
}

Luxko::Threading::ThreadPoolWaiter::ThreadPoolWaiter(ThreadPoolWaiter&& tg)
{
	_waiter = tg._waiter;
	tg._waiter = nullptr;
}

Luxko::Threading::ThreadPoolIO::ThreadPoolIO()
{
	_io = nullptr;
}

Luxko::Threading::ThreadPoolIO::~ThreadPoolIO()
{
	Close();
}

Luxko::Threading::ThreadPoolIO Luxko::Threading::ThreadPoolIO::Create(const Luxko::Win32Basic::KernelObjectHandle& deviceHandle, Callback ioHandler, void* ioHandlerContext, const ThreadPoolEnvironment& tpe)
{
	ThreadPoolIO tpi;
	tpi._io = CreateThreadpoolIo(deviceHandle.Get(), ioHandler, ioHandlerContext, tpe.Get());
	return std::move(tpi);
}

Luxko::Threading::ThreadPoolIO& Luxko::Threading::ThreadPoolIO::Interpret(PTP_IO* pPtpIo)
{
	auto pied = reinterpret_cast<ThreadPoolIO*>(pPtpIo);
	return *pied;
}

bool Luxko::Threading::ThreadPoolIO::Valid() const noexcept
{
	return _io != nullptr;
}

void Luxko::Threading::ThreadPoolIO::Close() noexcept
{
	if (_io) {
		CloseThreadpoolIo(_io);
		_io = nullptr;
	}
}

void Luxko::Threading::ThreadPoolIO::Start() noexcept
{
	StartThreadpoolIo(_io);
}

void Luxko::Threading::ThreadPoolIO::Cancel() noexcept
{
	CancelThreadpoolIo(_io);
}

void Luxko::Threading::ThreadPoolIO::Wait(bool cancelPending)
{
	BOOL cpB = FALSE;
	if (cancelPending) cpB = TRUE;
	WaitForThreadpoolIoCallbacks(_io, cpB);
}

PTP_IO Luxko::Threading::ThreadPoolIO::Get() const noexcept
{
	return _io;
}

Luxko::Threading::ThreadPoolIO& Luxko::Threading::ThreadPoolIO::operator=(ThreadPoolIO&& tpi)
{
	if (&tpi == this) return *this;
	Close();
	_io = tpi._io;
	tpi._io = nullptr;
	return *this;
}

Luxko::Threading::ThreadPoolIO::ThreadPoolIO(ThreadPoolIO&& tpi)
{
	_io = tpi._io;
	tpi._io = nullptr;
}
