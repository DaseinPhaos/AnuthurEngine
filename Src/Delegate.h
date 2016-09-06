//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#pragma once
#include "CommonHeader.h"

namespace Luxko {
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

		
		template<class LambdaType>
		static inline RT LambdaFuncStub(InsPtr insPtr, PTs... args) {
			return (*static_cast<LambdaType*>(insPtr))(args...);
		}

	public:
		Delegate() :_stub(nullptr, nullptr) {}
		~Delegate() = default;

		template<class LambdaType>
		void Bind(LambdaType* lambda) {
			_stub.first = static_cast<InsPtr>(lambda);
			_stub.second = &LambdaFuncStub<LambdaType>;
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

		RT Invoke(PTs... args)const {
			return _stub.second(_stub.first, args...);
		}

	private:
		Stub _stub;
		
	};
}