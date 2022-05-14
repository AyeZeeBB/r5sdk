#pragma once
#include "tier1/NetAdr2.h"
#include "tier1/mempool.h"
#include "common/protocol.h"
#include "public/include/inetmsghandler.h"
#include "engine/datablock.h"
#include "engine/net_chan.h"
#include "engine/debugoverlay.h"
#include "engine/clockdriftmgr.h"
#include "engine/framesnapshot.h"
#include "engine/packed_entity.h"

inline bool* cl_m_bPaused = nullptr;
inline int* cl_host_tickcount = nullptr;

struct __declspec(align(8)) CClientSnapshotManager
{
	void* __vftable /*VFT*/;
	void* m_Frames;
	CUtlMemoryPool m_ClientFramePool;
};

struct IServerMessageHandler : INetMessageHandler
{};
struct CS_INetChannelHandler : INetChannelHandler
{};


///////////////////////////////////////////////////////////////////////////////
class CBaseClientState : CS_INetChannelHandler, IConnectionlessPacketHandler, IServerMessageHandler, CClientSnapshotManager
{
public:
	bool IsPaused();
	float GetClientTime();
	int GetClientTickCount() const;	// Get the client tick count.
	void SetClientTickCount(int tick); // Set the client tick count.

	int m_Socket;
	int _padding_maybe;
	CNetChan* m_NetChannel;
	double m_flConnectTime;
	_DWORD m_nRetryNumber;
	_DWORD m_nChallengeRetryLimit;
	_BYTE encrypted_connection_MAYBE;
	_BYTE gap79[3];
	v_netadr_t addr;
	bool m_bUnk_used_during_auth;
	char m_bSendChallengeRequest;
	_BYTE m_bDoNetParamsReconnect_MAYBE;
	_BYTE field_97;
	SIGNONSTATE m_nSignonState;
	_BYTE gap9C[4];
	double m_flNextCmdTime;
	int m_nServerCount;
	int field_AC;
	float m_flClockDriftUnknown_TIME;
	CClockDriftMgr m_ClockDriftMgr;
	_BYTE field_148;
	_BYTE field_149;
	int m_nDeltaTick;
	int m_nStringTableAckTick;
	int m_nProcesseedDeltaTick;
	int m_nProcessedStringTableAckTick;
	bool m_bPendingTicksAvailable;
	__declspec(align(4)) _BYTE m_bPaused;
	char field_161;
	char field_162;
	_DWORD dword164;
	int m_nViewEntity_MAYBE_ClientSlot_Plus_One;
	int m_nPlayerSlot;
	char m_szLevelFileName[64];
	char m_szLevelBaseName[64];
	char field_1F0[64];
	char field_230[64];
	_BYTE m_szServerAddresString[128];
	char buffer[16];
	int m_bInMpLobbyMenu;
	int m_nTeam;
	_DWORD m_nMaxClients;
	_BYTE m_bSignonChallengeReceived;
	_DWORD challenge;
	v_netadr_t challengeAddr;
	_BYTE byte33C;
	_QWORD m_pServerClasses;
	int m_nServerClasses;
	int m_nServerClassBits;
	__int64 m_StringTableContainer;
	char m_PersistenceData[98304];
#if defined (GAMEDLL_S0) || defined (GAMEDLL_S1)
	char pads0[8];
#endif
	_BYTE m_bPersistenceBaselineRecvd;
	__unaligned __declspec(align(4)) _QWORD m_nPersistenceBaselineEntries;
	char field_18364;
	char field_18365;
	char buffer_0x400[1024];
	NetDataBlockReceiver blockReceiver;
	char client_requested_disconnect;
	char error_message[512];
	_BYTE gap18CA1[3];
	_DWORD last_usercmd_time_from_server_maybe;
	CFrameSnapshot* prev_frame_maybe;
	_QWORD qword18CB0;
	CFrameSnapshot* current_frame_maybe;
	_BYTE gap18CC0[8];
	char IsClockCorrectionEnabled_MAYBE;
	char m_b_unknown;
	bool m_bLocalPredictionInitialized_MAYBE;
	int field_18CCC;
	int dword18CD0;
	int field_18CD4;
	float GetFrameTime;
	int outgoing_command;
	int current_movement_sequence_number;
	char gap18CE4[4];
	__int64 qword18CE8;
	int field_18CF0;
	int hit_prespawn;
	int field_18CF8;
	int dword18CFC;
	float m_flClockDriftUnknown_rounded;
	char something_with_prediction;
	char field_18D05;
	char gap18D06[2];
	int dword18D08;
	char gap18D0C[13];
	char do_local_prediction_update;
	char gap18D1A[2];
	int dword18D1C;
	__int64 qword18D20;
	int dword18D28;
	int dword18D2C;
	float field_18D30;
	float m_flUnk1;
	float m_flUnk2;
	int dword18D3C;
	int dword18D40;
	char gap18D44[4];
	__int64 qword18D48;
	__int64 qword18D50;
	__int64 qword18D58;
	int dword18D60;
	char gap18D64[4];
	__int64 qword18D68;
	char gap18D70[8];
	char buffer_47128[47128];
	char entitlements_bitfield[16];
	__int64 maybe_some_ll_stuff;
	__int64 qword245A8;
	__int64 qword245B0;
	__int64 qword245B8;
	__int64 qword245C0;
	__int64 qword245C8;
	__int64 qword245D0;
	__int64 qword245D8;
	__int64 qword245E0;
	__int64 qword245E8;
	__int64 qword245F0;
	int dword245F8;
	char gap245FC[1024];
	__unaligned __declspec(align(1)) __int64 qword249FC;
	char gap24A04[4];
	__int64 m_pModelPrecacheTable;
	__int64 qword24A10;
	__int64 m_pInstanceBaselineTable;
	__int64 m_pLightStyleTable;
	__int64 m_pUserInfoTable;
	__int64 m_pServerStartupTable;
	PackedEntity m_pEntityBaselines_maybe[4096];
	char byte34A38;
	char field_34A39[7];
};
extern CBaseClientState* g_pBaseClientState;

