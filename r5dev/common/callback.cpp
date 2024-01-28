//=============================================================================//
//
// Purpose: Callback functions for ConVar's.
//
//=============================================================================//

#include "core/stdafx.h"
#include "core/init.h"
#include "windows/id3dx.h"
#include "tier0/fasttimer.h"
#include "tier1/cvar.h"
#ifndef CLIENT_DLL
#include "engine/server/sv_rcon.h"
#endif // !CLIENT_DLL
#ifndef DEDICATED
#include "engine/client/cl_rcon.h"
#include "engine/client/cdll_engine_int.h"
#include "engine/client/clientstate.h"
#endif // !DEDICATED
#include "engine/client/client.h"
#include "engine/net.h"
#include "engine/host_cmd.h"
#include "engine/host_state.h"
#include "engine/enginetrace.h"
#ifndef CLIENT_DLL
#include "engine/server/server.h"
#endif // !CLIENT_DLL

#include "rtech/pak/pakencode.h"
#include "rtech/pak/pakdecode.h"
#include "rtech/pak/pakparse.h"
#include "rtech/pak/pakstate.h"
#include "rtech/pak/paktools.h"

#include "rtech/playlists/playlists.h"

#include "filesystem/basefilesystem.h"
#include "filesystem/filesystem.h"
#include "vpklib/packedstore.h"
#include "vscript/vscript.h"
#include "localize/localize.h"
#include "ebisusdk/EbisuSDK.h"
#ifndef DEDICATED
#include "geforce/reflex.h"
#include "gameui/IBrowser.h"
#include "gameui/IConsole.h"
#endif // !DEDICATED
#ifndef CLIENT_DLL
#include "networksystem/bansystem.h"
#endif // !CLIENT_DLL
#include "public/edict.h"
#include "public/worldsize.h"
#include "mathlib/crc32.h"
#include "mathlib/mathlib.h"
#include "common/completion.h"
#include "common/callback.h"
#ifndef DEDICATED
#include "materialsystem/cmaterialglue.h"
#endif // !DEDICATED
#include "public/bspflags.h"
#include "public/cmodel.h"
#include "public/idebugoverlay.h"
#include "public/localize/ilocalize.h"
#ifndef CLIENT_DLL
#include "game/server/detour_impl.h"
#include "game/server/gameinterface.h"
#endif // !CLIENT_DLL
#ifndef DEDICATED
#include "game/client/cliententitylist.h"
#include "game/client/viewrender.h"
#endif // !DEDICATED


/*
=====================
MP_GameMode_Changed_f
=====================
*/
void MP_GameMode_Changed_f(IConVar* pConVar, const char* pOldString, float flOldValue)
{
	v_SetupGamemode(mp_gamemode->GetString());
}

/*
=====================
MP_HostName_Changed_f
=====================
*/
void MP_HostName_Changed_f(IConVar* pConVar, const char* pOldString, float flOldValue)
{
#ifndef DEDICATED
	g_Browser.SetHostName(pylon_matchmaking_hostname->GetString());
#endif // !DEDICATED
}

#ifndef DEDICATED
/*
=====================
ToggleConsole_f
=====================
*/
void ToggleConsole_f(const CCommand& args)
{
	g_Console.m_bActivate ^= true;
	ResetInput(); // Disable input to game when console is drawn.
}

/*
=====================
ToggleBrowser_f
=====================
*/
void ToggleBrowser_f(const CCommand& args)
{
	g_Browser.m_bActivate ^= true;
	ResetInput(); // Disable input to game when browser is drawn.
}
#endif // !DEDICATED
#ifndef CLIENT_DLL
/*
=====================
Host_Kick_f

  helper function for
  bansystem
=====================
*/
void _Author_Client_f(const CCommand& args, EKickType type)
{
	if (args.ArgC() < 2)
	{
		return;
	}

	const char* szReason = args.ArgC() > 2 ? args.Arg(2) : nullptr;

	switch(type)
	{
		case KICK_NAME:
		{
			g_BanSystem.KickPlayerByName(args.Arg(1), szReason);
			break;
		}
		case KICK_ID:
		{
			g_BanSystem.KickPlayerById(args.Arg(1), szReason);
			break;
		}
		case BAN_NAME:
		{
			g_BanSystem.BanPlayerByName(args.Arg(1), szReason);
			break;
		}
		case BAN_ID:
		{
			g_BanSystem.BanPlayerById(args.Arg(1), szReason);
			break;
		}
		default:
		{
			// Code bug.
			Assert(0);
		}
	}
}


/*
=====================
Host_Kick_f
=====================
*/
void Host_Kick_f(const CCommand& args)
{
	_Author_Client_f(args, EKickType::KICK_NAME);
}

/*
=====================
Host_KickID_f
=====================
*/
void Host_KickID_f(const CCommand& args)
{
	_Author_Client_f(args, EKickType::KICK_ID);
}

/*
=====================
Host_Ban_f
=====================
*/
void Host_Ban_f(const CCommand& args)
{
	_Author_Client_f(args, EKickType::BAN_NAME);
}

/*
=====================
Host_BanID_f
=====================
*/
void Host_BanID_f(const CCommand& args)
{
	_Author_Client_f(args, EKickType::BAN_ID);
}

/*
=====================
Host_Unban_f
=====================
*/
void Host_Unban_f(const CCommand& args)
{
	if (args.ArgC() < 2)
	{
		return;
	}

	g_BanSystem.UnbanPlayer(args.Arg(1));
}

/*
=====================
Host_ReloadBanList_f
=====================
*/
void Host_ReloadBanList_f(const CCommand& args)
{
	g_BanSystem.LoadList(); // Reload banned list.
}

/*
=====================
Host_ReloadPlaylists_f
=====================
*/
void Host_ReloadPlaylists_f(const CCommand& args)
{
	v__DownloadPlaylists_f();
	Playlists_SDKInit(); // Re-Init playlist.
}

/*
=====================
Host_Changelevel_f

  Goes to a new map, 
  taking all clients along
=====================
*/
void Host_Changelevel_f(const CCommand& args)
{
	const int argCount = args.ArgC();

	if (argCount >= 2
		&& IsOriginInitialized()
		&& g_pServer->IsActive())
	{
		const char* levelName = args[1];
		const char* landMarkName = argCount > 2 ? args[2] : "";

		v_SetLaunchOptions(args);
		v_HostState_ChangeLevelMP(levelName, landMarkName);
	}
}

