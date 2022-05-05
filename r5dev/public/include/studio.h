#ifndef STUDIO_H
#define STUDIO_H
#include "mathlib/vector.h"

// This flag is set if no hitbox information was specified
#define STUDIOHDR_FLAGS_AUTOGENERATED_HITBOX	( 1 << 0 )

// NOTE:  This flag is set at loadtime, not mdl build time so that we don't have to rebuild
// models when we change materials.
#define STUDIOHDR_FLAGS_USES_ENV_CUBEMAP		( 1 << 1 )

// Use this when there are translucent parts to the model but we're not going to sort it 
#define STUDIOHDR_FLAGS_FORCE_OPAQUE			( 1 << 2 )

// Use this when we want to render the opaque parts during the opaque pass
// and the translucent parts during the translucent pass
#define STUDIOHDR_FLAGS_TRANSLUCENT_TWOPASS		( 1 << 3 )

// This is set any time the .qc files has $staticprop in it
// Means there's no bones and no transforms
#define STUDIOHDR_FLAGS_STATIC_PROP				( 1 << 4 )

// NOTE:  This flag is set at loadtime, not mdl build time so that we don't have to rebuild
// models when we change materials.
#define STUDIOHDR_FLAGS_USES_FB_TEXTURE		    ( 1 << 5 )

// This flag is set by studiomdl.exe if a separate "$shadowlod" entry was present
//  for the .mdl (the shadow lod is the last entry in the lod list if present)
#define STUDIOHDR_FLAGS_HASSHADOWLOD			( 1 << 6 )

// NOTE:  This flag is set at loadtime, not mdl build time so that we don't have to rebuild
// models when we change materials.
#define STUDIOHDR_FLAGS_USES_BUMPMAPPING		( 1 << 7 )

// NOTE:  This flag is set when we should use the actual materials on the shadow LOD
// instead of overriding them with the default one (necessary for translucent shadows)
#define STUDIOHDR_FLAGS_USE_SHADOWLOD_MATERIALS	( 1 << 8 )

// NOTE:  This flag is set when we should use the actual materials on the shadow LOD
// instead of overriding them with the default one (necessary for translucent shadows)
#define STUDIOHDR_FLAGS_OBSOLETE				( 1 << 9 )

// NOTE:  This flag is set when we need to draw in the additive stage of the deferred rendering
#define STUDIOHDR_FLAGS_NEEDS_DEFERRED_ADDITIVE	( 1 << 10 )

// NOTE:  This flag is set at mdl build time
#define STUDIOHDR_FLAGS_NO_FORCED_FADE			( 1 << 11 )

// NOTE:  The npc will lengthen the viseme check to always include two phonemes
#define STUDIOHDR_FLAGS_FORCE_PHONEME_CROSSFADE	( 1 << 12 )

// This flag is set when the .qc has $constantdirectionallight in it
// If set, we use constantdirectionallightdot to calculate light intensity
// rather than the normal directional dot product
// only valid if STUDIOHDR_FLAGS_STATIC_PROP is also set
#define STUDIOHDR_FLAGS_CONSTANT_DIRECTIONAL_LIGHT_DOT ( 1 << 13 )

// Flag to mark delta flexes as already converted from disk format to memory format
#define STUDIOHDR_FLAGS_FLEXES_CONVERTED		( 1 << 14 )

// Indicates the studiomdl was built in preview mode
#define STUDIOHDR_FLAGS_BUILT_IN_PREVIEW_MODE	( 1 << 15 )

// Ambient boost (runtime flag)
#define STUDIOHDR_FLAGS_AMBIENT_BOOST			( 1 << 16 )

// Don't cast shadows from this model (useful on first-person models)
#define STUDIOHDR_FLAGS_DO_NOT_CAST_SHADOWS		( 1 << 17 )

// alpha textures should cast shadows in vrad on this model (ONLY prop_static!)
#define STUDIOHDR_FLAGS_CAST_TEXTURE_SHADOWS	( 1 << 18 )

// Model has a quad-only Catmull-Clark SubD cage
#define STUDIOHDR_FLAGS_SUBDIVISION_SURFACE		( 1 << 19 )

// flagged on load to indicate no animation events on this model
#define STUDIOHDR_FLAGS_NO_ANIM_EVENTS			( 1 << 20 )

// If flag is set then studiohdr_t.flVertAnimFixedPointScale contains the
// scale value for fixed point vert anim data, if not set then the
// scale value is the default of 1.0 / 4096.0.  Regardless use
// studiohdr_t::VertAnimFixedPointScale() to always retrieve the scale value
#define STUDIOHDR_FLAGS_VERT_ANIM_FIXED_POINT_SCALE	( 1 << 21 )

