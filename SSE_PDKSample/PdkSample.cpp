#include "stdafx.h"
#include "../SSE_PDK/SSE_PDK.h"
#include "ISteamApps006Lite.h"


typedef int (WINAPI *_MessageBoxW)(
	__in_opt HWND hWnd,
	__in_opt LPCWSTR lpText,
	__in_opt LPCWSTR lpCaption,
	__in UINT uType);


// SmartSteamEmu api that is retrieved from SmartInit()
CApi* api;

SIZE_T nMsgBoxId;
_MessageBoxW oMsgBoxW;

// Original interface
ISteamClient016* g_poClient;

int WINAPI MyMessageBoxW(
	__in_opt HWND hWnd,
	__in_opt LPCWSTR lpText,
	__in_opt LPCWSTR lpCaption,
	__in UINT uType)
{
	api->Log(L"SSE_PDK: %s (%X, %s, %s, %X)",
		__FUNCTIONW__,
		hWnd,
		lpText,
		lpCaption,
		uType);

	return 0;
}

LPVOID SSE_CALL MyInterfaceProvider(__in PCSTR pchVersion, int hSteamUser, int hSteamPipe)
{
	try
	{
		// In this example, we override default SteamApps interface with our own interface.
		if (strcmp(pchVersion, STEAMAPPS_INTERFACE_VERSION_006) == 0)
		{
			// Get original interface first and provide our interface
			ISteamApps006* oSteamApps006 = (ISteamApps006*) g_poClient->GetISteamApps(hSteamUser, hSteamPipe, STEAMAPPS_INTERFACE_VERSION_006);

			return new ISteamApps006Lite( hSteamPipe, hSteamUser, oSteamApps006 );
		}
	}
	catch (...)
	{
		api->Log( "SSE_PDK: Error while providing interface" );
	}

	return NULL;
}

SSE_PDK void SSE_CALL SmartInit(CApi* pApi)
{
	api = pApi;
	SSeModule = pApi->GetSSEModule();

	api->Log("SSE_PDK: Hello world!");

	// We are hooking MessageBoxW, calling message box will output to SSE log file.
	// See MyMessageBoxW() above;
	api->Hook(&nMsgBoxId, (LPVOID*)&oMsgBoxW, MessageBoxW, MyMessageBoxW);

	oMsgBoxW(0, L"Calling original message box", L"Hook test", MB_OK);
	MessageBoxW(0, L"Calling hooked message box", L"Hook test", MB_OK);

	// Get the original interface first.
	g_poClient = (ISteamClient016*) api->GetSteamClientInterface(STEAMCLIENT_INTERFACE_VERSION_016);

	api->RegisterInterfaceProvider(MyInterfaceProvider);
}

SSE_PDK void SSE_CALL SmartShutdown(bool bAllThreadsTerminated)
{
	api->UnregisterInterfaceProvider(MyInterfaceProvider);
	api->Unhook(nMsgBoxId);

	api->Log("SSE_PDK: Bye!");
}
