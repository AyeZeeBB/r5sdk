#pragma once
#ifndef DEDICATED
#include "common/sdkdefs.h"
#include "windows/resource.h"
#include "networksystem/serverlisting.h"
#include "networksystem/pylon.h"
#include "thirdparty/imgui/misc/imgui_utility.h"

#include "imgui_surface.h"

class CBrowser : public CImguiSurface
{
public:
    CBrowser(void);
    virtual ~CBrowser(void);

    virtual bool Init(void);

    virtual void RunFrame(void);
    void RunTask(void);

    virtual bool DrawSurface(void);

    void BrowserPanel(void);
    void RefreshServerList(void);

    void HiddenServersModal(void);
    void HostPanel(void);

    void UpdateHostingStatus(void);
    void InstallHostingDetails(const bool postFailed, const char* const hostMessage, const char* const hostToken, const string& hostIp);
    void SendHostingPostRequest(const NetGameServer_t& gameServer);

    void ProcessCommand(const char* pszCommand) const;

public:
    // Command callbacks
    static void ToggleBrowser_f();

private:
    inline void SetServerListMessage(const char* const message) { m_svServerListMessage = message; };
    inline void SetHostMessage(const char* const message) { m_svHostMessage = message; }
    inline void SetHostToken(const char* const message) { m_svHostToken = message; }

private:
    bool m_bReclaimFocusTokenField;
    bool m_bQueryListNonRecursive; // When set, refreshes the server list once the next frame.
    bool m_bQueryGlobalBanList;
    char m_szServerAddressBuffer[128];
    char m_szServerEncKeyBuffer[30];

    ID3D11ShaderResourceView* m_idLockedIcon;
    MODULERESOURCE m_rLockedIconBlob;

    ////////////////////
    //   Server List  //
    ////////////////////
    ImGuiTextFilter m_imServerBrowserFilter;
    string m_svServerListMessage;

    ////////////////////
    //   Host Server  //
    ////////////////////
    string m_svHostMessage;
    string m_svHostToken;
    ImVec4 m_HostMessageColor;

    ////////////////////
    // Private Server //
    ////////////////////
    string m_svHiddenServerRequestMessage;
    ImVec4 m_ivHiddenServerMessageColor;
};

extern CBrowser g_Browser;
#endif