#ifndef __SSE_PDK__
#define __SSE_PDK__

///////////////////////////////////////////////////////////////
#include <windows.h>
#include <vector>

#define SSE_CALL __cdecl
#define SSE_PDK extern "C" __declspec(dllexport)

class CApi;
static HMODULE SSeModule = nullptr;

#ifdef _WIN64
#define SSE_DLL "SmartSteamEmu64.dll"
#define STEAMPI_DLL "steam_api64.dll"
#else
#define SSE_DLL "SmartSteamEmu.dll"
#define STEAMPI_DLL "steam_api.dll"
#endif

#ifdef SSE_EMU
#define SSE_PURE
#else
#define SSE_PURE = 0
#include "OSW/Steamclient.h"
#include "SSE_Callback.h"
#endif

#include "ISSePlayerManagement.h"
#include "ISSeNetworking.h"
#include "ISSeSettings.h"
#include "ISSeInternal.h"
#include "ISSeUtils.h"
#include "ISSeImage.h"
#include "ISSeCrypto.h"
#include "ISSeOverlay.h"


///////////////////////////////////////////////////////////////


class CSSeLoader
{
public:
	static CApi* GetSSeApi()
	{
		typedef CApi* (SSE_CALL *_GetSSeApi)();
		HMODULE hSSE = LoadLibrary(TEXT(SSE_DLL));

		if (hSSE) {
			_GetSSeApi fnGetSSeApi = (_GetSSeApi) GetProcAddress( hSSE, "GetSSeApi" );
			if (fnGetSSeApi) {
				HandleAccessor(true, hSSE);
				return fnGetSSeApi();
			}
		}

		FreeLibrary( hSSE );
		return NULL;
	}

	static void Unload()
	{
		if (HandleAccessor())
		{
			FreeLibrary( HandleAccessor() );
			HandleAccessor(true);
		}
	}

private:
	static HMODULE HandleAccessor(bool bSet = false, HMODULE hMod = NULL)
	{
		static HMODULE m_hSSE = NULL;
		if (bSet) m_hSSE = hMod;
		return m_hSSE;
	}
};



// For steam client interface, both hSteamUser and hSteamPipe will be unused.
// If multiple plugins register the same interface, only the first one will be selected.
typedef LPVOID (__cdecl *_InterfaceProvider)(__in PCSTR pchInterfaceVersion, __in int hSteamUser, __in int hSteamPipe);


class CApi
{
public:
	// Write log to file, if the log enabled.
	virtual VOID Log(__in PCSTR format, ...) SSE_PURE;
	virtual VOID Log(__in PCWSTR format, ...) SSE_PURE;

	// Hooking functions, id based. When you hook, it will give you id on lpnHookId, and you can use this on other hook function.
	virtual DWORD Hook(__out_opt SIZE_T *lpnHookId, __out LPVOID *lplpCallOriginal, __in LPVOID lpFuncToHook, __in LPVOID lpNewFuncAddr) SSE_PURE;
	virtual DWORD Unhook(__in SIZE_T nHookId) SSE_PURE;
	virtual DWORD EnableHook(__in SIZE_T nHookId, __in BOOL bEnable) SSE_PURE;

	// Get steam client interface.
	virtual LPVOID GetSteamClientInterface(__in PCSTR pchVersion) SSE_PURE;

	// Register new interface or to override built-in interface.
	virtual VOID RegisterInterfaceProvider(__in _InterfaceProvider pInterfaceProvider) SSE_PURE;
	virtual VOID UnregisterInterfaceProvider(__in _InterfaceProvider pInterfaceProvider) SSE_PURE;

	// Get current app id
	virtual AppId_t GetAppId() SSE_PURE;

	// Get Emulator Root folder or SmartSteamEmu/[<persona_name>/] storage folder
	virtual const char* GetEmuRoot() SSE_PURE;
	virtual const char* GetEmuStorage() SSE_PURE;

	// Get game path or name
	virtual const char* GetExePath() SSE_PURE;
	virtual const char* GetExeName() SSE_PURE;

	// Send steam callback to game. For PostCallback, set hSteamPipe to zero to send to all pipes created and hSteamUser will be ignored.
	virtual bool PostCallResult(HSteamUser hSteamUser, HSteamPipe hSteamPipe, SteamAPICall_t hSteamApi, int iCallback, void* pubCallback, uint32 cubCallback) SSE_PURE;
	virtual bool PostCallback(HSteamUser hSteamUser, HSteamPipe hSteamPipe, int iCallback, void* pubCallback, uint32 cubCallback) SSE_PURE;

	// Plugin folder path, ended with directory separator. Advised to read/write configuration file to this folder.
	virtual const char* GetPluginRoot() SSE_PURE;

	// Get HMODULE of SmartSteamEmu.dll
	virtual HMODULE GetSSEModule() SSE_PURE;

	// If user has disabled HTTP in configuration file, plugin can enable them back to use ISteamHTTP interface.
	virtual void EnableHttpInterface(HSteamPipe hSteamPipe, HSteamUser hSteamUser, bool bEnable = true) SSE_PURE;

	// Release a memory allocated by some helper function when bAllocateMemory = true
	virtual void FreeMemory(const void* pBlock) SSE_PURE;

	virtual ISSePlayerManagement* GetISSePlayerManagement() SSE_PURE;
	virtual ISSeNetworking* GetISSeNetworking() SSE_PURE;
	virtual ISSeSettings* GetISSeSettings() SSE_PURE;
	virtual ISSeInternal* GetISSeInternal() SSE_PURE;
	virtual ISSeUtils* GetISSeUtils() SSE_PURE;
	virtual ISSeImage* GetISSeImage() SSE_PURE;
	virtual ISSeCrypto* GetISSeCrypto() SSE_PURE;
};


// Plugin entry point and exit point, this is optional.
SSE_PDK void SSE_CALL SmartInit(CApi* pApi);
SSE_PDK void SSE_CALL SmartShutdown(bool bAllThreadsTerminated);

// Will be called after all plugins SmartInit() has been called and before all plugins SmartShutdown() get called.
// Useful if plugins provide some interfaces that can be used by other plugins.
SSE_PDK void SSE_CALL SmartReady();
SSE_PDK void SSE_CALL SmartUnready();

// This function will be called by the game for every frame.
SSE_PDK void SSE_CALL SmartRunFrame();

// This function will be called when a Callback or CallResult (hSteamApi != NULL) is posted. Return false to swallow the Callback or CallResult and the game
// will not receive them. Otherwise, return true.
SSE_PDK bool SSE_CALL SmartCallbackFilter(HSteamPipe hSteamPipe, HSteamUser hSteamUser, int iCallback, uint8* pubParam, int cubParam, SteamAPICall_t hSteamApi, bool bEmittedFromPlugins);


#endif // __SSE_PDK__