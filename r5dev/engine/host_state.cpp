//=============================================================================//
//
// Purpose: Runs the state machine for the host & server.
//
//=============================================================================//

#include "core/stdafx.h"
#include "tier0/commandline.h"
#include "tier0/fasttimer.h"
#include "tier1/cmd.h"
#include "tier1/cvar.h"
#include "tier1/NetAdr2.h"
#include "tier2/socketcreator.h"
#include "vpc/keyvalues.h"
#ifdef DEDICATED
#include "engine/sv_rcon.h"
#else // 
#include "engine/cl_rcon.h"
#include "engine/cl_main.h"
#endif // DEDICATED
#include "engine/net.h"
#include "engine/gl_screen.h"
#include "engine/host.h"
#include "engine/host_cmd.h"
#include "engine/host_state.h"
#include "engine/sys_engine.h"
#include "engine/sys_utils.h"
#include "engine/modelloader.h"
#include "engine/cmodel_bsp.h"
#ifndef CLIENT_DLL
#include "engine/baseserver.h"
#endif // !CLIENT_DLL
#include "rtech/rtech_game.h"
#include "rtech/rtech_utils.h"
#include "rtech/stryder/stryder.h"
#ifndef DEDICATED
#include "vgui/vgui_baseui_interface.h"
#endif // DEDICATED
#include "client/vengineclient_impl.h"
#include "networksystem/pylon.h"
#include "public/include/bansystem.h"
#include "public/include/edict.h"
#ifndef CLIENT_DLL
#include "game/server/gameinterface.h"
#endif // !CLIENT_DLL

bool g_bLevelResourceInitialized = false;
//-----------------------------------------------------------------------------
// Purpose: state machine's main processing loop
//-----------------------------------------------------------------------------
FORCEINLINE void CHostState::FrameUpdate(CHostState* rcx, void* rdx, float time)
{
	static bool bInitialized = false;
	static ConVar* single_frame_shutdown_for_reload = g_pCVar->FindVar("single_frame_shutdown_for_reload");
	if (!bInitialized)
	{
		g_pHostState->Setup();
		bInitialized = true;
	}
#ifdef DEDICATED
	g_pRConServer->RunFrame();
#else // 
	g_pRConClient->RunFrame();
#endif // DEDICATED

	HostStates_t oldState{};
	if (setjmp(*host_abortserver))
	{
		g_pHostState->Init();
		return;
	}
	else
	{
#ifndef CLIENT_DLL
		*g_bAbortServerSet = true;
#endif // !CLIENT_DLL
		do
		{
			Cbuf_Execute();
			oldState = g_pHostState->m_iCurrentState;

			switch (g_pHostState->m_iCurrentState)
			{
			case HostStates_t::HS_NEW_GAME:
			{
				DevMsg(eDLL_T::ENGINE, "%s - Loading level: '%s'\n", "CHostState::FrameUpdate", g_pHostState->m_levelName);
				g_pHostState->State_NewGame();
				break;
			}
			case HostStates_t::HS_CHANGE_LEVEL_SP:
			{
				g_pHostState->State_ChangeLevelSP();
				g_pHostState->UnloadPakFile(); // Unload our loaded rpaks. Calling this before the actual level change happens kills the game.
				break;
			}
			case HostStates_t::HS_CHANGE_LEVEL_MP:
			{
				g_pHostState->State_ChangeLevelMP();
				g_pHostState->UnloadPakFile();
				break;
			}
			case HostStates_t::HS_RUN:
			{
				CHostState_State_Run(&g_pHostState->m_iCurrentState, nullptr, time);
				break;
			}
			case HostStates_t::HS_GAME_SHUTDOWN:
			{
				DevMsg(eDLL_T::ENGINE, "%s - Shutdown host game\n", "CHostState::FrameUpdate");

				g_bLevelResourceInitialized = false;
				CHostState_GameShutDown(g_pHostState);
				g_pHostState->UnloadPakFile(); 
				break;
			}
			case HostStates_t::HS_RESTART:
			{
				DevMsg(eDLL_T::ENGINE, "%s - Restarting state machine\n", "CHostState::FrameUpdate");
				g_bLevelResourceInitialized = false;
#ifndef DEDICATED
				CL_EndMovie();
#endif // !DEDICATED
				Stryder_SendOfflineRequest(); // We have hostnames nulled anyway.
				g_pEngine->SetNextState(EngineState_t::DLL_RESTART);
				break;
			}
			case HostStates_t::HS_SHUTDOWN:
			{
				DevMsg(eDLL_T::ENGINE, "%s - Shutdown state machine\n", "CHostState::FrameUpdate");
				g_bLevelResourceInitialized = false;
#ifndef DEDICATED
				CL_EndMovie();
#endif // !DEDICATED
				Stryder_SendOfflineRequest(); // We have hostnames nulled anyway.
				g_pEngine->SetNextState(EngineState_t::DLL_CLOSE);
				break;
			}
			default:
			{
				break;
			}
			}

		} while ((oldState != HostStates_t::HS_RUN || g_pHostState->m_iNextState == HostStates_t::HS_LOAD_GAME && single_frame_shutdown_for_reload->GetBool())
			&& oldState != HostStates_t::HS_SHUTDOWN
			&& oldState != HostStates_t::HS_RESTART);
	}
}

