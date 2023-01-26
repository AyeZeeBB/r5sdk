//=====================================================================================//
//
// Purpose: Implementation of the pylon server backend.
//
// $NoKeywords: $
//=====================================================================================//

#include <core/stdafx.h>
#include <tier1/cvar.h>
#include <tier2/curlutils.h>
#include <networksystem/pylon.h>
#ifndef CLIENT_DLL
#include <engine/server/server.h>
#endif // !CLIENT_DLL

//-----------------------------------------------------------------------------
// Purpose: returns a vector of hosted servers.
// Input  : &svOutMessage - 
// Output : vector<NetGameServer_t>
//-----------------------------------------------------------------------------
vector<NetGameServer_t> CPylon::GetServerList(string& svOutMessage) const
{
    vector<NetGameServer_t> vslList;

    nlohmann::json jsRequestBody = nlohmann::json::object();
    jsRequestBody["version"] = SDK_VERSION;

    string svRequestBody = jsRequestBody.dump(4);
    string svResponse;

    if (pylon_showdebuginfo->GetBool())
    {
        DevMsg(eDLL_T::ENGINE, "%s - Sending server list request to comp-server:\n%s\n", __FUNCTION__, svRequestBody.c_str());
    }

    CURLINFO status;
    if (!QueryMasterServer(pylon_matchmaking_hostname->GetString(), "/servers", svRequestBody, svResponse, svOutMessage, status))
    {
        return vslList;
    }

    try
    {
        if (status == 200) // STATUS_OK
        {
            nlohmann::json jsResultBody = nlohmann::json::parse(svResponse);
            if (jsResultBody["success"].is_boolean() && jsResultBody["success"].get<bool>())
            {
                for (auto& obj : jsResultBody["servers"])
                {
                    vslList.push_back(
                        NetGameServer_t
                        {
                            obj.value("name",""),
                            obj.value("description",""),
                            obj.value("hidden","false") == "true",
                            obj.value("map",""),
                            obj.value("playlist",""),
                            obj.value("ip",""),
                            obj.value("port", ""),
                            obj.value("key",""),
                            obj.value("checksum",""),
                            obj.value("version", SDK_VERSION),
                            obj.value("playerCount", ""),
                            obj.value("maxPlayers", ""),
                            obj.value("timeStamp", 0),
                            obj.value("publicRef", ""),
                            obj.value("cachedId", ""),
                        }
                    );
                }
            }
            else
            {
                if (jsResultBody["error"].is_string())
                {
                    svOutMessage = jsResultBody["error"].get<string>();
                }
                else
                {
                    svOutMessage = string("Unknown error with status: ") + std::to_string(status);
                }
            }
        }
        else
        {
            if (status)
            {
                if (!svResponse.empty())
                {
                    nlohmann::json jsResultBody = nlohmann::json::parse(svResponse);

                    if (jsResultBody["error"].is_string())
                    {
                        svOutMessage = jsResultBody["error"].get<string>();
                    }
                    else
                    {
                        svOutMessage = string("Failed HTTP request: ") + std::to_string(status);
                    }

                    return vslList;
                }

                svOutMessage = string("Failed HTTP request: ") + std::to_string(status);
                return vslList;
            }

            svOutMessage = "Failed to reach comp-server: connection timed-out";
            return vslList;
        }
    }
    catch (const std::exception& ex)
    {
        Warning(eDLL_T::ENGINE, "%s - Exception while parsing comp-server response:\n%s\n", __FUNCTION__, ex.what());
    }

    return vslList;
}

