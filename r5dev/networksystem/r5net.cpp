// r5net.cpp : Defines the functions for the static library.
//

#include "core/stdafx.h"
#include "tier1/cvar.h"
#include "networksystem/r5net.h"

//-----------------------------------------------------------------------------
// Purpose: returns a vector of hosted servers.
//-----------------------------------------------------------------------------
vector<NetGameServer_t> R5Net::Client::GetServersList(string& svOutMessage)
{
    vector<NetGameServer_t> vslList{};

    nlohmann::json jsRequestBody = nlohmann::json::object();
    jsRequestBody["version"] = SDK_VERSION;

    string svRequestBody = jsRequestBody.dump(4);

    if (r5net_show_debug->GetBool())
    {
       DevMsg(eDLL_T::ENGINE, "%s - Sending server list request to comp-server:\n%s\n", __FUNCTION__, svRequestBody.c_str());
    }

    httplib::Result htResults = m_HttpClient.Post("/servers", jsRequestBody.dump(4).c_str(), jsRequestBody.dump(4).length(), "application/json");
    if (htResults && r5net_show_debug->GetBool())
    {
        DevMsg(eDLL_T::ENGINE, "%s - replied with '%d'.\n", __FUNCTION__, htResults->status);
    }

    if (htResults && htResults->status == 200) // STATUS_OK
    {
        nlohmann::json jsResultBody = nlohmann::json::parse(htResults->body);
        if (jsResultBody["success"].is_boolean() && jsResultBody["success"].get<bool>())
        {
            for (auto &obj : jsResultBody["servers"])
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
                        obj.value("cachedID", ""),
                    }
                );
            }
        }
        else
        {
            if (jsResultBody["err"].is_string())
            {
                svOutMessage = jsResultBody["err"].get<string>();
            }
            else
            {
                svOutMessage = "An unknown error occured!";
            }
        }
    }
    else
    {
        if (htResults)
        {
            if (!htResults->body.empty())
            {
                nlohmann::json jsResultBody = nlohmann::json::parse(htResults->body);

                if (jsResultBody["err"].is_string())
                {
                    svOutMessage = jsResultBody["err"].get<string>();
                }
                else
                {
                    svOutMessage = string("Failed to reach comp-server: ") + std::to_string(htResults->status);
                }

                return vslList;
            }

            svOutMessage = string("Failed to reach comp-server: ") + std::to_string(htResults->status);
            return vslList;
        }

        svOutMessage = "Failed to reach comp-server: Unknown error code";
        return vslList;
    }

    return vslList;
}

