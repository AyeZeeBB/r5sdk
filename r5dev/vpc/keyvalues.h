#pragma once

#define MAKE_3_BYTES_FROM_1_AND_2( x1, x2 ) (( (( uint16_t )x2) << 8 ) | (uint8_t)(x1))
extern vector<string> g_vAllPlaylists;

//---------------------------------------------------------------------------------
// Purpose: Forward declarations
//---------------------------------------------------------------------------------
class KeyValues;

/* ==== KEYVALUES ======================================================================================================================================================= */
inline CMemory p_KeyValues_Init;
inline auto KeyValues_Init = p_KeyValues_Init.RCast<void* (*)(KeyValues* thisptr, const char* pSymbol, int64_t a3, bool bCreate)>();

inline CMemory p_KeyValues_FindKey;
inline auto KeyValues_FindKey = p_KeyValues_FindKey.RCast<void* (*)(KeyValues* thisptr, const char* pkeyName, bool bCreate)>();

inline CMemory p_KeyValues_GetCurrentPlaylist;
inline auto KeyValues_GetCurrentPlaylist = p_KeyValues_GetCurrentPlaylist.RCast<const char* (*)(void)>();

inline CMemory p_KeyValues_LoadPlaylist;
inline auto KeyValues_LoadPlaylist = p_KeyValues_LoadPlaylist.RCast<bool (*)(const char* pszPlaylist)>();

enum KeyValuesTypes
{
	TYPE_NONE              = 0x0,
	TYPE_STRING            = 0x1,
	TYPE_INT               = 0x2,
	TYPE_FLOAT             = 0x3,
	TYPE_PTR               = 0x4,
	TYPE_WSTRING           = 0x5,
	TYPE_COLOR             = 0x6,
	TYPE_UINT64            = 0x7,
	TYPE_COMPILED_INT_BYTE = 0x8,
	TYPE_COMPILED_INT_0    = 0x9,
	TYPE_COMPILED_INT_1    = 0xA,
	TYPE_NUMTYPES          = 0xB,
};

class KeyValues
{
public:
	static void Init(void);
	KeyValues* FindKey(const char* pKeyName, bool bCreate);
	const char* GetName(void) const;
	int GetInt(const char* pKeyName, int nDefaultValue);
	void SetInt(const char* pKeyName, int iValue);
	void SetFloat(const char* keyName, float flValue);

	static void InitPlaylist(void);
	static bool LoadPlaylist(const char* szPlaylist);

public:
	uint32_t m_iKeyName              : 24;         // 0x0000
	uint32_t m_iKeyNameCaseSensitive : 8;          // 0x0003
	char*            m_sValue;                     // 0x0008
	wchar_t*         m_wsValue;                    // 0x0010
	union                                          // 0x0018
	{
		int m_iValue;
		float m_flValue;
		void* m_pValue;
		unsigned char m_Color[4];
	};
	char             m_szShortName[8];             // 0x0020
	char             m_iDataType;                  // 0x0028
	uint16_t         m_iKeyNameCaseSensitive2;     // 0x002A
	KeyValues*       m_pPeer;                      // 0x0030
	KeyValues*       m_pSub;                       // 0x0038
	KeyValues*       m_pChain;                     // 0x0040
};

///////////////////////////////////////////////////////////////////////////////
void CKeyValueSystem_Attach();
void CKeyValueSystem_Detach();

///////////////////////////////////////////////////////////////////////////////
extern KeyValues** g_pPlaylistKeyValues;

