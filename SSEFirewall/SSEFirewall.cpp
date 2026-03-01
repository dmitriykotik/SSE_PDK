#include "stdafx.h"
#include "../SSE_PDK/SSE_PDK.h"
#include "SSEFirewall.h"

CApi* Api = nullptr;
ISSeOverlay* Overlay = nullptr;
ISSeSettings* Settings = nullptr;

_gethostbyname ogethostbyname = nullptr;
_getaddrinfo ogetaddrinfo = nullptr;
_GetAddrInfoW oGetAddrInfoW = nullptr;

std::vector<std::string> GResolveName;
CRITICAL_SECTION GCSResolveName;

struct hostent FAR* WSAAPI MyGetHostByName(const char FAR * name)
{
	struct hostent FAR* ent = ogethostbyname(name);
	
	if (name)
	{
#ifdef _DEBUG
		OutputDebugStringA(name);
#endif
		if (Settings->GetSettingBool("SSEFirewall", "Logging"))
		{
			Api->Log("[SSEFirewall] Request through gethostbyname: %s", name);
		}

		EnterCriticalSection(&GCSResolveName);
		GResolveName.push_back(name);
		LeaveCriticalSection(&GCSResolveName);

		if (Settings->GetSettingBool("Block", name))
		{
			if (ent && ent->h_length == 4)
			{
				int i = 0;
				while (ent->h_addr_list[i] != 0)
				{
					*(uint32*)ent->h_addr_list[i++] = 0x0100007f;
				}
			}
		}
	}

	return ent;
}

INT WSAAPI MyGetAddrInfo(
	__in_opt        PCSTR               pNodeName,
	__in_opt        PCSTR               pServiceName,
	__in_opt        const ADDRINFOA *   pHints,
	__deref_out     PADDRINFOA *        ppResult)
{
	INT nRetVal = ogetaddrinfo(pNodeName, pServiceName, pHints, ppResult);

	if (nRetVal == 0 && pNodeName)
	{
#ifdef _DEBUG
		OutputDebugStringA(pNodeName);
#endif
		if (Settings->GetSettingBool("SSEFirewall", "Logging"))
		{
			Api->Log("[SSEFirewall] Request through getaddrinfo: %s", pNodeName);
		}

		EnterCriticalSection(&GCSResolveName);
		GResolveName.push_back(pNodeName);
		LeaveCriticalSection(&GCSResolveName);

		if (Settings->GetSettingBool("Block", pNodeName) && ppResult)
		{
			addrinfo* res;

			for (res = *ppResult; res != nullptr; res = res->ai_next)
			{
				if (res->ai_addr->sa_family == AF_INET)
				{
					SOCKADDR_IN* addr = (SOCKADDR_IN*)res->ai_addr;
					addr->sin_addr.S_un.S_addr = 0x0100007f;
				}
				else if (res->ai_addr->sa_family == AF_INET6)
				{
					// ip6 is not tested
					char ipv6[] = IN6ADDR_LOOPBACK_INIT;
					SOCKADDR_IN6* addr = (SOCKADDR_IN6*)res->ai_addr;
					memcpy(addr->sin6_addr.u.Byte, ipv6, 16);
				}
			}
		}
	}

	return nRetVal;
}

INT WSAAPI MyGetAddrInfoW(
	__in_opt        PCWSTR              pNodeName,
	__in_opt        PCWSTR              pServiceName,
	__in_opt        const ADDRINFOW *   pHints,
	__deref_out     PADDRINFOW *        ppResult)
{
	INT nRetVal = oGetAddrInfoW(pNodeName, pServiceName, pHints, ppResult);

	if (nRetVal == 0 && pNodeName)
	{
		char* pszNodeName = Api->GetISSeUtils()->ToUtf8(pNodeName, true);
		if (pszNodeName)
		{
#ifdef _DEBUG
			OutputDebugStringA(pszNodeName);
#endif
			if (Settings->GetSettingBool("SSEFirewall", "Logging"))
			{
				Api->Log("[SSEFirewall] Request through GetAddrInfoW: %s", pszNodeName);
			}

			EnterCriticalSection(&GCSResolveName);
			GResolveName.push_back(pszNodeName);
			LeaveCriticalSection(&GCSResolveName);

			if (Settings->GetSettingBool("Block", pszNodeName) && ppResult)
			{
				addrinfoW* res;

				for (res = *ppResult; res != nullptr; res = res->ai_next)
				{
					if (res->ai_addr->sa_family == AF_INET)
					{
						SOCKADDR_IN* addr = (SOCKADDR_IN*)res->ai_addr;
						addr->sin_addr.S_un.S_addr = 0x0100007f;
					}
					else if (res->ai_addr->sa_family == AF_INET6)
					{
						// ip6 is not tested
						char ipv6[] = IN6ADDR_LOOPBACK_INIT;
						SOCKADDR_IN6* addr = (SOCKADDR_IN6*)res->ai_addr;
						memcpy(addr->sin6_addr.u.Byte, ipv6, 16);
					}
				}
			}

			Api->FreeMemory(pszNodeName);
		}
	}

	return nRetVal;
}

