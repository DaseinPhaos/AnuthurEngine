//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#pragma once
#ifdef LUXKOUTILITY_EXPORTS
#define LUXKOUTILITY_API _declspec(dllexport)
#else
#define LUXKOUTILITY_API _declspec(dllimport)
#endif
#include "CommonHeader.h"
#include <string>
#include <utility>


namespace Luxko {
	namespace FileSystem {

#pragma region FileRelatedScopedEnums
		enum class LUXKOUTILITY_API FileAccess :DWORD
		{
			Read = GENERIC_READ,
			Write = GENERIC_WRITE,
			ReadWrite = GENERIC_READ | GENERIC_WRITE
		};

		enum class LUXKOUTILITY_API FileShareMode :DWORD
		{
			NoSharing = 0x0, // Prevents other processes from opening a file or device if they request delete, read, or write access.
		
			Read = FILE_SHARE_READ, // Enables subsequent open operations on a file or device to request write access.
			
			Write = FILE_SHARE_WRITE,  // Enables subsequent open operations on a file or device to request read access.
			
			Delete = FILE_SHARE_DELETE // Enables subsequent open operations on a file or device to request delete access.
		};

		enum class LUXKOUTILITY_API FileCreationOption : DWORD
		{
			CreateAlways = CREATE_ALWAYS,
			
			CreateNew = CREATE_NEW,
			
			OpenAlways = OPEN_ALWAYS,
			
			OpenExisting = OPEN_EXISTING,
			
			TruncateExisiting = TRUNCATE_EXISTING // Open a file and truncates it so that the size is zero, only if it exists.
		};

		enum class LUXKOUTILITY_API FileAttribute : DWORD
		{
			Archive = FILE_ATTRIBUTE_ARCHIVE, // The file should be archived. Applications use this attribute to mark files for backup or removal.
			
			Encryted = FILE_ATTRIBUTE_ENCRYPTED, // The file or directory is encrypted. For a file, this means that all data in the file is encrypted. For directory, this means that encryption is the default for its newly created files and subdirectories.
			
			Hidden = FILE_ATTRIBUTE_HIDDEN, // This file is hidden.
			
			Normal = FILE_ATTRIBUTE_NORMAL, // This file does not have other attributes set. Valid only if used alone.
			
			Offline = FILE_ATTRIBUTE_OFFLINE, // The data of a file is not immediately available.
			
			Readonly = FILE_ATTRIBUTE_READONLY, // The file is read only.
			
			System = FILE_ATTRIBUTE_SYSTEM, // The file is part of or used exclusively by an OS.
			
			Temporary = FILE_ATTRIBUTE_TEMPORARY, // The file is being used for temporary storage.

			// The Following attributes can't be set upon file creation. Instead they could be returned by the system.
			Compressed = FILE_ATTRIBUTE_COMPRESSED,
			
			Device = FILE_ATTRIBUTE_DEVICE, // This value is reserved for system use.
			
			Directory = FILE_ATTRIBUTE_DIRECTORY, // This value indicates that the handle identifies a directory.
			
			IntegrityStream = FILE_ATTRIBUTE_INTEGRITY_STREAM, // The directory or user data stream is configured with integrity (only supported on ReFS volumes). It is not included in an ordinary directory listing. 
			
			ReparsePoint = FILE_ATTRIBUTE_REPARSE_POINT, // This identity has an associated reparse point, or a file that is a symbolic link.
			
			SparseFile = FILE_ATTRIBUTE_SPARSE_FILE, // A sparse file.
			
			Virtual = FILE_ATTRIBUTE_VIRTUAL // The value is reserved for system use.

		};

		LUXKOUTILITY_API inline FileAttribute operator|(FileAttribute a, FileAttribute b);
		LUXKOUTILITY_API inline FileAttribute operator&(FileAttribute a, FileAttribute b);

		enum class LUXKOUTILITY_API FileFlag : DWORD
		{
			NoFlags = 0x0UL,
			
			BackupSemantics = FILE_FLAG_BACKUP_SEMANTICS, // The file is being opened for a backup or restore operation.
			
			DeleteOnClose = FILE_FLAG_DELETE_ON_CLOSE, // The file is to be deleted immediately after all of its handles are closed.
			
