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
#include "FileSystem.h"
#include <crtdefs.h>
#include <process.h>
#include <string>

namespace Luxko {
	namespace Threading {

		enum class LUXKOUTILITY_API WaitObjectResult: DWORD {
			Object0Returned = WAIT_OBJECT_0,
			TimeOut = WAIT_TIMEOUT,
			Failed = WAIT_FAILED,
			Abandoned = WAIT_ABANDONED,
			IOCompletion = WAIT_IO_COMPLETION
		};

		class LUXKOUTILITY_API KernelObjectHandle {
		public:
			// Should use with caution: only pass in valid handle!
			explicit KernelObjectHandle(HANDLE handle)noexcept;
			KernelObjectHandle()noexcept;
			KernelObjectHandle(const KernelObjectHandle& kh) = delete;
			KernelObjectHandle& operator=(const KernelObjectHandle& kh) = delete;
			KernelObjectHandle(KernelObjectHandle&& kh)noexcept;
			KernelObjectHandle& operator=(KernelObjectHandle&& kh)noexcept;
			~KernelObjectHandle()noexcept;
			void Release()noexcept;
			HANDLE Get()const noexcept;
			bool Valid()const noexcept;
			WaitObjectResult WaitFor(DWORD milliSeconds = INFINITE)const;

			// This function calls the Win32 `SignalObjectAndWait` function,
			// which performs signaling and waiting in one atomatic operation.
			static WaitObjectResult SignalAndWait(
				const KernelObjectHandle& toSignal, // Must be a singable kernel object, i.e. a mutex, a semaphore or a event.
				const KernelObjectHandle& toWaitFor, // Can be any waitable kernel object.
				DWORD milliSeconds = INFINITE,
				bool altertable = false
			);

			//template<typename Iter>
			//WaitObjectResult WaitMultiple(const Iter& begin, const Iter& end,
			//	bool waitAll, DWORD milliSeconds = INFINITE) {
			//	DWORD i = 0;
			//	for (auto iter = begin; iter != end; ++iter, ++i) {
			//	}
			//}
		private:
			HANDLE _handle;
		};

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

//#pragma region Old Event Implementation
//		enum class EventAccessRight : DWORD {
//			AllAccess = EVENT_ALL_ACCESS, // All possible access rights for the event.
//			ModifyState = EVENT_MODIFY_STATE // Modify state access. Required for SetEvent, ResetEvent, PulseEvent functions.
//		};
//
//		class LUXKOUTILITY_API Event {
//		public:
//			Event(const Event&) = default;
//			Event& operator=(const Event&) = default;
//			virtual ~Event() = default;
//
//			bool Set() { return SetEvent(_hEvent) != 0; }
//			HANDLE Handle()const { return _hEvent; }
//		protected:
//			Event() {}
//			static Event OpenEvent(const std::wstring& eventName,
//				EventAccessRight ear = EventAccessRight::ModifyState,
//				SynchronizationAccessRight sar = SynchronizationAccessRight::Synchronize,
//				bool inheritable = false);
//			HANDLE _hEvent;
//		};
//
//		class LUXKOUTILITY_API EventNotifyOne :public Event {
//		public:
//			static EventNotifyOne Open(std::wstring& eventName,
//				EventAccessRight ear = EventAccessRight::ModifyState,
//				SynchronizationAccessRight sar = SynchronizationAccessRight::Synchronize,
//				bool inheritable = false);
//			static EventNotifyOne CreateNameLess(bool initiallySet, SECURITY_ATTRIBUTES* sa = nullptr);
//			static EventNotifyOne CreateNamed(bool initiallySet, const std::wstring& name,
//				SECURITY_ATTRIBUTES* sa = nullptr);
//			
//			bool NotifyOne() { return Set(); }
//
//			~EventNotifyOne() = default;
//			EventNotifyOne(const EventNotifyOne&) = default;
//			EventNotifyOne& operator=(const EventNotifyOne&) = default;
//		private:
//			EventNotifyOne() {}
//		};
//
//		class LUXKOUTILITY_API EventNotifyAll :public Event {
//		public:
//			static EventNotifyAll Open(std::wstring& eventName,
//				EventAccessRight ear = EventAccessRight::ModifyState,
//				SynchronizationAccessRight sar = SynchronizationAccessRight::Synchronize,
//				bool inheritable = false);
//			static EventNotifyAll CreateNameLess(bool initiallySet, SECURITY_ATTRIBUTES* sa = nullptr);
//			static EventNotifyAll CreateNamed(bool initiallySet, const std::wstring& name,
//				SECURITY_ATTRIBUTES* sa = nullptr);
//
//			bool NotifyAll() { return Set(); }
//			bool Reset() { return ResetEvent(_hEvent)!=0; }
//
//			~EventNotifyAll() = default;
//			EventNotifyAll(const EventNotifyAll&) = default;
//			EventNotifyAll& operator=(const EventNotifyAll&) = default;
//		private:
//			EventNotifyAll() {}
//		};
//#pragma endregion Old Event Implementation

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

			//void Sleep(DWORD milliSeconds);

			struct ThreadTimeInfo
			{
				FileSystem::FileTime creationTime;
				FileSystem::FileTime exitTime;
				FileSystem::FileTime kernelTime;
				FileSystem::FileTime userTime;
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


			enum class GetStatusCallResult: DWORD {
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
	}
}