/*
=====================
Detour_HotSwap_f

  Hot swaps the NavMesh
  while the game is running
=====================
*/
void Detour_HotSwap_f(const CCommand& args)
{
	if (!g_pServer->IsActive())
		return; // Only execute if server is initialized and active.

	Msg(eDLL_T::SERVER, "Executing NavMesh hot swap for level '%s'\n",
		g_ServerGlobalVariables->m_pszMapName);

	CFastTimer timer;

	timer.Start();
	Detour_HotSwap();

	timer.End();
	Msg(eDLL_T::SERVER, "Hot swap took '%lf' seconds\n", timer.GetDuration().GetSeconds());
}
#endif // !CLIENT_DLL
/*
=====================
Pak_ListPaks_f
=====================
*/
void Pak_ListPaks_f(const CCommand& args)
{
	Msg(eDLL_T::RTECH, "| id   | name                                               | status                               | asset count |\n");
	Msg(eDLL_T::RTECH, "|------|----------------------------------------------------|--------------------------------------|-------------|\n");

	uint32_t nTotalLoaded = 0;

	for (int16_t i = 0, n = *g_pLoadedPakCount; i < n; ++i)
	{
		const PakLoadedInfo_t& info = g_pLoadedPakInfo[i];

		if (info.status == EPakStatus::PAK_STATUS_FREED)
			continue;

		const char* szRpakStatus = Pak_StatusToString(info.status);

		// todo: make status into a string from an array/vector
		Msg(eDLL_T::RTECH, "| %04i | %-50s | %-36s | %11i |\n", info.handle, info.fileName, szRpakStatus, info.assetCount);
		nTotalLoaded++;
	}
	Msg(eDLL_T::RTECH, "|------|----------------------------------------------------|--------------------------------------|-------------|\n");
	Msg(eDLL_T::RTECH, "| %18i loaded paks.                                                                                |\n", nTotalLoaded);
	Msg(eDLL_T::RTECH, "|------|----------------------------------------------------|--------------------------------------|-------------|\n");
}

/*
=====================
Pak_ListTypes_f
=====================
*/
void Pak_ListTypes_f(const CCommand& args)
{
	Msg(eDLL_T::RTECH, "| ext  | description               | version | header size | native size |\n");
	Msg(eDLL_T::RTECH, "|------|---------------------------|---------|-------------|-------------|\n");

	uint32_t nRegistered = 0;

	for (int8_t i = 0; i < PAK_MAX_TYPES; ++i)
	{
		PakAssetBinding_t* type = &g_pPakGlobals->m_assetBindings[i];

		if (!type->description)
			continue;

		Msg(eDLL_T::RTECH, "| %-4s | %-25s | %7i | %11i | %11i |\n", FourCCToString(type->extension).c_str(), type->description, type->version, type->headerSize, type->nativeClassSize);
		nRegistered++;
	}
	Msg(eDLL_T::RTECH, "|------|---------------------------|---------|-------------|-------------|\n");
	Msg(eDLL_T::RTECH, "| %18i registered types.                                   |\n", nRegistered);
	Msg(eDLL_T::RTECH, "|------|---------------------------|---------|-------------|-------------|\n");
}

/*
=====================
Pak_RequestUnload_f
=====================
*/
void Pak_RequestUnload_f(const CCommand& args)
{
	if (args.ArgC() < 2)
	{
		return;
	}

	if (args.HasOnlyDigits(1))
	{
		const PakHandle_t pakHandle = atoi(args.Arg(1));
		const PakLoadedInfo_t* const pakInfo = Pak_GetPakInfo(pakHandle);

		if (!pakInfo)
		{
			Warning(eDLL_T::RTECH, "Found no pak entry for specified handle.\n");
			return;
		}

		Msg(eDLL_T::RTECH, "Requested pak unload for handle '%d'\n", pakHandle);
		g_pakLoadApi->UnloadAsync(pakHandle);
	}
	else
	{
		const PakLoadedInfo_t* const pakInfo = Pak_GetPakInfo(args.Arg(1));
		if (!pakInfo)
		{
			Warning(eDLL_T::RTECH, "Found no pak entry for specified name.\n");
			return;
		}

		Msg(eDLL_T::RTECH, "Requested pak unload for file '%s'\n", args.Arg(1));
		g_pakLoadApi->UnloadAsync(pakInfo->handle);
	}
}

/*
=====================
Pak_RequestLoad_f
=====================
*/
void Pak_RequestLoad_f(const CCommand& args)
{
	g_pakLoadApi->LoadAsync(args.Arg(1), AlignedMemAlloc(), NULL, 0);
}


/*
=====================
Pak_Swap_f
=====================
*/
void Pak_Swap_f(const CCommand& args)
{
	if (args.ArgC() < 2)
	{
		return;
	}

	const char* pakName = nullptr;

	PakHandle_t pakHandle = INVALID_PAK_HANDLE;
	const PakLoadedInfo_t* pakInfo = nullptr;

	if (args.HasOnlyDigits(1))
	{
		pakHandle = atoi(args.Arg(1));
		pakInfo = Pak_GetPakInfo(pakHandle);

		if (!pakInfo)
		{
			Warning(eDLL_T::RTECH, "Found no pak entry for specified handle.\n");
			return;
		}

		pakName = pakInfo->fileName;
	}
	else
	{
		pakName = args.Arg(1);
		pakInfo = Pak_GetPakInfo(pakName);

		if (!pakInfo)
		{
			Warning(eDLL_T::RTECH, "Found no pak entry for specified name.\n");
			return;
		}

		pakHandle = pakInfo->handle;
	}

	Msg(eDLL_T::RTECH, "Requested pak swap for handle '%d'\n", pakHandle);
	g_pakLoadApi->UnloadAsync(pakHandle);

	while (pakInfo->status != EPakStatus::PAK_STATUS_FREED) // Wait till this slot gets free'd.
		std::this_thread::sleep_for(std::chrono::seconds(1));

	g_pakLoadApi->LoadAsync(pakName, AlignedMemAlloc(), NULL, 0);
}

