#pragma once
#include "stdafx.h"
#include "Global.h"
#include <vector>
#include <windows.h>
#include <sstream>
#include <iostream>
#include <d3d9.h>
#include <Dwmapi.h>
#include <d3dx9.h>
#include <d3dx9core.h>

inline float BitsToFloat(UINT i)
{
	union Convertor_t
	{
		float f;
		unsigned long ul;
	} tmp;
	tmp.ul = i;
	return tmp.f;
}
#define IN_ATTACK				(1 << 0)
#define IN_JUMP					(1 << 1)
#define IN_DUCK					(1 << 2)
#define IN_FORWARD				(1 << 3)
#define IN_BACK					(1 << 4)
#define IN_USE					(1 << 5)
#define IN_CANCEL				(1 << 6)
#define IN_LEFT					(1 << 7)
#define IN_RIGHT				(1 << 8)
#define IN_MOVELEFT				(1 << 9)
#define IN_MOVERIGHT			(1 << 10)
#define IN_ATTACK2				(1 << 11)
#define IN_RUN					(1 << 12)
#define IN_RELOAD				(1 << 13)
#define IN_ALT1					(1 << 14)
#define IN_ALT2					(1 << 15)
#define IN_SCORE				(1 << 16)
#define IN_SPEED				(1 << 17)
#define IN_WALK					(1 << 18)
#define IN_ZOOM					(1 << 19)
#define IN_WEAPON1				(1 << 20)
#define IN_WEAPON2				(1 << 21)
#define IN_BULLRUSH				(1 << 22)
#define	FL_ONGROUND				(1 << 0)
#define FL_DUCKING				(1 << 1)
#define	FL_WATERJUMP			(1 << 3)
#define FL_ONTRAIN				(1 << 4)
#define FL_INRAIN				(1 << 5)
#define FL_FROZEN				(1 << 6)
#define FL_ATCONTROLS			(1 << 7)
#define	FL_CLIENT				(1 << 8)
#define FL_FAKECLIENT			(1 << 9)
#define	FL_INWATER				(1 << 10)

#define  Assert( _exp )										((void)0)
#define FLOAT32_NAN_BITS     (UINT)0x7FC00000	// not a number!
#define FLOAT32_NAN          BitsToFloat( FLOAT32_NAN_BITS )

#define VEC_T_NAN FLOAT32_NAN
#define NUM_ENT_ENTRY_BITS         (11 + 2)
#define NUM_ENT_ENTRIES            (1 << NUM_ENT_ENTRY_BITS)
#define INVALID_EHANDLE_INDEX       0xFFFFFFFF
#define NUM_SERIAL_NUM_BITS        16 // (32 - NUM_ENT_ENTRY_BITS)
#define NUM_SERIAL_NUM_SHIFT_BITS (32 - NUM_SERIAL_NUM_BITS)
#define ENT_ENTRY_MASK             (( 1 << NUM_SERIAL_NUM_BITS) - 1)
#define NETVAR(type, name, table, netvar)                           \
    type& name##() const {                                          \
        static int _##name = NetvarSys::Get().GetOffset(table, netvar);     \
        return *(type*)((uintptr_t)this + _##name);                 \
    }

#define PNETVAR(type, name, table, netvar)                           \
    type* name##() const {                                          \
        static int _##name = NetvarSys::Get().GetOffset(table, netvar);     \
        return (type*)((uintptr_t)this + _##name);                 \
    }

#define   DISPSURF_FLAG_SURFACE           (1<<0)
#define   DISPSURF_FLAG_WALKABLE          (1<<1)
#define   DISPSURF_FLAG_BUILDABLE         (1<<2)
#define   DISPSURF_FLAG_SURFPROP1         (1<<3)
#define   DISPSURF_FLAG_SURFPROP2         (1<<4)

#define   CONTENTS_EMPTY                0

