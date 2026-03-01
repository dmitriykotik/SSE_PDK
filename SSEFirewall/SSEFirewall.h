#pragma once

typedef struct hostent FAR * (WSAAPI *_gethostbyname)(__in const char FAR * name);
typedef INT(WSAAPI *_getaddrinfo)(
	__in_opt        PCSTR               pNodeName,
	__in_opt        PCSTR               pServiceName,
	__in_opt        const ADDRINFOA *   pHints,
	__deref_out     PADDRINFOA *        ppResult
	);

typedef INT(WSAAPI *_GetAddrInfoW)(
	__in_opt        PCWSTR              pNodeName,
	__in_opt        PCWSTR              pServiceName,
	__in_opt        const ADDRINFOW *   pHints,
	__deref_out     PADDRINFOW *        ppResult
	);