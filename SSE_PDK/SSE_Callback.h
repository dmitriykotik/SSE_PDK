#pragma once


typedef void (__cdecl *_RunCallbacks)(HSteamPipe hSteamPipe, bool bGameServerCallbacks);
typedef void (__cdecl *_RegisterCallback)(class CCallbackBase*, int);
typedef void (__cdecl *_UnregisterCallback)(class CCallbackBase*);
typedef void (__cdecl *_RegUnregCallResult)(class CCallbackBase*, SteamAPICall_t);


void inline SSE_RunCallbacks(HSteamPipe hSteamPipe, bool bGameServerCallbacks)
{
	static _RunCallbacks fn = NULL;
	if (fn == NULL)
	{
		HMODULE hMod = SSeModule;
		if (hMod == NULL) hMod = GetModuleHandle(TEXT(SSE_DLL));
		if (hMod == NULL) hMod = GetModuleHandle(TEXT(STEAMPI_DLL));

		if (hMod)
		{
			fn = (_RunCallbacks)GetProcAddress( hMod, "Steam_RunCallbacks" );
		}

		if (fn == NULL) return;
	}

	fn(hSteamPipe, bGameServerCallbacks);
}

void inline SSE_RegisterCallback( class CCallbackBase *pCallback, int iCallback )
{
	static _RegisterCallback fn = NULL;
	if (fn == NULL)
	{
		HMODULE hMod = SSeModule;
		if (hMod == NULL) hMod = GetModuleHandle(TEXT(SSE_DLL));
		if (hMod == NULL) hMod = GetModuleHandle(TEXT(STEAMPI_DLL));

		if (hMod)
		{
			fn = (_RegisterCallback)GetProcAddress( hMod, "SteamAPI_RegisterCallback" );
		}

		if (fn == NULL) return;
	}

	fn(pCallback, iCallback);
}

void inline SSE_UnregisterCallback( class CCallbackBase *pCallback )
{
	static _UnregisterCallback fn = NULL;
	if (fn == NULL)
	{
		HMODULE hMod = SSeModule;
		if (hMod == NULL) hMod = GetModuleHandle(TEXT(SSE_DLL));
		if (hMod == NULL) hMod = GetModuleHandle(TEXT(STEAMPI_DLL));

		if (hMod)
		{
			fn = (_UnregisterCallback)GetProcAddress( hMod, "SteamAPI_UnregisterCallback" );
		}

		if (fn == NULL) return;
	}

	fn(pCallback);
}

void inline SSE_RegisterCallResult( class CCallbackBase *pCallback, SteamAPICall_t hAPICall )
{
	static _RegUnregCallResult fn = NULL;
	if (fn == NULL)
	{
		HMODULE hMod = SSeModule;
		if (hMod == NULL) hMod = GetModuleHandle(TEXT(SSE_DLL));
		if (hMod == NULL) hMod = GetModuleHandle(TEXT(STEAMPI_DLL));

		if (hMod)
		{
			fn = (_RegUnregCallResult)GetProcAddress( hMod, "SteamAPI_RegisterCallResult" );
		}

		if (fn == NULL) return;
	}

	fn(pCallback, hAPICall);
}

void inline SSE_UnregisterCallResult( class CCallbackBase *pCallback, SteamAPICall_t hAPICall )
{
	static _RegUnregCallResult fn = NULL;
	if (fn == NULL)
	{
		HMODULE hMod = SSeModule;
		if (hMod == NULL) hMod = GetModuleHandle(TEXT(SSE_DLL));
		if (hMod == NULL) hMod = GetModuleHandle(TEXT(STEAMPI_DLL));

		if (hMod)
		{
			fn = (_RegUnregCallResult)GetProcAddress( hMod, "SteamAPI_UnregisterCallResult" );
		}

		if (fn == NULL) return;
	}

	fn(pCallback, hAPICall);
}

//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// Purpose: maps a steam async call result to a class member function
//			template params: T = local class, P = parameter struct
//-----------------------------------------------------------------------------
template< class T, class P >
class CSSeCallResult : private CCallbackBase
{
public:
	typedef void (T::*func_t)( P*, bool );

	CSSeCallResult()
	{
		m_hAPICall = k_uAPICallInvalid;
		m_pObj = NULL;
		m_Func = NULL;
		m_iCallback = P::k_iCallback;
	}

	void Set( SteamAPICall_t hAPICall, T *p, func_t func )
	{
		if ( m_hAPICall )
			SSE_UnregisterCallResult( this, m_hAPICall );

		m_hAPICall = hAPICall;
		m_pObj = p;
		m_Func = func;

		if ( hAPICall )
			SSE_RegisterCallResult( this, hAPICall );
	}