//-----------------------------------------------------------------------------
// Purpose: Gets the server by token string.
// Input  : &slOutServer - 
//			&svOutMessage - 
//			&svToken - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CPylon::GetServerByToken(NetGameServer_t& slOutServer, string& svOutMessage, const string& svToken) const
{
    nlohmann::json jsRequestBody = nlohmann::json::object();
    jsRequestBody["token"] = svToken;

    string svRequestBody = jsRequestBody.dump(4);
    string svResponseBuf;

    if (pylon_showdebuginfo->GetBool())
    {
        DevMsg(eDLL_T::ENGINE, "%s - Sending token connect request to comp-server:\n%s\n", __FUNCTION__, svRequestBody.c_str());
    }

    CURLINFO status;
    if (!QueryMasterServer(pylon_matchmaking_hostname->GetString(), "/server/byToken", svRequestBody, svResponseBuf, svOutMessage, status))
    {
        return false;
    }

    if (pylon_showdebuginfo->GetBool())
    {
        DevMsg(eDLL_T::ENGINE, "%s - Comp-server replied with status: '%d'\n", __FUNCTION__, status);
        try
        {
            string jsResultBody = nlohmann::json::parse(svResponseBuf).dump(4);
            DevMsg(eDLL_T::ENGINE, "%s - Comp-server response body:\n%s\n", __FUNCTION__, jsResultBody.c_str());
        }
        catch (const std::exception& ex)
        {
            DevMsg(eDLL_T::ENGINE, "%s - Encountered error parsing Comp-server response body: '%s'\n", __FUNCTION__, ex.what());
        }
    }

    try
    {
        if (status == 200) // STATUS_OK
        {
            if (!svResponseBuf.empty())
            {
                nlohmann::json jsResultBody = nlohmann::json::parse(svResponseBuf);

                if (jsResultBody["success"].is_boolean() && jsResultBody["success"])
                {
                    slOutServer = NetGameServer_t
                    {
                            jsResultBody["server"].value("name",""),
                            jsResultBody["server"].value("description",""),
                            jsResultBody["server"].value("hidden","false") == "true",
                            jsResultBody["server"].value("map",""),
                            jsResultBody["server"].value("playlist",""),
                            jsResultBody["server"].value("ip",""),
                            jsResultBody["server"].value("port", ""),
                            jsResultBody["server"].value("key",""),
                            jsResultBody["server"].value("checksum",""),
                            jsResultBody["server"].value("version", SDK_VERSION),
                            jsResultBody["server"].value("playerCount", ""),
                            jsResultBody["server"].value("maxPlayers", ""),
                            jsResultBody["server"].value("timeStamp", 0),
                            jsResultBody["server"].value("publicRef", ""),
                            jsResultBody["server"].value("cachedId", ""),
                    };
                    return true;
                }
                else
                {
                    if (jsResultBody["error"].is_string())
                    {
                        svOutMessage = jsResultBody["error"].get<string>();
                    }
                    else
                    {
                        svOutMessage = string("Unknown error with status: ") + std::to_string(status);
                    }

                    slOutServer = NetGameServer_t{};
                    return false;
                }
            }
        }
        else
        {
            if (!svResponseBuf.empty())
            {
                nlohmann::json jsResultBody = nlohmann::json::parse(svResponseBuf);

                if (jsResultBody["error"].is_string())
                {
                    svOutMessage = jsResultBody["error"].get<string>();
                }
                else
                {
                    svOutMessage = string("Server not found: ") + std::to_string(status);
                }

                return false;
            }

            svOutMessage = string("Failed HTTP request: ") + std::to_string(status);
            return false;

            svOutMessage = "Failed to reach comp-server: connection timed-out";
            slOutServer = NetGameServer_t{};
            return false;
        }
    }
    catch (const std::exception& ex)
    {
        Warning(eDLL_T::ENGINE, "%s - Exception while parsing comp-server response:\n%s\n", __FUNCTION__, ex.what());
    }

    return false;
}

