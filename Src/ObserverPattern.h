#pragma once
//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#include "Delegate.h"
namespace Luxko {
	
	template<class ... PTs>
	class Observer {
	public:
		Observer() = default;
		Observer(const Observer&) = default;
		Observer(Observer&&) = default;
		
		Observer& operator=(const Observer&) = default;
		Observer& operator=(Observer&&) = default;

		explicit Observer(const Delegate<void, PTs ...>& d) {
			toBeInvoked = d;
		}

		~Observer() {
			Remove();
		}

	public:
		void Notify(PTs ... args)const {
			toBeInvoked.Invoke(args...);
			if (pNext) pNext->Notify(args...);
		}

		void InsertAfter(Observer* pOb) {
			assert(pOb != nullptr);
			auto oldNext = pNext;
			pOb->pNext = oldNext;
			if(oldNext) oldNext->pLast = pOb;
			pNext = pOb;
			pOb->pLast = this;
		}

		void InsertAhead(Observer* pOb) {
			assert(pOb != nullptr);
			auto oldLast = pLast;
			pOb->pLast = oldLast;
			if (oldLast) oldLast->pNext = pOb;
			pLast = pOb;
			pOb->pNext = this;
		}

		void Remove()const {
			if (pLast) pLast->pNext = pNext;
			if (pNext) pNext->pLast = pLast;
		}

		// Helper methods
		static Observer CreateHead() {
			Observer r;
			r.toBeInvoked.Bind(NonOp);
			return r;
		}

		inline void Add(Observer* pOb) {
			InsertAfter(pOb);
		}

	private:
		static void NonOp(PTs ... args) {

		}

	public:
		Delegate<void, PTs ... > toBeInvoked;
		Observer* pLast = nullptr;
		Observer* pNext = nullptr;
	};

	
}