#define   CONTENTS_SOLID                0x1       
#define   CONTENTS_WINDOW               0x2
#define   CONTENTS_AUX                  0x4
#define   CONTENTS_GRATE                0x8
#define   CONTENTS_SLIME                0x10
#define   CONTENTS_WATER                0x20
#define   CONTENTS_BLOCKLOS             0x40 
#define   CONTENTS_OPAQUE               0x80 
#define   LAST_VISIBLE_CONTENTS         CONTENTS_OPAQUE

#define   ALL_VISIBLE_CONTENTS            (LAST_VISIBLE_CONTENTS | (LAST_VISIBLE_CONTENTS-1))

#define   CONTENTS_TESTFOGVOLUME        0x100
#define   CONTENTS_UNUSED               0x200     
#define   CONTENTS_BLOCKLIGHT           0x400
#define   CONTENTS_TEAM1                0x800 
#define   CONTENTS_TEAM2                0x1000 
#define   CONTENTS_IGNORE_NODRAW_OPAQUE 0x2000
#define   CONTENTS_MOVEABLE             0x4000
#define   CONTENTS_AREAPORTAL           0x8000
#define   CONTENTS_PLAYERCLIP           0x10000
#define   CONTENTS_MONSTERCLIP          0x20000
#define   CONTENTS_CURRENT_0            0x40000
#define   CONTENTS_CURRENT_90           0x80000
#define   CONTENTS_CURRENT_180          0x100000
#define   CONTENTS_CURRENT_270          0x200000
#define   CONTENTS_CURRENT_UP           0x400000
#define   CONTENTS_CURRENT_DOWN         0x800000

#define   CONTENTS_ORIGIN               0x1000000 

#define   CONTENTS_MONSTER              0x2000000 
#define   CONTENTS_DEBRIS               0x4000000
#define   CONTENTS_DETAIL               0x8000000 
#define   CONTENTS_TRANSLUCENT          0x10000000
#define   CONTENTS_LADDER               0x20000000
#define   CONTENTS_HITBOX               0x40000000

#define   SURF_LIGHT                    0x0001 
#define   SURF_SKY2D                    0x0002 
#define   SURF_SKY                      0x0004 
#define   SURF_WARP                     0x0008 
#define   SURF_TRANS                    0x0010
#define   SURF_NOPORTAL                 0x0020 
#define   SURF_TRIGGER                  0x0040 
#define   SURF_NODRAW                   0x0080 

#define   SURF_HINT                     0x0100 

#define   SURF_SKIP                     0x0200   
#define   SURF_NOLIGHT                  0x0400   
#define   SURF_BUMPLIGHT                0x0800   
#define   SURF_NOSHADOWS                0x1000   
#define   SURF_NODECALS                 0x2000   
#define   SURF_NOPAINT                  SURF_NODECALS
#define   SURF_NOCHOP                   0x4000   
#define   SURF_HITBOX                   0x8000   

