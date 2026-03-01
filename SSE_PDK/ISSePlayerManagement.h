#pragma once

class ISSeQueryPlayersResponse
{
public:
	virtual void PlayerResponse( uint8 uIndex, const char *pchName, uint32 nScore, float flTimePlayed, CSteamID PlayerSteamId, CSteamID OwnerSteamId ) = 0;
};


class ISSePlayerManagement
{
public:
	virtual bool QueryPlayers(const char* address, uint16 port, ISSeQueryPlayersResponse* pResp, uint32 timeoutMillis = 3000) = 0;
	virtual bool BanPlayer(const char* address, uint16 port, const char* AdminPassword, CSteamID PlayerSteamId, CSteamID OwnerSteamId, uint32 timeoutMillis = 3000) = 0;
	virtual bool UnbanPlayer(const char* address, uint16 port, const char* AdminPassword, CSteamID PlayerSteamId, CSteamID OwnerSteamId, uint32 timeoutMillis = 3000) = 0;
};

