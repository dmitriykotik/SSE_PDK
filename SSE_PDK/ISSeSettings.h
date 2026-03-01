#pragma once

//
//  Plugin can use this class to read or write its settings. The setting will be saved under 'SmartSteamEmu/Plugins' folder
//  as .ini file.
//  Before using this class, a call to Init() must be called and you must specify a name for .ini file.
//  If you want to load SmartSteamEmu configuration file, pass null to Init(). Please note that the configuration file will not
//  always SmartSteamEmu.ini and not always on same location.
//  Call Release() if you are no longer need to use this class. The class will freed and will be no longer usable.
//
//  Internal SmartSteamEmu settings is also accessible and modifiable and Init() is not neccessary.
//

class ISSeSettings
{
public:
	virtual void Init(const char* pSettingsName) = 0;
	virtual void Release() = 0;

	virtual uint32 GetSettingInt(const char* pSection, const char* pKeyName, int nDefault = 0) = 0;
	virtual bool GetSettingBool(const char* pSection, const char* pKeyName, bool bDefault = false) = 0;
	virtual const char* GetSettingString(const char* pSection, const char* pKeyName, const char* pDefault = "", bool bAllocateMemory = false) = 0;
	virtual bool SetSettingInt(const char* pSection, const char* pKeyName, int nValue) = 0;
	virtual bool SetSettingBool(const char* pSection, const char* pKeyName, bool bValue) = 0;
	virtual bool SetSettingString(const char* pSection, const char* pKeyName, const char* pValue) = 0;

	// Below is internal SmartSteamEmu settings that can be accessed and modified.

	virtual const char* GetLanguage() = 0;
	virtual void SetLanguage(const char* pszLanguage) = 0;

	virtual bool GetLowViolence() = 0;
	virtual void SetLowViolence(bool value) = 0;

	virtual const char* GetAvatarFilename() = 0;
	virtual void SetAvatarFilename(const char* pPath) = 0;

	virtual bool GetAutomaticallyJoinInvite() = 0;
	virtual void SetAutomaticallyJoinInvite(bool value) = 0;

	virtual bool GetEnableHTTP() = 0;
	virtual void SetEnableHTTP(bool value) = 0;

	virtual bool GetEnableInGameVoice() = 0;
	virtual void SetEnableInGameVoice(bool value) = 0;

	virtual bool GetEnableLobbyFilter() = 0;
	virtual void SetEnableLobbyFilter(bool value) = 0;

	virtual bool GetEnableOverlay() = 0;
	virtual void SetEnableOverlay(bool value) = 0;

	virtual bool GetDisableFriendList() = 0;
	virtual void SetDisableFriendList(bool value) = 0;

	virtual bool GetDisableLeaderboard() = 0;
	virtual void SetDisableLeaderboard(bool value) = 0;

	virtual bool GetSecuredServer() = 0;
	virtual void SetSecuredServer(bool value) = 0;

	virtual bool GetVR() = 0;
	virtual void SetVR(bool value) = 0;

	virtual bool GetRandomItemAwards() = 0;
	virtual void SetRandomItemAwards(bool value) = 0;

	virtual bool GetBypassDrm() = 0;
	virtual void SetBypassDrm(bool value) = 0;

	virtual uint32 GetQuickJoinHotkey(int pHotkeyArray[], uint32 uHotkeyCount, uint32* uHotkeyReturnedCount) = 0;
	virtual void SetQuickJoinHotkey(int pHotkeyArray[], uint32 uHotkeyCount) = 0;

	virtual bool GetAchievementsUnlockAll() = 0;
	virtual void SetAchievementsUnlockAll(bool value) = 0;

	virtual uint32 GetNetBroadcastAddress(const char* ppAddress[], uint32 uAddressCount, uint32* uAddressReturnedCount) = 0;
	virtual void SetNetBroadcastAddress(const char* ppAddress[], uint32 uAddressCount) = 0;

	virtual uint32 GetNetListenPort() = 0;
	virtual void SetNetListenPort(uint32 uListenPort) = 0;

	virtual uint32 GetNetMaximumPort() = 0;
	virtual void SetNetMaximumPort(uint32 uMax) = 0;

	virtual uint32 GetNetDiscoveryInterval() = 0;
	virtual void SetNetDiscoveryInterval(uint32 uMillis) = 0;

	virtual ENotificationPosition GetOverlayNotificationPosition() = 0;
	virtual void SetOverlayNotificationPosition(ENotificationPosition position) = 0;

	virtual void GetOverlayNotificationInset(int32* nHorizontalInset, int32* nVerticalInset) = 0;
	virtual void SetOverlayNotificationInset(int32 nHorizontalInset, int32 nVerticalInset) = 0;
};