// -----------------------------------------------------
// spatial content masks - used for spatial queries (traceline,etc.)
// -----------------------------------------------------
#define   MASK_ALL                      (0xFFFFFFFF)
#define   MASK_SOLID                    (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
#define   MASK_PLAYERSOLID              (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_PLAYERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
#define   MASK_NPCSOLID                 (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
#define   MASK_NPCFLUID                 (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER)
#define   MASK_WATER                    (CONTENTS_WATER|CONTENTS_MOVEABLE|CONTENTS_SLIME)
#define   MASK_OPAQUE                   (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_OPAQUE)
#define   MASK_OPAQUE_AND_NPCS          (MASK_OPAQUE|CONTENTS_MONSTER)
#define   MASK_BLOCKLOS                 (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_BLOCKLOS)
#define   MASK_BLOCKLOS_AND_NPCS        (MASK_BLOCKLOS|CONTENTS_MONSTER)
#define   MASK_VISIBLE                  (MASK_OPAQUE|CONTENTS_IGNORE_NODRAW_OPAQUE)
#define   MASK_VISIBLE_AND_NPCS         (MASK_OPAQUE_AND_NPCS|CONTENTS_IGNORE_NODRAW_OPAQUE)
#define   MASK_SHOT                     (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_HITBOX)
#define   MASK_SHOT_BRUSHONLY           (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_DEBRIS)
#define   MASK_SHOT_HULL                (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_GRATE)
#define   MASK_SHOT_PORTAL              (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTER)
#define   MASK_SOLID_BRUSHONLY          (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_GRATE)
#define   MASK_PLAYERSOLID_BRUSHONLY    (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_PLAYERCLIP|CONTENTS_GRATE)
#define   MASK_NPCSOLID_BRUSHONLY       (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP|CONTENTS_GRATE)
#define   MASK_NPCWORLDSTATIC           (CONTENTS_SOLID|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP|CONTENTS_GRATE)
#define   MASK_NPCWORLDSTATIC_FLUID     (CONTENTS_SOLID|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP)
#define   MASK_SPLITAREAPORTAL          (CONTENTS_WATER|CONTENTS_SLIME)
#define   MASK_CURRENT                  (CONTENTS_CURRENT_0|CONTENTS_CURRENT_90|CONTENTS_CURRENT_180|CONTENTS_CURRENT_270|CONTENTS_CURRENT_UP|CONTENTS_CURRENT_DOWN)
#define   MASK_DEADSOLID                (CONTENTS_SOLID|CONTENTS_PLAYERCLIP|CONTENTS_WINDOW|CONTENTS_GRATE)
#define BigShort( val )    WordSwap( val )
#define BigWord( val )    WordSwap( val )
#define BigLong( val )    DWordSwap( val )
#define BigDWord( val )    DWordSwap( val )
#define LittleShort( val )   ( val )
#define LittleWord( val )   ( val )
#define LittleLong( val )   ( val )
#define LittleDWord( val )   ( val )
#define SwapShort( val )   BigShort( val )
#define SwapWord( val )    BigWord( val )
#define SwapLong( val )    BigLong( val )
#define SwapDWord( val )   BigDWord( val )

#define CRC32_INIT_VALUE 0xFFFFFFFFUL
#define CRC32_XOR_VALUE  0xFFFFFFFFUL

#define NUM_BYTES 256
typedef float Quaternion[4];
typedef float RadianEuler[3];

#define MAX_QPATH  260

#define BONE_CALCULATE_MASK             0x1F
#define BONE_PHYSICALLY_SIMULATED       0x01    // bone is physically simulated when physics are active
#define BONE_PHYSICS_PROCEDURAL         0x02    // procedural when physics is active
#define BONE_ALWAYS_PROCEDURAL          0x04    // bone is always procedurally animated
#define BONE_SCREEN_ALIGN_SPHERE        0x08    // bone aligns to the screen, not constrained in motion.
#define BONE_SCREEN_ALIGN_CYLINDER      0x10    // bone aligns to the screen, constrained by it's own axis.

#define BONE_USED_MASK                  0x0007FF00
#define BONE_USED_BY_ANYTHING           0x0007FF00
#define BONE_USED_BY_HITBOX             0x00000100    // bone (or child) is used by a hit box
#define BONE_USED_BY_ATTACHMENT         0x00000200    // bone (or child) is used by an attachment point
#define BONE_USED_BY_VERTEX_MASK        0x0003FC00
#define BONE_USED_BY_VERTEX_LOD0        0x00000400    // bone (or child) is used by the toplevel model via skinned vertex
#define BONE_USED_BY_VERTEX_LOD1        0x00000800    
#define BONE_USED_BY_VERTEX_LOD2        0x00001000  
#define BONE_USED_BY_VERTEX_LOD3        0x00002000
#define BONE_USED_BY_VERTEX_LOD4        0x00004000
#define BONE_USED_BY_VERTEX_LOD5        0x00008000
#define BONE_USED_BY_VERTEX_LOD6        0x00010000
#define BONE_USED_BY_VERTEX_LOD7        0x00020000
#define BONE_USED_BY_BONE_MERGE         0x00040000    // bone is available for bone merge to occur against it

#define BONE_USED_BY_VERTEX_AT_LOD(lod) ( BONE_USED_BY_VERTEX_LOD0 << (lod) )
#define BONE_USED_BY_ANYTHING_AT_LOD(lod) ( ( BONE_USED_BY_ANYTHING & ~BONE_USED_BY_VERTEX_MASK ) | BONE_USED_BY_VERTEX_AT_LOD(lod) )

