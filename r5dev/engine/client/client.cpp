//===============================================================================//
//
// Purpose: 
//
// $NoKeywords: $
//
//===============================================================================//
// client.cpp: implementation of the CClient class.
//
///////////////////////////////////////////////////////////////////////////////////
#include "core/stdafx.h"
#include "engine/server/server.h"
#include "engine/client/client.h"

//---------------------------------------------------------------------------------
// Purpose: gets the client from buffer by index
//---------------------------------------------------------------------------------
CClient* CClient::GetClient(int nIndex) const
{
	return reinterpret_cast<CClient*>(
		(reinterpret_cast<uintptr_t>(g_pClient) + (nIndex * sizeof(CClient))));
}

//---------------------------------------------------------------------------------
// Purpose: gets the handle of this client
//---------------------------------------------------------------------------------
uint16_t CClient::GetHandle(void) const
{
	return m_nHandle;
}

//---------------------------------------------------------------------------------
// Purpose: gets the userID of this client
//---------------------------------------------------------------------------------
uint32_t CClient::GetUserID(void) const
{
	return m_nUserID;
}

//---------------------------------------------------------------------------------
// Purpose: gets the nucleusID of this client
//---------------------------------------------------------------------------------
uint64_t CClient::GetNucleusID(void) const
{
	return m_nNucleusID;
}

//---------------------------------------------------------------------------------
// Purpose: gets the signon state of this client
//---------------------------------------------------------------------------------
SIGNONSTATE CClient::GetSignonState(void) const
{
	return m_nSignonState;
}

//---------------------------------------------------------------------------------
// Purpose: gets the persistence state of this client
//---------------------------------------------------------------------------------
PERSISTENCE CClient::GetPersistenceState(void) const
{
	return m_nPersistenceState;
}

//---------------------------------------------------------------------------------
// Purpose: gets the net channel of this client
//---------------------------------------------------------------------------------
CNetChan* CClient::GetNetChan(void) const
{
	return m_NetChannel;
}

//---------------------------------------------------------------------------------
// Purpose: gets the name of this client (managed by server)
//---------------------------------------------------------------------------------
const char* CClient::GetServerName(void) const
{
	return m_szServerName;
}

//---------------------------------------------------------------------------------
// Purpose: gets the name of this client (obtained from connectionless packet)
//---------------------------------------------------------------------------------
const char* CClient::GetClientName(void) const
{
	return m_szClientName;
}

//---------------------------------------------------------------------------------
// Purpose: sets the handle of this client
//---------------------------------------------------------------------------------
void CClient::SetHandle(uint16_t nHandle)
{
	m_nHandle = nHandle;
}

//---------------------------------------------------------------------------------
// Purpose: sets the userID of this client
//---------------------------------------------------------------------------------
void CClient::SetUserID(uint32_t nUserID)
{
	m_nUserID = nUserID;
}

//---------------------------------------------------------------------------------
// Purpose: sets the nucleusID of this client
//---------------------------------------------------------------------------------
void CClient::SetNucleusID(uint64_t nNucleusID)
{
	m_nNucleusID = nNucleusID;
}

//---------------------------------------------------------------------------------
// Purpose: sets the signon state of this client
//---------------------------------------------------------------------------------
void CClient::SetSignonState(SIGNONSTATE nSignonState)
{
	m_nSignonState = nSignonState;
}

//---------------------------------------------------------------------------------
// Purpose: sets the persistence state of this client
//---------------------------------------------------------------------------------
void CClient::SetPersistenceState(PERSISTENCE nPersistenceState)
{
	m_nPersistenceState = nPersistenceState;
}

//---------------------------------------------------------------------------------
// Purpose: sets the net channel of this client
// !TODO  : Remove this and rebuild INetChannel
//---------------------------------------------------------------------------------
void CClient::SetNetChan(CNetChan* pNetChan)
{
	m_NetChannel = pNetChan;
}

//---------------------------------------------------------------------------------
// Purpose: checks if client is connected to server
// Output : true if connected, false otherwise
//---------------------------------------------------------------------------------
bool CClient::IsConnected(void) const
{
	return m_nSignonState >= SIGNONSTATE::SIGNONSTATE_CONNECTED;
}

//---------------------------------------------------------------------------------
// Purpose: checks if client is spawned to server
// Output : true if connected, false otherwise
//---------------------------------------------------------------------------------
bool CClient::IsSpawned(void) const
{
	return m_nSignonState >= SIGNONSTATE::SIGNONSTATE_NEW;
}

//---------------------------------------------------------------------------------
// Purpose: checks if client is active to server
// Output : true if connected, false otherwise
//---------------------------------------------------------------------------------
bool CClient::IsActive(void) const
{
	return m_nSignonState == SIGNONSTATE::SIGNONSTATE_FULL;
}

