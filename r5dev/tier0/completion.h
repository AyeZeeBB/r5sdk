#pragma once
#include "IConVar.h"

namespace
{
	/* ==== CONCOMMANDCALLBACK ============================================================================================================================================== */
#if defined (GAMEDLL_S1)
	ADDRESS p_Host_Map_f_CompletionFunc = g_mGameDll.FindPatternSIMD((std::uint8_t*)"\x48\x89\x5C\x24\x18\x55\x41\x56\x41\x00\x00\x00\x00\x40\x02", "xxxxxxxxx????xx");
	void (*Host_Map_f_CompletionFunc)(CCommand* cmd, char a2) = (void (*)(CCommand*, char))p_Host_Map_f_CompletionFunc.GetPtr(); /*48 89 5C 24 18 55 41 56 41 ?? ?? ?? ?? 40 02*/
#elif defined (GAMEDLL_S2) || defined (GAMEDLL_S3)
	ADDRESS p_Host_Map_f_CompletionFunc = g_mGameDll.FindPatternSIMD((std::uint8_t*)"\x40\x55\x41\x56\x41\x57\x48\x81\xEC\x00\x00\x00\x00\x83\x3D", "xxxxxxxxx????xx");
	void (*Host_Map_f_CompletionFunc)(CCommand* cmd, char a2) = (void (*)(CCommand*, char))p_Host_Map_f_CompletionFunc.GetPtr(); /*40 55 41 56 41 57 48 81 EC ?? ?? ?? ?? 83 3D*/
#endif
	ADDRESS p_DownloadPlaylists_f_CompletionFunc = g_mGameDll.FindPatternSIMD((std::uint8_t*)"\x33\xC9\xC6\x05\x00\x00\x00\x00\x00\xE9\x00\x00\x00\x00", "xxxx?????x????");
	void (*DownloadPlaylists_f_CompletionFunc)() = (void(*)())p_DownloadPlaylists_f_CompletionFunc.GetPtr(); /*33 C9 C6 05 ?? ?? ?? ?? ?? E9 ?? ?? ?? ??*/
}

///////////////////////////////////////////////////////////////////////////////
#ifndef DEDICATED
void _CGameConsole_f_CompletionFunc(const CCommand& args);
void _CCompanion_f_CompletionFunc(const CCommand& args);
#endif // !DEDICATED
void _Kick_f_CompletionFunc(const CCommand& args);
void _KickID_f_CompletionFunc(const CCommand& args);
void _Ban_f_CompletionFunc(const CCommand& args);
void _BanID_f_CompletionFunc(const CCommand& args);
void _Unban_f_CompletionFunc(const CCommand& args);
void _ReloadBanList_f_CompletionFunc(const CCommand& args);
void _Pak_ListPaks_f_CompletionFunc(const CCommand& args);
void _Pak_RequestUnload_f_CompletionFunc(const CCommand& args);
void _Pak_RequestLoad_f_CompletionFunc(const CCommand& args);
void _RTech_StringToGUID_f_CompletionFunc(const CCommand& args);
void _RTech_Decompress_f_CompletionFunc(const CCommand& args);
void _VPK_Unpack_f_CompletionFunc(const CCommand& args);
void _VPK_Mount_f_CompletionFunc(const CCommand& args);
void _NET_TraceNetChan_f_CompletionFunc(const CCommand& args);
void _NET_SetKey_f_CompletionFunc(const CCommand& args);
void _NET_GenerateKey_f_CompletionFunc(const CCommand& args);
#ifndef DEDICATED
void _RCON_CmdQuery_f_CompletionFunc(const CCommand& args);
void _RCON_Disconnect_f_CompletionFunc(const CCommand& args);
#endif // !DEDICATED
void _SQVM_ServerScript_f_CompletionFunc(const CCommand& args);
#ifndef DEDICATED
void _SQVM_ClientScript_f_CompletionFunc(const CCommand& args);
void _SQVM_UIScript_f_CompletionFunc(const CCommand& args);
void _CMaterial_GetMaterialAtCrossHair_f_ComplectionFunc(const CCommand& args);
#endif // !DEDICATED

///////////////////////////////////////////////////////////////////////////////
class HCompletion : public IDetour
{
	virtual void debugp()
	{
		std::cout << "| FUN: Host_Map_f_CompletionFunc            : 0x" << std::hex << std::uppercase << p_Host_Map_f_CompletionFunc.GetPtr()               << std::setw(npad) << " |" << std::endl;
		std::cout << "| FUN: DownloadPlaylist_f_CompletionFunc    : 0x" << std::hex << std::uppercase << p_DownloadPlaylists_f_CompletionFunc.GetPtr() << std::setw(npad) << " |" << std::endl;
		std::cout << "+----------------------------------------------------------------+" << std::endl;
	}
};
///////////////////////////////////////////////////////////////////////////////

REGISTER(HCompletion);