#define MAX_NUM_LODS 8
#define MAXSTUDIOBONES		128		// total bones actually used

#define BONE_TYPE_MASK                  0x00F00000
#define BONE_FIXED_ALIGNMENT            0x00100000    // bone can't spin 360 degrees, all interpolation is normalized around a fixed orientation

#define BONE_HAS_SAVEFRAME_POS          0x00200000    // Vector48
#define BONE_HAS_SAVEFRAME_ROT64        0x00400000    // Quaternion64
#define BONE_HAS_SAVEFRAME_ROT32        0x00800000    // Quaternion32

typedef void * (*CreateInterfaceFn)(const char * pName, int * pReturnCode);
struct VMatrix
{
	float m[4][4];

	inline float* operator[](int i)
	{
		return m[i];
	}

	inline const float* operator[](int i) const
	{
		return m[i];
	}
};
bool WorldToScreen(D3DXVECTOR3 from, D3DXVECTOR3& to, const VMatrix * M, int  width, int height);

class Interfaces
{
public:
	void * EngineClient; //can change to classes later
	void * EntList;
	void * Surface;
	void * CHLClient;
	void * EngineTrace;

	CreateInterfaceFn GetModuleFactory(HMODULE module);

	template<typename T>
	T* GetInterface(CreateInterfaceFn Functionm, const char * InterfaceVersion);

	void Initialize();
};

std::uint8_t * PatternScan(void * module, const char * signature);

enum Hitboxes
{
	HITBOX_HEAD,
	HITBOX_NECK,
	HITBOX_LOWER_NECK,
	HITBOX_PELVIS,
	HITBOX_STOMACH,
	HITBOX_LOWER_CHEST,
	HITBOX_CHEST,
	HITBOX_UPPER_CHEST,
	HITBOX_RIGHT_THIGH,
	HITBOX_LEFT_THIGH,
	HITBOX_RIGHT_CALF,
	HITBOX_LEFT_CALF,
	HITBOX_RIGHT_FOOT,
	HITBOX_LEFT_FOOT,
	HITBOX_RIGHT_HAND,
	HITBOX_LEFT_HAND,
	HITBOX_RIGHT_UPPER_ARM,
	HITBOX_RIGHT_FOREARM,
	HITBOX_LEFT_UPPER_ARM,
	HITBOX_LEFT_FOREARM,
	HITBOX_MAX
};

enum LifeState : unsigned char
{
	LIFE_ALIVE = 0,// alive
	LIFE_DYING = 1, // playing death animation or still falling off of a ledge waiting to hit ground
	LIFE_DEAD = 2 // dead. lying still.
};

class VMTHook
{
public:
	void * ClassBase;
	uintptr_t * OldTable;
	size_t TableLen;
	uintptr_t * NewTable;
	uint32_t OldProtection;

	size_t GetTableLen(uintptr_t * TableStart);
	bool Setup(void * Base);
	
	template<typename T>
	void HookIndex(int Index, T Function)
	{
		assert(Index >= 0 && Index < (int)TableLen);
		NewTable[Index] = reinterpret_cast<uintptr_t>(Function);
	}

	template<typename D>
	D GetOriginal(int Index)
	{
		return (D)OldTable[Index];
	}
};

