//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************
// This part is currently under building.
//**********************************************************************

#pragma once

#include "CommonHeader.h"
#include "BasicWin32Structures.h"
#include <crtdefs.h>
#include <process.h>
#include <string>

namespace Luxko {
	namespace Threading {
		using namespace Luxko::Win32Basic;
		LUXKOUTILITY_API void SleepFor(DWORD millieSeconds, bool altertable = false);



		enum class LUXKOUTILITY_API EventSynchronizationAccessRight :DWORD {
			// Basic access rights
			Delete = DELETE, // Required to delete the object.
			ReadControl = READ_CONTROL, // Required to read information in the security descriptor for the object.
			Synchronize = SYNCHRONIZE, // The right to use the object for synchronization. Enabling a thread to wait until the object is in the signaled state.
			WriteDac = WRITE_DAC, // Required to modify the DACL in the security descriptor for the object.
			WriteOwner = WRITE_OWNER, // Required to change the owner in the security descriptor for the object.

			// Event access rights
			All = EVENT_ALL_ACCESS,
			Modify = EVENT_MODIFY_STATE
		};

		LUXKOUTILITY_API EventSynchronizationAccessRight operator|(EventSynchronizationAccessRight a, EventSynchronizationAccessRight b);
		LUXKOUTILITY_API EventSynchronizationAccessRight operator&(EventSynchronizationAccessRight a, EventSynchronizationAccessRight b);

		enum class LUXKOUTILITY_API EventCreationFlags : DWORD {
			Default = 0x0,
			ManualReset = CREATE_EVENT_MANUAL_RESET,
			InitialSet = CREATE_EVENT_INITIAL_SET
		};

		LUXKOUTILITY_API EventCreationFlags operator|(EventCreationFlags a, EventCreationFlags b);
		LUXKOUTILITY_API EventCreationFlags operator&(EventCreationFlags a, EventCreationFlags b);

		class LUXKOUTILITY_API Event {
		public:
			Event() = default;
			Event(const Event&) = delete;
			Event& operator=(const Event&) = delete;
			Event(Event&& e)noexcept;
			Event& operator=(Event&& e)noexcept;
			~Event() = default;
			static Event Create(LPTSTR eventName = nullptr,
				EventCreationFlags flags = EventCreationFlags::Default,
				EventSynchronizationAccessRight access = EventSynchronizationAccessRight::All,
				PSECURITY_ATTRIBUTES sa = nullptr)noexcept;
			static Event Open(LPTSTR eventName,
				EventSynchronizationAccessRight access = EventSynchronizationAccessRight::All,
				bool inherit = false)noexcept;

			bool Set()noexcept;

			// Should only be used by non-auto-reset event, or it would be a no op.
			bool Reset()noexcept;

			const KernelObjectHandle& Get()const noexcept;

		private:

			KernelObjectHandle _hEvent;
		};

		enum class LUXKOUTILITY_API SemaphoreAccessRight : DWORD {
			All = SEMAPHORE_ALL_ACCESS,
			Modify = SEMAPHORE_MODIFY_STATE
		};

		class LUXKOUTILITY_API Semaphore {
		public:
			Semaphore() = default;
			Semaphore(const Semaphore&) = delete;
			Semaphore& operator=(const Semaphore&) = delete;
			Semaphore(Semaphore&& e)noexcept;
			Semaphore& operator=(Semaphore&& e)noexcept;
			~Semaphore() = default;
			static Semaphore Create(LONG maximumCount, LONG initialCount = 0l,
				LPTSTR semaphoreName = nullptr,
				SemaphoreAccessRight desiredAccess = SemaphoreAccessRight::All,
				PSECURITY_ATTRIBUTES sa = nullptr)noexcept;
			static Semaphore Open(LPTSTR semaphoreName, SemaphoreAccessRight desiredAccess = SemaphoreAccessRight::Modify,
				bool inherit = false)noexcept;

			bool IncreaseCount(LONG countToIncrease)noexcept;

			const KernelObjectHandle& Get()const noexcept;


		private:
			KernelObjectHandle _hSemaphore;
		};

		enum class MutexAccessRight : DWORD {
			All = MUTEX_ALL_ACCESS,
			Modify = MUTEX_MODIFY_STATE
		};

		class LUXKOUTILITY_API Mutex {
		public:
			Mutex() = default;
			Mutex(const Mutex&) = delete;
			Mutex& operator=(const Mutex&) = delete;
			Mutex(Mutex&& e)noexcept;
			Mutex& operator=(Mutex&& e)noexcept;
			~Mutex() = default;
			static Mutex Create(LPTSTR mutexName = nullptr, bool initiallyOwnedByCreator = false,
				MutexAccessRight desiredAccess = MutexAccessRight::All,
				PSECURITY_ATTRIBUTES sa = nullptr)noexcept;
			static Mutex Open(LPTSTR mutexName, bool inherit = false,
				MutexAccessRight desiredAccess = MutexAccessRight::Modify)noexcept;
			const KernelObjectHandle& Get()const noexcept;
			bool Release();

