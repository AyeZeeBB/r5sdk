//============================================================================//
//
// Purpose: Debug interface functions
//
//============================================================================//

#include "core/stdafx.h"
#include "common/pseudodefs.h"
#include "tier0/tslist.h"
#include "tier0/basetypes.h"
#include "tier1/cvar.h"
#include "tier2/renderutils.h"
#include "engine/client/clientstate.h"
#include "engine/host_cmd.h"
#include "engine/debugoverlay.h"
#include "materialsystem/cmaterialsystem.h"
#include "mathlib/mathlib.h"
#ifndef CLIENT_DLL
#include "game/server/ai_utility.h"
#include "game/server/ai_network.h"
#include "game/server/ai_networkmanager.h"
#endif // !CLIENT_DLL


//------------------------------------------------------------------------------
// Purpose: checks if overlay should be decayed
// Output : true to decay, false otherwise
//------------------------------------------------------------------------------
bool OverlayBase_t::IsDead() const
{
    if (r_debug_overlay_nodecay->GetBool())
    {
        // Keep rendering the overlay if nodecay is set.
        return false;
    }

    if (g_pClientState->IsPaused())
    {
        // Keep rendering the overlay if the client simulation is paused.
        return false;
    }

    if (m_nCreationTick == -1)
    {
        if (m_nOverlayTick == -1)
        {
            if (m_flEndTime == NDEBUG_PERSIST_TILL_NEXT_SERVER)
            {
                return false;
            }
            return g_pClientState->GetClientTime() >= m_flEndTime;
        }
        else
        {
            return m_nOverlayTick < *overlay_tickcount;
        }
        return false;
    }
    else
    {
        return m_nCreationTick < *render_tickcount;
    }
    return g_pClientState->GetClientTime() >= m_flEndTime;
}

//------------------------------------------------------------------------------
// Purpose: destroys the overlay
// Input  : *pOverlay - 
//------------------------------------------------------------------------------
void DestroyOverlay(OverlayBase_t* pOverlay)
{
    size_t pOverlaySize;

    EnterCriticalSection(&*s_OverlayMutex);
    switch (pOverlay->m_Type)
    {
    case OverlayType_t::OVERLAY_BOX:
        pOverlaySize = sizeof(OverlayBox_t);
        goto LABEL_MALLOC;
    case OverlayType_t::OVERLAY_SPHERE:
        pOverlaySize = sizeof(OverlaySphere_t);
        goto LABEL_MALLOC;
    case OverlayType_t::OVERLAY_LINE:
        pOverlaySize = sizeof(OverlayLine_t);
        goto LABEL_MALLOC;
    case OverlayType_t::OVERLAY_TRIANGLE:
        pOverlaySize = 6200i64;
        goto LABEL_MALLOC;
    case OverlayType_t::OVERLAY_SWEPT_BOX:
        pOverlay->m_Type = OverlayType_t::OVERLAY_UNK1;
        LeaveCriticalSection(&*s_OverlayMutex);
        return;
    case OverlayType_t::OVERLAY_BOX2:
        pOverlaySize = 88i64;
        goto LABEL_MALLOC;
    case OverlayType_t::OVERLAY_CAPSULE:
        pOverlaySize = sizeof(OverlayCapsule_t);
        break;
    case OverlayType_t::OVERLAY_UNK0:
        pOverlaySize = 88i64;
        goto LABEL_MALLOC;
    LABEL_MALLOC:
        pOverlay->m_Type = OverlayType_t::OVERLAY_UNK1;
        v_MemAlloc_Internal(pOverlay, pOverlaySize);
        break;
    default:
        break;
    }

    LeaveCriticalSection(&*s_OverlayMutex);
}