/* ==== CCLIENTSTATE ==================================================================================================================================================== */
#if defined (GAMEDLL_S0) || defined (GAMEDLL_S1)
//inline CMemory p_CClientState__CheckForResend = g_mGameDll.FindPatternSIMD(reinterpret_cast<rsig_t>("\x48\x89\x5C\x24\x00\x56\x57\x41\x57\x00\x81\xEC\x20\x04\x00\x00\x45\x0F\xB6\xF9\x00\x00\x00\x00\x8B\xF1\x48"), "xxxx?xxxx?xxxx?xxxxx????xxx"); /*48 89 5C 24 ?? 56 57 41 57 ?? 81 EC 20 04 ?? 00 45 0F B6 F9 ?? ?? ?? ?? 8B F1 48*/
//inline auto CClientState__CheckForResend = p_CClientState__CheckForResend.RCast<void(*)(CBaseClientState* thisptr, const char* a2, std::int64_t a3, char a4, int a5, std::uint8_t* a6)>();
#elif defined (GAMEDLL_S2)
//inline CMemory p_CClientState__CheckForResend = g_mGameDll.FindPatternSIMD(reinterpret_cast<rsig_t>("\x48\x89\x5C\x24\x00\x48\x89\x74\x24\x00\x48\x89\x7C\x24\x00\x41\x56\x48\x81\xEC\x00\x00\x00\x00\x45\x0F\xB6"), "xxxx?xxxx?xxxx?xxxxx????xxx"); /*48 89 5C 24 ?? 48 89 74 24 ?? 48 89 7C 24 ?? 41 56 48 81 EC ?? ?? ?? ?? 45 0F B6*/
//inline auto CClientState__CheckForResend = p_CClientState__CheckForResend.RCast<void(*)(CBaseClientState* thisptr, const char* a2, std::int64_t a3, char a4, int a5, std::uint8_t* a6)>();
#elif defined (GAMEDLL_S3)
//inline CMemory p_CClientState__CheckForResend = g_mGameDll.FindPatternSIMD(reinterpret_cast<rsig_t>("\x48\x89\x5C\x24\x00\x48\x89\x74\x24\x00\x48\x89\x7C\x24\x00\x41\x56\x48\x81\xEC\x00\x00\x00\x00\x48\x8B\x32"), "xxxx?xxxx?xxxx?xxxxx????xxx"); /*48 89 5C 24 ?? 48 89 74 24 ?? 48 89 7C 24 ?? 41 56 48 81 EC ?? ?? ?? ?? 48 8B 32*/
//inline auto CClientState__CheckForResend = p_CClientState__CheckForResend.RCast<void(*)(CBaseClientState* thisptr, const char* a2, std::int64_t a3, char a4, int a5, std::uint8_t* a6)>();
#endif