class CUserCmd
{
public:
	virtual ~CUserCmd()
	{ };
	int command_number;
	int tick_count;
	D3DXVECTOR3 viewangles;
	D3DXVECTOR3 aimdirection;
	float forwardmove;
	float sidemove;
	float upmove;
	int buttons;
	byte impulse;
	int weaponselect;
	int weaponsubtype;
	int random_seed;
	short mousedx;
	short mousedy;
	bool hasbeenpredicted;
	char pad_0x4C[0x18];
};
/*
class IPanel
{
public:
	const char *GetName(unsigned int vguiPanel)
	{
		typedef const char *(__thiscall* tGetName)(void*, unsigned int);
		return CallFunction<tGetName>(this, 36)(this, vguiPanel);
	}
#ifdef GetClassName
#undef GetClassName
#endif
	const char *GetClassName(unsigned int vguiPanel)
	{
		typedef const char *(__thiscall* tGetClassName)(void*, unsigned int);
		return CallFunction<tGetClassName>(this, 37)(this, vguiPanel);
	}
};
*/
class IPanel;
class CViewSetup
{
public:
	__int32   x;                  //0x0000 
	__int32   x_old;              //0x0004 
	__int32   y;                  //0x0008 
	__int32   y_old;              //0x000C 
	__int32   width;              //0x0010 
	__int32   width_old;          //0x0014 
	__int32   height;             //0x0018 
	__int32   height_old;         //0x001C 
	char      pad_0x0020[0x90];   //0x0020
	float     fov;                //0x00B0 
	float     viewmodel_fov;      //0x00B4 
	D3DXVECTOR3    origin;             //0x00B8 
	D3DXVECTOR3    angles;             //0x00C4 
	char      pad_0x00D0[0x7C];   //0x00D0

};//Size=0x014C
class IClientMode
{
public:
	virtual             ~IClientMode() {}
	virtual int         ClientModeCSNormal(void *) = 0;
	virtual void        Init() = 0;
	virtual void        InitViewport() = 0;
	virtual void        Shutdown() = 0;
	virtual void        Enable() = 0;
	virtual void        Disable() = 0;
	virtual void        Layout() = 0;
	virtual IPanel*     GetViewport() = 0;
	virtual void*       GetViewportAnimationController() = 0;
	virtual void        ProcessInput(bool bActive) = 0;
	virtual bool        ShouldDrawDetailObjects() = 0;
	virtual bool        ShouldDrawEntity(UINT *pEnt) = 0;
	virtual bool        ShouldDrawLocalPlayer(UINT *pPlayer) = 0;
	virtual bool        ShouldDrawParticles() = 0;
	virtual bool        ShouldDrawFog(void) = 0;
	virtual void        OverrideView(CViewSetup *pSetup) = 0;
	virtual int         KeyInput(int down, int keynum, const char *pszCurrentBinding) = 0;
	virtual void        StartMessageMode(int iMessageModeType) = 0;
	virtual IPanel*     GetMessagePanel() = 0;
	virtual void        OverrideMouseInput(float *x, float *y) = 0;
	virtual bool        CreateMove(float flInputSampleTime, void* usercmd) = 0;
	virtual void        LevelInit(const char *newmap) = 0;
	virtual void        LevelShutdown(void) = 0;
};

void CalculateAngle(D3DXVECTOR3& to, D3DXVECTOR3 Me, D3DXVECTOR3 Them);

float GetYawDiff(D3DXVECTOR3 Me, D3DXVECTOR3 Them);

float GetPitchDiff(D3DXVECTOR3 Me, D3DXVECTOR3 Them);

struct cplane_t
{
	D3DXVECTOR3 normal;
	float dist;
	byte type;
	byte signbits;
	byte pad[2];
};

struct csurface_t
{
	const char* name;
	short surfaceProps;
	unsigned short flags;
};

class CBaseTrace
{
public:
	D3DXVECTOR3 startpos;
	D3DXVECTOR3 endpos;
	cplane_t plane;

	float fraction;

	int contents;
	unsigned short dispFlags;

	bool allsolid;
	bool startsolid;

	CBaseTrace()
	{}
};

class CGameTrace : public CBaseTrace
{
public:
	bool DidHitWorld() const;
	bool DidHitNonWorldEntity() const;
	int GetEntityIndex() const;
	bool DidHit() const;
	bool IsVisible() const;

public:

	float fractionleftsolid;
	csurface_t surface;
	int hitgroup;
	short physicsbone;
	unsigned short worldSurfaceIndex;
	UINT* m_pEnt;
	int hitbox;

	CGameTrace()
	{ }

private:
	CGameTrace(const CGameTrace& vOther);
};

typedef CGameTrace trace_t;

