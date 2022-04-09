#pragma once

#if defined (GAMEDLL_S0) || defined (GAMEDLL_S1)
inline ADDRESS p_CModelLoader__FindModel = g_mGameDll.FindPatternSIMD(reinterpret_cast<rsig_t>("\x40\x55\x41\x55\x41\x56\x48\x8D\xAC\x24\x00\x00\x00\x00"), "xxxxxxxxxx????");
inline auto CModelLoader__FindModel = p_CModelLoader__FindModel.RCast<void* (*)(void* thisptr, const char* pszModelName)>(); /*40 55 41 55 41 56 48 8D AC 24 ? ? ? ?*/

inline ADDRESS p_CModelLoader__LoadModel = g_mGameDll.FindPatternSIMD(reinterpret_cast<rsig_t>("\x40\x53\x57\x41\x56\x48\x81\xEC\x00\x00\x00\x00\x48\x8B\xFA"), "xxxxxxxx????xxx");
inline auto CModelLoader__LoadModel = p_CModelLoader__LoadModel.RCast<void(*)(void* thisptr, void* mod)>(); /*40 53 57 41 56 48 81 EC ? ? ? ? 48 8B FA*/

inline ADDRESS p_CModelLoader__Studio_LoadModel = g_mGameDll.FindPatternSIMD(reinterpret_cast<rsig_t>("\x48\x89\x5C\x24\x00\x55\x56\x57\x41\x54\x41\x56\x48\x8D\xAC\x24\x00\x00\x00\x00"), "xxxx?xxxxxxxxxxx????");
inline auto CModelLoader__Studio_LoadModel = p_CModelLoader__Studio_LoadModel.RCast<void* (*)(void* thisptr)>(); /*48 89 5C 24 ? 55 56 57 41 54 41 56 48 8D AC 24 ? ? ? ?*/

inline ADDRESS p_CModelLoader__Map_LoadModelGuts = g_mGameDll.FindPatternSIMD(reinterpret_cast<rsig_t>("\x48\x89\x54\x24\x00\x48\x89\x4C\x24\x00\x55\x53\x41\x54\x41\x55\x48\x8D\xAC\x24\x00\x00\x00\x00\x48\x81\xEC\x00\x00\x00\x00\xFF\x05\x00\x00\x00\x00"), "xxxx?xxxx?xxxxxxxxxx????xxx????xx????"); // BSP.
inline auto CModelLoader__Map_LoadModelGuts = p_CModelLoader__Map_LoadModelGuts.RCast<uint64_t(*)(void* thisptr, void* mod)>(); /*48 89 54 24 ? 48 89 4C 24 ? 55 53 41 54 41 55 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? FF 05 ? ? ? ? */

inline ADDRESS p_CModelLoader__UnloadModel = g_mGameDll.FindPatternSIMD(reinterpret_cast<rsig_t>("\x48\x8B\xC4\x48\x89\x58\x18\x55\x48\x81\xEC\x00\x00\x00\x00\x48\x8B\xDA"), "xxxxxxxxxxx????xxx");
inline auto CModelLoader__UnloadModel = p_CModelLoader__UnloadModel.RCast<uint64_t(*)(void* thisptr, void* pModel)>(); /*48 8B C4 48 89 58 18 55 48 81 EC ? ? ? ? 48 8B DA*/
#elif defined (GAMEDLL_S2) || defined (GAMEDLL_S3)
inline ADDRESS p_CModelLoader__FindModel = g_mGameDll.FindPatternSIMD(reinterpret_cast<rsig_t>("\x40\x55\x41\x57\x48\x83\xEC\x48\x80\x3A\x2A"), "xxxxxxxxxxx");
inline auto CModelLoader__FindModel = p_CModelLoader__FindModel.RCast<void* (*)(void* thisptr, const char* pszModelName)>(); /*40 55 41 57 48 83 EC 48 80 3A 2A*/

