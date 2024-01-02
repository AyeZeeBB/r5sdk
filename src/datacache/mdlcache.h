#ifndef MDLCACHE_H
#define MDLCACHE_H
#include "tier0/threadtools.h"
#include "tier1/utldict.h"
#include "datacache/idatacache.h"
#include "datacache/imdlcache.h"
#include "public/studio.h"

struct RStaticProp_t
{
	studiohdr_t* m_pStudioHDR{};
	CStudioHWDataRef* m_pHardWareData{};
	const char* m_szPropName{};
	uint8_t m_pUnknown[0x62]{};
};

struct RMDLFallBack_t
{
	studiohdr_t* m_pErrorHDR;
	studiohdr_t* m_pEmptyHDR;
	MDLHandle_t m_hErrorMDL;
	MDLHandle_t m_hEmptyMDL;

	RMDLFallBack_t(void)
		: m_pErrorHDR(nullptr)
		, m_pEmptyHDR(nullptr)
		, m_hErrorMDL(NULL)
		, m_hEmptyMDL(NULL)
	{
	}

	// This must be cleared if 'common.rpak' is getting unloaded!
	void Clear(void)
	{
		m_pErrorHDR = nullptr;
		m_pEmptyHDR = nullptr;
		m_hErrorMDL = NULL;
		m_hEmptyMDL = NULL;
	}
};

// only models with type "mod_studio" have this data
struct studiodata_t
{
	DataCacheHandle_t m_MDLCache;
	void* m_pAnimData; // !TODO: reverse struct.
	unsigned short m_nRefCount;
	unsigned short m_nFlags;
	MDLHandle_t m_Handle;
	void* Unk3; // ptr to flags and model string.
	CStudioHWDataRef* m_pHardwareRef;
	void* m_pMaterialTable; // contains a large table of CMaterialGlue objects.
	int Unk5;
	char pad[72];
	CThreadFastMutex m_Mutex;
	int m_nGuidLock; // always -1, set to 1 and 0 in CMDLCache::FindUncachedMDL.
};

extern RMDLFallBack_t* g_pMDLFallback;
extern std::unordered_set<MDLHandle_t> g_vBadMDLHandles;

class CMDLCache : public IMDLCache
{
public:
	static studiohdr_t* FindMDL(CMDLCache* cache, MDLHandle_t handle, void* a3);
	static void FindCachedMDL(CMDLCache* cache, studiodata_t* pStudioData, void* a3);
	static studiohdr_t* FindUncachedMDL(CMDLCache* cache, MDLHandle_t handle, studiodata_t* pStudioData, void* a4);
	static studiohdr_t* GetStudioHDR(CMDLCache* cache, MDLHandle_t handle);
	static studiohwdata_t* GetHardwareData(CMDLCache* cache, MDLHandle_t handle);
	static studiohdr_t* GetErrorModel(void);
	static bool IsKnownBadModel(MDLHandle_t handle);


	studiodata_t* GetStudioData(MDLHandle_t handle)
	{
		EnterCriticalSection(&m_MDLMutex);
		studiodata_t* pStudioData = m_MDLDict.Element(handle);
		LeaveCriticalSection(&m_MDLMutex);

		return pStudioData;
	}

	const char* GetModelName(MDLHandle_t handle)
	{
		EnterCriticalSection(&m_MDLMutex);
		const char* szModelName = m_MDLDict.GetElementName(handle);
		LeaveCriticalSection(&m_MDLMutex);

		return szModelName;
	}

	void* GetMaterialTable(MDLHandle_t handle)
	{
		EnterCriticalSection(&m_MDLMutex);
		studiodata_t* pStudioData = m_MDLDict.Element(handle);
		LeaveCriticalSection(&m_MDLMutex);

		return &pStudioData->m_pMaterialTable;
	}

private:
	CUtlDict<studiodata_t*, MDLHandle_t> m_MDLDict;
	CRITICAL_SECTION m_MDLMutex;
	// !TODO: reverse the rest
};

inline studiohdr_t*(*CMDLCache__FindMDL)(CMDLCache* pCache, MDLHandle_t handle, void* a3);
inline void(*CMDLCache__FindCachedMDL)(CMDLCache* pCache, studiodata_t* pStudioData, void* a3);

inline studiohdr_t*(*CMDLCache__FindUncachedMDL)(CMDLCache* pCache, MDLHandle_t handle, studiodata_t* pStudioData, void* a4);
inline studiohdr_t*(*CMDLCache__GetStudioHDR)(CMDLCache* pCache, MDLHandle_t handle);

