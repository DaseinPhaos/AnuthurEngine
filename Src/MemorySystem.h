//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#pragma once


#include "CommonHeader.h"
#include "FileSystem.h"



namespace Luxko {
	namespace Memory {

#pragma region Heap Related Scoped Enums
		enum class LUXKOUTILITY_API HeapCreationOption : DWORD {
			Default = 0x0, // No special options.
			GenerateExceptions = HEAP_GENERATE_EXCEPTIONS, // Failed allocations generate an exception for SEH processing.
			NoSerialize = HEAP_NO_SERIALIZE, // Serialized access is not used when heap functions access this heap. This option can also be specified on individual heap function calls. A heap created with this option cannot be locked.
			EnableExecute = HEAP_CREATE_ENABLE_EXECUTE // All memory blocks that are allocated from this heap allow code execution.
		};

		LUXKOUTILITY_API HeapCreationOption operator&(HeapCreationOption a, HeapCreationOption b);
		LUXKOUTILITY_API HeapCreationOption operator|(HeapCreationOption a, HeapCreationOption b);

		enum class LUXKOUTILITY_API HeapAllocationOption : DWORD {
			Default = 0x0, // No special options.
			GenerateExceptions = HEAP_GENERATE_EXCEPTIONS, // Failed allocations generate an exception for SEH processing.
			NoSerialize = HEAP_NO_SERIALIZE, // Serialized access is not used when heap functions access this heap. This option can also be specified on individual heap function calls. A heap created with this option cannot be locked.
			ZeroMemory = HEAP_ZERO_MEMORY // The allocated memory will be initialized to zero.
		};

		LUXKOUTILITY_API HeapAllocationOption operator&(HeapAllocationOption a, HeapAllocationOption b);
		LUXKOUTILITY_API HeapAllocationOption operator|(HeapAllocationOption a, HeapAllocationOption b);

		enum class LUXKOUTILITY_API HeapReallocationOption : DWORD {
			Default = 0x0, // No special options.
			GenerateExceptions = HEAP_GENERATE_EXCEPTIONS, // Failed allocations generate an exception for SEH processing.
			NoSerialize = HEAP_NO_SERIALIZE, // Serialized access is not used when heap functions access this heap. This option can also be specified on individual heap function calls. A heap created with this option cannot be locked.
			ZeroMemory = HEAP_ZERO_MEMORY, // The additional allocated memory (if needed) will be initialized to zero.
			InPlace = HEAP_REALLOC_IN_PLACE_ONLY // There can be no movement when reallocating a memory block. If this value is not specified, the function may move the block to a new location. If this value is specified and the block cannot be resized without moving, the function fails, leaving the original memory block unchanged.
		};

		LUXKOUTILITY_API HeapReallocationOption operator&(HeapReallocationOption a, HeapReallocationOption b);
		LUXKOUTILITY_API HeapReallocationOption operator|(HeapReallocationOption a, HeapReallocationOption b);
#pragma endregion Heap Related Scoped Enums


		class LUXKOUTILITY_API Heap {
		public:
			Heap() :_isValid(false) {}
			Heap(const Heap&) = delete;
			Heap(Heap&& h);
			Heap& operator=(const Heap&) = delete;
			Heap& operator=(Heap&& h);
			~Heap();

			static Heap Create(size_t initialSize, size_t maximumSize = 0, HeapCreationOption options = HeapCreationOption::Default);

			void DestroyThis();
			
			void* Allocate(size_t bytes, HeapAllocationOption options = HeapAllocationOption::Default);
			bool DeAllocate(void* ptr, bool isSerialized = true);
			void* ReAllocate(void* ptr, size_t bytes, HeapReallocationOption options = HeapReallocationOption::Default);
			HANDLE Handle()const { return _hHeap; }

		private:
			
			HANDLE _hHeap;
			bool _isValid;
		};


#pragma region MappingObject Related Scoped Enums


		enum class LUXKOUTILITY_API PageProtectionOption :DWORD {
			ExecuteRead = PAGE_EXECUTE_READ, // Allows views to be mapped for read-only, copy-on-write, or execute access.

			ExecuteReadWrite = PAGE_EXECUTE_READWRITE, // Allows views to be mapped for read-only, copy-on-write, read/write, or execute access.

			ExecuteWriteCopy = PAGE_EXECUTE_WRITECOPY, // Allows views to be mapped for read-only, copy-on-write, or execute access. Equivalent to PAGE_EXECUTE_READ.

			ReadOnly = PAGE_READONLY, // Allows views to be mapped for read-only or copy-on-write access.

