//==========================  Open Steamworks  ================================
//
// This file is part of the Open Steamworks project. All individuals associated
// with this project do not claim ownership of the contents
// 
// The code, comments, and all related files, projects, resources,
// redistributables included with this project are Copyright Valve Corporation.
// Additionally, Valve, the Valve logo, Half-Life, the Half-Life logo, the
// Lambda logo, Steam, the Steam logo, Team Fortress, the Team Fortress logo,
// Opposing Force, Day of Defeat, the Day of Defeat logo, Counter-Strike, the
// Counter-Strike logo, Source, the Source logo, and Counter-Strike Condition
// Zero are trademarks and or registered trademarks of Valve Corporation.
// All other trademarks are property of their respective owners.
//
//=============================================================================

#ifndef ISTEAMAPPS006LITE_H
#define ISTEAMAPPS006LITE_H


//-----------------------------------------------------------------------------
// Purpose: interface to app data
//-----------------------------------------------------------------------------
class ISteamApps006Lite : ISteamApps006
{
public:

	ISteamApps006Lite ( HSteamPipe hSteamPipe, HSteamUser hSteamUser, ISteamApps006* pOrig = NULL )
	{
		// Store handle, whenever making call to api->PostCallXxx, always use these handles.
		m_hSteamUser = hSteamUser;
		m_hSteamPipe = hSteamPipe;

		// Original SSE ISteamApps006 interface
		m_pOrig = pOrig;
	}

	virtual bool BIsSubscribed()
	{
		return true;
	}

	virtual bool BIsLowViolence()
	{
		return m_pOrig->BIsLowViolence();
	}

	virtual bool BIsCybercafe()
	{
		return m_pOrig->BIsCybercafe();
	}

	virtual bool BIsVACBanned()
	{
		return m_pOrig->BIsVACBanned();
	}

	virtual const char *GetCurrentGameLanguage()
	{
		return m_pOrig->GetCurrentGameLanguage();
	}

	virtual const char *GetAvailableGameLanguages()
	{
		return m_pOrig->GetAvailableGameLanguages();
	}

	virtual bool BIsSubscribedApp( unsigned int appID )
	{
		return m_pOrig->BIsSubscribedApp(appID);
	}

	virtual bool BIsDlcInstalled( unsigned int appID )
	{
		return m_pOrig->BIsDlcInstalled(appID);
	}

	virtual uint32 GetEarliestPurchaseUnixTime( unsigned int nAppID )
	{
		return m_pOrig->GetEarliestPurchaseUnixTime(nAppID);
	}

	virtual bool BIsSubscribedFromFreeWeekend()
	{
		return m_pOrig->BIsSubscribedFromFreeWeekend();
	}

	virtual int GetDLCCount()
	{
		return m_pOrig->GetDLCCount();
	}

	virtual bool BGetDLCDataByIndex( int iDLC, unsigned int *pAppID, bool *pbAvailable, char *pchName, int cchNameBufferSize )
	{
		return m_pOrig->BGetDLCDataByIndex(iDLC, pAppID, pbAvailable, pchName, cchNameBufferSize);
	}

	virtual void InstallDLC( unsigned int nAppID ) 
	{
		m_pOrig->InstallDLC( nAppID );
	}

	virtual void UninstallDLC( unsigned int nAppID )
	{
		m_pOrig->UninstallDLC( nAppID );
	}

	virtual void RequestAppProofOfPurchaseKey( unsigned int nAppID )
	{
		m_pOrig->RequestAppProofOfPurchaseKey( nAppID );
	}

	virtual bool GetCurrentBetaName( char *pchName, int cchNameBufferSize )
	{
		return m_pOrig->GetCurrentBetaName( pchName, cchNameBufferSize );
	}

	virtual bool MarkContentCorrupt( bool bMissingFilesOnly )
	{
		return m_pOrig->MarkContentCorrupt( bMissingFilesOnly );
	}

	virtual uint32 GetInstalledDepots( unsigned int appID, unsigned int *pvecDepots, unsigned int cMaxDepots )
	{
		return m_pOrig->GetInstalledDepots( appID, pvecDepots, cMaxDepots );
	}

	virtual uint32 GetAppInstallDir( unsigned int appID, char *pchFolder, unsigned int cchFolderBufferSize )
	{
		return m_pOrig->GetAppInstallDir( appID, pchFolder, cchFolderBufferSize );
	}
	virtual bool BIsAppInstalled( unsigned int appID )
	{
		return m_pOrig->BIsAppInstalled( appID );
	}

	virtual CSteamID GetAppOwner()
	{
		return m_pOrig->GetAppOwner();
	}

	virtual const char *GetLaunchQueryParam( const char *pchKey )
	{
		return m_pOrig->GetLaunchQueryParam( pchKey );
	}

	virtual bool GetDlcDownloadProgress( AppId_t nAppID, uint64 *punBytesDownloaded, uint64 *punBytesTotal ) 
	{
		return m_pOrig->GetDlcDownloadProgress( nAppID, punBytesDownloaded, punBytesTotal );
	}

private:
	HSteamUser m_hSteamUser;
	HSteamPipe m_hSteamPipe;
	ISteamApps006* m_pOrig;
};

#define STEAMAPPS_INTERFACE_VERSION_006 "STEAMAPPS_INTERFACE_VERSION006"

#endif // ISTEAMAPPS006LITE_H