/*
=====================
RTech_StringToGUID_f
=====================
*/
void Pak_StringToGUID_f(const CCommand& args)
{
	if (args.ArgC() < 2)
	{
		return;
	}

	unsigned long long guid = Pak_StringToGuid(args.Arg(1));

	Msg(eDLL_T::RTECH, "______________________________________________________________\n");
	Msg(eDLL_T::RTECH, "] RTECH_HASH ]------------------------------------------------\n");
	Msg(eDLL_T::RTECH, "] GUID: '0x%llX'\n", guid);
}

/*
=====================
RTech_Decompress_f

  Decompresses input RPak file and
  dumps results to override path
=====================
*/
void Pak_Decompress_f(const CCommand& args)
{
	if (args.ArgC() < 2)
	{
		return;
	}

	CUtlString inPakFile;
	CUtlString outPakFile;

	inPakFile.Format(PLATFORM_PAK_PATH "%s", args.Arg(1));
	outPakFile.Format(PLATFORM_PAK_OVERRIDE_PATH "%s", args.Arg(1));

	Msg(eDLL_T::RTECH, "______________________________________________________________\n");
	Msg(eDLL_T::RTECH, "-+ RTech decompress ------------------------------------------\n");

	if (!FileSystem()->FileExists(inPakFile.String(), "GAME"))
	{
		Error(eDLL_T::RTECH, NO_ERROR, "%s - pak file '%s' does not exist!\n",
			__FUNCTION__, inPakFile.String());
		return;
	}

	Msg(eDLL_T::RTECH, " |-+ Processing: '%s'\n", inPakFile.String());
	FileHandle_t hPakFile = FileSystem()->Open(inPakFile.String(), "rb", "GAME");

	if (!hPakFile)
	{
		Error(eDLL_T::RTECH, NO_ERROR, "%s - Unable to open '%s' (insufficient rights?)\n",
			__FUNCTION__, inPakFile.String());
		return;
	}

	const ssize_t nFileSize = FileSystem()->Size(hPakFile);

	std::unique_ptr<uint8_t[]> pPakBufContainer(new uint8_t[nFileSize]);
	uint8_t* const pPakBuf = pPakBufContainer.get();

	FileSystem()->Read(pPakBuf, nFileSize, hPakFile);
	FileSystem()->Close(hPakFile);

	PakFileHeader_t* pHeader = reinterpret_cast<PakFileHeader_t*>(pPakBuf);

	SYSTEMTIME systemTime;
	FileTimeToSystemTime(&pHeader->fileTime, &systemTime);

	Msg(eDLL_T::RTECH, " | |-+ Header ------------------------------------------------\n");
	Msg(eDLL_T::RTECH, " |   |-- Magic    : '0x%08X'\n", pHeader->magic);
	Msg(eDLL_T::RTECH, " |   |-- Version  : '%hu'\n", pHeader->version);
	Msg(eDLL_T::RTECH, " |   |-- Flags    : '0x%04hX'\n", pHeader->flags);
	Msg(eDLL_T::RTECH, " |   |-- Time     : '%hu-%hu-%hu/%hu %hu:%hu:%hu.%hu'\n",
		systemTime.wYear, systemTime.wMonth, systemTime.wDay, systemTime.wDayOfWeek,
		systemTime.wHour, systemTime.wMinute, systemTime.wSecond, systemTime.wMilliseconds);
	Msg(eDLL_T::RTECH, " |   |-- Hash     : '0x%08llX'\n", pHeader->checksum);
	Msg(eDLL_T::RTECH, " |   |-- Entries  : '%u'\n", pHeader->assetCount);
	Msg(eDLL_T::RTECH, " |   |-+ Compression -----------------------------------------\n");
	Msg(eDLL_T::RTECH, " |     |-- Size comp: '%zu'\n", pHeader->compressedSize);
	Msg(eDLL_T::RTECH, " |     |-- Size decp: '%zu'\n", pHeader->decompressedSize);

	if (pHeader->magic != PAK_HEADER_MAGIC)
	{
		Error(eDLL_T::RTECH, NO_ERROR, "%s - pak file '%s' has invalid magic!\n",
			__FUNCTION__, inPakFile.String());

		return;
	}
	if (!(pHeader->flags & PAK_HEADER_FLAGS_COMPRESSED))
	{
		Error(eDLL_T::RTECH, NO_ERROR, "%s - pak file '%s' already decompressed!\n",
			__FUNCTION__, inPakFile.String());

		return;
	}

	if (pHeader->compressedSize != size_t(nFileSize))
	{
		Error(eDLL_T::RTECH, NO_ERROR, "%s - pak file '%s' decompressed size '%zu' doesn't match expected size '%zu'!\n",
			__FUNCTION__, inPakFile.String(), size_t(nFileSize), pHeader->compressedSize);

		return;
	}

	const bool usesCustomCompression = pHeader->flags & PAK_HEADER_FLAGS_ZSTREAM;

	PakDecoder_t decoder{};
	const uint64_t nDecompSize = Pak_InitDecoder(&decoder, pPakBuf, nullptr, UINT64_MAX, UINT64_MAX, nFileSize, NULL, sizeof(PakFileHeader_t), usesCustomCompression);

	if (nDecompSize != pHeader->decompressedSize)
	{
		Error(eDLL_T::RTECH, NO_ERROR, "%s - calculated size: '%llu' expected: '%llu'!\n",
			__FUNCTION__, nDecompSize, pHeader->decompressedSize);

		return;
	}
	else
	{
		Msg(eDLL_T::RTECH, " |     |-- Size calc: '%llu'\n", nDecompSize);
	}

	Msg(eDLL_T::RTECH, " |     |-- Ratio    : '%.02f'\n", (pHeader->compressedSize * 100.f) / pHeader->decompressedSize);


	std::unique_ptr<uint8_t[]> pDecompBufContainer(new uint8_t[pHeader->decompressedSize]);
	uint8_t* const pDecompBuf = pDecompBufContainer.get();

	// Needs full decode mask as we don't decompress in chunks.
	decoder.outputMask = UINT64_MAX;
	decoder.outputBuf = pDecompBuf;

	uint8_t nDecompResult = Pak_StreamToBufferDecode(&decoder, pHeader->compressedSize, pHeader->decompressedSize, usesCustomCompression);
	if (nDecompResult != 1)
	{
		Error(eDLL_T::RTECH, NO_ERROR, "%s - decompression failed for '%s' return value: '%hu'!\n",
			__FUNCTION__, inPakFile.String(), nDecompResult);
	}

	pHeader->flags &= ~PAK_HEADER_FLAGS_COMPRESSED; // Remove compressed flags.
	pHeader->compressedSize = pHeader->decompressedSize; // Equal compressed size with decompressed.

	FileSystem()->CreateDirHierarchy(PLATFORM_PAK_OVERRIDE_PATH, "GAME");
	FileHandle_t hDecompFile = FileSystem()->Open(outPakFile.String(), "wb", "GAME");

	if (!hDecompFile)
	{
		Error(eDLL_T::RTECH, NO_ERROR, "%s - Unable to write to '%s' (read-only?)\n",
			__FUNCTION__, outPakFile.String());

		return;
	}

	if (pHeader->patchIndex > 0) // Check if its an patch rpak.
	{
		// Loop through all the structs and patch their compress size.
		for (uint16_t i = 1, nPatchOffset = (sizeof(PakFileHeader_t) + sizeof(uint64_t));
			i <= pHeader->patchIndex; i++, nPatchOffset += sizeof(PakPatchFileHeader_t))
		{
			PakPatchFileHeader_t* pPatchHeader = reinterpret_cast<PakPatchFileHeader_t*>(pDecompBuf + nPatchOffset);
			Msg(eDLL_T::RTECH, " |     |-+ Patch #%02hu -----------------------------------------\n", i);
			Msg(eDLL_T::RTECH, " |     %s |-- Size comp: '%llu'\n", i < pHeader->patchIndex ? "|" : " ", pPatchHeader->m_sizeDisk);
			Msg(eDLL_T::RTECH, " |     %s |-- Size decp: '%llu'\n", i < pHeader->patchIndex ? "|" : " ", pPatchHeader->m_sizeMemory);

			pPatchHeader->m_sizeDisk = pPatchHeader->m_sizeMemory; // Fix size for decompress.
		}
	}

	memcpy_s(pDecompBuf, sizeof(PakFileHeader_t), pPakBuf, sizeof(PakFileHeader_t));// Overwrite first 0x80 bytes which are NULL with the header data.
	FileSystem()->Write(pDecompBuf, decoder.decompSize, hDecompFile);

	Msg(eDLL_T::RTECH, " |-- Checksum : '0x%08X'\n", crc32::update(NULL, pDecompBuf, decoder.decompSize));
	Msg(eDLL_T::RTECH, "-+ Decompressed pak file to: '%s'\n", outPakFile.String());
	Msg(eDLL_T::RTECH, "--------------------------------------------------------------\n");

	FileSystem()->Close(hDecompFile);
}

