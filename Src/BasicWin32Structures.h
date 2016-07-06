//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#pragma once

#include "CommonHeader.h"

namespace Luxko {
	namespace Win32Basic {
		class SystemTime;
		class LUXKOUTILITY_API FileTime {
		public:

			FileTime() {}
			explicit FileTime(const SystemTime& st);
			explicit FileTime(LARGE_INTEGER li);

			FileTime(const FILETIME& ft) :m_ft(ft) {}
			FileTime(const FileTime&) = default;
			FileTime& operator=(const FileTime&) = default;
			~FileTime() = default;
			bool operator==(const FileTime& ft)const;
			bool operator<(const FileTime& ft)const;

			FileTime ToLocal()const;
			FileTime ToGlobal()const;
			LARGE_INTEGER toLargeInteger()const;

		public:
			FILETIME m_ft;
		};

		class LUXKOUTILITY_API SystemTime {
		public:
			SystemTime() {}
			explicit SystemTime(const FileTime& ft);
			SystemTime(const SYSTEMTIME& st) :m_st(st) {}
			SystemTime(const SystemTime&) = default;
			SystemTime& operator=(const SystemTime&) = default;
			~SystemTime() = default;
			bool operator==(const SystemTime& st)const;
			SYSTEMTIME m_st;
		};


		enum class LUXKOUTILITY_API WaitObjectResult : DWORD {
			Object0Returned = WAIT_OBJECT_0,
			TimeOut = WAIT_TIMEOUT,
			Failed = WAIT_FAILED,
			Abandoned = WAIT_ABANDONED,
			IOCompletion = WAIT_IO_COMPLETION
		};

		class LUXKOUTILITY_API KernelObjectHandle {
		public:
			static KernelObjectHandle Duplicate(HANDLE sourceProcessHandleRaw,
				const KernelObjectHandle& sourceHandle, HANDLE targetProcessHandleRaw,
				DWORD desiredAccess, bool Inheritable);
			static KernelObjectHandle DuplicateSameAccess(HANDLE sourceProcessHandleRaw,
				const KernelObjectHandle& sourceHandle, HANDLE targetProcessHandleRaw, bool Inheritable);

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
	}
}