void __cdecl ConListRequests(const char* szArgv[], uint32 nArgc, void* pContext)
{
	Overlay->ConsolePrint("Hostname Requests:");

	EnterCriticalSection(&GCSResolveName);
	long long n = 0;
	for (auto it = GResolveName.begin(); it != GResolveName.end(); it++, n++)
	{
		std::string s;
		uint32 color = 0xff22b14c;

		if (Settings->GetSettingBool("Block", it->c_str()))
		{
			s += " [Blocked] ";
			color = 0xffed1c24;
		}
		else
		{
			s += " [Allowed] ";
		}

		s += "(";
		s += std::to_string(n);
		s += ") ";
		s += *it;
		Overlay->ConsolePrint(s.c_str(), color);
	}
	LeaveCriticalSection(&GCSResolveName);
}

void __cdecl ConBlock(const char* szArgv[], uint32 nArgc, void* pContext)
{
	if (nArgc == 0) {
		Overlay->ConsolePrint("Missing hostname", 0xffff0000);
		return;
	}

	Settings->SetSettingBool("Block", szArgv[0], true);
}

void __cdecl ConAllow(const char* szArgv[], uint32 nArgc, void* pContext)
{
	if (nArgc == 0) {
		Overlay->ConsolePrint("Missing hostname", 0xffff0000);
		return;
	}

	Settings->SetSettingBool("Block", szArgv[0], false);
}

void __cdecl ConListBlock(const char* szArgv[], uint32 nArgc, void* pContext)
{
	if (nArgc == 0) {
		Overlay->ConsolePrint("Missing index", 0xffff0000);
		return;
	}

	int n = atoi(szArgv[0]);
	EnterCriticalSection(&GCSResolveName);
	if (n < (int)GResolveName.size() && n >= 0)
	{
		Settings->SetSettingBool("Block", GResolveName[n].c_str(), true);
	}
	else
	{
		Overlay->ConsolePrint("Index out of bound", 0xffff0000);
	}
	LeaveCriticalSection(&GCSResolveName);
}

void __cdecl ConListAllow(const char* szArgv[], uint32 nArgc, void* pContext)
{
	if (nArgc == 0) {
		Overlay->ConsolePrint("Missing index", 0xffff0000);
		return;
	}

	int n = atoi(szArgv[0]);
	EnterCriticalSection(&GCSResolveName);
	if (n < (int)GResolveName.size() && n >= 0)
	{
		Settings->SetSettingBool("Block", GResolveName[n].c_str(), false);
	}
	else
	{
		Overlay->ConsolePrint("Index out of bound", 0xffff0000);
	}
	LeaveCriticalSection(&GCSResolveName);
}


SSE_PDK void SSE_CALL SmartInit(CApi* pApi)
{
	Api = pApi;
	SSeModule = pApi->GetSSEModule();

	Settings = Api->GetISSeSettings();
	Settings->Init("SSEFirewall.ini");
	
	InitializeCriticalSection(&GCSResolveName);

	HMODULE hSock = GetModuleHandleA("ws2_32.dll");
	if (hSock)
	{
		ogethostbyname = (_gethostbyname)GetProcAddress(hSock, "gethostbyname");
		ogetaddrinfo = (_getaddrinfo)GetProcAddress(hSock, "getaddrinfo");
		oGetAddrInfoW = (_GetAddrInfoW)GetProcAddress(hSock, "GetAddrInfoW");

		Api->Hook(nullptr, (LPVOID*)&ogethostbyname, ogethostbyname, MyGetHostByName);
		Api->Hook(nullptr, (LPVOID*)&ogetaddrinfo, ogetaddrinfo, MyGetAddrInfo);
		Api->Hook(nullptr, (LPVOID*)&oGetAddrInfoW, oGetAddrInfoW, MyGetAddrInfoW);
	}
}

SSE_PDK void SSE_CALL SmartReady()
{
	Overlay = (ISSeOverlay*)Api->GetSteamClientInterface(SSEOVERLAY_INTERFACE001);
	if (Overlay)
	{
		Overlay->ConsoleAddCommand("fw_list_request", "List hostname requests made by game", "fw_list_request", ConListRequests, nullptr);
		Overlay->ConsoleAddCommand("fw_block", "Block hostname given", "fw_block hostname", ConBlock, nullptr);
		Overlay->ConsoleAddCommand("fw_allow", "Allow hostname given", "fw_allow hostname", ConAllow, nullptr);
		Overlay->ConsoleAddCommand("fw_list_block", "Block hostname from request list", "fw_list_block number", ConListBlock, nullptr);
		Overlay->ConsoleAddCommand("fw_list_allow", "Allow hostname from request list", "fw_list_allow number", ConListAllow, nullptr);
	}
}

SSE_PDK void SSE_CALL SmartUnready()
{
	if (Overlay)
	{
		Overlay->ConsoleRemoveCommand("fw_list_request", ConListRequests);
		Overlay->ConsoleRemoveCommand("fw_block", ConBlock);
		Overlay->ConsoleRemoveCommand("fw_allow", ConAllow);
		Overlay->ConsoleRemoveCommand("fw_list_block", ConListBlock);
		Overlay->ConsoleRemoveCommand("fw_list_allow", ConListAllow);
	}
}

SSE_PDK void SSE_CALL SmartShutdown(bool bAllThreadsTerminated)
{
	DeleteCriticalSection(&GCSResolveName);
}