			NoBuffering = FILE_FLAG_NO_BUFFERING, // The file is being opened with no system caching for dataRW. Details at https://msdn.microsoft.com/en-us/library/windows/desktop/cc644950(v=vs.85).aspx .
			
			OpenNoRecall = FILE_FLAG_OPEN_NO_RECALL, // the file data is requested, but should continue to be located in remote storage. Used for remote storage systems.
			
			OpenReparsePoint = FILE_FLAG_OPEN_REPARSE_POINT, // Normal reparse point processing will not occur. If the file is not a reparse point, this flag is ignored. https://msdn.microsoft.com/en-us/library/windows/desktop/aa365503(v=vs.85).aspx
			
			Overlapped = FILE_FLAG_OVERLAPPED, // The file or device is being opened or created for asynchronous IO. https://msdn.microsoft.com/en-us/library/windows/desktop/aa363858(v=vs.85).aspx#synchronous_and_asynchronous_i_o_handles
			
			PosixSemantics = FILE_FLAG_POSIX_SEMANTICS, // Access will occur according to POSIX rules.
			
			RandomAccess = FILE_FLAG_RANDOM_ACCESS, // Access is intended to be random. No effect if FILE_FLAG_NO_BUFFERING . https://msdn.microsoft.com/en-us/library/windows/desktop/aa363858(v=vs.85).aspx#caching_behavior
			
			SessionAware = FILE_FLAG_SESSION_AWARE, // The file or device is being opened with session awareness.
			
			SequentialScan = FILE_FLAG_SEQUENTIAL_SCAN, // Access is intended to be sequential from beginning to end. Hint for file caching.
		
			WriteThrough = FILE_FLAG_WRITE_THROUGH, // Write operations will not go through any intermediate cache, but directly to disk.
		};


		LUXKOUTILITY_API inline FileFlag operator|(FileFlag a, FileFlag b);
		LUXKOUTILITY_API inline FileFlag operator&(FileFlag a, FileFlag b);
		enum class LUXKOUTILITY_API MoveFileFlag : DWORD
		{
			NoFlag = 0x0UL,
			
			ReplaceExisting = MOVEFILE_REPLACE_EXISTING, // Replace an existing file
			
			WriteThrough = MOVEFILE_WRITE_THROUGH, // The function won't return until the copied file is flushed through the disk
			
			AllowCopy = MOVEFILE_COPY_ALLOWED, // Use this flag to allow moving across volumes.
			
			DelayUntilReboot = MOVEFILE_DELAY_UNTIL_REBOOT // Ensures that the file move happens during next reboot. Administrator only, can be used along with AllowCopy.
		};

		LUXKOUTILITY_API inline MoveFileFlag operator|(MoveFileFlag a, MoveFileFlag b);
		LUXKOUTILITY_API inline MoveFileFlag operator&(MoveFileFlag a, MoveFileFlag b);


		enum class LUXKOUTILITY_API FilePosition : DWORD
		{
			Begin = FILE_BEGIN,
			End = FILE_END,
			Current = FILE_CURRENT
		};

		

#pragma endregion FileRelatedScopedEnums

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
			FILETIME m_ft;
			FileTime ToLocal()const;
			FileTime ToGlobal()const;
			LARGE_INTEGER toLargeInteger()const;
		};

		class LUXKOUTILITY_API SystemTime {
		public:
			SystemTime() {}
			explicit SystemTime(const FileTime& ft);
			SystemTime(const SYSTEMTIME& st):m_st(st) {}
			SystemTime(const SystemTime&) = default;
			SystemTime& operator=(const SystemTime&) = default;
			~SystemTime() = default;
			bool operator==(const SystemTime& st)const;
			SYSTEMTIME m_st;
		};

		class LUXKOUTILITY_API BasicFileInfo {
		public:
			BasicFileInfo() {}
			BasicFileInfo(const BasicFileInfo&) = default;
			BasicFileInfo& operator=(const BasicFileInfo&) = default;
			BasicFileInfo(const FILE_BASIC_INFO& fbi);
			~BasicFileInfo() = default;

			FileTime m_CreationTime;
			FileTime m_LastAccessTime;
			FileTime m_LastWriteTime;
			FileTime m_ChangeTime;
			FileAttribute m_Attribute;
		};

