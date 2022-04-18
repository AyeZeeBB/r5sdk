#pragma once
#include "mathlib/vector.h"
#include "mathlib/color.h"

// Something has to be hardcoded..
#if defined (GAMEDLL_S0) || defined (GAMEDLL_S1) || defined (GAMEDLL_S2)

constexpr auto MATERIALSYSTEM_VCALL_OFF_0 = 0x3F8;
constexpr auto CMATQUEUEDRENDERCONTEXT_VCALL_OFS_0 = 0x278;
constexpr auto CMATQUEUEDRENDERCONTEXT_VCALL_OFS_1 = 0x280;

#elif defined (GAMEDLL_S3)

constexpr auto MATERIALSYSTEM_VCALL_OFF_0 = 0x3F0;
constexpr auto CMATQUEUEDRENDERCONTEXT_VCALL_OFS_0 = 0x288;
constexpr auto CMATQUEUEDRENDERCONTEXT_VCALL_OFS_1 = 0x290;

#endif
constexpr auto CMATQUEUEDRENDERCONTEXT_VCALL_OFS_2 = 0x8;
constexpr auto NDEBUG_PERSIST_TILL_NEXT_SERVER = (0.01023f);

enum class OverlayType_t
{
	OVERLAY_BOX = 0,
	OVERLAY_SPHERE,
	OVERLAY_LINE,
	OVERLAY_TRIANGLE,
	OVERLAY_SWEPT_BOX,
	OVERLAY_BOX2,
	OVERLAY_CAPSULE,
	OVERLAY_UNK0,
	OVERLAY_UNK1
};

struct OverlayBase_t
{
	OverlayBase_t(void)
	{
		m_Type          = OverlayType_t::OVERLAY_BOX;
		m_nServerCount  = -1;
		m_nCreationTick = -1;
		m_flEndTime     = 0.0f;
		m_pNextOverlay  = NULL;
		unk0            = NULL;
	}
	bool IsDead(void) const;

	OverlayType_t   m_Type         {}; // What type of overlay is it?
	int             m_nCreationTick{}; // Duration -1 means go away after this frame #
	float           m_nServerCount {}; // Latch server count, too
	float           m_flEndTime    {}; // When does this box go away
	OverlayBase_t*  m_pNextOverlay {}; // 16
	int64_t         unk0           {}; // 24
};

struct OverlayLine_t : public OverlayBase_t
{
	OverlayLine_t(void) { m_Type = OverlayType_t::OVERLAY_LINE; }

	Vector3         origin{};
	Vector3         dest{};
	int             r{};
	int             g{};
	int             b{};
	int             a{};
	bool            noDepthTest{};
};

struct OverlayBox_t : public OverlayBase_t
{
	OverlayBox_t(void) { m_Type = OverlayType_t::OVERLAY_BOX; }

	Vector3         origin{};
	Vector3         mins{};
	Vector3         maxs{};
	QAngle          angles{};
	int             r{};
	int             g{};
	int             b{};
	int             a{};
};

struct OverlaySphere_t : public OverlayBase_t
{
	OverlaySphere_t(void) { m_Type = OverlayType_t::OVERLAY_SPHERE; }

	Vector3         vOrigin{};
	float           flRadius{};
	int             nTheta{};
	int             nPhi{};
	int             r{};
	int             g{};
	int             b{};
	int             a{};
};

void HDestroyOverlay(OverlayBase_t* pOverlay);
void DrawOverlay(OverlayBase_t* pOverlay);
void DebugOverlays_Attach();
void DebugOverlays_Detach();

inline CMemory p_DrawAllOverlays;
inline auto DrawAllOverlays = p_DrawAllOverlays.RCast<void (*)(char a1)>();

inline CMemory p_RenderBox;
inline auto RenderBox = p_RenderBox.RCast<void* (*)(Vector3 origin, QAngle angles, Vector3 vMins, Vector3 vMaxs, Color color, bool bZBuffer)>();