		private:
			KernelObjectHandle _hMutex;
		};

		class LUXKOUTILITY_API Overlapped {
		public:
			Overlapped(long long offset, const Event& e);
			Overlapped(const Overlapped&) = default;
			Overlapped& operator=(const Overlapped&) = default;
			~Overlapped() = default;
			long long Offset()const;
			void Offset(long long offset);
			void SetEvent(const Event& e);
			OVERLAPPED* OvPtr() { return &_ov; }
			DWORD ErrorCode()const;
			DWORD BytesTransferred()const;

		private:
			OVERLAPPED _ov;
		};

		class LUXKOUTILITY_API ConditionVariable {
			friend class CriticalSectionToken;
			friend class SlimRWLockExclusiveToken;
			friend class SlimRWLockSharedToken;
		public:
			ConditionVariable()noexcept;
			~ConditionVariable()noexcept {};
			ConditionVariable(const ConditionVariable&) = delete;
			ConditionVariable& operator=(const ConditionVariable&) = delete;
			void WakeOne()noexcept;
			void WakeAll()noexcept;
		private:
			CONDITION_VARIABLE _cv;
		};

		// Critical section should be associated with data structures in need of protection.
		// Whenever accessing these data, one should start a new code block,
		// construct a new CritialSectionToken there, access the data, then leave the code block.
		class LUXKOUTILITY_API CriticalSection {
			friend class CriticalSectionToken;
		public:
			CriticalSection()noexcept;
			~CriticalSection()noexcept;
			CriticalSection(const CriticalSection&) = delete;
			CriticalSection& operator=(const CriticalSection&) = delete;
			CriticalSection(CriticalSection&& cs)noexcept;
			CriticalSection& operator=(CriticalSection&& cs)noexcept;

			// Only the first Initialization ever happen(or ever after being moved) takes effect.
			// `spinCount` should range from [0..0x00ff ffff].
			bool Initialize(DWORD spinCount = 0)noexcept;

			// Return the old spin count.
			DWORD SetSpinCount(DWORD spinCount)noexcept;

		private:
			CRITICAL_SECTION _cs;
			bool _valid;
		};

		class LUXKOUTILITY_API CriticalSectionToken {
		public:
			explicit CriticalSectionToken(CriticalSection& cs, bool tryEnter = false);
			~CriticalSectionToken()noexcept;
			CriticalSectionToken(const CriticalSectionToken&) = delete;
			CriticalSectionToken& operator=(const CriticalSectionToken&) = delete;
			void Release()noexcept;
			bool SleepOnCV(ConditionVariable& cv, DWORD milliSeconds = INFINITE)noexcept;
			bool IsValid()const noexcept;
		private:
			LPCRITICAL_SECTION _ptr;
		};

		class LUXKOUTILITY_API SlimRWLock {
			friend class SlimRWLockSharedToken;
			friend class SlimRWLockExclusiveToken;
		public:
			SlimRWLock()noexcept;
			~SlimRWLock()noexcept {}
			SlimRWLock(const SlimRWLock&) = delete;
			SlimRWLock& operator=(const SlimRWLock&) = delete;
		private:
			SRWLOCK _lock;
		};

		class LUXKOUTILITY_API SlimRWLockSharedToken {
		public:
			explicit SlimRWLockSharedToken(SlimRWLock& srwLock);
			~SlimRWLockSharedToken()noexcept;
			SlimRWLockSharedToken(const SlimRWLockSharedToken&) = delete;
			SlimRWLockSharedToken& operator=(const SlimRWLockSharedToken&) = delete;
			void Release()noexcept;
			bool SleepOnCV(ConditionVariable& cv, DWORD milliSeconds = INFINITE)noexcept;
		private:
			PSRWLOCK _ptr;
		};

		class LUXKOUTILITY_API SlimRWLockExclusiveToken {
		public:
			explicit SlimRWLockExclusiveToken(SlimRWLock& srwLock);
			~SlimRWLockExclusiveToken()noexcept;
			SlimRWLockExclusiveToken(const SlimRWLockExclusiveToken&) = delete;
			SlimRWLockExclusiveToken& operator=(const SlimRWLockExclusiveToken&) = delete;
			void Release()noexcept;
			bool SleepOnCV(ConditionVariable& cv, DWORD milliSeconds = INFINITE)noexcept;
		private:
			PSRWLOCK _ptr;
		};