inline ADDRESS p_CModelLoader__LoadModel = g_mGameDll.FindPatternSIMD(reinterpret_cast<rsig_t>("\x40\x53\x57\x41\x57\x48\x81\xEC\x00\x00\x00\x00\x48\x8B\x05\x00\x00\x00\x00"), "xxxxxxxx????xxx????");
inline auto CModelLoader__LoadModel = p_CModelLoader__LoadModel.RCast<void(*)(void* thisptr, void* mod)>(); /*40 53 57 41 57 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ?*/

inline ADDRESS p_CModelLoader__Studio_LoadModel = g_mGameDll.FindPatternSIMD(reinterpret_cast<rsig_t>("\x48\x89\x5C\x24\x00\x55\x56\x57\x41\x54\x41\x57\x48\x81\xEC\x00\x00\x00\x00"), "xxxx?xxxxxxxxxx????");
inline auto CModelLoader__Studio_LoadModel = p_CModelLoader__Studio_LoadModel.RCast<void* (*)(void* thisptr)>(); /*48 89 5C 24 ? 55 56 57 41 54 41 57 48 81 EC ? ? ? ?*/

inline ADDRESS p_CModelLoader__Map_LoadModelGuts = g_mGameDll.FindPatternSIMD(reinterpret_cast<rsig_t>("\x48\x89\x54\x24\x00\x48\x89\x4C\x24\x00\x55\x53\x56\x57\x41\x54\x41\x55\x41\x57"), "xxxx?xxxx?xxxxxxxxxx"); // BSP.
inline auto CModelLoader__Map_LoadModelGuts = p_CModelLoader__Map_LoadModelGuts.RCast<uint64_t(*)(void* thisptr, void* mod)>(); /*48 89 54 24 ? 48 89 4C 24 ? 55 53 56 57 41 54 41 55 41 57*/

inline ADDRESS p_CModelLoader__UnloadModel = g_mGameDll.FindPatternSIMD(reinterpret_cast<rsig_t>("\x48\x89\x5C\x24\x00\x48\x89\x6C\x24\x00\x57\x48\x81\xEC\x00\x00\x00\x00\x48\x8B\xF9\x33\xED"), "xxxx?xxxx?xxxx????xxxxx");
inline auto CModelLoader__UnloadModel = p_CModelLoader__UnloadModel.RCast<uint64_t(*)(void* thisptr, void* pModel)>(); /*48 89 5C 24 ? 48 89 6C 24 ? 57 48 81 EC ? ? ? ? 48 8B F9 33 ED*/
#endif

void CModelLoader_Attach();
void CModelLoader_Detach();

///////////////////////////////////////////////////////////////////////////////
class HModelLoader : public IDetour
{
	virtual void debugp()
	{
		std::cout << "| FUN: CModelLoader::FindModel              : 0x" << std::hex << std::uppercase << p_CModelLoader__FindModel.GetPtr()         << std::setw(npad) << " |" << std::endl;
		std::cout << "| FUN: CModelLoader::LoadModel              : 0x" << std::hex << std::uppercase << p_CModelLoader__LoadModel.GetPtr()         << std::setw(npad) << " |" << std::endl;
		std::cout << "| FUN: CModelLoader::UnloadModel            : 0x" << std::hex << std::uppercase << p_CModelLoader__UnloadModel.GetPtr()       << std::setw(npad) << " |" << std::endl;
		std::cout << "| FUN: CModelLoader::Map_LoadModelGuts      : 0x" << std::hex << std::uppercase << p_CModelLoader__Map_LoadModelGuts.GetPtr() << std::setw(npad) << " |" << std::endl;
		std::cout << "| FUN: CModelLoader::Studio_LoadModel       : 0x" << std::hex << std::uppercase << p_CModelLoader__Studio_LoadModel.GetPtr()  << std::setw(npad) << " |" << std::endl;
		std::cout << "+----------------------------------------------------------------+" << std::endl;
	}
};
///////////////////////////////////////////////////////////////////////////////

REGISTER(HModelLoader);