///////////////////////////////////////////////////////////////////////////////
class VKeyValues : public IDetour
{
	virtual void GetAdr(void) const
	{
		spdlog::debug("| FUN: KeyValues::Init                      : {:#18x} |\n", p_KeyValues_Init.GetPtr());
		spdlog::debug("| FUN: KeyValues::FindKey                   : {:#18x} |\n", p_KeyValues_FindKey.GetPtr());
		spdlog::debug("| FUN: KeyValues::LoadPlaylist              : {:#18x} |\n", p_KeyValues_LoadPlaylist.GetPtr());
		spdlog::debug("| FUN: KeyValues::GetCurrentPlaylist        : {:#18x} |\n", p_KeyValues_GetCurrentPlaylist.GetPtr());
		spdlog::debug("| VAR: g_pPlaylistKeyValues                 : {:#18x} |\n", reinterpret_cast<uintptr_t>(g_pPlaylistKeyValues));
		spdlog::debug("+----------------------------------------------------------------+\n");
	}
	virtual void GetFun(void) const
	{
#if defined (GAMEDLL_S0) || defined (GAMEDLL_S1)
		p_KeyValues_Init               = g_mGameDll.FindPatternSIMD(reinterpret_cast<rsig_t>("\x40\x53\x48\x83\xEC\x20\x48\x8B\xD9\xC7\x44\x24\x30\xFF\xFF\xFF"), "xxxxxxxxxxxxxxxx");
		p_KeyValues_FindKey            = g_mGameDll.FindPatternSIMD(reinterpret_cast<rsig_t>("\x48\x89\x5C\x24\x10\x48\x89\x6C\x24\x18\x48\x89\x74\x24\x20\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x81\xEC\x20\x01\x00\x00\x45"), "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		p_KeyValues_GetCurrentPlaylist = g_mGameDll.FindPatternSIMD(reinterpret_cast<rsig_t>("\x48\x8B\x0D\x00\x00\x00\x00\x48\x85\xC9\x75\x08\x48\x8D\x05\x00\x00\x00\x00"), "xxx????xxxxxxxx????");
#elif defined (GAMEDLL_S2) || defined (GAMEDLL_S3)
		p_KeyValues_Init               = g_mGameDll.FindPatternSIMD(reinterpret_cast<rsig_t>("\x40\x53\x48\x83\xEC\x20\x48\x8B\x05\x00\x00\x00\x01\x48\x8B\xD9\x4C\x8B\xC2"), "xxxxxxxxx???xxxxxxx");
		p_KeyValues_FindKey            = g_mGameDll.FindPatternSIMD(reinterpret_cast<rsig_t>("\x40\x56\x57\x41\x57\x48\x81\xEC\x00\x00\x00\x00\x45"), "xxxxxxxx????x");
		p_KeyValues_GetCurrentPlaylist = g_mGameDll.FindPatternSIMD(reinterpret_cast<rsig_t>("\x48\x8B\x05\x00\x00\x00\x00\x48\x85\xC0\x75\x08\x48\x8D\x05\x00\x00\x00\x00\xC3\x0F\xB7\x50\x2A"), "xxx????xxxxxxxx????xxxxx");
#endif
		p_KeyValues_LoadPlaylist       = g_mGameDll.FindPatternSIMD(reinterpret_cast<rsig_t>("\xE8\x00\x00\x00\x00\x80\x3D\x00\x00\x00\x00\x00\x74\x0C"), "x????xx?????xx").FollowNearCallSelf().GetPtr();

		KeyValues_Init               = p_KeyValues_Init.RCast<void* (*)(KeyValues*, const char*, int64_t, bool)>(); /*40 53 48 83 EC 20 48 8B 05 ?? ?? ?? 01 48 8B D9 4C 8B C2*/
		KeyValues_FindKey            = p_KeyValues_FindKey.RCast<void* (*)(KeyValues*, const char*, bool)>();       /*40 56 57 41 57 48 81 EC 30 01 00 00 45 0F B6 F8*/
		KeyValues_LoadPlaylist       = p_KeyValues_LoadPlaylist.RCast<bool (*)(const char*)>();                     /*E8 ?? ?? ?? ?? 80 3D ?? ?? ?? ?? ?? 74 0C*/
		KeyValues_GetCurrentPlaylist = p_KeyValues_GetCurrentPlaylist.RCast<const char* (*)(void)>();               /*48 8B 05 ?? ?? ?? ?? 48 85 C0 75 08 48 8D 05 ?? ?? ?? ?? C3 0F B7 50 2A*/

	}
	virtual void GetVar(void) const
	{
#if defined (GAMEDLL_S0) || defined (GAMEDLL_S1)
		g_pPlaylistKeyValues = g_mGameDll.FindPatternSIMD(reinterpret_cast<rsig_t>(
			"\x48\x8B\xC4\x53\x57\x41\x56\x48\x81\xEC\x20"), 
			"xxxxxxxxxxx").FindPatternSelf("48 8B 2D", CMemory::Direction::DOWN, 100).ResolveRelativeAddressSelf(0x3, 0x7).RCast<KeyValues**>();
#elif defined (GAMEDLL_S2) || defined (GAMEDLL_S3)
		g_pPlaylistKeyValues = g_mGameDll.FindPatternSIMD(reinterpret_cast<rsig_t>(
			"\x48\x89\x5C\x24\x08\x48\x89\x6C\x24\x10\x48\x89\x74\x24\x18\x57\x48\x83\xEC\x20\x48\x8B\xF9\xE8\xB4"), 
			"xxxxxxxxxxxxxxxxxxxxxxxxx").FindPatternSelf("48 8B 0D", CMemory::Direction::DOWN, 100).ResolveRelativeAddressSelf(0x3, 0x7).RCast<KeyValues**>();
#endif
	}
	virtual void GetCon(void) const { }
	virtual void Attach(void) const { }
	virtual void Detach(void) const { }
};
///////////////////////////////////////////////////////////////////////////////

REGISTER(VKeyValues);