//------------------------------------------------------------------------------
// Purpose: draws a generic overlay
// Input  : *pOverlay - 
//------------------------------------------------------------------------------
void DrawOverlay(OverlayBase_t* pOverlay)
{
    EnterCriticalSection(&*s_OverlayMutex);

    switch (pOverlay->m_Type)
    {
    case OverlayType_t::OVERLAY_BOX:
    {
        OverlayBox_t* pBox = static_cast<OverlayBox_t*>(pOverlay);
        if (pBox->a < 1)
        {
            if (r_debug_overlay_invisible->GetBool())
            {
                pBox->a = 255;
            }
            else
            {
                LeaveCriticalSection(&*s_OverlayMutex);
                return;
            }
        }

        v_RenderBox(pBox->transforms, pBox->mins, pBox->maxs, Color(pBox->r, pBox->g, pBox->b, pBox->a), r_debug_overlay_zbuffer->GetBool());
        break;
    }
    case OverlayType_t::OVERLAY_SPHERE:
    {
        OverlaySphere_t* pSphere = static_cast<OverlaySphere_t*>(pOverlay);
        if (pSphere->a < 1)
        {
            if (r_debug_overlay_invisible->GetBool())
            {
                pSphere->a = 255;
            }
            else
            {
                LeaveCriticalSection(&*s_OverlayMutex);
                return;
            }
        }

        if (r_debug_overlay_wireframe->GetBool())
        {
            v_RenderWireframeSphere(pSphere->vOrigin, pSphere->flRadius, pSphere->nTheta, pSphere->nPhi, 
                Color(pSphere->r, pSphere->g, pSphere->b, pSphere->a), r_debug_overlay_zbuffer->GetBool());
        }
        else
        {
            DebugDrawSphere(pSphere->vOrigin, pSphere->flRadius, Color(pSphere->r, pSphere->g, pSphere->b, pSphere->a), 16, r_debug_overlay_zbuffer->GetBool());
        }
        break;
    }
    case OverlayType_t::OVERLAY_LINE:
    {
        OverlayLine_t* pLine = static_cast<OverlayLine_t*>(pOverlay);
        if (pLine->a < 1)
        {
            if (r_debug_overlay_invisible->GetBool())
            {
                pLine->a = 255;
            }
            else
            {
                LeaveCriticalSection(&*s_OverlayMutex);
                return;
            }
        }

        v_RenderLine(pLine->origin, pLine->dest, Color(pLine->r, pLine->g, pLine->b, pLine->a), !pLine->noDepthTest);
        break;
    }
    case OverlayType_t::OVERLAY_TRIANGLE:
    {
        //printf("TRIANGLE %p\n", pOverlay);
        break;
    }
    case OverlayType_t::OVERLAY_SWEPT_BOX:
    {
        //printf("SBOX %p\n", pOverlay);
        break;
    }
    case OverlayType_t::OVERLAY_BOX2:
    {
        //printf("BOX2 %p\n", pOverlay);
        break;
    }
    case OverlayType_t::OVERLAY_CAPSULE:
    {
        OverlayCapsule_t* pCapsule = static_cast<OverlayCapsule_t*>(pOverlay);
        if (pCapsule->a < 1)
        {
            if (r_debug_overlay_invisible->GetBool())
            {
                pCapsule->a = 255;
            }
            else
            {
                LeaveCriticalSection(&*s_OverlayMutex);
                return;
            }
        }

        QAngle angles;

        VectorAngles(pCapsule->end, pCapsule->start, angles);
        AngleInverse(angles, angles);

        DebugDrawCapsule(pCapsule->start, angles, pCapsule->radius, pCapsule->start.DistTo(pCapsule->end), 
            Color(pCapsule->r, pCapsule->g, pCapsule->b, pCapsule->a), r_debug_overlay_zbuffer->GetBool());
        break;
    }
    case OverlayType_t::OVERLAY_UNK0:
    {
        //printf("UNK0 %p\n", pOverlay);
        break;
    }
    case OverlayType_t::OVERLAY_UNK1:
    {
        //printf("UNK1 %p\n", pOverlay);
        break;
    }
    }

    LeaveCriticalSection(&*s_OverlayMutex);
}

//------------------------------------------------------------------------------
// Purpose : draw AIN script nodes
//------------------------------------------------------------------------------
void DrawAIScriptNodes()
{
#ifndef CLIENT_DLL
    if (*g_pAINetwork)
    {
        OverlayBox_t::Transforms vTransforms;

        for (int i = ai_script_nodes_draw_index->GetInt(); i < (*g_pAINetwork)->GetNumScriptNodes(); i++)
        {
            if (i < 0)
            {
                return;
            }

            vTransforms.xmm[0] = _mm_set_ps((*g_pAINetwork)->m_ScriptNode[i].m_vOrigin.x - 50.f, 0.0f, 0.0f, 1.0f);
            vTransforms.xmm[1] = _mm_set_ps((*g_pAINetwork)->m_ScriptNode[i].m_vOrigin.y - 50.f, 0.0f, 1.0f, 0.0f);
            vTransforms.xmm[2] = _mm_set_ps((*g_pAINetwork)->m_ScriptNode[i].m_vOrigin.z - 50.f, 1.0f, 0.0f, 0.0f);

            v_RenderBox(vTransforms, {0, 0, 0}, {100, 100, 100}, Color(0, 255, 0, 255), r_debug_overlay_zbuffer->GetBool());

            if (i > 0)
            {
                v_RenderLine((*g_pAINetwork)->m_ScriptNode[i - 1].m_vOrigin, (*g_pAINetwork)->m_ScriptNode[i].m_vOrigin, Color(255, 0, 0, 255), r_debug_overlay_zbuffer->GetBool());
            }
        }
    }
#endif // !CLIENT_DLL
}

