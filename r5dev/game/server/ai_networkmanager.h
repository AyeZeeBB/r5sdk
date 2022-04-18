//=============================================================================//
//
// Purpose:
//
//=============================================================================//
#pragma once
#include "game/server/ai_network.h"

#if defined (GAMEDLL_S0) || defined (GAMEDLL_S1)
const int AINETWORK_OFFSET = 2808;
#elif defined (GAMEDLL_S2) || defined (GAMEDLL_S3)
const int AINETWORK_OFFSET = 2840;
#endif

const string HULL_SIZE[5] = 
{
	"small",
	"med_short",
	"medium",
	"large",
	"extra_large"
};

/* ==== CAI_NETWORKMANAGER ============================================================================================================================================== */
inline CMemory p_CAI_NetworkManager__ShouldRebuild = nullptr;
inline auto CAI_NetworkManager__ShouldRebuild = p_CAI_NetworkManager__ShouldRebuild.RCast<void* (*)(void* thisptr, CAI_Network* pNetwork, void* a3, int a4)>();
#if defined (GAMEDLL_S0) || defined (GAMEDLL_S1)
inline CMemory p_CAI_NetworkManager__LoadNetworkGraph = nullptr;
inline auto CAI_NetworkManager__LoadNetworkGraph = p_CAI_NetworkManager__LoadNetworkGraph.RCast<void* (*)(void* thisptr, void* pBuffer, const char* pszFileName, int a4)>();
#elif defined (GAMEDLL_S2) || defined (GAMEDLL_S3)
inline CMemory p_CAI_NetworkManager__LoadNetworkGraph = nullptr;
inline auto CAI_NetworkManager__LoadNetworkGraph = p_CAI_NetworkManager__LoadNetworkGraph.RCast<void* (*)(void* thisptr, void* pBuffer, const char* pszFileName)>();

#endif
/* ==== CAI_NETWORKBUILDER ============================================================================================================================================== */
inline CMemory p_CAI_NetworkBuilder__Build;
inline auto CAI_NetworkBuilder__Build = p_CAI_NetworkBuilder__Build.RCast<void* (*)(void* thisptr, CAI_Network* pNetwork, void* a3, int a4)>();

inline int                 * g_nAiNodeClusters       = nullptr;
inline AINodeClusters    *** g_pppAiNodeClusters     = nullptr;
inline int                 * g_nAiNodeClusterLinks   = nullptr;
inline AINodeClusterLinks*** g_pppAiNodeClusterLinks = nullptr;

void CAI_NetworkManager_Attach();
void CAI_NetworkManager_Detach();

//-----------------------------------------------------------------------------
// CAI_NetworkBuilder
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CAI_NetworkBuilder
{
public:
	static void SaveNetworkGraph(CAI_Network* pNetwork);
};


