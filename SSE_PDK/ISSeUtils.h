#pragma once

class ISSeEnumerateFolderResult
{
public:
	virtual void EnumerateResult(const char* pFilename, const char* pFullPath, uint64 uFileSize, bool bIsDirectory) = 0;
	virtual void EnumerateComplete() = 0;
};

class ISSeUtils
{
public:
	// Generate a random number between uMin to uMax.
	virtual uint32 GenerateRandom(uint32 uMin, uint32 uMax = 0xffffffff) = 0;
	virtual float GenerateRandomF(float uMin, float uMax = 1.0f) = 0;

	// Checks for file or folder existence, return false when the specified path isn't found.
	virtual bool IsFileExists(const char* pPath) = 0;

	// Enumerate the content of the folder. The result will be returned on ISSeEnumerateFolderResult::EnumerateResult one by one
	// and triggers ISSeEnumerateFolderResult::EnumerateComplete when finish enumerating folder.
	virtual bool EnumerateFolder(const char* FolderPath, ISSeEnumerateFolderResult* pEnumerateFolderResult, const char* FileMask = "*", bool Recursive = false) = 0;

	// Copy folder including all files and folders inside it.
	virtual bool CopyRecursive(const char* pFrom, const char* pTo) = 0;

	// Copy a single file.
	virtual bool FileCopy(const char* pFrom, const char* pTo) = 0;

	// Delete file or folder recursively. The deletion will continue if there is an error but will return false.
	virtual bool FileDelete(const char* pFileToDelete) = 0;

	// Rename a file or folder to new one.
	virtual bool FileRename(const char* pOldPath, const char* pNewPath, bool bReplaceExisting = false) = 0;

	// Create folders for path. You must include '\\' at the end of the path or the last path will be
	// treated as file and will be ignored.
	virtual void CreatePath(const char* pPath) = 0;

	// Convert a relative path to absolute. If the path already absolute, this function will do nothing.
	// Find Order:
	//  1)  Absolute path, return immediately
	//  2)  User storage\\{app_id}
	//  3)  User storage
	//  4)  Common path\\{app_id}
	//  5)  Common path
	//  6)  Root path\\SmartSteamEmu\\Common\\{app_id}
	//  7)  Root path\\SmartSteamEmu\\Common
	//  8)  Root path\\SmartSteamEmu
	//  9)  Root path
	//  10) Current folder
	// The call isn't thread safe unless bAllocateMemory = true and when done, you must free the
	// return value manually using CApi::FreeMemory.
	virtual char* MakePathAbsolute(const char* pRelativePath, bool bAllocateMemory = false) = 0;

	// Convert wide string to utf8. The call isn't thread safe unless bAllocateMemory = true and when done,
	// you must free the return value manually using CApi::FreeMemory.
	virtual char* ToUtf8(const wchar_t* pString, bool bAllocateMemory = false) = 0;

	// Convert utf8 to wide string. It is recommended that all calls to windows API must use the unicode version
	// where available. The call isn't thread safe unless bAllocateMemory = true and when done,
	// you must free the return value manually using CApi::FreeMemory.
	virtual wchar_t* ToWide(const char* pString, bool bAllocateMemory = false) = 0;
};