//-----------------------------------------------------------------------------
// Purpose: state machine initialization
//-----------------------------------------------------------------------------
FORCEINLINE void CHostState::Init(void)
{
	static ConVar* single_frame_shutdown_for_reload = g_pCVar->FindVar("single_frame_shutdown_for_reload");

	if (m_iNextState != HostStates_t::HS_SHUTDOWN)
	{
		if (m_iNextState == HostStates_t::HS_GAME_SHUTDOWN)
		{
			CHostState_GameShutDown(this);
		}
		else
		{
			m_iCurrentState = HostStates_t::HS_RUN;
			if (m_iNextState != HostStates_t::HS_SHUTDOWN || !single_frame_shutdown_for_reload->GetInt())
				m_iNextState = HostStates_t::HS_RUN;
		}
	}
	m_flShortFrameTime = 1.0;
	m_levelName[0] = 0;
	m_landMarkName[0] = 0;
	m_mapGroupName[0] = 0;
	m_bSplitScreenConnect = 256;
	m_vecLocation.Init();
	m_angLocation.Init();
	m_iCurrentState = HostStates_t::HS_NEW_GAME;
}

//-----------------------------------------------------------------------------
// Purpose: state machine setup
//-----------------------------------------------------------------------------
FORCEINLINE void CHostState::Setup(void) const
{
	g_pHostState->LoadConfig();
	g_pConVar->ClearHostNames();
#ifdef DEDICATED
	g_pRConServer->Init();
#else // 
	g_pRConClient->Init();
#endif // DEDICATED

	std::thread think(&CHostState::Think, this);
	think.detach();

	*m_bRestrictServerCommands = true; // Restrict commands.
	ConCommandBase* disconnect = g_pCVar->FindCommandBase("disconnect");
	disconnect->AddFlags(FCVAR_SERVER_CAN_EXECUTE); // Make sure server is not restricted to this.
	g_pCVar->FindVar("net_usesocketsforloopback")->SetValue(1);

	if (net_userandomkey->GetBool())
	{
		NET_GenerateKey();
	}

#ifdef DEDICATED
	const char* szNoMap = "server_idle";
#else // DEDICATED
	const char* szNoMap = "main_menu";
#endif
	snprintf(const_cast<char*>(m_levelName), sizeof(m_levelName), szNoMap);
}

