// SSEBanHammer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "../SSE_PDK/SSE_PDK.h"


std::vector<std::pair<CSteamID, CSteamID>> g_Players;


class CSSeQueryPlayersResponse : public ISSeQueryPlayersResponse
{
public:
	virtual void PlayerResponse(uint8 uIndex, const char *pchName, uint32 nScore, float flTimePlayed, CSteamID PlayerSteamId, CSteamID OwnerSteamId)
	{
		std::cout << " "
			<< (int)uIndex
			<< ": " << PlayerSteamId.Render()
			<< "/" << OwnerSteamId.Render()
			<< ", Score = " << nScore
			<< ", Duration = " << (int)flTimePlayed << "s"
			<< ", Name = " << pchName
			<< std::endl;

		std::pair<CSteamID, CSteamID> pair = std::make_pair(PlayerSteamId, OwnerSteamId);
		g_Players.push_back(pair);
	}
};

void ShowHelp()
{
	std::cout << "Help, command are case-sensitive: " << std::endl;
	std::cout << "    help - Display this help." << std::endl;
	std::cout << "    refresh - Refresh the player list." << std::endl;
	std::cout << "    ban <player_index> - Ban the player." << std::endl;
	std::cout << "    unban <player_index> - Unban the player." << std::endl;
	std::cout << "    set <address/port/password> <value> - Set the server address or port or value." << std::endl;
	std::cout << "    exit - Terminates the application." << std::endl;
}

int _tmain(int argc, _TCHAR* argv[])
{
	std::cout << "SSEBanHammer " << std::endl;
	std::cout << "    This tool allow you to ban or unban players as long as the steam game server is alive." << std::endl;
	std::cout << "    You also can edit the SmartSteamEmu.ini manually under [PlayerManagement] to manage" << std::endl;
	std::cout << "    the player" << std::endl;
	std::cout << std::endl;

	CApi* api = CSSeLoader::GetSSeApi();
	if (api == NULL)
	{
		std::cout << "Unable to get SmartSteamEmu interface" << std::endl;
		return 1;
	}

	ISSePlayerManagement* pm = api->GetISSePlayerManagement();
	if (pm != NULL)
	{
		bool bExit = false;
		std::string sJunk;

		while (!bExit)
		{
			std::string sAddress;
			std::string sPort;
			std::string sPass;
			uint16 usPort;

			std::cout << "Enter your server address: ";
			std::getline( std::cin, sAddress );
			std::cout << "Enter your server port: ";
			std::getline( std::cin, sPort );
			std::cout << "Enter your admin password: ";
			std::getline( std::cin, sPass );

			usPort = (uint16)std::stoul( sPort );

			std::cout << std::endl;
			std::cout << "Type 'help' to display help. " << std::endl;

			CSSeQueryPlayersResponse resp;
			while (!bExit)
			{
				std::cout << std::endl;

				g_Players.clear();
				if (!pm->QueryPlayers(sAddress.c_str(), usPort, &resp))
				{
					std::cout << "Unable to query server at " << sAddress << ":" << sPort << std::endl;
					std::cout << "Continue? (Y/n): ";
					std::getline( std::cin, sJunk );

					if (sJunk == "n" || sJunk == "N")
					{
						bExit = true;
						continue;
					}
				}

				std::cout << std::endl;

				bool bRefresh = false;
				while (!bRefresh && !bExit)
				{
					std::cout << "> ";

					// Parse the command int vector
					std::string sCommand;
					std::getline( std::cin, sCommand );
					std::istringstream iss(sCommand);
					std::vector<std::string> vCommand;
					do
					{
						std::string sub;
						iss >> sub;
						if (!sub.empty()) {
							vCommand.push_back(sub);
						}
					} while (iss);

					if (vCommand.size())
					{
						if (vCommand[0] == "help")
						{
							ShowHelp();
						}
						else if (vCommand[0] == "refresh")
						{
							bRefresh = true;
						}
						else if (vCommand[0] == "ban" && vCommand.size() >= 2)
						{
							uint32 uIndex = std::stoul(vCommand[1]);
							if (uIndex >= g_Players.size())
							{
								std::cout << "Invalid index." << std::endl;
							}
							else
							{
								bool bRes = pm->BanPlayer(sAddress.c_str(), usPort, sPass.c_str(), g_Players[uIndex].first, g_Players[uIndex].second);
								std::cout << "Banning "
									<< g_Players[uIndex].first.Render()
									<< "/"
									<< g_Players[uIndex].second.Render()
									<< (bRes ? " success." : " failed.")
									<< std::endl;
							}
						}
						else if (vCommand[0] == "unban" && vCommand.size() >= 2)
						{
							uint32 uIndex = std::stoul(vCommand[1]);
							if (uIndex >= g_Players.size())
							{
								std::cout << "Invalid index." << std::endl;
							}
							else
							{
								bool bRes = pm->UnbanPlayer(sAddress.c_str(), usPort, sPass.c_str(), g_Players[uIndex].first, g_Players[uIndex].second);
								std::cout << "Unbanning "
									<< g_Players[uIndex].first.Render()
									<< "/"
									<< g_Players[uIndex].second.Render()
									<< (bRes ? " success." : " failed.")
									<< std::endl;
							}
						}
						else if (vCommand[0] == "set" && vCommand.size() >= 3)
						{
							if (vCommand[1] == "address")
							{
								sAddress = vCommand[2];
							}
							else if (vCommand[1] == "port")
							{
								usPort = (uint16)std::stoul(vCommand[2]);
							}
							else if (vCommand[1] == "password")
							{
								sPass = vCommand[2];
							}
						}
						else if (vCommand[0] == "exit")
						{
							bExit = true;
						}
						else
						{
							std::cout << "Unknown or invalid command." << std::endl;
						}
					}
				}
			}
		}
	}
	else
	{
		std::cout << "Unable to get PlayerManagement interface" << std::endl;
	}

	CSSeLoader::Unload();
	return 0;
}