		enum class ThreadCreationFlags : DWORD {
			Default = 0,
			Suspended = CREATE_SUSPENDED
		};

		enum class ThreadPriority : int {
			TimeCritical = THREAD_PRIORITY_TIME_CRITICAL,
			Highest = THREAD_PRIORITY_HIGHEST,
			AboveNormal = THREAD_PRIORITY_ABOVE_NORMAL,
			Normal = THREAD_PRIORITY_NORMAL,
			BelowNormal = THREAD_PRIORITY_BELOW_NORMAL,
			Lowest = THREAD_PRIORITY_LOWEST,
			Idle = THREAD_PRIORITY_IDLE
		};

		class LUXKOUTILITY_API Thread {
		public:
			Thread(PTHREAD_START_ROUTINE functionInvoked,
				void* param, ThreadCreationFlags flags = ThreadCreationFlags::Default,
				DWORD stackSizeInBytes = 0, PSECURITY_ATTRIBUTES sa = nullptr);
			Thread(const Thread&) = delete;
			Thread& operator=(const Thread&) = delete;
			Thread(Thread&& e)noexcept;
			Thread& operator=(Thread&& e)noexcept;
			~Thread() = default;

			void Begin()noexcept;
			bool Terminate(DWORD exitCode)noexcept;

			// Return the last hanging count, if failed return 0xFFFFFFFF
			DWORD Resume()noexcept;
			DWORD Suspend()noexcept;

			struct ThreadTimeInfo
			{
				FileTime creationTime;
				FileTime exitTime;
				FileTime kernelTime;
				FileTime userTime;
			};

			ThreadTimeInfo GetThreadTimeInfo();

			ThreadPriority Priority();

			bool Priority(ThreadPriority priority);

			//bool PriorityBoost();
			//bool PriorityBoost(bool enable);



			const KernelObjectHandle& Get()const noexcept;
			DWORD StackSize()const noexcept;
			DWORD ID()const noexcept;
			PTHREAD_START_ROUTINE FunctionInvoked()const noexcept;
			void* ParametersPassed()const noexcept;



		private:
			DWORD _tID;
			DWORD _stackSizeInBytes; // 0 means default size decided by the linker.
			ThreadCreationFlags _creationFlags;
			KernelObjectHandle _hThread;
			PTHREAD_START_ROUTINE _functionInvoked;
			PSECURITY_ATTRIBUTES _psa;
			void* _paramtersPassed;
		};

		struct IOCStatusInfo {
			DWORD NumberOfBytes;
			ULONG_PTR CompletionKey;
			Overlapped* pOverlapped;
		};

		class LUXKOUTILITY_API IOCPort {
		public:
			IOCPort() = default;
			IOCPort(const IOCPort&) = delete;
			IOCPort& operator=(const IOCPort&) = delete;
			IOCPort(IOCPort&& e)noexcept;
			IOCPort& operator=(IOCPort&& e)noexcept;
			~IOCPort() = default;

			static IOCPort Create(DWORD numberOfConcurrentThreads);

			const KernelObjectHandle& Get()const noexcept;


			enum class GetStatusCallResult : DWORD {
				Success = 0,
				FailedIOCRequest,
				BadCall,
				TimeOut = WAIT_TIMEOUT,
			};


			bool AssociateDevice(const KernelObjectHandle& deviceHandle, ULONG_PTR completionKey)noexcept;
			GetStatusCallResult GetIOCStatusQueued(IOCStatusInfo& statusInfoHolder, DWORD milliSecondsToWait = INFINITE);
			bool PostIOCStatusToQueue(const IOCStatusInfo& statusInfoToPost);

		private:
			KernelObjectHandle _hIOCompletionPort;
		};

		class LUXKOUTILITY_API ThreadPoolEnvironment {
		public:
			ThreadPoolEnvironment();
			ThreadPoolEnvironment(const ThreadPoolEnvironment&) = default;
			ThreadPoolEnvironment& operator=(const ThreadPoolEnvironment&) = default;
			ThreadPoolEnvironment(ThreadPoolEnvironment&& tpe);
			ThreadPoolEnvironment& operator=(ThreadPoolEnvironment&& tpe);
			~ThreadPoolEnvironment();

			PTP_CALLBACK_ENVIRON Get()const noexcept;
			PTP_POOL GetPool()const noexcept;

			static ThreadPoolEnvironment Create();
			void Close()noexcept;
			//bool Valid()const noexcept;

			bool SetMinimumThreadCount(DWORD minimum);
			void SetMaximumThreadCount(DWORD maximum);

		private:
			PTP_POOL _pool;
			PTP_CALLBACK_ENVIRON _pEnvironment;
			TP_CALLBACK_ENVIRON _environment;
		};

