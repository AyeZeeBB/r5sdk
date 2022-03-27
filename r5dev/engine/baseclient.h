#pragma once
#include "client/client.h"
#include "common/protocol.h"

class CBaseServer;

class CBaseClient
{
public:
	CBaseClient* GetClient(int nIndex) const;
	int32_t GetUserID(void) const;
	int64_t GetOriginID(void) const;
	SIGNONSTATE GetSignonState(void) const;
	PERSISTENCE GetPersistenceState(void) const;
	void* GetNetChan(void) const;
	void SetUserID(int32_t nUserID);
	void SetOriginID(int64_t nOriginID);
	void SetSignonState(SIGNONSTATE nSignonState);
	void SetPersistenceState(PERSISTENCE nPersistenceState);
	void SetNetChan(void* pNetChan); // !TODO: HACK!
	bool IsConnected(void) const;
	bool IsSpawned(void) const;
	bool IsActive(void) const;
	bool IsPersistenceAvailable(void) const;
	bool IsPersistenceReady(void) const;
	bool IsFakeClient(void) const;
	bool IsHumanPlayer(void) const;
	static bool Connect(CBaseClient* pClient, const char* szName, void* pNetChannel, bool bFakePlayer, void* a5, char* szMessage, int nMessageSize);
	static void* Clear(CBaseClient* pBaseClient);

private:
	// [ PIXIE ]: AMOS PLEASE VERIFY STRUCT INTEGRITY FOR EARLIER SEASONS. THERE WAS A PADDING AFTER ORIGINID BEFORE.
	char pad_0000[16]; //0x0000
	std::int32_t m_nUserID; //0x0010
	char pad_0014[844]; //0x0014
	void* m_ConVars; //0x0360
	char pad_0368[8]; //0x0368
	CBaseServer* m_Server; //0x0370
	char pad_0378[40]; //0x0378
	void* m_NetChannel; //0x03A0
	char pad_03A8[8]; //0x03A8
	SIGNONSTATE m_nSignonState; //0x03B0
	std::int32_t m_nDeltaTick; //0x03B4
	std::int64_t m_nOriginID; //0x03B8
	char pad_03C0[480]; //0x03C0
	bool m_bFakePlayer; //0x05A0
	bool m_bReceivedPacket; //0x05A1
	bool m_bLowViolence; //0x05A2
	bool m_bFullyAuthenticated; //0x05A3
	char pad_05A4[24]; //0x05A4
	PERSISTENCE m_nPersistenceState; //0x05BC
	char pad_05C0[302676]; //0x05C0
	std::int32_t m_LastMovementTick; //0x4A414
	char pad_4A418[168]; //0x4A418

};

namespace
{
	/* ==== CBASECLIENT ===================================================================================================================================================== */
	ADDRESS p_CBaseClient_Connect = g_mGameDll.FindPatternSIMD((std::uint8_t*)"\x40\x53\x41\x56\x41\x57\x48\x83\xEC\x20\x48\x8B\xD9\x48\x89\x74", "xxxxxxxxxxxxxxxx"); /*40 53 41 56 41 57 48 83 EC 20 48 8B D9 48 89 74*/
	bool (*CBaseClient_Connect)(CBaseClient* pClient, const char* szName, void* pNetChannel, bool bFakePlayer, void* a5, char* szMessage, int nMessageSize) = (bool (*)(CBaseClient*, const char*, void*, bool, void*, char*, int))p_CBaseClient_Connect.GetPtr();

	ADDRESS p_CBaseClient_Clear = g_mGameDll.FindPatternSIMD((std::uint8_t*)"\x40\x53\x41\x56\x41\x57\x48\x83\xEC\x20\x48\x8B\xD9\x48\x89\x74", "xxxxxxxxxxxxxxxx");
	void* (*CBaseClient_Clear)(CBaseClient* pClient) = (void* (*)(CBaseClient*))p_CBaseClient_Clear.GetPtr(); /*40 53 41 56 41 57 48 83 EC 20 48 8B D9 48 89 74*/
}

///////////////////////////////////////////////////////////////////////////////
void CBaseClient_Attach();
void CBaseClient_Detach();

///////////////////////////////////////////////////////////////////////////////
class HBaseClient : public IDetour
{
	virtual void debugp()
	{
		std::cout << "| FUN: CBaseClient::Connect                 : 0x" << std::hex << std::uppercase << p_CBaseClient_Connect.GetPtr() << std::setw(npad) << " |" << std::endl;
		std::cout << "| FUN: CBaseClient::Clear                   : 0x" << std::hex << std::uppercase << p_CBaseClient_Clear.GetPtr()   << std::setw(npad) << " |" << std::endl;
		std::cout << "+----------------------------------------------------------------+" << std::endl;
	}
};
///////////////////////////////////////////////////////////////////////////////

REGISTER(HBaseClient);