/*
=====================
RTech_Compress_f

  Compresses input RPak file and
  dumps results to base path
=====================
*/
void Pak_Compress_f(const CCommand& args)
{
	if (args.ArgC() < 2)
	{
		return;
	}

	CUtlString inPakFile;
	CUtlString outPakFile;

	inPakFile.Format(PLATFORM_PAK_OVERRIDE_PATH "%s", args.Arg(1));
	outPakFile.Format(PLATFORM_PAK_PATH "%s", args.Arg(1));

	// NULL means default compress level
	const int compressLevel = args.ArgC() > 2 ? atoi(args.Arg(2)) : NULL;

	if (!Pak_EncodePakFile(inPakFile.String(), outPakFile.String(), compressLevel))
	{
		Error(eDLL_T::RTECH, NO_ERROR, "%s - compression failed for '%s'!\n",
			__FUNCTION__, inPakFile.String());
	}
}

/*
=====================
VPK_Pack_f

  Packs VPK files into
  'PLATFORM' VPK directory.
=====================
*/
void VPK_Pack_f(const CCommand& args)
{
	if (args.ArgC() < 4)
	{
		return;
	}

	const char* workspacePath = fs_packedstore_workspace->GetString();

	if (!FileSystem()->IsDirectory(workspacePath, "PLATFORM"))
	{
		Error(eDLL_T::FS, NO_ERROR, "Workspace path \"%s\" doesn't exist!\n", workspacePath);
		return;
	}

	VPKPair_t pair(args.Arg(1), args.Arg(2), args.Arg(3), NULL);
	Msg(eDLL_T::FS, "*** Starting VPK build command for: '%s'\n", pair.m_DirName.String());

	CFastTimer timer;
	timer.Start();

	CPackedStoreBuilder builder;

	builder.InitLzEncoder(fs_packedstore_max_helper_threads->GetInt(), fs_packedstore_compression_level->GetString());
	builder.PackStore(pair, workspacePath, "vpk/");

	timer.End();
	Msg(eDLL_T::FS, "*** Time elapsed: '%lf' seconds\n", timer.GetDuration().GetSeconds());
	Msg(eDLL_T::FS, "\n");
}

/*
=====================
VPK_Unpack_f

  Unpacks VPK files into
  workspace directory.
=====================
*/
void VPK_Unpack_f(const CCommand& args)
{
	if (args.ArgC() < 2)
	{
		return;
	}

	CUtlString fileName = args.Arg(1);
	VPKDir_t vpk(fileName, (args.ArgC() > 2));

	if (vpk.Failed())
	{
		Error(eDLL_T::FS, NO_ERROR, "Failed to parse directory tree file \"%s\"!\n", fileName.String());
		return;
	}

	Msg(eDLL_T::FS, "*** Starting VPK extraction command for: '%s'\n", fileName.String());

	CFastTimer timer;
	timer.Start();

	CPackedStoreBuilder builder;

	builder.InitLzDecoder();
	builder.UnpackStore(vpk, fs_packedstore_workspace->GetString());

	timer.End();
	Msg(eDLL_T::FS, "*** Time elapsed: '%lf' seconds\n", timer.GetDuration().GetSeconds());
	Msg(eDLL_T::FS, "\n");
}

/*
=====================
VPK_Mount_f

  Mounts input VPK file for
  internal FileSystem usage
=====================
*/
void VPK_Mount_f(const CCommand& args)
{
	if (args.ArgC() < 2)
	{
		return;
	}

	FileSystem()->MountVPKFile(args.Arg(1));
}