		class LUXKOUTILITY_API ThreadPoolWork {
		public:
			using Callback = void(NTAPI * CALLBACK)(PTP_CALLBACK_INSTANCE, void*, PTP_WORK);
			ThreadPoolWork();
			ThreadPoolWork(const ThreadPoolWork&) = default;
			ThreadPoolWork& operator=(const ThreadPoolWork&) = default;
			ThreadPoolWork(ThreadPoolWork&& tg);
			ThreadPoolWork& operator=(ThreadPoolWork&& tg);
			~ThreadPoolWork();

			// use with caution!
			static ThreadPoolWork& Interpret(PTP_WORK* pWork)noexcept;

			static ThreadPoolWork Create(Callback workHandler, void* workHandlerContext,
				const ThreadPoolEnvironment& tpe);
			void Close()noexcept;
			void Submit()noexcept;
			bool Valid()const noexcept;

			// should only be used on valid works
			void Wait(bool cancelPending);

			PTP_WORK Get()const noexcept;
		private:
			PTP_WORK _work;
		};

		class LUXKOUTILITY_API ThreadPoolTimer {
		public:
			using Callback = void(NTAPI * CALLBACK)(PTP_CALLBACK_INSTANCE, void*, PTP_TIMER);
			ThreadPoolTimer();
			ThreadPoolTimer(const ThreadPoolTimer&) = default;
			ThreadPoolTimer& operator=(const ThreadPoolTimer&) = default;
			ThreadPoolTimer(ThreadPoolTimer&& tg);
			ThreadPoolTimer& operator=(ThreadPoolTimer&& tg);
			~ThreadPoolTimer();

			static ThreadPoolTimer& Interpret(PTP_TIMER* pTimer)noexcept;
			static ThreadPoolTimer Create(Callback timerHandler,
				void* timerHandlerContext,
				const ThreadPoolEnvironment& tpe
			);
			void Close()noexcept;
			void Set(const  FileTime& firstSetAt,
				DWORD msPeriod = 0, DWORD msWindowLength = 0)noexcept;
			bool Valid()const noexcept;
			bool IsSet()noexcept;

			// should only be used on valid timers
			void Wait(bool cancelPending);

			PTP_TIMER Get()const noexcept;
		private:
			PTP_TIMER _timer;
		};

		class LUXKOUTILITY_API ThreadPoolWaiter {
		public:
			using Callback = void(NTAPI * CALLBACK)(PTP_CALLBACK_INSTANCE, void*, PTP_WAIT, TP_WAIT_RESULT);
			ThreadPoolWaiter();
			ThreadPoolWaiter(const ThreadPoolWaiter&) = default;
			ThreadPoolWaiter& operator=(const ThreadPoolWaiter&) = default;
			ThreadPoolWaiter(ThreadPoolWaiter&& tg);
			ThreadPoolWaiter& operator=(ThreadPoolWaiter&& tg);
			~ThreadPoolWaiter();

			static ThreadPoolWaiter& Interpret(PTP_TIMER* pTimer)noexcept;
			static ThreadPoolWaiter Create(Callback timerHandler,
				void* timerHandlerContext, const ThreadPoolEnvironment& tpe);
			void Close()noexcept;
			void Set(const KernelObjectHandle& objectToWait,
				FileTime* waitTime = nullptr // 0 == no waiting, nullptr == always wait
			)noexcept;
			bool Valid()const noexcept;

			// should only be used on valid works
			void Wait(bool cancelPending);

			PTP_WAIT Get()const noexcept;
		private:
			PTP_WAIT _waiter;
		};

		class LUXKOUTILITY_API ThreadPoolIO {
		public:
			using Callback = void(NTAPI * CALLBACK)(PTP_CALLBACK_INSTANCE, void*, void*, ULONG, ULONG_PTR, PTP_IO);
			ThreadPoolIO();
			ThreadPoolIO(const ThreadPoolIO&) = delete;
			ThreadPoolIO& operator=(const ThreadPoolIO&) = delete;
			ThreadPoolIO(ThreadPoolIO&& tpi);
			ThreadPoolIO& operator=(ThreadPoolIO&& tpi);
			~ThreadPoolIO();

			static ThreadPoolIO Create(const KernelObjectHandle& deviceHandle,
				Callback ioHandler, void* ioHandlerContext,
				const ThreadPoolEnvironment& tpe);

			static ThreadPoolIO& Interpret(PTP_IO* pPtpIo);

			bool Valid()const noexcept;

			void Close()noexcept;
			void Start()noexcept;
			void Cancel()noexcept;
			void Wait(bool cancelPending);

			PTP_IO Get()const noexcept;



		private:
			PTP_IO _io;
		};


	}
}
