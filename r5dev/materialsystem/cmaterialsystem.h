#pragma once

/* ==== MATERIALSYSTEM ================================================================================================================================================== */
inline CMemory p_CMaterialSystem__Init;
inline auto CMaterialSystem__Init = p_CMaterialSystem__Init.RCast<void* (*)(void* thisptr)>();
#ifndef DEDICATED
inline CMemory p_DrawStreamOverlay;
inline auto DrawStreamOverlay = p_DrawStreamOverlay.RCast<const char* (*)(void* thisptr, uint8_t* a2, void* unused, void* a4)>();

inline CMemory p_StreamDB_Init;
inline auto StreamDB_Init = p_StreamDB_Init.RCast<void (*)(const char* pszStreamDbFile)>();

inline void* g_pMaterialSystem            = nullptr;
inline int* total_streaming_tex_memory    = nullptr;
inline int* unfree_streaming_tex_memory   = nullptr;
inline int* unusable_streaming_tex_memory = nullptr;
#endif // !DEDICATED

void CMaterialSystem_Attach();
void CMaterialSystem_Detach();
///////////////////////////////////////////////////////////////////////////////
class HMaterialSystem : public IDetour
{
	virtual void GetAdr(void) const
	{
		std::cout << "| FUN: CMaterialSystem::Init                : 0x" << std::hex << std::uppercase << p_CMaterialSystem__Init.GetPtr() << std::setw(nPad) << " |" << std::endl;
#ifndef DEDICATED
		std::cout << "| FUN: DrawStreamOverlay                    : 0x" << std::hex << std::uppercase << p_DrawStreamOverlay.GetPtr()   << std::setw(nPad) << " |" << std::endl;
		std::cout << "| FUN: StreamDB_Init                        : 0x" << std::hex << std::uppercase << p_StreamDB_Init.GetPtr()       << std::setw(nPad) << " |" << std::endl;
		std::cout << "| VAR: g_pMaterialSystem                    : 0x" << std::hex << std::uppercase << g_pMaterialSystem              << std::setw(0)    << " |" << std::endl;
#endif // !DEDICATED
		std::cout << "+----------------------------------------------------------------+" << std::endl;
	}
	virtual void GetFun(void) const
	{
		p_CMaterialSystem__Init = g_mGameDll.FindPatternSIMD(reinterpret_cast<rsig_t>("\x48\x89\x5C\x24\x00\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x83\xEC\x70\x48\x83\x3D\x00\x00\x00\x00\x00"), "xxxx?xxxxxxxxxxxxxxxxxx?????");
		CMaterialSystem__Init = p_CMaterialSystem__Init.RCast<void* (*)(void*)>(); /*48 89 5C 24 ?? 55 56 57 41 54 41 55 41 56 41 57 48 83 EC 70 48 83 3D ?? ?? ?? ?? ??*/
#ifndef DEDICATED
		p_DrawStreamOverlay = g_mGameDll.FindPatternSIMD(reinterpret_cast<rsig_t>("\x41\x56\xB8\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x48\x2B\xE0\xC6\x02\x00"), "xxx????x????xxxxxx");
		DrawStreamOverlay = p_DrawStreamOverlay.RCast<const char* (*)(void*, uint8_t*, void*, void*)>(); // 41 56 B8 ?? ?? ?? ?? E8 ?? ?? ?? ?? 48 2B E0 C6 02 00 //

		p_StreamDB_Init = g_mGameDll.FindPatternSIMD(reinterpret_cast<rsig_t>("\x48\x89\x5C\x24\x00\x48\x89\x6C\x24\x00\x48\x89\x74\x24\x00\x48\x89\x7C\x24\x00\x41\x54\x41\x56\x41\x57\x48\x83\xEC\x40\x48\x8B\xE9"), "xxxx?xxxx?xxxx?xxxx?xxxxxxxxxxxxx");
		StreamDB_Init = p_StreamDB_Init.RCast<void (*)(const char*)>(); /*48 89 5C 24 ?? 48 89 6C 24 ?? 48 89 74 24 ?? 48 89 7C 24 ?? 41 54 41 56 41 57 48 83 EC 40 48 8B E9*/
#endif // !DEDICATED
	}
	virtual void GetVar(void) const
	{
#ifndef DEDICATED
		g_pMaterialSystem = g_mGameDll.FindPatternSIMD(reinterpret_cast<rsig_t>(
			"\x48\x8B\x0D\x00\x00\x00\x00\x48\x85\xC9\x74\x11\x48\x8B\x01\x48\x8D\x15\x00\x00\x00\x00"), "xxx????xxxxxxxxxxx????").ResolveRelativeAddressSelf(0x3, 0x7).RCast<void*>();

		total_streaming_tex_memory = p_DrawStreamOverlay.Offset(0x0).FindPatternSelf("48 8B 05", CMemory::Direction::DOWN).ResolveRelativeAddressSelf(0x3, 0x7).RCast<int*>();
		unfree_streaming_tex_memory = p_DrawStreamOverlay.Offset(0x20).FindPatternSelf("48 8B 05", CMemory::Direction::DOWN).ResolveRelativeAddressSelf(0x3, 0x7).RCast<int*>();
		unusable_streaming_tex_memory = p_DrawStreamOverlay.Offset(0x50).FindPatternSelf("48 8B 05", CMemory::Direction::DOWN).ResolveRelativeAddressSelf(0x3, 0x7).RCast<int*>();
#endif // !DEDICATED
	}
	virtual void GetCon(void) const { }
	virtual void Attach(void) const { }
	virtual void Detach(void) const { }
};
///////////////////////////////////////////////////////////////////////////////

REGISTER(HMaterialSystem);