	bool IsActive() const
	{
		return ( m_hAPICall != k_uAPICallInvalid );
	}

	void Cancel()
	{
		if ( m_hAPICall != k_uAPICallInvalid )
		{
			SSE_UnregisterCallResult( this, m_hAPICall );
			m_hAPICall = k_uAPICallInvalid;
		}

	}

	~CSSeCallResult()
	{
		Cancel();
	}

private:
	virtual void Run( void *pvParam )
	{
		m_hAPICall = k_uAPICallInvalid; // caller unregisters for us
		(m_pObj->*m_Func)( (P *)pvParam, false );		
	}
	void Run( void *pvParam, bool bIOFailure, SteamAPICall_t hSteamAPICall )
	{
		if ( hSteamAPICall == m_hAPICall )
		{
			m_hAPICall = k_uAPICallInvalid; // caller unregisters for us
			(m_pObj->*m_Func)( (P *)pvParam, bIOFailure );			
		}
	}
	int GetCallbackSizeBytes()
	{
		return sizeof( P );
	}

	SteamAPICall_t m_hAPICall;
	T *m_pObj;
	func_t m_Func;
};



//-----------------------------------------------------------------------------
// Purpose: maps a steam callback to a class member function
//			template params: T = local class, P = parameter struct
//-----------------------------------------------------------------------------
template< class T, class P, bool bGameServer >
class CSSeCallback : private CCallbackBase
{
public:
	typedef void (T::*func_t)( P* );

	// If you can't support constructing a callback with the correct parameters
	// then uncomment the empty constructor below and manually call
	// ::Register() for your object
	// Or, just call the regular constructor with (NULL, NULL)

#ifdef ENABLE_CALLBACK_EMPTY_CONSTRUCTOR
	CSSeCallback() {}
#endif
	// constructor for initializing this object in owner's constructor
	CSSeCallback( T *pObj, func_t func ) : m_pObj( pObj ), m_Func( func )
	{
		if ( pObj && func )
			Register( pObj, func );
	}

	~CSSeCallback()
	{
		if ( m_nCallbackFlags & k_ECallbackFlagsRegistered )
			Unregister();
	}

	// manual registration of the callback
	void Register( T *pObj, func_t func )
	{
		if ( !pObj || !func )
			return;

		if ( m_nCallbackFlags & k_ECallbackFlagsRegistered )
			Unregister();

		if ( bGameServer )
		{
			m_nCallbackFlags |= k_ECallbackFlagsGameServer;
		}
		m_pObj = pObj;
		m_Func = func;
		// SSE_RegisterCallback sets k_ECallbackFlagsRegistered
		SSE_RegisterCallback( this, P::k_iCallback );
	}

	void Unregister()
	{
		// SSE_UnregisterCallback removes k_ECallbackFlagsRegistered
		SSE_UnregisterCallback( this );
	}

	void SetGameserverFlag() { m_nCallbackFlags |= k_ECallbackFlagsGameServer; }
private:
	virtual void Run( void *pvParam )
	{
		(m_pObj->*m_Func)( (P *)pvParam );
	}
	virtual void Run( void *pvParam, bool, SteamAPICall_t )
	{
		(m_pObj->*m_Func)( (P *)pvParam );
	}
	int GetCallbackSizeBytes()
	{
		return sizeof( P );
	}

	T *m_pObj;
	func_t m_Func;
};


// Allows you to defer registration of the callback
template< class T, class P, bool bGameServer >
class CSSeCallbackManual : public CSSeCallback< T, P, bGameServer >
{
public:
	CSSeCallbackManual() : CSSeCallback< T, P, bGameServer >( NULL, NULL ) {}
};



#ifdef _MSC_VER
// disable this warning; this pattern need for steam callback registration
#pragma warning( disable: 4355 )	// 'this' : used in base member initializer list
#endif

// utility macro for declaring the function and callback object together
#define SSE_CALLBACK( thisclass, func, param, var ) CSSeCallback< thisclass, param, false > var; void func( param *pParam )
#define SSE_GAMESERVER_CALLBACK( thisclass, func, param, var ) CSSeCallback< thisclass, param, true > var; void func( param *pParam )

// same as above, but lets you defer the callback binding by calling Register later
#define SSE_CALLBACK_MANUAL( thisclass, func, param, var ) CSSeCallbackManual< thisclass, param, false > var; void func( param *pParam )