inline studiohwdata_t*(*CMDLCache__GetHardwareData)(CMDLCache* pCache, MDLHandle_t handle);
inline bool(*CStudioHWDataRef__SetFlags)(CStudioHWDataRef* ref, int64_t flags); // Probably incorrect name.

inline CMDLCache* g_pMDLCache = nullptr;
inline PSRWLOCK g_pMDLLock = nullptr; // Possibly a member? research required.

///////////////////////////////////////////////////////////////////////////////
class VMDLCache : public IDetour
{
	virtual void GetAdr(void) const
	{
		LogFunAdr("CMDLCache::FindMDL", CMDLCache__FindMDL);
		LogFunAdr("CMDLCache::FindCachedMDL", CMDLCache__FindCachedMDL);
		LogFunAdr("CMDLCache::FindUncachedMDL", CMDLCache__FindUncachedMDL);
		LogFunAdr("CMDLCache::GetStudioHDR", CMDLCache__GetStudioHDR);
		LogFunAdr("CMDLCache::GetHardwareData", CMDLCache__GetHardwareData);
		LogFunAdr("CStudioHWDataRef::SetFlags", CStudioHWDataRef__SetFlags);

		LogVarAdr("g_MDLCache", g_pMDLCache);
		LogVarAdr("g_MDLLock", g_pMDLLock);
	}
	virtual void GetFun(void) const
	{
		g_GameDll.FindPatternSIMD("48 89 5C 24 ?? 48 89 6C 24 ?? 48 89 74 24 ?? 57 48 83 EC 20 48 8B F1 0F B7 EA").GetPtr(CMDLCache__FindMDL);
		g_GameDll.FindPatternSIMD("4D 85 C0 74 7A 48 89 6C 24 ??").GetPtr(CMDLCache__FindCachedMDL);
		g_GameDll.FindPatternSIMD("48 89 5C 24 ?? 48 89 6C 24 ?? 48 89 74 24 ?? 48 89 7C 24 ?? 41 56 48 83 EC 20 48 8B E9 0F B7 FA").GetPtr(CMDLCache__FindUncachedMDL);
		g_GameDll.FindPatternSIMD("40 53 48 83 EC 20 48 8D 0D ?? ?? ?? ?? 0F B7 DA FF 15 ?? ?? ?? ?? 48 8B 05 ?? ?? ?? ?? 48 8D 14 5B 48 8D 0D ?? ?? ?? ?? 48 8B 5C D0 ?? FF 15 ?? ?? ?? ?? 48 8B 03 48 8B 48 08").GetPtr(CMDLCache__GetStudioHDR);
		g_GameDll.FindPatternSIMD("48 89 5C 24 ?? 57 48 83 EC 20 48 8D 0D ?? ?? ?? ?? 0F B7 DA FF 15 ?? ?? ?? ?? 48 8B 05 ?? ?? ?? ?? 48 8D 14 5B 48 8D 0D ?? ?? ?? ?? 48 8B 7C D0 ?? FF 15 ?? ?? ?? ?? 48 8B 1F").GetPtr(CMDLCache__GetHardwareData);
		g_GameDll.FindPatternSIMD("48 83 EC 08 4C 8D 14 12").GetPtr(CStudioHWDataRef__SetFlags);
	}
	virtual void GetVar(void) const
	{
		// Get MDLCache singleton from CStudioRenderContext::Connect
		g_pMDLCache = g_GameDll.FindPatternSIMD("48 83 EC 28 48 8B 05 ?? ?? ?? ?? 48 8D 0D ?? ?? ?? ?? 48 85 C0 48 0F 45 C8 FF 05 ?? ?? ?? ?? 48 83 3D ?? ?? ?? ?? ?? 48 8D 05 ?? ?? ?? ??")
			.FindPatternSelf("48 8D 05").ResolveRelativeAddressSelf(0x3, 0x7).RCast<CMDLCache*>();

		g_pMDLLock = CMemory(CMDLCache__GetHardwareData).Offset(0x35).FindPatternSelf("48 8D 0D").ResolveRelativeAddressSelf(0x3, 0x7).RCast<PSRWLOCK>();
	}
	virtual void GetCon(void) const { }
	virtual void Detour(const bool bAttach) const;
};
///////////////////////////////////////////////////////////////////////////////

#endif // MDLCACHE_H