struct matrix3x4_t
{
	matrix3x4_t()
	{}

	matrix3x4_t(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23)
	{
		m_flMatVal[0][0] = m00;
		m_flMatVal[0][1] = m01;
		m_flMatVal[0][2] = m02;
		m_flMatVal[0][3] = m03;
		m_flMatVal[1][0] = m10;
		m_flMatVal[1][1] = m11;
		m_flMatVal[1][2] = m12;
		m_flMatVal[1][3] = m13;
		m_flMatVal[2][0] = m20;
		m_flMatVal[2][1] = m21;
		m_flMatVal[2][2] = m22;
		m_flMatVal[2][3] = m23;
	}

	//-----------------------------------------------------------------------------
	// Creates a matrix where the X axis = forward
	// the Y axis = left, and the Z axis = up
	//-----------------------------------------------------------------------------
	void Init(const D3DXVECTOR3& xAxis, const D3DXVECTOR3& yAxis, const D3DXVECTOR3& zAxis, const D3DXVECTOR3& vecOrigin)
	{
		m_flMatVal[0][0] = xAxis.x;
		m_flMatVal[0][1] = yAxis.x;
		m_flMatVal[0][2] = zAxis.x;
		m_flMatVal[0][3] = vecOrigin.x;
		m_flMatVal[1][0] = xAxis.y;
		m_flMatVal[1][1] = yAxis.y;
		m_flMatVal[1][2] = zAxis.y;
		m_flMatVal[1][3] = vecOrigin.y;
		m_flMatVal[2][0] = xAxis.z;
		m_flMatVal[2][1] = yAxis.z;
		m_flMatVal[2][2] = zAxis.z;
		m_flMatVal[2][3] = vecOrigin.z;
	}

	//-----------------------------------------------------------------------------
	// Creates a matrix where the X axis = forward
	// the Y axis = left, and the Z axis = up
	//-----------------------------------------------------------------------------
	matrix3x4_t(const D3DXVECTOR3& xAxis, const D3DXVECTOR3& yAxis, const D3DXVECTOR3& zAxis, const D3DXVECTOR3& vecOrigin)
	{
		Init(xAxis, yAxis, zAxis, vecOrigin);
	}

	inline void SetOrigin(D3DXVECTOR3 const& p)
	{
		m_flMatVal[0][3] = p.x;
		m_flMatVal[1][3] = p.y;
		m_flMatVal[2][3] = p.z;
	}

	inline void Invalidate(void)
	{
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				m_flMatVal[i][j] = VEC_T_NAN;
			}
		}
	}

	float* operator[](int i)
	{
		Assert((i >= 0) && (i < 3));
		return m_flMatVal[i];
	}

	const float* operator[](int i) const
	{
		Assert((i >= 0) && (i < 3));
		return m_flMatVal[i];
	}

	float* Base()
	{
		return &m_flMatVal[0][0];
	}

	const float* Base() const
	{
		return &m_flMatVal[0][0];
	}

	float m_flMatVal[3][4];
};

class VectorAligned {
public:

	void SetVector(D3DXVECTOR3 n)
	{
		x = n.x;
		y = n.y;
		z = n.z;
	}
	float x;
	float y;
	float z;
	float w;
};

struct Ray_t
{
	Ray_t()
	{ }

	__declspec(align(16))VectorAligned m_Start;

	__declspec(align(16))VectorAligned m_Delta;
	__declspec(align(16))VectorAligned m_StartOffset;
	__declspec(align(16))VectorAligned m_Extents;
	const matrix3x4_t* m_pWorldAxisTransform;
	bool m_IsRay;
	bool m_IsSwept;

	void Init(VectorAligned vecStart, VectorAligned vecEnd)
	{
		m_Delta.x = (vecEnd.x - vecStart.x);
		m_Delta.y = (vecEnd.y - vecStart.y);
		m_Delta.z = (vecEnd.z - vecStart.z);
		m_IsSwept = ((m_Delta.x * m_Delta.x + m_Delta.y * m_Delta.y + m_Delta.z * m_Delta.z) != 0);
		m_Extents.x = 0;
		m_Extents.y = 0;
		m_Extents.z = 0;
		m_pWorldAxisTransform = nullptr;
		m_IsRay = true;
		m_StartOffset.x = 0;
		m_StartOffset.y = 0;
		m_StartOffset.z = 0;
		m_Start = vecStart;
	}
};