/*
=====================
VPK_Unmount_f

  Unmounts input VPK file
  and clears its cache
=====================
*/
void VPK_Unmount_f(const CCommand& args)
{
	if (args.ArgC() < 2)
	{
		return;
	}

	FileSystem()->UnmountVPKFile(args.Arg(1));
}

/*
=====================
NET_SetKey_f

  Sets the input netchannel encryption key
=====================
*/
void NET_SetKey_f(const CCommand& args)
{
	if (args.ArgC() < 2)
	{
		return;
	}

	NET_SetKey(args.Arg(1));
}

/*
=====================
NET_GenerateKey_f

  Sets a random netchannel encryption key
=====================
*/
void NET_GenerateKey_f(const CCommand& args)
{
	NET_GenerateKey();
}

/*
=====================
NET_UseRandomKeyChanged_f

  Use random AES encryption
  key for game packets
=====================
*/
void NET_UseRandomKeyChanged_f(IConVar* pConVar, const char* pOldString, float flOldValue)
{
	if (ConVar* pConVarRef = g_pCVar->FindVar(pConVar->GetCommandName()))
	{
		if (strcmp(pOldString, pConVarRef->GetString()) == NULL)
			return; // Same value.

		if (pConVarRef->GetBool())
			NET_GenerateKey();
		else
			NET_SetKey(DEFAULT_NET_ENCRYPTION_KEY);
	}
}

/*
=====================
NET_UseSocketsForLoopbackChanged_f

  Use random AES encryption
  key for game packets
=====================
*/
void NET_UseSocketsForLoopbackChanged_f(IConVar* pConVar, const char* pOldString, float flOldValue)
{
	if (ConVar* pConVarRef = g_pCVar->FindVar(pConVar->GetCommandName()))
	{
		if (strcmp(pOldString, pConVarRef->GetString()) == NULL)
			return; // Same value.

#ifndef CLIENT_DLL
		// Reboot the RCON server to switch address type.
		if (RCONServer()->IsInitialized())
		{
			Msg(eDLL_T::SERVER, "Rebooting RCON server...\n");
			RCONServer()->Shutdown();
			RCONServer()->Init();
		}
#endif // !CLIENT_DLL
	}
}

/*
=====================
SIG_GetAdr_f

  Logs the sigscan
  results to the console.
=====================
*/
void SIG_GetAdr_f(const CCommand& args)
{
	if (!IsCert() && !IsRetail())
		DetourAddress();
}

/*
=====================
CON_Help_f

  Shows the colors and
  description of each
  context.
=====================
*/
void CON_Help_f(const CCommand& args)
{
	Msg(eDLL_T::COMMON, "Contexts:\n");
	SQVM_PrintFunc(reinterpret_cast<HSQUIRRELVM>(SQCONTEXT::SERVER), (SQChar*)(" = Server DLL (Script)\n"));
	SQVM_PrintFunc(reinterpret_cast<HSQUIRRELVM>(SQCONTEXT::CLIENT), (SQChar*)(" = Client DLL (Script)\n"));
	SQVM_PrintFunc(reinterpret_cast<HSQUIRRELVM>(SQCONTEXT::UI), (SQChar*)(" = UI DLL (Script)\n"));

	Msg(eDLL_T::SERVER, " = Server DLL (Code)\n");
	Msg(eDLL_T::CLIENT, " = Client DLL (Code)\n");
	Msg(eDLL_T::UI, " = UI DLL (Code)\n");
	Msg(eDLL_T::ENGINE, " = Engine DLL (Code)\n");
	Msg(eDLL_T::FS, " = FileSystem (Code)\n");
	Msg(eDLL_T::RTECH, " = PakLoad API (Code)\n");
	Msg(eDLL_T::MS, " = MaterialSystem (Code)\n");
	Msg(eDLL_T::AUDIO, " = Audio DLL (Code)\n");
	Msg(eDLL_T::VIDEO, " = Video DLL (Code)\n");
	Msg(eDLL_T::NETCON, " = NetConsole (Code)\n");
}

#ifndef DEDICATED
/*
=====================
CON_LogHistory_f

  Shows the game console 
  submission history.
=====================
*/
void CON_LogHistory_f(const CCommand& args)
{
	const vector<string> vHistory = g_Console.GetHistory();
	for (size_t i = 0, nh = vHistory.size(); i < nh; i++)
	{
		Msg(eDLL_T::COMMON, "%3d: %s\n", i, vHistory[i].c_str());
	}
}

/*
=====================
CON_RemoveLine_f

  Removes a range of lines
  from the console.
=====================
*/
void CON_RemoveLine_f(const CCommand& args)
{
	if (args.ArgC() < 3)
	{
		Msg(eDLL_T::CLIENT, "Usage 'con_removeline': start(int) end(int)\n");
		return;
	}

	int start = atoi(args[1]);
	int end = atoi(args[2]);

	g_Console.RemoveLog(start, end);
}

/*
=====================
CON_ClearLines_f

  Clears all lines from
  the developer console.
=====================
*/
void CON_ClearLines_f(const CCommand& args)
{
	g_Console.ClearLog();
}

/*
=====================
CON_ClearHistory_f

  Clears all submissions from the
  developer console history.
=====================
*/
void CON_ClearHistory_f(const CCommand& args)
{
	g_Console.ClearHistory();
}

