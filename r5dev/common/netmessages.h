#pragma once

struct CNetMessage
{
	void* iNetMessageVTable;
	int m_nGroup;
	bool m_bReliable;
	char padding[3];
	void* m_NetChannel;
};

struct VecNetMessages
{
	CNetMessage** items;
	int64_t m_nAllocationCount;
	int64_t m_nGrowSize;
	int m_Size;
	int padding_;
};

struct VecNetDataFragments
{
	void** items;
	int64_t m_nAllocationCount;
	int64_t m_nGrowSize;
	int m_Size;
	int padding_;
};

//-------------------------------------------------------------------------
// MM_HEARTBEAT
//-------------------------------------------------------------------------
inline CMemory MM_Heartbeat__ToString; // server HeartBeat? (baseserver.cpp).

///////////////////////////////////////////////////////////////////////////////
class HMM_Heartbeat : public IDetour
{
	virtual void GetAdr(void) const
	{
		std::cout << "| FUN: MM_Heartbeat::ToString               : 0x" << std::hex << std::uppercase << MM_Heartbeat__ToString.GetPtr() << std::setw(nPad) << " |" << std::endl;
		std::cout << "+----------------------------------------------------------------+" << std::endl;
	}
	virtual void GetFun(void) const
	{
		MM_Heartbeat__ToString = g_mGameDll.FindPatternSIMD(reinterpret_cast<rsig_t>("\x48\x83\xEC\x38\xE8\x00\x00\x00\x00\x3B\x05\x00\x00\x00\x00"), "xxxxx????xx????");
		// 0x1402312A0 // 48 83 EC 38 E8 ? ? ? ? 3B 05 ? ? ? ? //
	}
	virtual void GetVar(void) const { }
	virtual void GetCon(void) const { }
	virtual void Attach(void) const { }
	virtual void Detach(void) const { }
};
///////////////////////////////////////////////////////////////////////////////

REGISTER(HMM_Heartbeat);