enum TraceType_t
{
	TRACE_EVERYTHING = 0,
	TRACE_WORLD_ONLY, // NOTE: This does *not* test static props!!!
	TRACE_ENTITIES_ONLY, // NOTE: This version will *not* test static props
	TRACE_EVERYTHING_FILTER_PROPS, // NOTE: This version will pass the IHandleEntity for props through the filter, unlike all other filters
};

class ITraceFilter
{
public:
	virtual bool ShouldHitEntity(UINT* pEntityHandle, int contentsMask)
	{
		return !(pEntityHandle == pSkip);
	}

	virtual TraceType_t GetTraceType()
	{
		return TRACE_EVERYTHING;
	}

	void* pSkip;
};

class CTraceFilter : public ITraceFilter
{
public:
	virtual bool ShouldHitEntity(UINT* pEntityHandle, int contentsMask)
	{
		return !(pEntityHandle == pSkip);
	}

	virtual TraceType_t GetTraceType() const
	{
		return TRACE_EVERYTHING;
	}

	void* pSkip;
};

class CHudTexture
{
public:
	char szShortName[64]; //0x0000
	char szTextureFile[64]; //0x0040
	bool bRenderUsingFont; //0x0080
	bool bPrecached; //0x0081
	char cCharacterInFont; //0x0082
	BYTE pad_0x0083; //0x0083
	int hFont; //0x0084
	int iTextureId; //0x0088
	float afTexCoords[4]; //0x008C
	int iPosX[4]; //0x009C
};//Size=0x00AC

enum CSWeaponType
{
	WEAPONTYPE_KNIFE = 0,
	WEAPONTYPE_PISTOL,
	WEAPONTYPE_SUBMACHINEGUN,
	WEAPONTYPE_RIFLE,
	WEAPONTYPE_SHOTGUN,
	WEAPONTYPE_SNIPER_RIFLE,
	WEAPONTYPE_MACHINEGUN,
	WEAPONTYPE_C4,
	WEAPONTYPE_GRENADE,
	WEAPONTYPE_UNKNOWN
};