//-----------------------------------------------------------------------------
// Purpose: think
//-----------------------------------------------------------------------------
FORCEINLINE void CHostState::Think(void) const
{
	static bool bInitialized = false;
	static CFastTimer banListTimer;
	static CFastTimer pylonTimer;
	static CFastTimer statsTimer;
	static ConVar* hostname = g_pCVar->FindVar("hostname");

	for (;;) // Loop running at 20-tps.
	{
		if (!bInitialized) // Initialize clocks.
		{
			banListTimer.Start();
#ifdef DEDICATED
			pylonTimer.Start();
#endif // DEDICATED
			statsTimer.Start();
			bInitialized = true;
		}

		if (banListTimer.GetDurationInProgress().GetSeconds() > sv_banlistRefreshInterval->GetDouble())
		{
			g_pBanSystem->BanListCheck();
			banListTimer.Start();
		}
#ifdef DEDICATED
		if (pylonTimer.GetDurationInProgress().GetSeconds() > sv_pylonRefreshInterval->GetDouble())
		{
			KeepAliveToPylon();
			pylonTimer.Start();
		}
#endif // DEDICATED
		if (statsTimer.GetDurationInProgress().GetSeconds() > sv_statusRefreshInterval->GetDouble())
		{
			std::string svCurrentPlaylist = KeyValues_GetCurrentPlaylist();
			std::int64_t nPlayerCount = g_pServer->GetNumHumanPlayers();

			SetConsoleTitleA(fmt::format("{} - {}/{} Players ({} on {})",
				hostname->GetString(), nPlayerCount, g_ServerGlobalVariables->m_nMaxClients, svCurrentPlaylist.c_str(), m_levelName).c_str());
			statsTimer.Start();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
}

//-----------------------------------------------------------------------------
// Purpose: load and execute configuration files
//-----------------------------------------------------------------------------
FORCEINLINE void CHostState::LoadConfig(void) const
{
	if (!g_pCmdLine->CheckParm("-devsdk"))
	{
#ifndef CLIENT_DLL
		Cbuf_AddText(Cbuf_GetCurrentPlayer(), "exec \"autoexec_server.cfg\"", cmd_source_t::kCommandSrcCode);
		Cbuf_AddText(Cbuf_GetCurrentPlayer(), "exec \"rcon_server.cfg\"", cmd_source_t::kCommandSrcCode);
#endif //!CLIENT_DLL
#ifndef DEDICATED
		Cbuf_AddText(Cbuf_GetCurrentPlayer(), "exec \"autoexec_client.cfg\"", cmd_source_t::kCommandSrcCode);
		Cbuf_AddText(Cbuf_GetCurrentPlayer(), "exec \"rcon_client.cfg\"", cmd_source_t::kCommandSrcCode);
#endif // !DEDICATED
		Cbuf_AddText(Cbuf_GetCurrentPlayer(), "exec \"autoexec.cfg\"", cmd_source_t::kCommandSrcCode);
	}
	else // Development configs.
	{
#ifndef CLIENT_DLL
		Cbuf_AddText(Cbuf_GetCurrentPlayer(), "exec \"autoexec_server_dev.cfg\"", cmd_source_t::kCommandSrcCode);
		Cbuf_AddText(Cbuf_GetCurrentPlayer(), "exec \"rcon_server_dev.cfg\"", cmd_source_t::kCommandSrcCode);
#endif //!CLIENT_DLL
#ifndef DEDICATED
		Cbuf_AddText(Cbuf_GetCurrentPlayer(), "exec \"autoexec_client_dev.cfg\"", cmd_source_t::kCommandSrcCode);
		Cbuf_AddText(Cbuf_GetCurrentPlayer(), "exec \"rcon_client_dev.cfg\"", cmd_source_t::kCommandSrcCode);
#endif // !DEDICATED
		Cbuf_AddText(Cbuf_GetCurrentPlayer(), "exec \"autoexec_dev.cfg\"", cmd_source_t::kCommandSrcCode);
	}
	Cbuf_Execute();
}

//-----------------------------------------------------------------------------
// Purpose: shutdown active game
//-----------------------------------------------------------------------------
FORCEINLINE void CHostState::GameShutDown(void)
{
	g_bLevelResourceInitialized = false;
	if (m_bActiveGame)
	{
#ifndef CLIENT_DLL
		g_pServerGameDLL->GameShutdown();
#endif // !CLIENT_DLL
		m_bActiveGame = 0;
#ifdef DEDICATED
		const char* szNoMap = "server_idle";
#else // DEDICATED
		const char* szNoMap = "main_menu";
#endif
		snprintf(const_cast<char*>(m_levelName), sizeof(m_levelName), szNoMap);
	}
}

//-----------------------------------------------------------------------------
// Purpose: unloads all pakfiles loaded by the SDK
//-----------------------------------------------------------------------------
FORCEINLINE void CHostState::UnloadPakFile(void)
{
	for (auto& it : g_nLoadedPakFileId)
	{
		if (it >= 0)
		{
#ifdef GAMEDLL_S3
			RPakLoadedInfo_t pakInfo = g_pRTech->GetPakLoadedInfo(it);
			if (pakInfo.m_pszFileName)
			{
				DevMsg(eDLL_T::RTECH, "%s - Unloading PakFile '%s'\n", "CHostState::UnloadPakFile", pakInfo.m_pszFileName);
			}
#endif // GAMEDLL_S3
			RTech_UnloadPak(it);
		}
	}
	g_nLoadedPakFileId.clear();
}

//-----------------------------------------------------------------------------
// Purpose: initialize new game
//-----------------------------------------------------------------------------
FORCEINLINE void CHostState::State_NewGame(void)
{
	LARGE_INTEGER time{};

	g_bLevelResourceInitialized = false;
	m_bSplitScreenConnect = false;
	if (!g_pServerGameClients) // Init Game if it ain't valid.
	{
		SV_InitGameDLL();
	}

	if (!CModelLoader__Map_IsValid(g_pModelLoader, m_levelName) // Check if map is valid and if we can start a new game.
		|| !Host_NewGame(m_levelName, nullptr, m_bBackgroundLevel, m_bSplitScreenConnect, time) || !g_pServerGameClients)
	{
		Error(eDLL_T::ENGINE, "%s - Error: Map not valid\n", "CHostState::State_NewGame");
#ifndef DEDICATED
		SCR_EndLoadingPlaque();
#endif // !DEDICATED
		GameShutDown();
	}

	m_iCurrentState = HostStates_t::HS_RUN; // Set current state to run.

	// If our next state isn't a shutdown or its a forced shutdown then set next state to run.
	if (m_iNextState != HostStates_t::HS_SHUTDOWN || !g_pCVar->FindVar("host_hasIrreversibleShutdown")->GetBool())
	{
		m_iNextState = HostStates_t::HS_RUN;
	}
}

//-----------------------------------------------------------------------------
// Purpose: change singleplayer level
//-----------------------------------------------------------------------------
FORCEINLINE void CHostState::State_ChangeLevelSP(void)
{
	DevMsg(eDLL_T::ENGINE, "%s - Changing singleplayer level to: '%s'\n", "CHostState::State_ChangeLevelSP", m_levelName);
	m_flShortFrameTime = 1.5; // Set frame time.
	g_bLevelResourceInitialized = false;

	if (CModelLoader__Map_IsValid(g_pModelLoader, m_levelName)) // Check if map is valid and if we can start a new game.
	{
		Host_ChangeLevel(true, m_levelName, m_mapGroupName); // Call change level as singleplayer level.
	}
	else
	{
		Error(eDLL_T::ENGINE, "%s - Error: Unable to find map: '%s'\n", "CHostState::State_ChangeLevelSP", m_levelName);
	}

	m_iCurrentState = HostStates_t::HS_RUN; // Set current state to run.

	// If our next state isn't a shutdown or its a forced shutdown then set next state to run.
	if (m_iNextState != HostStates_t::HS_SHUTDOWN || !g_pCVar->FindVar("host_hasIrreversibleShutdown")->GetBool())
	{
		m_iNextState = HostStates_t::HS_RUN;
	}
}

//-----------------------------------------------------------------------------
// Purpose: change multiplayer level
//-----------------------------------------------------------------------------
FORCEINLINE void CHostState::State_ChangeLevelMP(void)
{
	DevMsg(eDLL_T::ENGINE, "%s - Changing multiplayer level to: '%s'\n", "CHostState::State_ChangeLevelMP", m_levelName);
	m_flShortFrameTime = 0.5; // Set frame time.
	g_bLevelResourceInitialized = false;

#ifndef CLIENT_DLL
	g_pServerGameDLL->LevelShutdown();
#endif // !CLIENT_DLL
	if (CModelLoader__Map_IsValid(g_pModelLoader, m_levelName)) // Check if map is valid and if we can start a new game.
	{
#ifndef DEDICATED
		using EnabledProgressBarForNextLoadFn = void(*)(void*);
		(*reinterpret_cast<EnabledProgressBarForNextLoadFn**>(g_pEngineVGui))[31](g_pEngineVGui); // EnabledProgressBarForNextLoad
#endif // !DEDICATED
		Host_ChangeLevel(false, m_levelName, m_mapGroupName); // Call change level as multiplayer level.
	}
	else
	{
		Error(eDLL_T::ENGINE, "%s - Error: Unable to find map: '%s'\n", "CHostState::State_ChangeLevelMP", m_levelName);
	}

	m_iCurrentState = HostStates_t::HS_RUN; // Set current state to run.

	// If our next state isn't a shutdown or its a forced shutdown then set next state to run.
	if (m_iNextState != HostStates_t::HS_SHUTDOWN || !g_pCVar->FindVar("host_hasIrreversibleShutdown")->GetBool())
	{
		m_iNextState = HostStates_t::HS_RUN;
	}
}

///////////////////////////////////////////////////////////////////////////////
void CHostState_Attach()
{
	DetourAttach((LPVOID*)&CHostState_FrameUpdate, &CHostState::FrameUpdate);
}

void CHostState_Detach()
{
	DetourDetach((LPVOID*)&CHostState_FrameUpdate, &CHostState::FrameUpdate);
}

///////////////////////////////////////////////////////////////////////////////
CHostState* g_pHostState = nullptr;
