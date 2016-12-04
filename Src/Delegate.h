//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#pragma once
#include "CommonHeader.h"

#define __luxfinline inline
namespace Luxko {

	// Note that if used improperly there might be a chance
	// to introduce dangling pointers.
	template <class RT, class ... PTs>
	class Delegate {
		using InsPtr = void*;
		using FuncType = RT(*)(PTs ...);
		template<class C> using ClassMethodType = RT(C::*)(PTs ...);
		using InternalFuncType = RT(*)(InsPtr, PTs&& ...);
		using Stub = std::pair<InsPtr, InternalFuncType>;

		template<FuncType func>
		static __luxfinline RT FreeFuncStub(InsPtr, PTs&&... args) {
			return func(std::forward<PTs>(args)...);
		}

		template<class C, ClassMethodType<C> func>
		static __luxfinline RT ClassMethodStub(InsPtr insPtr, PTs&&... args) {
			return (static_cast<C*>(insPtr)->*func)(std::forward<PTs>(args)...);
		}

		
		template<class FuncObjType>
		static __luxfinline RT FunctionObjectStub(InsPtr insPtr, PTs&&... args) {
			return (*static_cast<FuncObjType*>(insPtr))(std::forward<PTs>(args)...);
		}


	public:


		static Delegate FromDefault() {
			Delegate result;
			static auto defaultCtor = [](PTs...args) {
				return RT(args...);
			};
			result.Bind(&defaultCtor);
			return result;
		}

		Delegate() :_stub(nullptr, nullptr) {}

		Delegate(const Delegate&) = default;
		Delegate(Delegate&&) = default;
		Delegate& operator=(const Delegate&) = default;
		Delegate& operator=(Delegate&&) = default;

		~Delegate() = default;

		template<class FuncObjType>
		void Bind(FuncObjType* lambda) {
			_stub.first = static_cast<InsPtr>(lambda);
			_stub.second = FunctionObjectStub<FuncObjType>;
		}

		template<FuncType ft>
		void Bind() {
			_stub.first = nullptr;
			_stub.second = FreeFuncStub<ft>;
		}


		template<class C, ClassMethodType<C> cmt>
		void Bind(C* instance) {
			_stub.first = static_cast<InsPtr>(instance);
			_stub.second = ClassMethodStub<C, cmt>;
		}

		__luxfinline RT Invoke(PTs...args)const {
			return _stub.second(_stub.first, std::forward<PTs>(args)...);
		}

		inline bool operator==(const Delegate<RT, PTs...>& d)const noexcept {
			return _stub == d._stub;
		}

		inline bool operator!=(const Delegate<RT, PTs...>& d)const noexcept {
			return !(this->operator ==(d));
		}

		inline size_t getHash()const {
			const auto h1 = std::hash<InsPtr>{}(_stub.first);
			const auto h2 = std::hash<InternalFuncType>{}(_stub.second);
			return h1 ^ (h2 << 1);
		}
	private:
		Stub _stub;
		
	};

	template<class RT, class ... PTs>
	class DEvent {
	public:
		using delegate_type = Delegate<RT, PTs...>;
	private:
		// using EventSink = std::list<delegate_type>;
		using EventSink = std::forward_list<delegate_type>;
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
			// _events.push_back(std::move(d));
			_events.push_front(std::move(d));
		}

		inline void Remove(Delegate<RT, PTs...> d) {
			_events.remove(d);
		}
	private:
		EventSink _events;
	};
}

// Hasher for Delegate
namespace std {
	template <class RT, class ... PTs>
	struct hash<Luxko::Delegate<RT, PTs...>> {
		using argument_type = Luxko::Delegate<RT, PTs...>;
		using result_type = size_t;

		result_type operator()(const argument_type& d)const {
			return d.getHash();
		}
	};
}