//---------------------------------------------------------------------------------
// Purpose: checks if client's persistence data is available
// Output : true if available, false otherwise
//---------------------------------------------------------------------------------
bool CClient::IsPersistenceAvailable(void) const
{
	return m_nPersistenceState >= PERSISTENCE::PERSISTENCE_AVAILABLE;
}

//---------------------------------------------------------------------------------
// Purpose: checks if client's persistence data is ready
// Output : true if ready, false otherwise
//---------------------------------------------------------------------------------
bool CClient::IsPersistenceReady(void) const
{
	return m_nPersistenceState == PERSISTENCE::PERSISTENCE_READY;
}

//---------------------------------------------------------------------------------
// Purpose: checks if client is a fake client
// Output : true if connected, false otherwise
//---------------------------------------------------------------------------------
bool CClient::IsFakeClient(void) const
{
	return m_bFakePlayer;
}

//---------------------------------------------------------------------------------
// Purpose: checks if this client is an actual human player
// Output : true if human, false otherwise
//---------------------------------------------------------------------------------
bool CClient::IsHumanPlayer(void) const
{
	if (!IsConnected())
		return false;

	if (IsFakeClient())
		return false;

	return true;
}

//---------------------------------------------------------------------------------
// Purpose: throw away any residual garbage in the channel
//---------------------------------------------------------------------------------
void CClient::Clear(void)
{
	g_ServerPlayer[GetUserID()].Reset(); // Reset ServerPlayer slot.
	v_CClient_Clear(this);
}

//---------------------------------------------------------------------------------
// Purpose: throw away any residual garbage in the channel
// Input  : *pClient - 
//---------------------------------------------------------------------------------
void CClient::VClear(CClient* pClient)
{
	g_ServerPlayer[pClient->GetUserID()].Reset(); // Reset ServerPlayer slot.
	v_CClient_Clear(pClient);
}

//---------------------------------------------------------------------------------
// Purpose: connect new client
// Input  : *szName - 
//			*pNetChannel - 
//			bFakePlayer - 
//			*a5 - 
//			*szMessage -
//			nMessageSize - 
// Output : true if connection was successful, false otherwise
//---------------------------------------------------------------------------------
bool CClient::Connect(const char* szName, void* pNetChannel, bool bFakePlayer, void* a5, char* szMessage, int nMessageSize)
{
	return v_CClient_Connect(this, szName, pNetChannel, bFakePlayer, a5, szMessage, nMessageSize);
}

//---------------------------------------------------------------------------------
// Purpose: connect new client
// Input  : *pClient - 
//			*szName - 
//			*pNetChannel - 
//			bFakePlayer - 
//			*a5 - 
//			*szMessage -
//			nMessageSize - 
// Output : true if connection was successful, false otherwise
//---------------------------------------------------------------------------------
bool CClient::VConnect(CClient* pClient, const char* szName, void* pNetChannel, bool bFakePlayer, void* a5, char* szMessage, int nMessageSize)
{
	bool bResult = v_CClient_Connect(pClient, szName, pNetChannel, bFakePlayer, a5, szMessage, nMessageSize);
	g_ServerPlayer[pClient->GetUserID()].Reset(); // Reset ServerPlayer slot.
	return bResult;
}

//---------------------------------------------------------------------------------
// Purpose: disconnect client
// Input  : nRepLvl - 
//			*szReason - 
//			... - 
//---------------------------------------------------------------------------------
void CClient::Disconnect(const Reputation_t nRepLvl, const char* szReason, ...)
{
	if (m_nSignonState != SIGNONSTATE::SIGNONSTATE_NONE)
	{
		char szBuf[1024];
		{/////////////////////////////
			va_list vArgs{};
			va_start(vArgs, szReason);

			vsnprintf(szBuf, sizeof(szBuf), szReason, vArgs);

			szBuf[sizeof(szBuf) - 1] = '\0';
			va_end(vArgs);
		}/////////////////////////////
		v_CClient_Disconnect(this, nRepLvl, szBuf);
	}
}

///////////////////////////////////////////////////////////////////////////////////
void CBaseClient_Attach()
{
	DetourAttach((LPVOID*)&v_CClient_Clear, &CClient::VClear);
	DetourAttach((LPVOID*)&v_CClient_Connect, &CClient::VConnect);
}
void CBaseClient_Detach()
{
	DetourDetach((LPVOID*)&v_CClient_Clear, &CClient::VClear);
	DetourDetach((LPVOID*)&v_CClient_Connect, &CClient::VConnect);
}

///////////////////////////////////////////////////////////////////////////////
CClient* g_pClient = nullptr;