// If flag is set then model data is processed for EDGE
// the flag is set at tool time when producing PS3-format assets
#define STUDIOHDR_FLAGS_PS3_EDGE_FORMAT			( 1 << 22 )

// this is a specific case to indicate a model is over budget
#define STUDIOHDR_FLAGS_OVER_BUDGET					( 1 << 23 )

// this is a specific case to indicate a model is over budget
#define STUDIOHDR_FLAGS_IGNORE_BUDGETS				( 1 << 24 )

// internally generated combined model
#define STUDIOHDR_FLAGS_COMBINED					( 1 << 25 )

// Model has an additional set of UVs
#define STUDIOHDR_FLAGS_EXTRA_VERTEX_DATA			( 1 << 26 )

// NOTE:  This flag is set at loadtime, not mdl build time so that we don't have to rebuild
// models when we change materials.
#define STUDIOHDR_BAKED_VERTEX_LIGHTING_IS_INDIRECT_ONLY	( 1 << 27 )

enum
{
	STUDIODATA_FLAGS_STUDIOMESH_LOADED = 0x0001,
	STUDIODATA_FLAGS_VCOLLISION_LOADED = 0x0002,
	STUDIODATA_ERROR_MODEL = 0x0004,
	STUDIODATA_FLAGS_NO_STUDIOMESH = 0x0008,
	STUDIODATA_FLAGS_NO_VERTEX_DATA = 0x0010,
	//										= 0x0020,	// unused
	STUDIODATA_FLAGS_PHYSICS2COLLISION_LOADED = 0x0040,
	STUDIODATA_FLAGS_VCOLLISION_SCANNED = 0x0080,

	STUDIODATA_FLAGS_COMBINED_PLACEHOLDER = 0x0100,
	STUDIODATA_FLAGS_COMBINED = 0x0200,
	STUDIODATA_FLAGS_COMBINED_UNAVAILABLE = 0x0400,
	STUDIODATA_FLAGS_COMBINED_ASSET = 0x0800,
};

#pragma pack(push, 1)
struct studiohdr_t
{
	int id;                  // 'IDST'
	int version;             // R5 = '6'
	int checksum;
	int tableIndex;          // Offset

	char name[0x40];

	int length;              // size of data

	Vector3 eyeposition;     // ideal eye position
	Vector3 illumposition;   // illumination center
	Vector3 hull_min;        // ideal movement hull size
	Vector3 hull_max;
	Vector3 view_bbmin;      // clipping bounding box
	Vector3 view_bbmax;

	int flags;

	int numbones;            // bones
	int boneindex;

	int numbonecontrollers;
	int bonecontrollerindex;

	int numhitboxsets;
	int hitboxsetindex;

	int numlocalanim;        // animations/poses
	int localanimindex;      // animation descriptions

	int numlocalseq;         // sequences
	int localseqindex;

	int activitylistversion; // initialization flag - have the sequences been indexed ?
	int eventsindexed;

	int numtextures;
	int textureindex;

	int numcdtextures;
	int cdtextureindex;

	int numskinref;      // Total number of references (submeshes)
	int numskinfamilies; // Total skins per reference
	int skinindex;       // Offset to data

	int numbodyparts;
	int bodypartindex;

	int numlocalattachments;
	int localattachmentindex;

	uint8_t Unknown2[0x14];

	int submeshLodsIndex;

	uint8_t Unknown3[0x58];
	float fadeDist;
	float gatherSize;
	uint8_t Unknown4[0x4];
	int boneRemapInfoIndex;
	int boneRemapCount;
};
#pragma pack(pop)

// studio model data
struct studiomeshdata_t
{
	int   m_NumGroup;
	void* m_pMeshGroup; // studiomeshgroup_t
};

struct studioloddata_t
{
	studiomeshdata_t* m_pMeshData;
	// !TODO:
};

struct studiohwdata_t
{
	int                   m_RootLOD; // calced and clamped, nonzero for lod culling
	int                   m_NumLODs;
	studioloddata_t*        m_pLODs;
	int           m_NumStudioMeshes;
	// !TODO:
};

class CStudioHdr
{
public:
	static int LookupSequence(CStudioHdr* pStudio, const char* pszName);

	int64_t m_nUnk0;
	studiohdr_t* m_pStudioHdr;
	uint8_t m_Pad[0x85A]; // Compatible between S0 - S3.
	void* m_pMdlCacheVTable;
};

class CStudioHWDataRef
{
public:
	bool IsDataRef(void) const { return true; }

	CStudioHWDataRef* m_pVTable;
	uint8_t pad0[0x8];
	studiohwdata_t m_HardwareData;
};

#endif // STUDIO_H