class WeaponInfo_t
{
public:
	BYTE pad_0x0000[4]; //0x0000
	bool bParsedScript; //0x0004
	bool bLoadedHudElements; //0x0005
	char szClassName[80]; //0x0006
	char szPrintName[80]; //0x0056
	char szViewModel[80]; //0x00A6
	char szWorldModel[80]; //0x00F6
	char szBulletType[32]; //0x0146
	char szWorldDroppedModel[80];//0x0166
	char szAnimationPrefix[16]; //0x01B6
	BYTE pad_0x01C6[2]; //0x01C6
	int iSlot; //0x01C8
	int iPosition; //0x01CC
	int iMaxClip1; //0x01D0
	int iMaxClip2; //0x01D4
	int iDefaultClip1; //0x01D8
	int iDefaultClip2; //0x01DC
	int iWeight; //0x01E0
	int iRumbleEffect; //0x01E4
	bool bAutoSwitchTo; //0x01E8
	bool bAutoSwitchFrom; //0x01E9
	BYTE pad_0x01EA[2]; //0x01EA
	int iFlags; //0x01EC
	char szAmmo1[16]; //0x01F0
	char szAmmo2[16]; //0x0200
	char szAIAddOn[80]; //0x0210
	char aShootSounds[17][80]; //0x0260
	char PAD_NEW[80];
	int iAmmoType;
	int iAmmo2Type;
	bool bMeleeWeapon;
	bool bBuiltRightHanded;
	bool bAllowFlipping;
	BYTE pad_0x07BB;
	int iSpriteCount;
	CHudTexture* iconWeaponS;
	CHudTexture* iconWeapon;
	CHudTexture* iconAmmo;
	CHudTexture* iconAmmo2;
	CHudTexture* iconCrosshair;
	CHudTexture* iconAutoaim;
	CHudTexture* iconZoomedCrosshair;
	CHudTexture* iconZoomedAutoaim;
	CHudTexture* iconWeaponSmall;
	bool bShowUsageHint;
	BYTE pad_0x07E5[3];
	CSWeaponType iWeaponType;
	int iTeam;
	int iWeaponID;
	bool bFullAuto;
	BYTE pad_0x07F5[3];
	float flHeatPerShot;
	int iWeaponPrice;
	float flArmorRatio;
	float flMaxPlayerSpeed;
	float flMaxPlayerSpeedAlt;
	int iCrosshairMinDistance;
	int iCrosshairDeltaDistance;
	float flPenetration;
	int iDamage;
	float flRange;
	float flRangeModifier;
	int iBullets;
	float flCycleTime;
	float flCycleTimeAlt;
	char szHeatEffect[80];
	float aflSmokeColor[3];
	char szMuzzleFlashEffect_1stPerson[80];
	char szMuzzleFlashEffect_3rdPerson[80];
	char szEjectBrassEffect[80];
	char szTracerEffect[80];
	int iTracerFrequency;
	float flSpread;
	float flSpreadAlt;
	float flInaccuracyCrouch;
	float flInaccuracyCrouchAlt;
	float flInaccuracyStand;
	float flInaccuracyStandAlt;
	float flInaccuracyJump;
	float flInaccuracyJumpAlt;
	float flInaccuracyLand;
	float flInaccuracyLandAlt;
	float flInaccuracyLadder;
	float flInaccuracyLadderAlt;
	float flInaccuracyFire;
	float flInaccuracyFireAlt;
	float flInaccuracyMove;
	float flInaccuracyMoveAlt;
	float flRecoveryTimeStand;
	float flRecoveryTimeCrouch;
	float flInaccuracyReload;
	float flInaccuracyAltSwitch;
	float flRecoilAngle;
	float flRecoilAngleAlt;
	float flRecoilAngleVariance;
	float flRecoilAngleVarianceAlt;
	float flRecoilMagnitude;
	float flRecoilMagnitudeAlt;
	float flRecoilMagnitudeVariance;
	float flRecoilMagnitudeVarianceAlt;
	int iRecoilSeed;
	float flFlinchVelocityModifierLarge;
	float flFlinchVelocityModifierSmall;
	float flTimeToIdle;
	float flIdleInterval;
	float recoilTable[2][128];
	int iZoomLevels;
	int iZoomFOV[3];
	float fZoomTime[3];
	bool bHideViewModelZoomed;
	char szZoomInSound[80];
	char szZoomOutSound[80];
	BYTE pad_0x0F11[3];
	float flBotAudibleRange;
	bool bCanUseWithShield;
	char szWrongTeamMsg[32];
	char szAnimPrefix[16];
	char szShieldViewModel[64];
	char szAddonModel[80];
	char szAddonLocation[80];
	char szSilencerModel[80];
	BYTE pad_0x1079[3];
	float flAddonScale;
	float flThrowVelocity;
	int iKillAward;
	BYTE pad_0x1088[8];
};

template< typename T >
T GetVFunc(void* vTable, int iIndex);
class CBaseCombatWeapon
{
public:
	char __pad[0x64];
	int index;
	int& GetWeaponID();
	float& GetNextPrimaryAttack();
	float& GetAccuracyPenalty();
	int& GetXUIDLow();
	int& GetXUIDHigh();
	int& GetEntityQuality();
	int& GetAccountID();
	int& GetItemIDHigh();
	int& GetItemDefinitionIndex();
	int& GetFallbackPaintKit();
	int& GetFallbackStatTrak();
	float& GetFallbackWear();
	bool IsEmpty();
	bool IsReloading();
	void UpdateAccuracyPenalty();
	float GetWeaponSpread();
	float GetWeaponCone();
	WeaponInfo_t* GetCSWpnData();
	bool IsGun();
	std::string GetWeaponName();
};