//-----------------------------------------------------------------------------
// Purpose: Sends host server POST request.
// Input  : &svOutMessage - 
//			&svOutToken - 
//			&netGameServer - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CPylon::PostServerHost(string& svOutMessage, string& svOutToken, const NetGameServer_t& netGameServer) const
{
    nlohmann::json jsRequestBody = nlohmann::json::object();
    jsRequestBody["name"] = netGameServer.m_svHostName;
    jsRequestBody["description"] = netGameServer.m_svDescription;
    jsRequestBody["hidden"] = netGameServer.m_bHidden;
    jsRequestBody["map"] = netGameServer.m_svHostMap;
    jsRequestBody["playlist"] = netGameServer.m_svPlaylist;
    jsRequestBody["ip"] = netGameServer.m_svIpAddress;
    jsRequestBody["port"] = netGameServer.m_svGamePort;
    jsRequestBody["key"] = netGameServer.m_svEncryptionKey;
    jsRequestBody["checksum"] = netGameServer.m_svRemoteChecksum;
    jsRequestBody["version"] = netGameServer.m_svSDKVersion;
    jsRequestBody["playerCount"] = netGameServer.m_svPlayerCount;
    jsRequestBody["maxPlayers"] = netGameServer.m_svMaxPlayers;
    jsRequestBody["timeStamp"] = netGameServer.m_nTimeStamp;
    jsRequestBody["publicRef"] = netGameServer.m_svPublicRef;
    jsRequestBody["cachedId"] = netGameServer.m_svCachedId;

    string svRequestBody = jsRequestBody.dump(4);
    string svResponseBuf;

    if (pylon_showdebuginfo->GetBool())
    {
        DevMsg(eDLL_T::ENGINE, "%s - Sending post host request to comp-server:\n%s\n", __FUNCTION__, svRequestBody.c_str());
    }

    CURLINFO status;
    if (!QueryMasterServer(pylon_matchmaking_hostname->GetString(), "/servers/add", svRequestBody, svResponseBuf, svOutMessage, status))
    {
        return false;
    }

    if (pylon_showdebuginfo->GetBool())
    {
        DevMsg(eDLL_T::ENGINE, "%s - Comp-server replied with status: '%d'\n", __FUNCTION__, status);
        try
        {
            string jsResultBody = nlohmann::json::parse(svResponseBuf).dump(4);
            DevMsg(eDLL_T::ENGINE, "%s - Comp-server response body:\n%s\n", __FUNCTION__, jsResultBody.c_str());
        }
        catch (const std::exception& ex)
        {
            DevMsg(eDLL_T::ENGINE, "%s - Encountered error parsing Comp-server response body: '%s'\n", __FUNCTION__, ex.what());
        }
    }

    try
    {
        if (status == 200) // STATUS_OK
        {
            nlohmann::json jsResultBody = nlohmann::json::parse(svResponseBuf);
            if (jsResultBody["success"].is_boolean() && jsResultBody["success"].get<bool>())
            {
                if (jsResultBody["token"].is_string())
                {
                    svOutToken = jsResultBody["token"].get<string>();
                }
                else
                {
                    svOutToken = string();
                }

                return true;
            }
            else
            {
                if (jsResultBody["error"].is_string())
                {
                    svOutMessage = jsResultBody["error"].get<string>();
                }
                else
                {
                    svOutMessage = string("Unknown error with status: ") + std::to_string(status);
                }
                return false;
            }
        }
        else
        {
            if (!svResponseBuf.empty())
            {
                nlohmann::json jsResultBody = nlohmann::json::parse(svResponseBuf);

                if (jsResultBody["error"].is_string())
                {
                    svOutMessage = jsResultBody["error"].get<string>();
                }
                else
                {
                    svOutMessage = string("Failed HTTP request: ") + std::to_string(status);
                }

                svOutToken = string();
                return false;
            }

            svOutToken = string();
            svOutMessage = string("Failed HTTP request: ") + std::to_string(status);
            return false;

            svOutToken = string();
            svOutMessage = "Failed to reach comp-server: connection timed-out";
            return false;
        }
    }
    catch (const std::exception& ex)
    {
        Warning(eDLL_T::ENGINE, "%s - Exception while parsing comp-server response:\n%s\n", __FUNCTION__, ex.what());
    }

    return false;
}