///////////////////////////////////////////////////////////////////////////////
class HCAI_NetworkManager : public IDetour
{
	virtual void GetAdr(void) const
	{
		std::cout << "| FUN: CAI_NetworkManager::LoadNetworkGraph : 0x" << std::hex << std::uppercase << p_CAI_NetworkManager__ShouldRebuild.GetPtr() << std::setw(nPad) << " |" << std::endl;
		std::cout << "| FUN: CAI_NetworkManager::ShouldRebuild    : 0x" << std::hex << std::uppercase << p_CAI_NetworkManager__ShouldRebuild.GetPtr() << std::setw(nPad) << " |" << std::endl;
		std::cout << "| FUN: CAI_NetworkBuilder::Build            : 0x" << std::hex << std::uppercase << p_CAI_NetworkBuilder__Build.GetPtr()         << std::setw(nPad) << " |" << std::endl;
		std::cout << "| VAR: g_nAiNodeClusters                    : 0x" << std::hex << std::uppercase << g_nAiNodeClusters                            << std::setw(0)    << " |" << std::endl;
		std::cout << "| VAR: g_pppAiNodeClusters                  : 0x" << std::hex << std::uppercase << g_pppAiNodeClusters                          << std::setw(0)    << " |" << std::endl;
		std::cout << "| VAR: g_nAiNodeClusterLinks                : 0x" << std::hex << std::uppercase << g_nAiNodeClusterLinks                        << std::setw(0)    << " |" << std::endl;
		std::cout << "| VAR: g_pppAiNodeClusterLinks              : 0x" << std::hex << std::uppercase << g_pppAiNodeClusterLinks                      << std::setw(0)    << " |" << std::endl;
		std::cout << "+----------------------------------------------------------------+" << std::endl;
	}
	virtual void GetFun(void) const
	{
		p_CAI_NetworkManager__ShouldRebuild = g_mGameDll.FindPatternSIMD(reinterpret_cast<rsig_t>("\x40\x53\x48\x83\xEC\x20\x48\x8B\xD9\x48\x8B\x0D\x00\x00\x00\x00\x8B\x41\x6C"), "xxxxxxxxxxxx????xxx");
#if defined (GAMEDLL_S0) || defined (GAMEDLL_S1)
		p_CAI_NetworkManager__LoadNetworkGraph = g_mGameDll.FindPatternSIMD(reinterpret_cast<rsig_t>("\x4C\x89\x44\x24\x00\x48\x89\x4C\x24\x00\x55\x53\x57\x41\x54\x41\x55\x41\x56"), "xxxx?xxxx?xxxxxxxxx");
		CAI_NetworkManager__LoadNetworkGraph = p_CAI_NetworkManager__LoadNetworkGraph.RCast<void* (*)(void*, void*, const char*, int)>(); /*4C 89 44 24 ?? 48 89 4C 24 ?? 55 53 57 41 54 41 55 41 56*/
#elif defined (GAMEDLL_S2) || defined (GAMEDLL_S3)
		p_CAI_NetworkManager__LoadNetworkGraph = g_mGameDll.FindPatternSIMD(reinterpret_cast<rsig_t>("\x4C\x89\x44\x24\x00\x48\x89\x4C\x24\x00\x55\x53\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\x6C\x24\x00\x48\x81\xEC\x00\x00\x00\x00\x48\x8B\xFA"), "xxxx?xxxx?xxxxxxxxxxxxxxxx?xxx????xxx");
		CAI_NetworkManager__LoadNetworkGraph = p_CAI_NetworkManager__LoadNetworkGraph.RCast<void* (*)(void*, void*, const char*)>(); /*4C 89 44 24 ?? 48 89 4C 24 ?? 55 53 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 ?? 48 81 EC ?? ?? ?? ?? 48 8B FA*/
#endif
#if defined (GAMEDLL_S0) || defined (GAMEDLL_S1)
		p_CAI_NetworkBuilder__Build = g_mGameDll.FindPatternSIMD(reinterpret_cast<rsig_t>("\x48\x89\x5C\x24\x00\x48\x89\x6C\x24\x00\x48\x89\x74\x24\x00\x48\x89\x4C\x24\x00\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x83\xEC\x30\x48\x63\xBA\x00\x00\x00\x00"), "xxxx?xxxx?xxxx?xxxx?xxxxxxxxxxxxxxxx????");
#elif defined (GAMEDLL_S2) || defined (GAMEDLL_S3)
		p_CAI_NetworkBuilder__Build = g_mGameDll.FindPatternSIMD(reinterpret_cast<rsig_t>("\x48\x89\x54\x24\x00\x48\x89\x4C\x24\x00\x53\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x83\xEC\x38\x8B\xB2\x00\x00\x00\x00"), "xxxx?xxxx?xxxxxxxxxxxxxxxxxx????");
#endif
		CAI_NetworkManager__ShouldRebuild = p_CAI_NetworkManager__ShouldRebuild.RCast<void* (*)(void*, CAI_Network*, void*, int)>(); /*40 53 48 83 EC 20 48 8B D9 48 8B 0D ?? ?? ?? ?? 8B 41 6C*/
		CAI_NetworkBuilder__Build         = p_CAI_NetworkBuilder__Build.RCast<void* (*)(void*, CAI_Network*, void*, int)>();         /*48 89 5C 24 ?? 48 89 6C 24 ?? 48 89 74 24 ?? 48 89 4C 24 ?? 57 41 54 41 55 41 56 41 57 48 83 EC 30 48 63 BA ?? ?? ?? ??*/
		CAI_NetworkBuilder__Build         = p_CAI_NetworkBuilder__Build.RCast<void* (*)(void*, CAI_Network*, void*, int)>();         /*48 89 54 24 ?? 48 89 4C 24 ?? 53 55 56 57 41 54 41 55 41 56 41 57 48 83 EC 38 8B B2 ?? ?? ?? ??*/
	}
	virtual void GetVar(void) const
	{
		g_nAiNodeClusters = g_mGameDll.FindPatternSIMD(reinterpret_cast<rsig_t>("\x4C\x0F\xBF\x12"), "xxxx")
			.FindPatternSelf("83 3D", CMemory::Direction::DOWN).ResolveRelativeAddressSelf(0x2, 0x7).RCast<int*>();
		g_pppAiNodeClusters = g_mGameDll.FindPatternSIMD(reinterpret_cast<rsig_t>("\xF3\x0F\x10\x52\x00\x4C\x8B\xCA"), "xxxx?xxx")
			.FindPatternSelf("48 8B 35", CMemory::Direction::DOWN).ResolveRelativeAddressSelf(0x3, 0x7).RCast<AINodeClusters***>();
		g_nAiNodeClusterLinks = g_mGameDll.FindPatternSIMD(reinterpret_cast<rsig_t>("\x49\xFF\xC0\x48\x83\xC2\x04\x4D\x3B\xC2\x7C\xD4"), "xxxxxxxxxxxx")
			.FindPatternSelf("8B 3D", CMemory::Direction::DOWN).ResolveRelativeAddressSelf(0x2, 0x6).RCast<int*>();
		g_pppAiNodeClusterLinks = g_mGameDll.FindPatternSIMD(reinterpret_cast<rsig_t>("\xF3\x0F\x10\x52\x00\x4C\x8B\xCA"), "xxxx?xxx")
			.FindPatternSelf("4C 8B 1D", CMemory::Direction::DOWN).ResolveRelativeAddressSelf(0x3, 0x7).RCast<AINodeClusterLinks***>();
	}
	virtual void GetCon(void) const { }
	virtual void Attach(void) const { }
	virtual void Detach(void) const { }
};
///////////////////////////////////////////////////////////////////////////////

REGISTER(HCAI_NetworkManager);