///////////////////////////////////////////////////////////////////////////////
class VBaseClientState : public IDetour
{
	virtual void GetAdr(void) const
	{
		//spdlog::debug("| FUN: CClientState::CheckForResend         : {:#18x} |\n", p_CClientState__CheckForResend.GetPtr());
		spdlog::debug("| VAR: cl_m_bPaused                         : {:#18x} |\n", reinterpret_cast<uintptr_t>(cl_m_bPaused));
		spdlog::debug("| VAR: cl_host_tickcount                    : {:#18x} |\n", reinterpret_cast<uintptr_t>(cl_host_tickcount));
		spdlog::debug("+----------------------------------------------------------------+\n");
	}
	virtual void GetFun(void) const { }
	virtual void GetVar(void) const
	{

#if defined (GAMEDLL_S0) || defined (GAMEDLL_S1)
		CMemory localRef = g_mGameDll.FindPatternSIMD(reinterpret_cast<rsig_t>(
			"\x40\x55\x48\x83\xEC\x50\x48\x8B\x05\x00\x00\x00\x00"), "xxxxxxxxx????");

		cl_m_bPaused = localRef.Offset(0x90)
			.FindPatternSelf("80 3D ? ? ? 0B ?", CMemory::Direction::DOWN, 150).ResolveRelativeAddressSelf(0x2, 0x2).RCast<bool*>();
		cl_host_tickcount = localRef.Offset(0xC0)
			.FindPatternSelf("66 0F 6E", CMemory::Direction::DOWN, 150).ResolveRelativeAddressSelf(0x4, 0x8).RCast<int*>();
#elif defined (GAMEDLL_S2) || defined (GAMEDLL_S3)

		CMemory localRef = g_mGameDll.FindPatternSIMD(reinterpret_cast<rsig_t>(
			"\x40\x55\x48\x83\xEC\x30\x48\x8B\x05\x00\x00\x00\x00\x0F\xB6\xE9"), "xxxxxxxxx????xxx");

		cl_m_bPaused = localRef.Offset(0x70)
			.FindPatternSelf("80 3D ? ? ? 01 ?", CMemory::Direction::DOWN, 150).ResolveRelativeAddressSelf(0x2, 0x7).RCast<bool*>();
		cl_host_tickcount = localRef.Offset(0xC0)
			.FindPatternSelf("66 0F 6E", CMemory::Direction::DOWN, 150).ResolveRelativeAddressSelf(0x4, 0x8).RCast<int*>();
#endif
	}
	virtual void GetCon(void) const
	{
#ifndef DEDICATED
		g_pBaseClientState = g_mGameDll.FindPatternSIMD(reinterpret_cast<rsig_t>("\x0F\x84\x00\x00\x00\x00\x48\x8D\x0D\x00\x00\x00\x00\x48\x83\xC4\x28"), "xx????xxx????xxxx").FindPatternSelf("48 8D").ResolveRelativeAddressSelf(0x3, 0x7).RCast<CBaseClientState*>(); /*0F 84 ? ? ? ? 48 8D 0D ? ? ? ? 48 83 C4 28*/
#endif
	}
	virtual void Attach(void) const { }
	virtual void Detach(void) const { }
};
///////////////////////////////////////////////////////////////////////////////

REGISTER(VBaseClientState);