//-----------------------------------------------------------------------------
// Purpose: Sends host server POST request.
// Input  : &svOutMessage - 
//			&svOutToken - 
//			&slServerListing - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool R5Net::Client::PostServerHost(string& svOutMessage, string& svOutToken, const NetGameServer_t& slServerListing)
{
    nlohmann::json jsRequestBody = nlohmann::json::object();
    jsRequestBody["name"] = slServerListing.m_svHostName;
    jsRequestBody["description"] = slServerListing.m_svDescription;
    jsRequestBody["hidden"] = slServerListing.m_bHidden;
    jsRequestBody["map"] = slServerListing.m_svMapName;
    jsRequestBody["playlist"] = slServerListing.m_svPlaylist;
    jsRequestBody["ip"] = slServerListing.m_svIpAddress;
    jsRequestBody["port"] = slServerListing.m_svGamePort;
    jsRequestBody["key"] = slServerListing.m_svEncryptionKey;
    jsRequestBody["checksum"] = slServerListing.m_svRemoteChecksum;
    jsRequestBody["version"] = slServerListing.m_svSDKVersion;
    jsRequestBody["playerCount"] = slServerListing.m_svPlayerCount;
    jsRequestBody["maxPlayers"] = slServerListing.m_svMaxPlayers;
    jsRequestBody["timeStamp"] = slServerListing.m_nTimeStamp;
    jsRequestBody["publicRef"] = slServerListing.m_svPublicRef;
    jsRequestBody["cachedID"] = slServerListing.m_svCachedID;

    string svRequestBody = jsRequestBody.dump(4);
    if (r5net_show_debug->GetBool())
    {
        DevMsg(eDLL_T::ENGINE, "%s - Sending post host request to comp-server:\n%s\n", __FUNCTION__, svRequestBody.c_str());
    }

    httplib::Result htResults = m_HttpClient.Post("/servers/add", svRequestBody.c_str(), svRequestBody.length(), "application/json");
    if (htResults && r5net_show_debug->GetBool())
    {
        DevMsg(eDLL_T::ENGINE, "%s - Comp-server replied with '%d'\n", __FUNCTION__, htResults->status);
    }

    if (htResults && htResults->status == 200) // STATUS_OK
    {
        nlohmann::json jsResultBody = nlohmann::json::parse(htResults->body);
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
            if (jsResultBody["err"].is_string())
            {
                svOutMessage = jsResultBody["err"].get<string>();
            }
            else
            {
                svOutMessage = "An unknown error occured!";
            }
            return false;
        }
    }
    else
    {
        if (htResults)
        {
            if (!htResults->body.empty())
            {
                nlohmann::json jsResultBody = nlohmann::json::parse(htResults->body);

                if (jsResultBody["err"].is_string())
                {
                    svOutMessage = jsResultBody["err"].get<string>();
                }
                else
                {
                    svOutMessage = string("Failed to reach comp-server ") + std::to_string(htResults->status);
                }

                svOutToken = string();
                return false;
            }

            svOutToken = string();
            svOutMessage = string("Failed to reach comp-server: ") + std::to_string(htResults->status);
            return false;
        }

        svOutToken = string();
        svOutMessage = "Failed to reach comp-server: Unknown error code";
        return false;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Purpose: Gets the server by token string.
// Input  : &slOutServer - 
//			&svOutMessage - 
//			svToken - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool R5Net::Client::GetServerByToken(NetGameServer_t& slOutServer, string& svOutMessage, const string& svToken)
{
    nlohmann::json jsRequestBody = nlohmann::json::object();
    jsRequestBody["token"] = svToken;

    string svRequestBody = jsRequestBody.dump(4);

    if (r5net_show_debug->GetBool())
    {
        DevMsg(eDLL_T::ENGINE, "%s - Sending token connect request to comp-server:\n%s\n", __FUNCTION__, svRequestBody.c_str());
    }

    httplib::Result htResults = m_HttpClient.Post("/server/byToken", jsRequestBody.dump(4).c_str(), jsRequestBody.dump(4).length(), "application/json");

    if (r5net_show_debug->GetBool())
    {
        DevMsg(eDLL_T::ENGINE, "%s - Comp-server replied with '%d'\n", __FUNCTION__, htResults->status);
    }

    if (htResults && htResults->status == 200) // STATUS_OK
    {
        if (!htResults->body.empty())
        {
            nlohmann::json jsResultBody = nlohmann::json::parse(htResults->body);

            if (htResults && jsResultBody["success"].is_boolean() && jsResultBody["success"])
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
                        jsResultBody["server"].value("cachedID", ""),
                };
                return true;
            }
            else
            {
                if (jsResultBody["err"].is_string())
                {
                    svOutMessage = jsResultBody["err"].get<string>();
                }
                else
                {
                    svOutMessage = "";
                }

                slOutServer = NetGameServer_t{};
                return false;
            }
        }
    }
    else
    {
        if (htResults)
        {
            if (!htResults->body.empty())
            {
                nlohmann::json jsResultBody = nlohmann::json::parse(htResults->body);

                if (jsResultBody["err"].is_string())
                {
                    svOutMessage = jsResultBody["err"].get<string>();
                }
                else
                {
                    svOutMessage = string("Failed to reach comp-server: ") + std::to_string(htResults->status);
                }

                return false;
            }

            svOutMessage = string("Failed to reach comp-server: ") + std::to_string(htResults->status);
            return false;
        }

        svOutMessage = "Failed to reach comp-server: Unknown error code";
        slOutServer = NetGameServer_t{};
        return false;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Purpose: Checks if client is banned on the comp server.
// Input  : svIpAddress - 
//			nOriginID - 
//			&svOutErrCl - 
// Output : Returns true if banned, false if not banned.
//-----------------------------------------------------------------------------
bool R5Net::Client::GetClientIsBanned(const string& svIpAddress, uint64_t nOriginID, string& svOutErrCl)
{
    nlohmann::json jsRequestBody = nlohmann::json::object();
    jsRequestBody["oid"] = nOriginID;
    jsRequestBody["ip"] = svIpAddress;

    httplib::Result htResults = m_HttpClient.Post("/banlist/isBanned", jsRequestBody.dump(4).c_str(), jsRequestBody.dump(4).length(), "application/json");
    if (htResults && htResults->status == 200)
    {
        nlohmann::json jsResultBody = nlohmann::json::parse(htResults->body);
        if (jsResultBody["success"].is_boolean() && jsResultBody["success"].get<bool>())
        {
            if (jsResultBody["banned"].is_boolean() && jsResultBody["banned"].get<bool>())
            {
                svOutErrCl = jsResultBody.value("errCl", "#DISCONNECT_BANNED");
                return true;
            }
        }
    }
    return false;
}
///////////////////////////////////////////////////////////////////////////////
R5Net::Client* g_pR5net(new R5Net::Client("127.0.0.1:3000"));