//------------------------------------------------------------------------------
// Purpose : draw NavMesh BVTree
//------------------------------------------------------------------------------
void DrawNavMeshBVTree()
{
#ifndef CLIENT_DLL
    const dtNavMesh* mesh = GetNavMeshForHull(navmesh_debug_type->GetInt());
    if (!mesh)
        return;

    OverlayBox_t::Transforms vTransforms;
    for (int i = navmesh_draw_bvtree->GetInt(); i < mesh->getTileCount(); ++i)
    {
        const dtMeshTile* tile = &mesh->m_tiles[i];
        if (!tile->header)
            continue;

        const float cs = 1.0f / tile->header->bvQuantFactor;
        for (int j = 0; j < tile->header->bvNodeCount; ++j)
        {
            const dtBVNode* node = &tile->bvTree[j];
            if (node->i < 0) // Leaf indices are positive.
                continue;

            vTransforms.xmm[0] = _mm_set_ps(0.0f, 0.0f, 0.0f, 1.0f);
            vTransforms.xmm[1] = _mm_set_ps(0.0f, 0.0f, 1.0f, 0.0f);
            vTransforms.xmm[2] = _mm_set_ps(0.0f, 1.0f, 0.0f, 0.0f);

            const Vector3D vMins(
                tile->header->bmin[0] + node->bmin[0] * cs,
                tile->header->bmin[1] + node->bmin[1] * cs,
                tile->header->bmin[2] + node->bmin[2] * cs);
            const Vector3D vMaxs(
                tile->header->bmin[0] + node->bmax[0] * cs,
                tile->header->bmin[1] + node->bmax[1] * cs,
                tile->header->bmin[2] + node->bmax[2] * cs);

            v_RenderBox(vTransforms, vMins, vMaxs, Color(255, 255, 255, 255), r_debug_overlay_zbuffer->GetBool());
        }
    }
#endif // !CLIENT_DLL
}

//------------------------------------------------------------------------------
// Purpose : draw NavMesh portals
//------------------------------------------------------------------------------
static void DrawNavMeshPortals()
{
#ifndef CLIENT_DLL
    const dtNavMesh* mesh = GetNavMeshForHull(navmesh_debug_type->GetInt());
    if (!mesh)
        return;

    OverlayBox_t::Transforms vTransforms;
    for (int i = navmesh_draw_portal->GetInt(); i < mesh->getTileCount(); ++i)
    {
        const dtMeshTile* tile = &mesh->m_tiles[i];
        if (!tile->header)
            continue;

        // Draw portals
        const float padx = 0.04f;
        const float padz = tile->header->walkableClimb;

        for (int side = 0; side < 8; ++side)
        {
            unsigned short m = DT_EXT_LINK | static_cast<unsigned short>(side);
            for (int i = 0; i < tile->header->polyCount; ++i)
            {
                const dtPoly* poly = &tile->polys[i];

                // Create new links.
                const int nv = poly->vertCount;
                for (int j = 0; j < nv; ++j)
                {
                    // Skip edges which do not point to the right side.
                    if (poly->neis[j] != m)
                        continue;

                    // Create new links
                    const float* va = &tile->verts[poly->verts[j] * 3];
                    const float* vb = &tile->verts[poly->verts[(j + 1) % nv] * 3];

                    if (side == 0 || side == 4)
                    {
                        Color col = side == 0 ? Color(188, 0, 0, 188) : Color(188, 0, 188, 188);
                        const float x = va[0] + ((side == 0) ? -padx : padx);

                        v_RenderLine(Vector3D(x, va[1], va[2] - padz), Vector3D(x, va[1], va[2] + padz), col, r_debug_overlay_zbuffer->GetBool());
                        v_RenderLine(Vector3D(x, va[1], va[2] + padz), Vector3D(x, vb[1], vb[2] + padz), col, r_debug_overlay_zbuffer->GetBool());
                        v_RenderLine(Vector3D(x, vb[1], vb[2] + padz), Vector3D(x, vb[1], vb[2] - padz), col, r_debug_overlay_zbuffer->GetBool());
                        v_RenderLine(Vector3D(x, vb[1], vb[2] - padz), Vector3D(x, va[1], va[2] - padz), col, r_debug_overlay_zbuffer->GetBool());
                    }
                    else if (side == 2 || side == 6)
                    {
                        Color col = side == 2 ? Color(0, 188, 0, 188) : Color(0, 188, 188, 188);
                        const float y = va[1] + ((side == 2) ? -padx : padx);

                        v_RenderLine(Vector3D(va[0], y, va[2] - padz), Vector3D(va[0], y, va[2] + padz), col, r_debug_overlay_zbuffer->GetBool());
                        v_RenderLine(Vector3D(va[0], y, va[2] + padz), Vector3D(vb[0], y, vb[2] + padz), col, r_debug_overlay_zbuffer->GetBool());
                        v_RenderLine(Vector3D(vb[0], y, vb[2] + padz), Vector3D(vb[0], y, vb[2] - padz), col, r_debug_overlay_zbuffer->GetBool());
                        v_RenderLine(Vector3D(vb[0], y, vb[2] - padz), Vector3D(va[0], y, va[2] - padz), col, r_debug_overlay_zbuffer->GetBool());
                    }
                }
            }
        }
    }
#endif // !CLIENT_DLL
}

