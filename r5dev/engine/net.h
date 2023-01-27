#pragma once

#ifndef NETCONSOLE
#include "engine/net_chan.h"
#define MAX_STREAMS         2
#define FRAGMENT_BITS       8
#define FRAGMENT_SIZE       (1<<FRAGMENT_BITS)

// user message
#define MAX_USER_MSG_DATA 511 // <-- 255 in Valve Source.

#define NETMSG_TYPE_BITS	8	// must be 2^NETMSG_TYPE_BITS > SVC_LASTMSG (6 in Valve Source).
#define NETMSG_LENGTH_BITS	12	// 512 bytes (11 in Valve Source, 256 bytes).
#define NET_MIN_MESSAGE 5 // Even connectionless packets require int32 value (-1) + 1 byte content

constexpr unsigned int AES_128_KEY_SIZE = 16;
constexpr unsigned int AES_128_B64_ENCODED_SIZE = 24;
constexpr const char* DEFAULT_NET_ENCRYPTION_KEY = "WDNWLmJYQ2ZlM0VoTid3Yg==";

/* ==== CNETCHAN ======================================================================================================================================================== */
inline CMemory p_NET_Init;
inline auto v_NET_Init = p_NET_Init.RCast<void* (*)(bool bDeveloper)>();

inline CMemory p_NET_Shutdown;
inline auto v_NET_Shutdown = p_NET_Shutdown.RCast<void (*)(void* thisptr, const char* szReason, uint8_t bBadRep, bool bRemoveNow)>();

inline CMemory p_NET_SetKey;
inline auto v_NET_SetKey = p_NET_SetKey.RCast<void (*)(uintptr_t pKey, const char* szHash)>();

inline CMemory p_NET_ReceiveDatagram;
inline auto v_NET_ReceiveDatagram = p_NET_ReceiveDatagram.RCast<bool (*)(int iSocket, netpacket_s* pInpacket, bool bRaw)>();

inline CMemory p_NET_SendDatagram;
inline auto v_NET_SendDatagram = p_NET_SendDatagram.RCast<int (*)(SOCKET s, void* pPayload, int iLenght, v_netadr_t* pAdr, bool bEncrypted)>();

inline CMemory p_NET_PrintFunc;
inline auto v_NET_PrintFunc = p_NET_PrintFunc.RCast<void(*)(const char* fmt)>();

///////////////////////////////////////////////////////////////////////////////
bool NET_ReceiveDatagram(int iSocket, netpacket_s* pInpacket, bool bRaw);
int  NET_SendDatagram(SOCKET s, void* pPayload, int iLenght, v_netadr_t* pAdr, bool bEncrypted);
void NET_SetKey(string svNetKey);
void NET_GenerateKey();
void NET_PrintFunc(const char* fmt, ...);
void NET_Shutdown(void* thisptr, const char* szReason, uint8_t bBadRep, bool bRemoveNow);
void NET_RemoveChannel(CClient* pClient, int nIndex, const char* szReason, uint8_t bBadRep, bool bRemoveNow);

///////////////////////////////////////////////////////////////////////////////
extern string g_svNetKey;
extern uintptr_t g_pNetKey;
inline std::mutex g_NetKeyMutex;

///////////////////////////////////////////////////////////////////////////////
class VNet : public IDetour
{
	virtual void GetAdr(void) const
	{
		LogFunAdr("NET_Init", p_NET_Init.GetPtr());
		LogFunAdr("NET_Shutdown", p_NET_Shutdown.GetPtr());
		LogFunAdr("NET_SetKey", p_NET_SetKey.GetPtr());
		LogFunAdr("NET_ReceiveDatagram", p_NET_ReceiveDatagram.GetPtr());
		LogFunAdr("NET_SendDatagram", p_NET_SendDatagram.GetPtr());
		LogFunAdr("NET_PrintFunc", p_NET_PrintFunc.GetPtr());
		LogVarAdr("g_pNetKey", g_pNetKey);
	}
	virtual void GetFun(void) const
	{
#if defined (GAMEDLL_S0) || defined (GAMEDLL_S1) || defined (GAMEDLL_S2)
		p_NET_Init     = g_GameDll.FindPatternSIMD("48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 48 89 7C 24 20 41 54 41 56 41 57 48 81 EC C0 01 ??");
		p_NET_Shutdown = g_GameDll.FindPatternSIMD("48 89 6C 24 18 56 57 41 56 48 83 EC 30 83 B9 D8");
#elif defined (GAMEDLL_S3)
		p_NET_Init     = g_GameDll.FindPatternSIMD("48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 48 89 7C 24 20 41 54 41 56 41 57 48 81 EC F0 01 ??");
		p_NET_Shutdown = g_GameDll.FindPatternSIMD("48 89 6C 24 18 56 57 41 56 48 83 EC 30 83 B9 D0");
#endif
		p_NET_SetKey          = g_GameDll.FindPatternSIMD("48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 48 83 EC 20 48 8B F9 41 B8");
		p_NET_ReceiveDatagram = g_GameDll.FindPatternSIMD("48 89 74 24 18 48 89 7C 24 20 55 41 54 41 55 41 56 41 57 48 8D AC 24 50 EB");
		p_NET_SendDatagram    = g_GameDll.FindPatternSIMD("48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 41 56 41 57 48 81 EC ?? 05 ?? ??");
		p_NET_PrintFunc       = g_GameDll.FindPatternSIMD("48 89 54 24 10 4C 89 44 24 18 4C 89 4C 24 20 C3 48");

		v_NET_Init            = p_NET_Init.RCast<void* (*)(bool)>();                                        /*48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 48 89 7C 24 20 41 54 41 56 41 57 48 81 EC F0 01 00*/
		v_NET_Shutdown        = p_NET_Shutdown.RCast<void (*)(void*, const char*, uint8_t, bool)>();        /*48 89 6C 24 18 56 57 41 56 48 83 EC 30 83 B9 D0*/
		v_NET_SetKey          = p_NET_SetKey.RCast<void (*)(uintptr_t, const char*)>();                     /*48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 48 83 EC 20 48 8B F9 41 B8*/
		v_NET_ReceiveDatagram = p_NET_ReceiveDatagram.RCast<bool (*)(int, netpacket_s*, bool)>();           /*E8 ?? ?? ?? ?? 84 C0 75 35 48 8B D3*/
		v_NET_SendDatagram    = p_NET_SendDatagram.RCast<int (*)(SOCKET, void*, int, v_netadr_t*, bool)>(); /*48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 41 56 41 57 48 81 EC ?? 05 00 00*/
		v_NET_PrintFunc       = p_NET_PrintFunc.RCast<void(*)(const char*)>();                              /*48 89 54 24 10 4C 89 44 24 18 4C 89 4C 24 20 C3 48*/

	}
	virtual void GetVar(void) const
	{
		g_pNetKey = g_GameDll.FindString("client:NetEncryption_NewKey").FindPatternSelf("48 8D ?? ?? ?? ?? ?? 48 3B", CMemory::Direction::UP, 300).ResolveRelativeAddressSelf(0x3, 0x7).GetPtr();
	}
	virtual void GetCon(void) const { }
	virtual void Attach(void) const;
	virtual void Detach(void) const;
};
///////////////////////////////////////////////////////////////////////////////
#endif // !NETCONSOLE

const char* NET_ErrorString(int iCode);