		class LUXKOUTILITY_API File {
		public:
			// Creation
			static File Create(const std::wstring& fileName, 
							FileAccess access = FileAccess::ReadWrite,
							FileShareMode shareMode = FileShareMode::NoSharing, 
							FileCreationOption creationOp = FileCreationOption::CreateAlways,
							FileAttribute attribute = FileAttribute::Normal, 
							FileFlag flags = FileFlag::NoFlags,
							SECURITY_ATTRIBUTES* pSA=nullptr);

			static File CreateAccordingTo(const std::wstring& fileName, const File& f, 
									FileAccess access = FileAccess::ReadWrite, 
									FileShareMode shareMode = FileShareMode::NoSharing,
									SECURITY_ATTRIBUTES* pSA = nullptr);

			// File Manipulation by Name
			static bool Delete(const std::wstring& fileName);
			static bool Copy(const std::wstring& sourceFileName, const std::wstring& targetName, bool replaceExists);
			static bool HardLink(const std::wstring& linkName, const std::wstring& sourceFileName);
			static bool Move(const std::wstring& oldFileName, const std::wstring& newFileName, MoveFileFlag moveFlags = MoveFileFlag::NoFlag);
			
			File() :_isValid(false) {}
			~File();
			File(const File& f) = delete;
			File(File&& f);
			File& operator=(const File& f) = delete;
			File& operator=(File&& f);
			void swap(File& f);

			// Handle Manipulation
			bool Valid()const { return _isValid; }
			HANDLE Handle()const { return _hFile; }
			HANDLE Free() { _isValid = false; return _hFile;  }
			void Close();
			

			// File Manipulation by Handle
			long long GetPos()const;
			bool SetPos(long long distance, FilePosition fp, long long * newPos = nullptr);
			long long Size()const;
			std::wstring GetFullName(bool includeVolume=true)const;
			std::wstring GetFileName()const;
			bool SetFileName(const std::wstring& newName);
			BasicFileInfo GetBasicInfo()const;
			bool SetBasicInfo( BasicFileInfo& bfi);

			bool Read(void* data, DWORD bytesToRead, DWORD& bytesRead);
			bool Write(void* data, DWORD bytesToWrite, DWORD& bytesWritten);
			//bool Lock()


		private:
			
			HANDLE _hFile;
			bool _isValid;
		};

		class LUXKOUTILITY_API SearchResult {
		public:
			SearchResult() {}
			SearchResult(const BasicFileInfo& bfi, const std::wstring& fName, long long fileLength) :
				m_basicInfo(bfi),m_fileName(fName),m_fileSize(fileLength) {}
			SearchResult(const WIN32_FIND_DATA& data);
			SearchResult(const SearchResult&) = default;
			SearchResult& operator=(const SearchResult&) = default;
			~SearchResult() = default;

			BasicFileInfo m_basicInfo;
			std::wstring m_fileName; // Generates Warning C4251, can be safely ignored according to https://msdn.microsoft.com/query/dev14.query?appId=Dev14IDEF1&l=EN-US&k=k(C4251)&rd=true
			long long m_fileSize;
		};

		// @Luxko: Current Implementation on searching kinda sucks.
		class LUXKOUTILITY_API Searching {
		public:
			Searching():_isValid(false) {}
			Searching(const std::wstring& fileName);
			Searching(Searching&& s);
			Searching(const Searching&) = delete;
			Searching& operator=(const Searching&) = delete;
			Searching& operator=(Searching&&);
			~Searching();
			
			bool lastResultValid()const;
			SearchResult lastResult()const;
			bool SearchNext();
			
			void Close();

		private:
			HANDLE _sHandle;
			bool _isValid;
			SearchResult _lastSearchResult;
		};

		class LUXKOUTILITY_API Directory {
		public:
			// Directory Manipulation by Name
			static bool Create(const std::wstring& directoryName);
			static bool Remove(const std::wstring& directoryName);
			static bool SetCurrent(const std::wstring& dirctoryName);
			static std::wstring GetCurrent();
			static std::wstring GetParentUnlessRoot(const std::wstring& dirPath);
			static std::wstring GetChild(const std::wstring& dirPath, const std::wstring& childPath);
		};

		LUXKOUTILITY_API void swap(File& a, File& b);
		
	}
}