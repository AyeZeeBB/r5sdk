﻿//====== Copyright � 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose: 
//
// $Workfile:     $
// $Date:         $
// $NoKeywords: $
//=============================================================================//

#ifndef CMODEL_H
#define CMODEL_H
#ifdef _WIN32
#pragma once
#endif
#include "mathlib/mathlib.h"

struct Ray_t
{
	VectorAligned m_Start;
	VectorAligned m_Delta;
	VectorAligned m_StartOffset;
	int m_nUnk30;
	int m_nUnk34;
	int m_nUnk38;
	int m_nUnk3C;
	int m_nUnk40;
	int m_nUnk44;
	int m_nFlags;
	const matrix3x4_t* m_pWorldAxisTransform;
	int m_nUnk58;
	bool m_IsRay;
	bool m_IsSwept;
	void* m_pUnk60;

	Ray_t() : m_pWorldAxisTransform(NULL) {};

	void Init(Vector3D const& start, Vector3D const& end)
	{
		VectorSubtract(end, start, m_Delta);
		m_IsSwept = (m_Delta.LengthSqr() != 0);

		m_nUnk30 = NULL;
		m_nUnk34 = NULL;
		m_nUnk3C = NULL;
		m_nUnk44 = NULL;
		m_nUnk44 = NULL;

		m_nFlags = 0x3F800000; // !TODO: Reverse these flags!

		m_pWorldAxisTransform = NULL;
		m_IsRay = true;

		m_nUnk58 = NULL;
		m_pUnk60 = NULL;

		VectorClear(m_StartOffset);
		VectorCopy(start, m_Start);
	}
};
static_assert(sizeof(Ray_t) == 0x70);

struct csurface_t
{
	const char* name;
	short surfaceProp;
	uint16_t flags;
};

struct cplanetrace_t
{
	Vector3D normal;
	float dist;
};

#endif // CMODEL_H