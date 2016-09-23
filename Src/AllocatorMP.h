//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#pragma once
#include "CommonHeader.h"

namespace Luxko {
	template <size_t ChunkSize>
	struct DataChunk {
		union DCU
		{
			char data[ChunkSize];
			DataChunk* pNext;
		}u;
	};

	template <size_t ChunkPerPage>
	struct DataPage {
		static constexpr auto pageSize = 4096u;
		using chunk_type = DataChunk<pageSize / ChunkPerPage>;

		chunk_type chunks[ChunkPerPage];

		chunk_type* initialize() {
			memset(chunks, 0, sizeof(chunk_type)*ChunkPerPage);
			for (auto i = 0u; i < ChunkPerPage - 1; ++i) {
				chunks[i].u.pNext = &(chunks[i + 1]);
			}
			return &(chunks[0]);
		}
	};

	template<size_t ChunkPerPage>
	class ChunkAllocatorImpl {
	public:
		using chunk_type = typename DataPage<ChunkPerPage>::chunk_type;

		ChunkAllocatorImpl() { initialize(); }

		inline void* allocate(size_t n) {
			assert(n <= DataPage<ChunkPerPage>::pageSize / ChunkPerPage);
			if (m_pFirstFreePage == nullptr) addPage();
			auto result = reinterpret_cast<void*>(m_pFirstFreePage);
			m_pFirstFreePage = m_pFirstFreePage->u.pNext;
			return result;
		}

		inline void deallocate(void* p)noexcept {
			auto newlyFreedChunk = reinterpret_cast<chunk_type*>(p);
			newlyFreedChunk->u.pNext = m_pFirstFreePage;
			m_pFirstFreePage = newlyFreedChunk;
		}

	private:
		void initialize() {
			m_pages.emplace_back();
			m_pFirstFreePage = m_pages.back().initialize();
		}

		void addPage() {
			m_pages.emplace_back();
			auto newHead = m_pages.back().initialize();
			m_pages.back().chunks[ChunkPerPage - 1].u.pNext = m_pFirstFreePage;
			m_pFirstFreePage = newHead;
		}

		std::list<DataPage<ChunkPerPage>> m_pages;
		chunk_type* m_pFirstFreePage;
	};

	class ChunkAllocators {
	public:
		ChunkAllocators() = delete;
		template<class T>
		class ChunkAllocator16 {
		public:
			static auto constexpr chunkPerPage = DataPage<16>::pageSize / 16;
			using value_type = T;
			using chunk_type = typename DataPage<chunkPerPage>::chunk_type;

			ChunkAllocator16() { }
			template<class U>
			ChunkAllocator16(const ChunkAllocator16<U>&) { }

			inline T* allocate(size_t n) {
				assert(sizeof(T)*n <= DataPage<chunkPerPage>::pageSize / chunkPerPage);
				return reinterpret_cast<T*>(alloc16Impl.allocate(n * sizeof(T)));
			}

			inline void deallocate(T* p, size_t n)noexcept {
				if (p != nullptr) alloc16Impl.deallocate(p);
			}

		};


		template<class T>
		class ChunkAllocator64 {
		public:
			static auto constexpr chunkPerPage = DataPage<64>::pageSize / 64;
			using value_type = T;
			using chunk_type = typename DataPage<chunkPerPage>::chunk_type;

			ChunkAllocator64() { }
			template<class U>
			ChunkAllocator64(const ChunkAllocator64<U>&) { }

			inline T* allocate(size_t n) {
				assert(sizeof(T)*n <= DataPage<chunkPerPage>::pageSize / chunkPerPage);
				return reinterpret_cast<T*>(alloc64Impl.allocate(n * sizeof(T)));
			}

			inline void deallocate(T* p, size_t n)noexcept {
				if (p != nullptr) alloc64Impl.deallocate(p);
			}

		};

		template<class T>
		class ChunkAllocator256 {
		public:
			static auto constexpr chunkPerPage = DataPage<256>::pageSize / 256;
			using value_type = T;
			using chunk_type = typename DataPage<chunkPerPage>::chunk_type;

			ChunkAllocator256() { }
			template<class U>
			ChunkAllocator256(const ChunkAllocator256<U>&) { }

			inline T* allocate(size_t n) {
				assert(sizeof(T)*n <= DataPage<chunkPerPage>::pageSize / chunkPerPage);
				return reinterpret_cast<T*>(alloc256Impl.allocate(n * sizeof(T)));
			}

			inline void deallocate(T* p, size_t n)noexcept {
				if (p != nullptr) alloc256Impl.deallocate(p);
			}

		};

		template<class T>
		class ChunkAllocator1024 {
		public:
			static auto constexpr chunkPerPage = DataPage<1024>::pageSize / 1024;
			using value_type = T;
			using chunk_type = typename DataPage<chunkPerPage>::chunk_type;

