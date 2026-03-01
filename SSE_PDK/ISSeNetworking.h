#pragma once

enum ESSeSocketType
{
	eSSeSocketTypeUDP,
	eSSeSocketTypeTCP,
	eSSeSocketTypeICMP
};

struct TSSeSockAddr
{
	// Host-byte order
	uint32 m_Ip;
	uint32 m_Port;
};

// Do not block in this callback, otherwise network performance will degrade badly.
// Return true if you handle the data or false for emulator to handle it.
typedef bool (SSE_CALL *_FnGameSocketListener)(const uint8* pubData, uint32 cubData, uint32 uIpFrom, uint16 uPort, void* pContext);


// All functions in this interface return and accept host-byte order.
class ISSeSocket
{
public:
	virtual uint32 GetLastError() = 0;

	virtual void Release() = 0;

	virtual bool Connect(const char* cszHost, const uint16 usPort) = 0;
	virtual bool Connect(const uint32 ulAddress, const uint16 usPort) = 0;
	virtual bool Disconnect() = 0;
	virtual bool Listen(int iBacklog = 0) = 0;
	virtual bool Bind(uint16 usPort, uint32 ulAddress = 0) = 0;
	virtual bool Accept(ISSeSocket* pSocket, TSSeSockAddr* pRemoteAddress = nullptr) = 0;

	virtual bool IsConnected() = 0;
	virtual uint32 GetRemoteIP(char* pIpAddrDest = nullptr, uint32 cubIpAddrDest = 0) = 0;
	virtual uint16 GetRemotePort() = 0;

	virtual int Receive(void* pubData, uint32 cubData, int iFlags = 0) = 0;
	virtual int ReceiveFrom(void* pubData, uint32 cubData, TSSeSockAddr* pRecvFromAddr) = 0;
	virtual int Send(const void* pubData, uint32 cubData, int iFlags = 0) = 0;
	virtual int SendTo(const void* pubData, uint32 cubData, TSSeSockAddr* pSendToAddr) = 0;

	virtual uint32 BytesAvailable() = 0;
	virtual bool Readable(uint32 uTimeoutMicroSec = 0) = 0;
	virtual bool Writeable(uint32 uTimeoutMicroSec = 0) = 0;

	virtual bool SetTimeout(int nTimeoutMillis) = 0;
	virtual bool SetBroadcast(bool bBroadcast) = 0;
	virtual bool SetBlocking(bool bBlocking) = 0;

	virtual uint32 GetSocketIp() = 0;
	virtual uint16 GetSocketPort() = 0;
};


class ISSeNetworking
{
public:
	// Get network GUID, this guid is unique for each player.
	virtual uint64 GetNetworkGuid() = 0;

	// Get the internet public IP, this IP may return 0 if user doesn't connect to the internet or
	// the query to get the public ip address is not finished yet. Returned in host-byte order.
	virtual uint32 GetPublicIp() = 0;

	// Get the computer IP address. Returned in host-byte order.
	virtual uint32 GetPrivateIp() = 0;

	// Get the emulator port. Returned in host-byte order.
	virtual uint16 GetEmulatorPort() = 0;

	// Checks if we are connected to others.
	virtual bool IsConnected(uint64 uNetGuid) = 0;

	// Connect manually using ip address and port. This doesn't add entry in broadcast address and may
	// not reconnect in case of network disruption.
	virtual bool Connect(uint32 uIpAddress, uint16 uPort) = 0;

	// Get the total of broadcast address.
	virtual uint32 GetBroadcastAddressCount() = 0;

	// Get broadcast address. If uPort return 0, it means the broadcast will using default emulator port up until
	// maximum port to send broadcast data. Return false if the index isn't valid.
	virtual bool GetBroadcastAddress(uint32 uIndex, uint32* uIpAddress, uint16* uPort) = 0;

	// Add a broadcast address. Set uPort to 0 to use emulator port up until maximum port.
	virtual uint32 AddBroadcastAddress(uint32 uIpAddress, uint16 uPort) = 0;

	// Remove a broadcast address.
	virtual bool RemoveBroadcastAddress(uint32 uIpAddress, uint16 uPort) = 0;

	// Get broadcast address discovery interval, unit returned in millisecond.
	virtual uint32 GetDiscoveryInterval() = 0;

	// Set discovery interval in millisecond unit.
	virtual void SetDiscoveryInterval(uint32 uMillis) = 0;

	// Helper to convert host-byte order ip address to string. Not thread-safe.
	virtual const char* IpToString(uint32 uIp) = 0;

	// Helper to convert host-byte order ip address to string, the size of the string must be at least 16 bytes.
	// Thread-safe.
	virtual void IpToString(uint32 uIp, char* szOutput) = 0;

	// Helper to convert an ip address to host-byte order integer.
	virtual uint32 StringToIp(const char* szIpPort, uint16* lpPort = NULL) = 0;

	// Helper to checks if the ip given falls into public ip range.
	virtual bool IsPublicIp(uint32 uIp) = 0;

	// Helper to convert host to network byte and vice-versa.
	virtual uint16 htons(uint16 uHost) = 0;
	virtual uint32 htonl(uint32 uHost) = 0;
	virtual uint64 htonl64(uint64 uHost) = 0;
	virtual uint16 ntohs(uint16 uNet) = 0;
	virtual uint32 ntohl(uint32 uNet) = 0;
	virtual uint64 ntohl64(uint64 uNet) = 0;

	// Helper function to resolve address to ip. Ip return in host-order.
	virtual uint32 ResolveAddress(const char* pszAddress) = 0;

	// Helper function to resolve address to ip. The ip address will be copied in pIpArray. Return total ip.
	virtual uint32 ResolveAddress(const char* pszAddress, uint32* pIpArray, uint32 uIpCount) = 0;

	// Create a network socket. Once finished, release the socket.
	virtual ISSeSocket* CreateSocket(ESSeSocketType eType) = 0;

	virtual void GameSocketAddListener(_FnGameSocketListener pListener, void* pContext) = 0;
	virtual void GameSocketRemoveListener(_FnGameSocketListener pListener, void* pContext) = 0;
	virtual uint32 GameSocketSend(const void* pubData, uint32 cubData, uint32 uIp, uint16 uPort) = 0;
	virtual uint32 GameSocketSend(const void* pubData, uint32 cubData, TSSeSockAddr* pTargetAddr) = 0;
};
