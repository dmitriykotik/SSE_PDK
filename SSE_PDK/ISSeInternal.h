#pragma once

typedef void (SSE_CALL *_FnActivateGameOverlay)(void* pContext, const char* pchDialog);
typedef void (SSE_CALL *_FnActivateGameOverlayToUser)(void* pContext, const char *pchDialog, CSteamID steamID);
typedef void (SSE_CALL *_FnActivateGameOverlayToWebPage)(void* pContext, const char *pchURL);
typedef void (SSE_CALL *_FnActivateGameOverlayToStore)(void* pContext, AppId_t nAppID, EOverlayToStoreFlag eFlag);
typedef void (SSE_CALL *_FnActivateGameOverlayInviteDialog)(void* pContext, CSteamID steamIDLobby);

struct TSSeConnectedPlayerInfo
{
	CSteamID m_PlayerSteamId;
	CSteamID m_OwnerSteamId;	
	FriendGameInfo_t m_GameInfo;
	uint64 m_NetworkGuid;
	uint32 m_Ip;
	uint16 m_Port;
};

class ISSeInternal
{
public:
	virtual uint32 GetConnectedPlayerCount() = 0;
	virtual bool GetConnectedPlayerInfo(uint32 index, TSSeConnectedPlayerInfo* pInfo) = 0;
	virtual bool GetConnectedPlayerInfo(CSteamID PlayerSteamId, TSSeConnectedPlayerInfo* pInfo) = 0;

	virtual int AddImageRgba(uint32 uWidth, uint32 uHeight, void* pubImage, uint32 cubImage = 0) = 0;
	virtual void DeleteImageRgba(int iImage) = 0;

	virtual bool SupplyStats(const char* pchStats, const char* pchDisplayName, int32 nDefaultValue) = 0;
	virtual bool SupplyAchievement(const char* pchAchievementName, const char* pchDisplayName, const char* pchDescription, bool bHidden, int iImage = 0) = 0;
	virtual bool SetAchievementDisplayName(const char* pchAchievementName, const char* pchDisplayName) = 0;
	virtual bool SetAchievementDescription(const char* pchAchievementName, const char* pchDescription) = 0;
	virtual bool SetAchievementIcon(const char* pchAchievementName, int iImage) = 0;
	virtual bool SetAchievementHidden(const char* pchAchievementName, bool bHidden) = 0;

	virtual void RemoveListener(void* pFunction, void* pContext) = 0;
	virtual void AddActivateGameOverlayListener(_FnActivateGameOverlay pFunction, void* pContext) = 0;
	virtual void AddActivateGameOverlayToUserListener(_FnActivateGameOverlayToUser pFunction, void* pContext) = 0;
	virtual void AddActivateGameOverlayToWebPageListener(_FnActivateGameOverlayToWebPage pFunction, void* pContext) = 0;
	virtual void AddActivateGameOverlayToStoreListener(_FnActivateGameOverlayToStore pFunction, void* pContext) = 0;
	virtual void AddActivateGameOverlayInviteDialogListener(_FnActivateGameOverlayInviteDialog pFunction, void* pContext) = 0;
};