/*
=====================
RCON_CmdQuery_f

  Issues an RCON command to the
  RCON server.
=====================
*/
void RCON_CmdQuery_f(const CCommand& args)
{
	const int64_t argCount = args.ArgC();

	if (argCount < 2)
	{
		const char* pszAddress = rcon_address->GetString();

		if (RCONClient()->IsInitialized()
			&& !RCONClient()->IsConnected()
			&& pszAddress[0])
		{
			RCONClient()->Connect(pszAddress);
		}
	}
	else
	{
		if (!RCONClient()->IsInitialized())
		{
			Warning(eDLL_T::CLIENT, "Failed to issue command to RCON server: %s\n", "uninitialized");
			return;
		}
		else if (RCONClient()->IsConnected())
		{
			vector<char> vecMsg;
			bool bSuccess = false;
			const SocketHandle_t hSocket = RCONClient()->GetSocket();

			if (strcmp(args.Arg(1), "PASS") == 0) // Auth with RCON server using rcon_password ConVar value.
			{
				if (argCount > 2)
				{
					bSuccess = RCONClient()->Serialize(vecMsg, args.Arg(2), "", cl_rcon::request_t::SERVERDATA_REQUEST_AUTH);
				}
				else // Use 'rcon_password' ConVar as password.
				{
					bSuccess = RCONClient()->Serialize(vecMsg, rcon_password->GetString(), "", cl_rcon::request_t::SERVERDATA_REQUEST_AUTH);
				}

				if (bSuccess)
				{
					RCONClient()->Send(hSocket, vecMsg.data(), int(vecMsg.size()));
				}

				return;
			}
			else if (strcmp(args.Arg(1), "disconnect") == 0) // Disconnect from RCON server.
			{
				RCONClient()->Disconnect("issued by user");
				return;
			}

			bSuccess = RCONClient()->Serialize(vecMsg, args.Arg(1), args.ArgS(), cl_rcon::request_t::SERVERDATA_REQUEST_EXECCOMMAND);
			if (bSuccess)
			{
				RCONClient()->Send(hSocket, vecMsg.data(), int(vecMsg.size()));
			}
			return;
		}
		else
		{
			Warning(eDLL_T::CLIENT, "Failed to issue command to RCON server: %s\n", "unconnected");
			return;
		}
	}
}

/*
=====================
RCON_Disconnect_f

  Disconnect from RCON server
=====================
*/
void RCON_Disconnect_f(const CCommand& args)
{
	const bool bIsConnected = RCONClient()->IsConnected();
	RCONClient()->Disconnect("issued by user");

	if (bIsConnected) // Log if client was indeed connected.
	{
		Msg(eDLL_T::CLIENT, "User closed RCON connection\n");
	}
}

/*
=====================
RCON_SendLogs_f

  request logs from RCON server
=====================
*/
void RCON_InputOnlyChanged_f(IConVar* pConVar, const char* pOldString, float flOldValue)
{
	RCONClient()->RequestConsoleLog(RCONClient()->ShouldReceive());
}

/*
=====================
GFX_NVN_Changed_f

  force update NVIDIA Reflex
  Low Latency parameters
=====================
*/
void GFX_NVN_Changed_f(IConVar* pConVar, const char* pOldString, float flOldValue)
{
	GFX_MarkLowLatencyParametersOutOfDate();
}
#endif // !DEDICATED

void LanguageChanged_f(IConVar* pConVar, const char* pOldString, float flOldValue)
{
	if (ConVar* pConVarRef = g_pCVar->FindVar(pConVar->GetCommandName()))
	{
		const char* pNewString = pConVarRef->GetString();

		if (strcmp(pOldString, pConVarRef->GetString()) == NULL)
			return; // Same language.

		if (!Localize_IsLanguageSupported(pNewString))
		{
			// if new text isn't valid but the old value is, reset the value
			if (Localize_IsLanguageSupported(pOldString))
				pNewString = pOldString;
			else // this shouldn't really happen, but if neither the old nor new values are valid, set to english
				pNewString = g_LanguageNames[0];
		}

		pConVarRef->SetValue(pNewString);
		g_MasterServer.SetLanguage(pNewString);
	}
}

/*
=====================
RCON_PasswordChanged_f

  Change RCON password
  on server and client
=====================
*/
void RCON_PasswordChanged_f(IConVar* pConVar, const char* pOldString, float flOldValue)
{
	if (ConVar* pConVarRef = g_pCVar->FindVar(pConVar->GetCommandName()))
	{
		if (strcmp(pOldString, pConVarRef->GetString()) == NULL)
			return; // Same password.

#ifndef DEDICATED
		if (!RCONClient()->IsInitialized())
			RCONClient()->Init(); // Initialize first.
#endif // !DEDICATED
#ifndef CLIENT_DLL
		if (RCONServer()->IsInitialized())
			RCONServer()->SetPassword(pConVarRef->GetString());
		else
			RCONServer()->Init(); // Initialize first.
#endif // !CLIENT_DLL
	}
}

#ifndef CLIENT_DLL
/*
=====================
RCON_WhiteListAddresChanged_f

  Change whitelist address
  on RCON server
=====================
*/
void RCON_WhiteListAddresChanged_f(IConVar* pConVar, const char* pOldString, float flOldValue)
{
	if (ConVar* pConVarRef = g_pCVar->FindVar(pConVar->GetCommandName()))
	{
		if (strcmp(pOldString, pConVarRef->GetString()) == NULL)
			return; // Same address.

		if (!RCONServer()->SetWhiteListAddress(pConVarRef->GetString()))
		{
			Warning(eDLL_T::SERVER, "Failed to set RCON whitelist address: %s\n", pConVarRef->GetString());
		}
	}
}

/*
=====================
RCON_ConnectionCountChanged_f

  Change max connection
  count on RCON server
=====================
*/
void RCON_ConnectionCountChanged_f(IConVar* pConVar, const char* pOldString, float flOldValue)
{
	if (!RCONServer()->IsInitialized())
		return; // Not initialized; no sockets at this point.

	if (ConVar* pConVarRef = g_pCVar->FindVar(pConVar->GetCommandName()))
	{
		if (strcmp(pOldString, pConVarRef->GetString()) == NULL)
			return; // Same count.

		const int maxCount = pConVarRef->GetInt();

		int count = RCONServer()->GetAuthenticatedCount();
		CSocketCreator* pCreator = RCONServer()->GetSocketCreator();

		if (count < maxCount)
		{
			if (!pCreator->IsListening())
			{
				pCreator->CreateListenSocket(*RCONServer()->GetNetAddress());
			}
		}
		else
		{
			while (count > maxCount)
			{
				RCONServer()->Disconnect(count-1, "too many authenticated sockets");
				count = RCONServer()->GetAuthenticatedCount();
			}

			pCreator->CloseListenSocket();
			RCONServer()->CloseNonAuthConnection();
		}
	}
}
#endif // !CLIENT_DLL

/*
=====================
SQVM_ServerScript_f

  Executes input on the
  VM in SERVER context.
=====================
*/
void SQVM_ServerScript_f(const CCommand& args)
{
	if (args.ArgC() >= 2)
	{
		Script_Execute(args.ArgS(), SQCONTEXT::SERVER);
	}
}