			ChunkAllocator1024() { }
			template<class U>
			ChunkAllocator1024(const ChunkAllocator1024<U>&) { }

			inline T* allocate(size_t n) {
				assert(sizeof(T)*n <= DataPage<chunkPerPage>::pageSize / chunkPerPage);
				return reinterpret_cast<T*>(alloc1024Impl.allocate(n * sizeof(T)));
			}

			inline void deallocate(T* p, size_t n)noexcept {
				if (p != nullptr) alloc1024Impl.deallocate(p);
			}

		};

	private:
		static ChunkAllocatorImpl<256> alloc16Impl;
		static ChunkAllocatorImpl<64> alloc64Impl;
		static ChunkAllocatorImpl<16> alloc256Impl;
		static ChunkAllocatorImpl<4> alloc1024Impl;
	};

	template <class T, class U>
	constexpr bool operator== (const ChunkAllocators::ChunkAllocator16<T>&, const ChunkAllocators::ChunkAllocator16<U>&) noexcept
	{
		return true;
	}

	template <class T, class U>
	constexpr bool operator!= (const ChunkAllocators::ChunkAllocator16<T>&, const ChunkAllocators::ChunkAllocator16<U>&) noexcept
	{
		return false;
	}

	template <class T, class U>
	constexpr bool operator== (const ChunkAllocators::ChunkAllocator64<T>&, const ChunkAllocators::ChunkAllocator64<U>&) noexcept
	{
		return true;
	}

	template <class T, class U>
	constexpr bool operator!= (const ChunkAllocators::ChunkAllocator64<T>&, const ChunkAllocators::ChunkAllocator64<U>&) noexcept
	{
		return false;
	}

	template <class T, class U>
	constexpr bool operator== (const ChunkAllocators::ChunkAllocator256<T>&, const ChunkAllocators::ChunkAllocator256<U>&) noexcept
	{
		return true;
	}

	template <class T, class U>
	constexpr bool operator!= (const ChunkAllocators::ChunkAllocator256<T>&, const ChunkAllocators::ChunkAllocator256<U>&) noexcept
	{
		return false;
	}

	template <class T, class U>
	constexpr bool operator== (const ChunkAllocators::ChunkAllocator1024<T>&, const ChunkAllocators::ChunkAllocator1024<U>&) noexcept
	{
		return true;
	}

	template <class T, class U>
	constexpr bool operator!= (const ChunkAllocators::ChunkAllocator1024<T>&, const ChunkAllocators::ChunkAllocator1024<U>&) noexcept
	{
		return false;
	}



	template<class T>
	class AllocatorMP {
	public:
		using Alloc16 = ChunkAllocators::ChunkAllocator16<T>;
		using Alloc64 = ChunkAllocators::ChunkAllocator64<T>;
		using Alloc256 = ChunkAllocators::ChunkAllocator256<T>;
		using Alloc1024 = ChunkAllocators::ChunkAllocator1024<T>;
		using value_type = T;

		AllocatorMP() = default;

		template<class U>
		AllocatorMP(const AllocatorMP<U>&) {}


		T* allocate(size_t n) {
			auto sizeToBeAlloc = sizeof(T)*n;
			if (sizeToBeAlloc <= 16) {
				return m_a16.allocate(n);
			}
			else if (sizeToBeAlloc <= 64) {
				return m_a64.allocate(n);
			}
			else if (sizeToBeAlloc <= 256) {
				return m_a256.allocate(n);
			}
			else if (sizeToBeAlloc <= 1024) {
				return m_a1024.allocate(n);
			}
			else {
				// return static_cast< T* > (::new(n * sizeof(T)));
				return static_cast<T*>(malloc(n * sizeof(T)));
			}
		}

		void deallocate(T* p, size_t n)noexcept {
			auto size = sizeof(T)*n;
			if (size <= 16) {
				m_a16.deallocate(p, n);
			}
			else if (size <= 64) {
				m_a64.deallocate(p, n);
			}
			else if (size <= 256) {
				m_a256.deallocate(p, n);
			}
			else if (size <= 1024) {
				m_a1024.deallocate(p, n);
			}
			else {
				//::delete(p);
				free(reinterpret_cast<void*>(p));
			}
		}

	private:
		Alloc16 m_a16;
		Alloc64 m_a64;
		Alloc256 m_a256;
		Alloc1024 m_a1024;
	};

	template<class T, class U>
	bool operator==(const AllocatorMP<T>&, const AllocatorMP<U>&)noexcept { return true; }

	template<class T, class U>
	bool operator!=(const AllocatorMP<T>&, const AllocatorMP<U>&)noexcept { return false; }

}