			ReadWrite = PAGE_READWRITE, // Allows views to be mapped for read-only, copy-on-write, or rw access.

			WriteCopy = PAGE_WRITECOPY // Allows views to be mapped for read-only or copy-on-write access.
		};

		enum class LUXKOUTILITY_API PageProtectionFlags :DWORD {
			NoFlags = 0x0,

			Commit = SEC_COMMIT, // If the file mapping object is backed by the operating system paging file (the hfile parameter is INVALID_HANDLE_VALUE), specifies that when a view of the file is mapped into a process address space, the entire range of pages is committed rather than reserved. The system must have enough committable pages to hold the entire mapping. Otherwise, CreateFileMapping fails.
								 // If no attributes specified, this is assumed. Mutual exclusive with SEC_RESERVE

			Image = SEC_IMAGE, // Specifies that the file backed up is an executable image file. No other attributes can be combined with this.

			ImageNoExecute = SEC_IMAGE_NO_EXECUTE, // Specifies that the file is an executable image file that will not be executed and the loaded image file will have no forced integrity checks run. Must be combined with PAGE_READONLY.

			LargePage = SEC_LARGE_PAGES, // Enables large pages to be used for file mapping objects that are backed by the operating system paging file. The maximum size of the file mapping object must be a multiple of the minimum size of a large page returned by the GetLargePageMinimum function. If it is not, CreateFileMapping fails. 

			NoCache = SEC_NOCACHE, // Sets all pages to be non-cachable.
			
			Reserve = SEC_RESERVE, // If the file mapping object is backed by the operating system paging file (the hfile parameter is INVALID_HANDLE_VALUE), specifies that when a view of the file is mapped into a process address space, the entire range of pages is reserved for later use by the process rather than committed.
									// Reserved pages can be committed in subsequent calls to the VirtualAlloc function.After the pages are committed, they cannot be freed or decommitted with the VirtualFree function.

			WriteCombine = SEC_WRITECOMBINE // Sets all pages to be write-combined.
		};

		LUXKOUTILITY_API PageProtectionFlags operator|(PageProtectionFlags a, PageProtectionFlags b);

		enum class LUXKOUTILITY_API FileMapAccessRight :DWORD {
			All = FILE_MAP_ALL_ACCESS, // Includes all access rights except FILE_MAP_EXECUTE.

			Execute = FILE_MAP_EXECUTE, // Allows mapping of executable views of the file mapping object. The object should be created under corresponding page protection options.

			Read = FILE_MAP_READ, // Allows mapping of read-only or copy-on-write views of the file mapping object.

			Write = FILE_MAP_WRITE, // Allows mapping of read-only, copy-on-write, or rw views of the mapped object.

			Copy = FILE_MAP_COPY // Allows mapping of copy-on-write view of the file. The file mapping object must have been created with PAGE_READONLY, PAGE_READ_EXECUTE, PAGE_WRITECOPY, PAGE_EXECUTE_WRITECOPY, PAGE_READWRITE, or PAGE_EXECUTE_READWRITE protection.
		};

		FileMapAccessRight operator|(FileMapAccessRight a, FileMapAccessRight b);
		
#pragma endregion MappingObject Related Scoped Enums

		class LUXKOUTILITY_API MappingObject {
		public:
			MappingObject() :_isValid(false) {}
			MappingObject(const MappingObject& m) = delete;
			MappingObject(MappingObject&& m);
			MappingObject& operator=(const MappingObject&) = delete;
			MappingObject& operator=(MappingObject&& m);
			~MappingObject();


			static MappingObject CreateFromFile(const FileSystem::File& file,
				std::wstring& mapObjName, PageProtectionOption ppo, 
				PageProtectionFlags ppf = PageProtectionFlags::NoFlags, 
				long long maximumSize = 0x0LL, SECURITY_ATTRIBUTES* psa = nullptr);

			static MappingObject CreateFromPage(std::wstring& mapObjName, long long maximumSize,
				PageProtectionOption ppo, PageProtectionFlags ppf = PageProtectionFlags::NoFlags,
				SECURITY_ATTRIBUTES* psa = nullptr);

			static MappingObject OpenByName(std::wstring& mapObjName, FileMapAccessRight desiredAccess, bool inheritable);


			void* Map(FileMapAccessRight access, long long offset, size_t bytesToMap);
			static bool UnMap(void* baseAddress);
			static bool Flush(void* baseAddress, size_t bytesToFlush);

			void Close();
			HANDLE Handle()const { return _hMappedFile; }
			bool Valid()const { return _isValid; }
			
		private:
			HANDLE _hMappedFile;
			bool _isValid;
		};
	}
}