inline CMemory p_RenderLine;
inline auto RenderLine = p_RenderLine.RCast<void* (*)(Vector3 origin, Vector3 dest, Color color, bool bZBuffer)>();

inline CMemory p_DestroyOverlay;
inline auto DestroyOverlay = p_DestroyOverlay.RCast<void (*)(OverlayBase_t* pOverlay)>();

inline int* client_debugdraw_int_unk = nullptr;
inline float* client_debugdraw_float_unk = nullptr;

inline OverlayBase_t** s_pOverlays = nullptr;
inline LPCRITICAL_SECTION s_OverlayMutex = nullptr;

inline int* render_tickcount = nullptr;
inline int* overlay_tickcount = nullptr;

///////////////////////////////////////////////////////////////////////////////
class HDebugOverlay : public IDetour
{
	virtual void GetAdr(void) const
	{
		std::cout << "| FUN: DrawAllOverlays                      : 0x" << std::hex << std::uppercase << p_DrawAllOverlays.GetPtr() << std::setw(nPad) << " |" << std::endl;
		std::cout << "| FUN: RenderBox                            : 0x" << std::hex << std::uppercase << p_RenderBox.GetPtr()       << std::setw(nPad) << " |" << std::endl;
		std::cout << "| FUN: RenderLine                           : 0x" << std::hex << std::uppercase << p_RenderLine.GetPtr()      << std::setw(nPad) << " |" << std::endl;
		std::cout << "| FUN: DestroyOverlay                       : 0x" << std::hex << std::uppercase << p_DestroyOverlay.GetPtr()  << std::setw(nPad) << " |" << std::endl;
		std::cout << "| VAR: s_pOverlays                          : 0x" << std::hex << std::uppercase << s_pOverlays                << std::setw(0) << " |" << std::endl;
		std::cout << "| VAR: s_OverlayMutex                       : 0x" << std::hex << std::uppercase << s_OverlayMutex             << std::setw(0) << " |" << std::endl;
		std::cout << "| VAR: client_debugdraw_int_unk             : 0x" << std::hex << std::uppercase << client_debugdraw_int_unk   << std::setw(0) << " |" << std::endl;
		std::cout << "| VAR: client_debugdraw_float_unk           : 0x" << std::hex << std::uppercase << client_debugdraw_float_unk << std::setw(0) << " |" << std::endl;
		std::cout << "| VAR: overlay_tickcount                    : 0x" << std::hex << std::uppercase << overlay_tickcount          << std::setw(0) << " |" << std::endl;
		std::cout << "| VAR: render_tickcount                     : 0x" << std::hex << std::uppercase << render_tickcount           << std::setw(0) << " |" << std::endl;
		std::cout << "+----------------------------------------------------------------+" << std::endl;
	}
	virtual void GetFun(void) const
	{
#if defined (GAMEDLL_S0) || defined (GAMEDLL_S1)
		p_DrawAllOverlays = g_mGameDll.FindPatternSIMD(reinterpret_cast<rsig_t>("\x40\x55\x48\x83\xEC\x50\x48\x8B\x05\x00\x00\x00\x00"), "xxxxxxxxx????");
		p_RenderBox       = g_mGameDll.FindPatternSIMD(reinterpret_cast<rsig_t>("\x48\x89\x5C\x24\x00\x48\x89\x6C\x24\x00\x44\x89\x4C\x24\x00"), "xxxx?xxxx?xxxx?");
#elif defined (GAMEDLL_S2) || defined (GAMEDLL_S3)
		p_DrawAllOverlays = g_mGameDll.FindPatternSIMD(reinterpret_cast<rsig_t>("\x40\x55\x48\x83\xEC\x30\x48\x8B\x05\x00\x00\x00\x00\x0F\xB6\xE9"), "xxxxxxxxx????xxx");
		p_RenderBox       = g_mGameDll.FindPatternSIMD(reinterpret_cast<rsig_t>("\x48\x89\x5C\x24\x00\x48\x89\x6C\x24\x00\x44\x89\x4C\x24\x00"), "xxxx?xxxx?xxxx?");
#endif
		p_RenderLine     = g_mGameDll.FindPatternSIMD(reinterpret_cast<rsig_t>("\x48\x89\x74\x24\x00\x44\x89\x44\x24\x00\x57\x41\x56"), "xxxx?xxxx?xxx");
		p_DestroyOverlay = g_mGameDll.FindPatternSIMD(reinterpret_cast<rsig_t>("\x40\x53\x48\x83\xEC\x20\x48\x8B\xD9\x48\x8D\x0D\x00\x00\x00\x00\xFF\x15\x00\x00\x00\x00\x48\x63\x03"), "xxxxxxxxxxxx????xx????xxx");

		DrawAllOverlays = p_DrawAllOverlays.RCast<void (*)(char)>();                                /*40 55 48 83 EC 30 48 8B 05 ?? ?? ?? ?? 0F B6 E9*/
		DestroyOverlay = p_DestroyOverlay.RCast<void (*)(OverlayBase_t*)>();                        /*40 53 48 83 EC 20 48 8B D9 48 8D 0D ?? ?? ?? ?? FF 15 ?? ?? ?? ?? 48 63 03 */
		RenderBox = p_RenderBox.RCast<void* (*)(Vector3, QAngle, Vector3, Vector3, Color, bool)>(); /*48 89 5C 24 ?? 48 89 6C 24 ?? 44 89 4C 24 ??*/
		RenderLine = p_RenderLine.RCast<void* (*)(Vector3, Vector3, Color, bool)>();                /*48 89 74 24 ?? 44 89 44 24 ?? 57 41 56*/
	}
	virtual void GetVar(void) const
	{
		client_debugdraw_int_unk = p_DrawAllOverlays.Offset(0xC0).FindPatternSelf("F3 0F 59", CMemory::Direction::DOWN, 150).ResolveRelativeAddressSelf(0x4, 0x8).RCast<int*>();
		client_debugdraw_float_unk = p_DrawAllOverlays.Offset(0xD0).FindPatternSelf("F3 0F 10", CMemory::Direction::DOWN, 150).ResolveRelativeAddressSelf(0x4, 0x8).RCast<float*>();

		s_pOverlays = p_DrawAllOverlays.Offset(0x10).FindPatternSelf("48 8B 3D", CMemory::Direction::DOWN, 150).ResolveRelativeAddressSelf(0x3, 0x7).RCast<OverlayBase_t**>();
		s_OverlayMutex = p_DrawAllOverlays.Offset(0x10).FindPatternSelf("48 8D 0D", CMemory::Direction::DOWN, 150).ResolveRelativeAddressSelf(0x3, 0x7).RCast<LPCRITICAL_SECTION>();

#if defined (GAMEDLL_S0) || defined (GAMEDLL_S1)
		render_tickcount = p_DrawAllOverlays.Offset(0x80).FindPatternSelf("3B 0D", CMemory::Direction::DOWN, 150).ResolveRelativeAddressSelf(0x2, 0x6).RCast<int*>();
		overlay_tickcount = p_DrawAllOverlays.Offset(0x70).FindPatternSelf("3B 0D", CMemory::Direction::DOWN, 150).ResolveRelativeAddressSelf(0x2, 0x6).RCast<int*>();
#elif defined (GAMEDLL_S2) || defined (GAMEDLL_S3)
		render_tickcount = p_DrawAllOverlays.Offset(0x50).FindPatternSelf("3B 05", CMemory::Direction::DOWN, 150).ResolveRelativeAddressSelf(0x2, 0x6).RCast<int*>();
		overlay_tickcount = p_DrawAllOverlays.Offset(0x70).FindPatternSelf("3B 05", CMemory::Direction::DOWN, 150).ResolveRelativeAddressSelf(0x2, 0x6).RCast<int*>();
#endif
	}
	virtual void GetCon(void) const { }
	virtual void Attach(void) const { }
	virtual void Detach(void) const { }
};
///////////////////////////////////////////////////////////////////////////////

REGISTER(HDebugOverlay);