#ifndef DEDICATED
/*
=====================
SQVM_ClientScript_f

  Executes input on the
  VM in CLIENT context.
=====================
*/
void SQVM_ClientScript_f(const CCommand& args)
{
	if (args.ArgC() >= 2)
	{
		Script_Execute(args.ArgS(), SQCONTEXT::CLIENT);
	}
}

/*
=====================
SQVM_UIScript_f

  Executes input on the
  VM in UI context.
=====================
*/
void SQVM_UIScript_f(const CCommand& args)
{
	if (args.ArgC() >= 2)
	{
		Script_Execute(args.ArgS(), SQCONTEXT::UI);
	}
}

/*
=====================
Mat_CrossHair_f

  Print the material under the crosshair.
=====================
*/
void Mat_CrossHair_f(const CCommand& args)
{
	CMaterialGlue* material = v_GetMaterialAtCrossHair();
	if (material)
	{
		Msg(eDLL_T::MS, "______________________________________________________________\n");
		Msg(eDLL_T::MS, "-+ Material --------------------------------------------------\n");
		Msg(eDLL_T::MS, " |-- ADDR: '%llX'\n", material);
		Msg(eDLL_T::MS, " |-- GUID: '%llX'\n", material->assetGuid);
		Msg(eDLL_T::MS, " |-- Num Streaming Textures: '%d'\n", material->numStreamingTextureHandles);
		Msg(eDLL_T::MS, " |-- Material width: '%d'\n", material->width);
		Msg(eDLL_T::MS, " |-- Material height: '%d'\n", material->height);
		Msg(eDLL_T::MS, " |-- Samplers: '%08X'\n", material->samplers);

		std::function<void(CMaterialGlue*, const char*)> fnPrintChild = [](CMaterialGlue* material, const char* print)
		{
			Msg(eDLL_T::MS, " |-+\n");
			Msg(eDLL_T::MS, " | |-+ Child material ----------------------------------------\n");
			Msg(eDLL_T::MS, print, material);
			Msg(eDLL_T::MS, " |     |-- GUID: '%llX'\n", material->assetGuid);
			Msg(eDLL_T::MS, " |     |-- Material name: '%s'\n", material->name);
		};

		Msg(eDLL_T::MS, " |-- Material name: '%s'\n", material->name);
		Msg(eDLL_T::MS, " |-- Material surface name 1: '%s'\n", material->surfaceProp);
		Msg(eDLL_T::MS, " |-- Material surface name 2: '%s'\n", material->surfaceProp2);
		Msg(eDLL_T::MS, " |-- DX buffer: '%llX'\n", material->dxBuffer);
		Msg(eDLL_T::MS, " |-- DX buffer VFTable: '%llX'\n", material->unkD3DPointer);

		material->depthShadowMaterial
			? fnPrintChild(material->depthShadowMaterial, " |   |-+ DepthShadow: '%llX'\n")
			: Msg(eDLL_T::MS, " |   |-+ DepthShadow: 'NULL'\n");
		material->depthPrepassMaterial
			? fnPrintChild(material->depthPrepassMaterial, " |   |-+ DepthPrepass: '%llX'\n")
			: Msg(eDLL_T::MS, " |   |-+ DepthPrepass: 'NULL'\n");
		material->depthVSMMaterial
			? fnPrintChild(material->depthVSMMaterial, " |   |-+ DepthVSM: '%llX'\n")
			: Msg(eDLL_T::MS, " |   |-+ DepthVSM: 'NULL'\n");
		material->depthShadowTightMaterial
			? fnPrintChild(material->depthShadowTightMaterial, " |   |-+ DepthShadowTight: '%llX'\n")
			: Msg(eDLL_T::MS, " |   |-+ DepthShadowTight: 'NULL'\n");
		material->colpassMaterial
			? fnPrintChild(material->colpassMaterial, " |   |-+ ColPass: '%llX'\n")
			: Msg(eDLL_T::MS, " |   |-+ ColPass: 'NULL'\n");

		Msg(eDLL_T::MS, "-+ Texture GUID map ------------------------------------------\n");
		Msg(eDLL_T::MS, " |-- Texture handles: '%llX'\n", material->textureHandles);
		Msg(eDLL_T::MS, " |-- Streaming texture handles: '%llX'\n", material->streamingTextureHandles);

		Msg(eDLL_T::MS, "--------------------------------------------------------------\n");
	}
	else
	{
		Msg(eDLL_T::MS, "%s: No material found >:(\n", __FUNCTION__);
	}
}

/*
=====================
Line_f

  Draws a line at 
  start<x1 y1 z1> end<x2 y2 z2>.
=====================
*/
void Line_f(const CCommand& args)
{
	if (args.ArgC() != 7)
	{
		Msg(eDLL_T::CLIENT, "Usage 'line': start(vector) end(vector)\n");
		return;
	}

	Vector3D start, end;
	for (int i = 0; i < 3; ++i)
	{
		start[i] = float(atof(args[i + 1]));
		end[i] = float(atof(args[i + 4]));
	}

	g_pDebugOverlay->AddLineOverlay(start, end, 255, 255, 0, !r_debug_draw_depth_test->GetBool(), 100);
}

/*
=====================
Sphere_f

  Draws a sphere at origin(x1 y1 z1) 
  radius(float) theta(int) phi(int).
=====================
*/
void Sphere_f(const CCommand& args)
{
	if (args.ArgC() != 7)
	{
		Msg(eDLL_T::CLIENT, "Usage 'sphere': origin(vector) radius(float) theta(int) phi(int)\n");
		return;
	}

	Vector3D start;
	for (int i = 0; i < 3; ++i)
	{
		start[i] = float(atof(args[i + 1]));
	}

	float radius = float(atof(args[4]));
	int theta = atoi(args[5]);
	int phi = atoi(args[6]);

	g_pDebugOverlay->AddSphereOverlay(start, radius, theta, phi, 20, 210, 255, 0, 100);
}