//-----------------------------------------------------------------------------
// Purpose: Send keep alive request to Pylon Master Server.
// Input  : &netGameServer - 
// Output : Returns true on success, false otherwise.
//-----------------------------------------------------------------------------
bool CPylon::KeepAlive(const NetGameServer_t& netGameServer) const
{
#ifndef CLIENT_DLL
    if (g_pServer->IsActive() && sv_pylonVisibility->GetBool()) // Check for active game.
    {
        string m_szHostToken;
        string m_szHostRequestMessage;

        bool result = PostServerHost(m_szHostRequestMessage, m_szHostToken, netGameServer);
        return result;
    }
#endif // !CLIENT_DLL
    return false;
}

//-----------------------------------------------------------------------------
// Purpose: Checks if client is banned on the comp server.
// Input  : &svIpAddress - 
//			nNucleusID - 
//			&svOutReason - 
// Output : Returns true if banned, false if not banned.
//-----------------------------------------------------------------------------
bool CPylon::CheckForBan(const string& svIpAddress, const uint64_t nNucleusID, string& svOutReason) const
{
    nlohmann::json jsRequestBody = nlohmann::json::object();
    jsRequestBody["id"] = nNucleusID;
    jsRequestBody["ip"] = svIpAddress;

    string svRequestBody = jsRequestBody.dump(4);
    string svResponseBuf;
    string svOutMessage;
    CURLINFO status;

    if (!QueryMasterServer(pylon_matchmaking_hostname->GetString(), "/banlist/isBanned", svRequestBody, svResponseBuf, svOutMessage, status))
    {
        Error(eDLL_T::ENGINE, NO_ERROR, "%s - Failed to query comp-server: %s\n", __FUNCTION__, svOutMessage.c_str());
        return false;
    }

    try
    {
        if (status == 200)
        {
            nlohmann::json jsResultBody = nlohmann::json::parse(svResponseBuf);
            if (jsResultBody["success"].is_boolean() && jsResultBody["success"].get<bool>())
            {
                if (jsResultBody["banned"].is_boolean() && jsResultBody["banned"].get<bool>())
                {
                    svOutReason = jsResultBody.value("reason", "#DISCONNECT_BANNED");
                    return true;
                }
            }
        }
        else
        {
            Error(eDLL_T::ENGINE, NO_ERROR, "%s - Failed to query comp-server: status code = %d\n", __FUNCTION__, status);
        }
    }
    catch (const std::exception& ex)
    {
        Warning(eDLL_T::ENGINE, "%s - Exception while parsing comp-server response:\n%s\n", __FUNCTION__, ex.what());
    }
    return false;
}

//-----------------------------------------------------------------------------
// Purpose: Sends query to master server.
// Input  : &svHostName - 
//			&svApi - 
//			&svInRequest - 
//          &svResponse - 
//          &svOutMessage - 
//          &outStatus - 
// Output : Returns true if successful, false otherwise.
//-----------------------------------------------------------------------------
bool CPylon::QueryMasterServer(const string& svHostName, const string& svApi, const string& svInRequest, 
    string& svOutResponse, string& svOutMessage, CURLINFO& outStatus) const
{
    string svUrl;
    CURLFormatUrl(svUrl, svHostName, svApi);

    curl_slist* sList = nullptr;
    CURL* curl = CURLInitRequest(svUrl, svInRequest, svOutResponse, sList);
    if (!curl)
    {
        return false;
    }

    CURLcode res = CURLSubmitRequest(curl, sList);
    if (!CURLHandleError(curl, res, svOutMessage))
    {
        return false;
    }

    outStatus = CURLRetrieveInfo(curl);
    return true;
}

///////////////////////////////////////////////////////////////////////////////
CPylon* g_pMasterServer(new CPylon());