//------------------------------------------------------------------------------
// Purpose : overlay drawing entrypoint
// Input  : bDraw - 
//------------------------------------------------------------------------------
void DrawAllOverlays(bool bDraw)
{
    if (!enable_debug_overlays->GetBool())
    {
        return;
    }
    if (ai_script_nodes_draw->GetBool())
    {
        DrawAIScriptNodes();
    }
    if (navmesh_draw_bvtree->GetInt() > -1)
    {
        DrawNavMeshBVTree();
    }
    if (navmesh_draw_portal->GetInt() > -1)
    {
        DrawNavMeshPortals();
    }

    EnterCriticalSection(&*s_OverlayMutex);

    OverlayBase_t* pCurrOverlay = *s_pOverlays; // rdi
    OverlayBase_t* pPrevOverlay = nullptr;      // rsi
    OverlayBase_t* pNextOverlay = nullptr;      // rbx

    while (pCurrOverlay)
    {
        if (pCurrOverlay->IsDead())
        {
            if (pPrevOverlay)
            {
                // If I had a last overlay reset it's next pointer
                pPrevOverlay->m_pNextOverlay = pCurrOverlay->m_pNextOverlay;
            }
            else
            {
                // If the first line, reset the s_pOverlays pointer
                *s_pOverlays = pCurrOverlay->m_pNextOverlay;
            }

            pNextOverlay = pCurrOverlay->m_pNextOverlay;
            DestroyOverlay(pCurrOverlay);
            pCurrOverlay = pNextOverlay;
        }
        else
        {
            bool bDraw{ };
            if (pCurrOverlay->m_nCreationTick == -1)
            {
                if (pCurrOverlay->m_nOverlayTick == *overlay_tickcount)
                {
                    // Draw overlay if unk0 == *overlay_tickcount
                    bDraw = true;
                }
                if (pCurrOverlay->m_nOverlayTick == -1)
                {
                    // Draw overlay if unk0 == -1
                    bDraw = true;
                }
            }
            else
            {
                bDraw = pCurrOverlay->m_nCreationTick == *render_tickcount;
            }
            if (bDraw)
            {
                if (bDraw)
                {
                    DrawOverlay(pCurrOverlay);
                }
            }
            pPrevOverlay = pCurrOverlay;
            pCurrOverlay = pCurrOverlay->m_pNextOverlay;
        }
    }
    LeaveCriticalSection(&*s_OverlayMutex);
}

///////////////////////////////////////////////////////////////////////////////
void DebugOverlays_Attach()
{
    DetourAttach((LPVOID*)&v_DrawAllOverlays, &DrawAllOverlays);
}

void DebugOverlays_Detach()
{
    DetourDetach((LPVOID*)&v_DrawAllOverlays, &DrawAllOverlays);
}