/*
=====================
Capsule_f

  Draws a capsule at start<x1 y1 z1> 
  end<x2 y2 z2> radius <x3 y3 z3>.
=====================
*/
void Capsule_f(const CCommand& args)
{
	if (args.ArgC() != 10)
	{
		Msg(eDLL_T::CLIENT, "Usage 'capsule': start(vector) end(vector) radius(vector)\n");
		return;
	}

	Vector3D start, end, radius;
	for (int i = 0; i < 3; ++i)
	{
		start[i] = float(atof(args[i + 1]));
		end[i] = float(atof(args[i + 4]));
		radius[i] = float(atof(args[i + 7]));
	}
	g_pDebugOverlay->AddCapsuleOverlay(start, end, radius, { 0,0,0 }, { 0,0,0 }, 141, 233, 135, 0, 100);
}
#endif // !DEDICATED

/*
=====================
BHit_f

  Bullet trajectory tracing
  from shooter to target entity.
=====================
*/
void BHit_f(const CCommand& args)
{
#ifndef CLIENT_DLL // Stubbed to suppress server warnings as this is a GAMEDLL command!
	if (args.ArgC() != 9)
		return;

	if (!bhit_enable->GetBool())
		return;

	if (sv_visualizetraces->GetBool())
	{
		Vector3D vecAbsStart;
		Vector3D vecAbsEnd;

		for (int i = 0; i < 3; ++i)
			vecAbsStart[i] = float(atof(args[i + 4]));

		QAngle vecBulletAngles;
		for (int i = 0; i < 2; ++i)
			vecBulletAngles[i] = float(atof(args[i + 7]));

		vecBulletAngles.z = 180.f; // Flipped axis.
		AngleVectors(vecBulletAngles, &vecAbsEnd);

		vecAbsEnd.MulAdd(vecAbsStart, vecAbsEnd, MAX_COORD_RANGE);

		Ray_t ray(vecAbsStart, vecAbsEnd);
		trace_t trace;

		g_pEngineTraceServer->TraceRay(ray, TRACE_MASK_NPCWORLDSTATIC, &trace);

		g_pDebugOverlay->AddLineOverlay(trace.startpos, trace.endpos, 0, 255, 0, !bhit_depth_test->GetBool(), sv_visualizetraces_duration->GetFloat());
		g_pDebugOverlay->AddLineOverlay(trace.endpos, vecAbsEnd, 255, 0, 0, !bhit_depth_test->GetBool(), sv_visualizetraces_duration->GetFloat());
	}
#endif // !CLIENT_DLL

#ifndef DEDICATED
	if (bhit_abs_origin->GetBool() && r_visualizetraces->GetBool())
	{
		const int iEnt = atoi(args[2]);
		if (const IClientEntity* pEntity = g_pClientEntityList->GetClientEntity(iEnt))
		{
			g_pDebugOverlay->AddSphereOverlay( // Render a debug sphere at the client's predicted entity origin.
				pEntity->GetAbsOrigin(), 10.f, 8, 6, 20, 60, 255, 0, r_visualizetraces_duration->GetFloat());
		}
	}
#endif // !DEDICATED
}

/*
=====================
CVHelp_f

  Show help text for a
  particular convar/concommand
=====================
*/
void CVHelp_f(const CCommand& args)
{
	cv->CvarHelp(args);
}

/*
=====================
CVList_f

  List all ConCommandBases
=====================
*/
void CVList_f(const CCommand& args)
{
	cv->CvarList(args);
}

/*
=====================
CVDiff_f

  List all ConVar's 
  who's values deviate 
  from default value
=====================
*/
void CVDiff_f(const CCommand& args)
{
	cv->CvarDifferences(args);
}

/*
=====================
CVFlag_f

  List all ConVar's
  with specified flags
=====================
*/
void CVFlag_f(const CCommand& args)
{
	cv->CvarFindFlags_f(args);
}

/*
=====================
CC_CreateFakePlayer_f

  Creates a fake player
  on the server
=====================
*/
#ifndef CLIENT_DLL
void CC_CreateFakePlayer_f(const CCommand& args)
{
	if (!g_pServer->IsActive())
		return;

	if (args.ArgC() < 3)
	{
		Msg(eDLL_T::SERVER, "usage 'sv_addbot': name(string) teamid(int)\n");
		return;
	}

	const int numPlayers = g_pServer->GetNumClients();

	// Already at max, don't create.
	if (numPlayers >= g_ServerGlobalVariables->m_nMaxClients)
		return;

	const char* playerName = args.Arg(1);

	int teamNum = atoi(args.Arg(2));
	const int maxTeams = int(g_pServer->GetMaxTeams()) + 1;

	// Clamp team count, going above the limit will
	// cause a crash. Going below 0 means that the
	// engine will assign the bot to the last team.
	if (teamNum > maxTeams)
		teamNum = maxTeams;

	g_pEngineServer->LockNetworkStringTables(true);

	const edict_t nHandle = g_pEngineServer->CreateFakeClient(playerName, teamNum);
	g_pServerGameClients->ClientFullyConnect(nHandle, false);

	g_pEngineServer->LockNetworkStringTables(false);
}
#endif // !CLIENT_DLL

#ifndef DEDICATED
static double s_flScriptExecTimeBase = 0.0f;
static int s_nScriptExecCount = 0;
#endif // !DEDICATED
/*
=====================
Cmd_Exec_f

  executes a cfg file
=====================
*/
void Cmd_Exec_f(const CCommand& args)
{
#ifndef DEDICATED
	// Prevent users from running neo strafe commands and other quick hacks.
	// TODO: when reBar becomes a thing, we should verify this function and
	// flag users that patch them out.
	if (!ThreadInServerFrameThread() && g_pClientState->IsActive())
	{
		const int execQuota = sv_quota_scriptExecsPerSecond->GetInt();

		if (execQuota > 0)
		{
			const double flCurrentTime = Plat_FloatTime();

			// Reset every second.
			if ((flCurrentTime - s_flScriptExecTimeBase) > 1.0)
			{
				s_flScriptExecTimeBase = flCurrentTime;
				s_nScriptExecCount = 0;
			}

			if (s_nScriptExecCount >= execQuota)
			{
				DevWarning(eDLL_T::ENGINE, "Client is simulating and exec count = %d of %d; dropped exec command: %s\n",
					s_nScriptExecCount, execQuota, args.ArgS());

				return;
			}

			s_nScriptExecCount++;
		}
	}
#endif // !DEDICATED
	v__Cmd_Exec_f(args);
}


void VCallback::Detour(const bool bAttach) const
{
	DetourSetup(&v__Cmd_Exec_f, &Cmd_Exec_f, bAttach);
}
