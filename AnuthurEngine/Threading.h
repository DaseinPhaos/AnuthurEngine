//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************
// This part is currently under building.
//**********************************************************************

#pragma once
#ifdef LUXKOUTILITY_EXPORTS
#define LUXKOUTILITY_API _declspec(dllexport)
#else
#define LUXKOUTILITY_API _declspec(dllimport)
#endif

#include "CommonHeader.h"
#include <string>

namespace Luxko {
	namespace Threading {

		enum class SynchronizationAccessRight:DWORD {
			Delete = DELETE, // Required to delete the object.
			ReadControl = READ_CONTROL, // Required to read information in the security descriptor for the object.
			Synchronize = SYNCHRONIZE, // The right to use the object for synchronization. Enabling a thread to wait until the object is in the signaled state.
			WriteDac = WRITE_DAC, // Required to modify the DACL in the security descriptor for the object.
			WriteOwner = WRITE_OWNER // Required to change the owner in the security descriptor for the object.
		};

		LUXKOUTILITY_API SynchronizationAccessRight operator|(SynchronizationAccessRight a, SynchronizationAccessRight b);
		LUXKOUTILITY_API SynchronizationAccessRight operator&(SynchronizationAccessRight a, SynchronizationAccessRight b);

		enum class EventAccessRight : DWORD {
			AllAccess = EVENT_ALL_ACCESS, // All possible access rights for the event.
			ModifyState = EVENT_MODIFY_STATE // Modify state access. Required for SetEvent, ResetEvent, PulseEvent functions.
		};

		class LUXKOUTILITY_API Event {
		public:
			Event(const Event&) = default;
			Event& operator=(const Event&) = default;
			virtual ~Event() = default;

			bool Set() { return SetEvent(_hEvent) != 0; }
			HANDLE Handle()const { return _hEvent; }
		protected:
			Event() {}
			static Event OpenEvent(const std::wstring& eventName,
				EventAccessRight ear = EventAccessRight::ModifyState,
				SynchronizationAccessRight sar = SynchronizationAccessRight::Synchronize,
				bool inheritable = false);
			HANDLE _hEvent;
		};

		class LUXKOUTILITY_API EventNotifyOne :public Event {
		public:
			static EventNotifyOne Open(std::wstring& eventName,
				EventAccessRight ear = EventAccessRight::ModifyState,
				SynchronizationAccessRight sar = SynchronizationAccessRight::Synchronize,
				bool inheritable = false);
			static EventNotifyOne CreateNameLess(bool initiallySet, SECURITY_ATTRIBUTES* sa = nullptr);
			static EventNotifyOne CreateNamed(bool initiallySet, const std::wstring& name,
				SECURITY_ATTRIBUTES* sa = nullptr);
			
			bool NotifyOne() { return Set(); }

			~EventNotifyOne() = default;
			EventNotifyOne(const EventNotifyOne&) = default;
			EventNotifyOne& operator=(const EventNotifyOne&) = default;
		private:
			EventNotifyOne() {}
		};

		class LUXKOUTILITY_API EventNotifyAll :public Event {
		public:
			static EventNotifyAll Open(std::wstring& eventName,
				EventAccessRight ear = EventAccessRight::ModifyState,
				SynchronizationAccessRight sar = SynchronizationAccessRight::Synchronize,
				bool inheritable = false);
			static EventNotifyAll CreateNameLess(bool initiallySet, SECURITY_ATTRIBUTES* sa = nullptr);
			static EventNotifyAll CreateNamed(bool initiallySet, const std::wstring& name,
				SECURITY_ATTRIBUTES* sa = nullptr);

			bool NotifyAll() { return Set(); }
			bool Reset() { return ResetEvent(_hEvent)!=0; }

			~EventNotifyAll() = default;
			EventNotifyAll(const EventNotifyAll&) = default;
			EventNotifyAll& operator=(const EventNotifyAll&) = default;
		private:
			EventNotifyAll() {}
		};

		class LUXKOUTILITY_API Overlap {
		public:
			Overlap(long long offset, const Event& e);
			Overlap(const Overlap&) = default;
			Overlap& operator=(const Overlap&) = default;
			~Overlap() = default;
			long long Offset()const;
			void Offset(long long offset);
			void SetEvent(const Event& e);
			OVERLAPPED* OvPtr() { return &_ov; }
			
		private:
			OVERLAPPED _ov;
		};



	}
}
