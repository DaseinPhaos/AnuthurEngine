//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#pragma once
#include "CommonHeader.h"

namespace Luxko {

	// Note that if used improperly there might be a chance
	// to introduce dangling pointers.
	template <class RT, class ... PTs>
	class Delegate {
		using InsPtr = void*;
		using FuncType = RT(*)(PTs ...);
		template<class C> using ClassMethodType = RT(C::*)(PTs ...);
		using InternalFuncType = RT(*)(InsPtr, PTs ...);
		using Stub = std::pair<InsPtr, InternalFuncType>;

		template<FuncType func>
		static inline RT FreeFuncStub(InsPtr, PTs... args) {
			return func(args...);
		}

		template<class C, ClassMethodType<C> func>
		static inline RT ClassMethodStub(InsPtr insPtr, PTs... args) {
			return (static_cast<C*>(insPtr)->*func)(args...);
		}

		
		template<class FuncObjType>
		static inline RT FunctionObjectStub(InsPtr insPtr, PTs... args) {
			return (*static_cast<FuncObjType*>(insPtr))(args...);
		}

	public:
		Delegate() :_stub(nullptr, nullptr) {}
		~Delegate() = default;

		template<class FuncObjType>
		void Bind(FuncObjType* lambda) {
			_stub.first = static_cast<InsPtr>(lambda);
			_stub.second = &FunctionObjectStub<FuncObjType>;
		}

		template<FuncType ft>
		void Bind() {
			_stub.first = nullptr;
			_stub.second = &FreeFuncStub<ft>;
		}


		template<class C, ClassMethodType<C> cmt>
		void Bind(C* instance) {
			_stub.first = static_cast<InsPtr>(instance);
			_stub.second = &ClassMethodStub<C, cmt>;
		}

		inline RT Invoke(PTs... args)const {
			return _stub.second(_stub.first, args...);
		}


		inline bool operator==(const Delegate<RT, PTs...>& d) {
			return _stub == d._stub;
		}

		inline bool operator!=(const Delegate<RT, PTs...>& d) {
			return !(this->operator ==(d));
		}
	private:
		Stub _stub;
		
	};

	template<class RT, class ... PTs>
	class DEvent {
	public:
		using delegate_type = Delegate<RT, PTs...>;
	private:
		using EventSink = std::list<delegate_type>;
	public:
		DEvent() = default;
		~DEvent() = default;

		DEvent(const DEvent&) = delete;
		DEvent& operator=(const DEvent&) = delete;
		
		DEvent(DEvent&& d) {
			_events = std::move(d._events);
		}
		DEvent& operator=(DEvent&& d) {
			_events = std::move(d._events);
			return *this;
		}

		void Raise(PTs... args) {
			for (auto& e : _events)
			{
				e.Invoke(args...);
			}
		}

		inline void Add(Delegate<RT, PTs...> d) {
			_events.push_back(std::move(d));
		}

		inline void Remove(Delegate<RT, PTs...> d) {
			_events.remove(d);
		}
	private:
		EventSink _events;
	};
}