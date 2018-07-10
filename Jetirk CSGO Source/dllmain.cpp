// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include <windows.h>
#include "imgui.h"
#include "imconfig.h"
#include "imgui_internal.h"
#include "imgui_impl_dx9.h"
#include <d3d9.h>
#include <Dwmapi.h>
#include <d3dx9.h>
#include <d3dx9core.h>
#include <vector>
#include "Random.h"
#include "DXClass.h"
#include "Global.h"
#pragma comment (lib, "d3dx9")


HMODULE Library;
DxManager DxMan;
VMTHook DxHooker, ClientModeHooker;
Interfaces Interface;
int InGame = 0;
DWORD ClientBase, EngineBase;
IClientMode * ClientMode;
CUserCmd * UserCmd;



using EndScene = long(__stdcall *)(IDirect3DDevice9*);
using ClientCmd_Unrestricted = void(__thiscall *)(const char *);
using GetEntityFn = UINT(__fastcall*)(void * entityList, void * edx, int index);
using GetEntFromHandleFn = UINT(__fastcall*)(void * entlist, void * edx, DWORD handle);
using GetLocalPlayerFn = int(__fastcall*)(void * engineClient, void * edx);
using GetViewMatrixFn = VMatrix*(__fastcall*)(void * engineClient, void * edx);
using GetScreenSizeFn = void(__fastcall*)(void * surface, void * edx, int * width, int * height);
using Reset = long(__stdcall *)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
using CreateMove = bool(__fastcall*)(float, CUserCmd *);
using SetViewAnglesFn = void(__fastcall*)(void * pussy, void * edx, D3DXVECTOR3& va);
using TraceRayFn = void(__fastcall*)(void * Jews, void * edx, const Ray_t& ray, unsigned int fMask, ITraceFilter * pFilter, trace_t * pTrace);
DWORD delay = 0;
DWORD lasttime = 0;
//using CreateMove = void(__thiscall*)(IBaseClientDLL*, int, float, bool);


struct Offsets_t {
	DWORD m_ArmorValue = 0xB248;
	DWORD m_Collision = 0x318;
	DWORD m_CollisionGroup = 0x470;
	DWORD m_Local = 0x2FAC;
	DWORD m_MoveType = 0x258;
	DWORD m_OriginalOwnerXuidHigh = 0x316C;
	DWORD m_OriginalOwnerXuidLow = 0x3168;
	DWORD m_aimPunchAngle = 0x301C;
	DWORD m_aimPunchAngleVel = 0x3028;
	DWORD m_bGunGameImmunity = 0x38B0;
	DWORD m_bHasDefuser = 0xB258;
	DWORD m_bHasHelmet = 0xB23C;
	DWORD m_bInReload = 0x3245;
	DWORD m_bIsDefusing = 0x38A4;
	DWORD m_bIsScoped = 0x389C;
	DWORD m_bSpotted = 0x939;
	DWORD m_bSpottedByMask = 0x97C;
	DWORD m_dwBoneMatrix = 0x2698;
	DWORD m_fAccuracyPenalty = 0x32C0;
	DWORD m_fFlags = 0x100;
	DWORD m_flFallbackWear = 0x3178;
	DWORD m_flFlashDuration = 0xA308;
	DWORD m_flFlashMaxAlpha = 0xA304;
	DWORD m_flNextPrimaryAttack = 0x31D8;
	DWORD m_hActiveWeapon = 0x2EE8;
	DWORD m_hMyWeapons = 0x2DE8;
	DWORD m_hObserverTarget = 0x3380;
	DWORD m_hOwner = 0x29BC;
	DWORD m_hOwnerEntity = 0x148;
	DWORD m_iAccountID = 0x2FA8;
	DWORD m_iClip1 = 0x3204;
	DWORD m_iCompetitiveRanking = 0x1A44;
	DWORD m_iCompetitiveWins = 0x1B48;
	DWORD m_iCrosshairId = 0xB2B4;
	DWORD m_iEntityQuality = 0x2F8C;
	DWORD m_iFOVStart = 0x31D8;
	DWORD m_iGlowIndex = 0xA320;
	DWORD m_iHealth = 0xFC;
	DWORD m_iItemDefinitionIndex = 0x2F88;
	DWORD m_iItemIDHigh = 0x2FA0;
	DWORD m_iObserverMode = 0x336C;
	DWORD m_iShotsFired = 0xA2C0;
	DWORD m_iState = 0x31F8;
	DWORD m_iTeamNum = 0xF0;
	DWORD m_lifeState = 0x25B;
	DWORD m_nFallbackPaintKit = 0x3170;
	DWORD m_nFallbackSeed = 0x3174;
	DWORD m_nFallbackStatTrak = 0x317C;
	DWORD m_nForceBone = 0x267C;
	DWORD m_nTickBase = 0x3424;
	DWORD m_rgflCoordinateFrame = 0x440;
	DWORD m_szCustomName = 0x301C;
	DWORD m_szLastPlaceName = 0x35A8;
	DWORD m_vecOrigin = 0x134;
	DWORD m_vecVelocity = 0x110;
	DWORD m_vecViewOffset = 0x104;
	DWORD m_viewPunchAngle = 0x3010;

	DWORD dwClientState = 0x5A3334;
	DWORD dwClientState_GetLocalPlayer = 0x180;
	DWORD dwClientState_Map = 0x28C;
	DWORD dwClientState_MapDirectory = 0x188;
	DWORD dwClientState_MaxPlayer = 0x310;
	DWORD dwClientState_PlayerInfo = 0x5240;
	DWORD dwClientState_State = 0x108;
	DWORD dwClientState_ViewAngles = 0x4D10;
	DWORD dwClientState_IsHLTV = 0x4CC8;
	DWORD dwEntityList = 0x4A8C844;
	DWORD dwForceAttack = 0x2ECEC10;
	DWORD dwForceAttack2 = 0x2ECEC1C;
	DWORD dwForceBackward = 0x2ECEC34;
	DWORD dwForceForward = 0x2ECEC28;
	DWORD dwForceJump = 0x4F237DC;
	DWORD dwForceLeft = 0x2ECEBD4;
	DWORD dwForceRight = 0x2ECEBE0;
	DWORD dwGameDir = 0x641C50;
	DWORD dwGameRulesProxy = 0x4F8E0FC;
	DWORD dwGetAllClasses = 0x4F8E1D0;
	DWORD dwGlobalVars = 0x5A3038;
	DWORD dwGlowObjectManager = 0x4FA9848;
	DWORD dwInput = 0x4ED7458;
	DWORD dwInterfaceLinkList = 0x6EAF74;
	DWORD dwLocalPlayer = 0xAAFFEC;
	DWORD dwMouseEnable = 0xAB5850;
	DWORD dwMouseEnablePtr = 0xAB5820;
	DWORD dwPlayerResource = 0x2ECCF4C;
	DWORD dwRadarBase = 0x4EC153C;
	DWORD dwSensitivity = 0xAB56EC;
	DWORD dwSensitivityPtr = 0xAB56C0;
	DWORD dwViewMatrix = 0x4A7E2A4;
	DWORD dwWeaponTable = 0x4ED3A04;
	DWORD dwWeaponTableIndex = 0x3270;
	DWORD dwYawPtr = 0xAB54B0;
	DWORD dwZoomSensitivityRatioPtr = 0xABA518;
	DWORD dwbSendPackets = 0xCAC4A;
	DWORD dwppDirect3DDevice9 = 0x9CF40;
	DWORD dwSetClanTag = 0x849D0;
	DWORD m_pStudioHdr = 0x293C;
}Offsets;


LRESULT __stdcall  WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	DxMan.ProcessMessage(msg, wParam, lParam);

	if (DxMan.MenuVisible && DxMan.ImGui_ImplDX9_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	return CallWindowProcW((WNDPROC)DxMan.OldWndProc, hWnd, msg, wParam, lParam);
}


template<typename FunctionType>
FunctionType CallFunction(void * ClassBase, int Index)
{
	int * pVTable = *(int**)ClassBase;
	int FunctionAddr = pVTable[Index];
	return (FunctionType)FunctionAddr;
}

typedef void(*pConMsg)(char*, ...);

pConMsg ConMsg;



void newThread() {
	ConMsg = (pConMsg)GetProcAddress(GetModuleHandle("tier0.dll"), "Msg");
	//	ConMsg("Hello world! I'm at: %.8X\n", ConMsg);
}

void inline SinCos(float radians, float* sine, float* cosine)
{
	*sine = sin(radians);
	*cosine = cos(radians);
}


void AngleVectors(const D3DXVECTOR3& angles, D3DXVECTOR3 * forward)
{
	float sp, sy, cp, cy;

	SinCos(D3DXToRadian(angles.y), &sy, &cy);
	SinCos(D3DXToRadian(angles.x), &sp, &cp);

	forward->x = cp * cy;
	forward->y = cp * sy;
	forward->z = -sp;
}

void TraceRay(const Ray_t& ray, unsigned int fMask, ITraceFilter * pTraceFilter, trace_t * pTrace)
{
	TraceRayFn penis = CallFunction<TraceRayFn>(Interface.EngineTrace, 5);
	penis(Interface.EngineTrace, NULL, ray, fMask, pTraceFilter, pTrace);
}




void glow()
{
	HMODULE mod = GetModuleHandle("client.dll");
	DWORD ClientBase = (DWORD)mod;
	mod = GetModuleHandle("engine.dll");
	DWORD EngineBase = (DWORD)mod;
	DWORD GlowBase = 0;

	GlowBase = *(DWORD*)(ClientBase + Offsets.dwGlowObjectManager);

}
void DxDrawEsp();
bool bReset = false;
long __stdcall EndSceneHook(IDirect3DDevice9 * device)
{
	DWORD OldState = 0;

	auto oEndScene = DxHooker.GetOriginal<EndScene>(42);
	DxMan.DXDevice->GetRenderState(D3DRS_COLORWRITEENABLE, &OldState);
	DxMan.DXDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
	DxMan.Render();
	DxDrawEsp();
	DxMan.DXDevice->SetRenderState(D3DRS_COLORWRITEENABLE, OldState );
//	ConMsg("EndScene is hooked\n");
	return oEndScene(device);
}

long __stdcall ResetHook(IDirect3DDevice9 * device, D3DPRESENT_PARAMETERS * pPresentationParameters)
{
	auto oReset = DxHooker.GetOriginal<Reset>(16);
	DxMan.OnDeviceLost();
	ConMsg("Reset is hooked\n");
	auto hr = oReset(device, pPresentationParameters);
	//if (hr >= 0) {
		ConMsg("Reseting devices\n");
		DxMan.OnDeviceReset();
	//
		return hr;
}

void SetViewAngles(D3DXVECTOR3& va)
{
	SetViewAnglesFn _SetEm = CallFunction<SetViewAnglesFn>(Interface.EngineClient, 19);
	return _SetEm(Interface.EngineClient, NULL, va);
}
void trig();

void BunnyHop();
void DoAimBot();

bool __stdcall CreateMoveHook(float SampleTime, CUserCmd* cmd)
{
	UserCmd = cmd;
	trig();
	BunnyHop();
	DoAimBot();
	SetViewAngles(UserCmd->viewangles);
	
	return false; 
}


void UCMDH(const char * str)
{
	ClientCmd_Unrestricted oCCU = CallFunction<ClientCmd_Unrestricted>(Interface.EngineClient, 114);
	oCCU(str);
}

D3DXVECTOR3 GetBonePos(UINT Entity, int Bone)
{
	DWORD BoneAddress = *(DWORD*)(Entity + Offsets.m_dwBoneMatrix);
	D3DXVECTOR3 Pos;
	Pos.x = *(float*)(BoneAddress + 0x30 * Bone + 0x0C);
	Pos.y = *(float*)(BoneAddress + 0x30 * Bone + 0x1C);
	Pos.z = *(float*)(BoneAddress + 0x30 * Bone + 0x2C);
	return Pos;
}

UINT GetEntity(int Index)
{
	GetEntityFn _GetEntity = CallFunction<GetEntityFn>(Interface.EntList, 3);
	return _GetEntity(Interface.EntList, NULL, Index);
}

int GetLocalPlayer()
{
	GetLocalPlayerFn _GetLocalPlayer = CallFunction<GetLocalPlayerFn>(Interface.EngineClient, 12);
	return _GetLocalPlayer(Interface.EngineClient, NULL);
}

int GetEntityFlags(UINT entity)
{
	return *(int*)(entity + Offsets.m_fFlags);
}

int GetEntityTeam(UINT entity)
{
	return *(int*)(entity + Offsets.m_iTeamNum);
}

D3DXVECTOR3 GetEntityOrigin(UINT entity)
{
	return *(D3DXVECTOR3*)(entity + Offsets.m_vecOrigin);
}

D3DXVECTOR3 GetEntityHead(UINT entity)
{
	D3DXVECTOR3 Origin = GetEntityOrigin(entity);
	D3DXVECTOR3 Head = Origin;
	Head.z += 64.062561;
	return Head;
}

int GetEntityHealth(UINT entity)
{
	return *(int*)(entity + Offsets.m_iHealth);
}
/*
struct CollisionProperty *GetEntityCollisionProperty(UINT entity)
{
	return (struct CollisionProperty*)(entity + OFFSET_ENTITY_VECMINS);
}
*/
BOOL GetEntityDormant(UINT entity)
{
	return *(BOOL*)(entity + 0xE9);
}

CBaseCombatWeapon * GetEntityWeapon(UINT ent)
{
	DWORD Weapon = *(DWORD*)(ent + Offsets.m_hActiveWeapon);
	GetEntFromHandleFn _GetEntityh = CallFunction<GetEntFromHandleFn>(Interface.EntList, 4);
	return (CBaseCombatWeapon*)_GetEntityh(Interface.EntList, NULL, Weapon);
}

VMatrix * GetViewMatrix()
{
	GetViewMatrixFn _fucknames = CallFunction<GetViewMatrixFn>(Interface.EngineClient, 37);
	return _fucknames(Interface.EngineClient, NULL);
}


void GetScreenSize(int * Width, int * Height)
{
	GetScreenSizeFn _getsize = CallFunction<GetScreenSizeFn>(Interface.Surface, 44); //was 35
	_getsize(Interface.Surface, NULL, Width, Height);
}
void BunnyHop()
{
	static bool LastJumped = false;
	static bool ShouldFake = false;
	if (DxMan.BunnyHopOn) {
		UINT Lp = GetEntity(GetLocalPlayer());

		if (!LastJumped && ShouldFake) {
			ShouldFake = false;
			UserCmd->buttons |= IN_JUMP;
		}
		else if (UserCmd->buttons & IN_JUMP) {
			if (GetEntityFlags(Lp) & FL_ONGROUND) {
				LastJumped = true;
				ShouldFake = true;
			}
			else {
				UserCmd->buttons &= ~IN_JUMP;
				LastJumped = false;
			}
		}
		else {
			LastJumped = false;
			ShouldFake = false;
		}
	}
}

void trig()
{
	if (DxMan.TriggerOn) {
		D3DXVECTOR3 Source, Dst, Forward;
		trace_t Trace;
		Ray_t Ray;
		CTraceFilter Filter;
		VectorAligned VASource, VADst;
		D3DXVECTOR3 ViewAngles = UserCmd->viewangles;
		//add punch here
		AngleVectors(ViewAngles, &Forward);
		Forward *= GetEntityWeapon(GetEntity(GetLocalPlayer()))->GetCSWpnData()->flRange;//8192;
		Filter.pSkip = (void*)GetEntity(GetLocalPlayer());
		Source = GetEntityHead(GetEntity(GetLocalPlayer()));
		Dst = Source + Forward;
		VASource.x = Source.x;
		VASource.y = Source.y;
		VASource.z = Source.z;
		VASource.w = 0;
		VADst.x = Dst.x;
		VADst.y = Dst.y;
		VADst.z = Dst.z;
		VADst.w = 0;
		Ray.Init(VASource,VADst );
		TraceRay(Ray, 0x46004003, &Filter, &Trace);
		if (!Trace.m_pEnt)
			return;
	//	UINT Ent = (UINT)Trace.m_pEnt;
	//	if (GetEntityDormant(Ent) == TRUE)
	//		return; 
	//	if (GetEntityDormant(Ent) == FALSE && GetEntityHealth(Ent) > 0 && GetEntityTeam(Ent) != GetEntityTeam(GetEntity(GetLocalPlayer()))) {
	//		UserCmd->buttons |= IN_ATTACK;
	//	}
	}


}

void DrawSkeleton(int i)
{
	if (GetLocalPlayer() == i)
		return;
	D3DXVECTOR2 Line[2];
	D3DXVECTOR3 to, from;
	UINT Ent = GetEntity(i);
	if (Ent == 0 || GetEntityHealth(Ent) < 1 || GetEntityDormant(Ent) == TRUE)
		return;
	VMatrix * m = GetViewMatrix();
	int width, height;  
	GetScreenSize(&width, &height);
	DxMan.pLine->Begin();
	DxMan.pLine->SetWidth(1);
	WorldToScreen(GetBonePos(Ent, 79), from, m, width, height);
	WorldToScreen(GetBonePos(Ent, 78), to, m, width, height);
	Line[0].x = from.x;
	Line[0].y = from.y;
	Line[1].x = to.x;
	Line[1].y = to.y;
	DxMan.pLine->Draw(Line, 2, D3DCOLOR_RGBA(255, 255, 255, 255));

	WorldToScreen(GetBonePos(Ent, 78), from, m, width, height);
	WorldToScreen(GetBonePos(Ent, 77), to, m, width, height);
	Line[0].x = from.x;
	Line[0].y = from.y;
	Line[1].x = to.x;
	Line[1].y = to.y;
	DxMan.pLine->Draw(Line, 2, D3DCOLOR_RGBA(255, 255, 255, 255));

	WorldToScreen(GetBonePos(Ent, 79), from, m, width, height);
	WorldToScreen(GetBonePos(Ent, 78), to, m, width, height);
	Line[0].x = from.x;
	Line[0].y = from.y;
	Line[1].x = to.x;
	Line[1].y = to.y;
	DxMan.pLine->Draw(Line, 2, D3DCOLOR_RGBA(255, 255, 255, 255));

	WorldToScreen(GetBonePos(Ent, 78), from, m, width, height);
	WorldToScreen(GetBonePos(Ent, 3), to, m, width, height);
	Line[0].x = from.x;
	Line[0].y = from.y;
	Line[1].x = to.x;
	Line[1].y = to.y;
	DxMan.pLine->Draw(Line, 2, D3DCOLOR_RGBA(255, 255, 255, 255));

	WorldToScreen(GetBonePos(Ent, 72), from, m, width, height);
	WorldToScreen(GetBonePos(Ent, 71), to, m, width, height);
	Line[0].x = from.x;
	Line[0].y = from.y;
	Line[1].x = to.x;
	Line[1].y = to.y;
	DxMan.pLine->Draw(Line, 2, D3DCOLOR_RGBA(255, 255, 255, 255));

	WorldToScreen(GetBonePos(Ent, 71), from, m, width, height);
	WorldToScreen(GetBonePos(Ent, 70), to, m, width, height);
	Line[0].x = from.x;
	Line[0].y = from.y;
	Line[1].x = to.x;
	Line[1].y = to.y;
	DxMan.pLine->Draw(Line, 2, D3DCOLOR_RGBA(255, 255, 255, 255));

	WorldToScreen(GetBonePos(Ent, 70), from, m, width, height);
	WorldToScreen(GetBonePos(Ent, 3), to, m, width, height);
	Line[0].x = from.x;
	Line[0].y = from.y;
	Line[1].x = to.x;
	Line[1].y = to.y;
	DxMan.pLine->Draw(Line, 2, D3DCOLOR_RGBA(255, 255, 255, 255));

	WorldToScreen(GetBonePos(Ent, 3), from, m, width, height);
	WorldToScreen(GetBonePos(Ent, 7), to, m, width, height);
	Line[0].x = from.x;
	Line[0].y = from.y;
	Line[1].x = to.x;
	Line[1].y = to.y;
	DxMan.pLine->Draw(Line, 2, D3DCOLOR_RGBA(255, 255, 255, 255));

	WorldToScreen(GetBonePos(Ent, 7), from, m, width, height);
	WorldToScreen(GetBonePos(Ent, 8), to, m, width, height);
	Line[0].x = from.x;
	Line[0].y = from.y;
	Line[1].x = to.x;
	Line[1].y = to.y;
	DxMan.pLine->Draw(Line, 2, D3DCOLOR_RGBA(255, 255, 255, 255));

	WorldToScreen(GetBonePos(Ent, 7), from, m, width, height);
	WorldToScreen(GetBonePos(Ent, 41), to, m, width, height);
	Line[0].x = from.x;
	Line[0].y = from.y;
	Line[1].x = to.x;
	Line[1].y = to.y;
	DxMan.pLine->Draw(Line, 2, D3DCOLOR_RGBA(255, 255, 255, 255));

	WorldToScreen(GetBonePos(Ent, 7), from, m, width, height);
	WorldToScreen(GetBonePos(Ent, 11), to, m, width, height);
	Line[0].x = from.x;
	Line[0].y = from.y;
	Line[1].x = to.x;
	Line[1].y = to.y;
	DxMan.pLine->Draw(Line, 2, D3DCOLOR_RGBA(255, 255, 255, 255));

	WorldToScreen(GetBonePos(Ent, 41), from, m, width, height);
	WorldToScreen(GetBonePos(Ent, 42), to, m, width, height);
	Line[0].x = from.x;
	Line[0].y = from.y;
	Line[1].x = to.x;
	Line[1].y = to.y;
	DxMan.pLine->Draw(Line, 2, D3DCOLOR_RGBA(255, 255, 255, 255));

	WorldToScreen(GetBonePos(Ent, 11), from, m, width, height);
	WorldToScreen(GetBonePos(Ent, 12), to, m, width, height);
	Line[0].x = from.x;
	Line[0].y = from.y;
	Line[1].x = to.x;
	Line[1].y = to.y;
	DxMan.pLine->Draw(Line, 2, D3DCOLOR_RGBA(255, 255, 255, 255));

	WorldToScreen(GetBonePos(Ent, 42), from, m, width, height);
	WorldToScreen(GetBonePos(Ent, 43), to, m, width, height);
	Line[0].x = from.x;
	Line[0].y = from.y;
	Line[1].x = to.x;
	Line[1].y = to.y;
	DxMan.pLine->Draw(Line, 2, D3DCOLOR_RGBA(255, 255, 255, 255));

	WorldToScreen(GetBonePos(Ent, 12), from, m, width, height);
	WorldToScreen(GetBonePos(Ent, 13), to, m, width, height);
	Line[0].x = from.x;
	Line[0].y = from.y;
	Line[1].x = to.x;
	Line[1].y = to.y;
	DxMan.pLine->Draw(Line, 2, D3DCOLOR_RGBA(255, 255, 255, 255));

	DxMan.pLine->End();
}

bool IsVisible(UINT Ent)
{
	
	D3DXVECTOR3 Source, Dst;//, Forward;
	trace_t Trace;
	Ray_t Ray;
	CTraceFilter Filter;
	VectorAligned VASource, VADst;
	D3DXVECTOR3 ViewAngles = UserCmd->viewangles;
	//add punch here
//		AngleVectors(ViewAngles, &Forward);
//		Forward *= GetEntityWeapon(GetEntity(GetLocalPlayer()))->GetCSWpnData()->flRange;//8192;
	Filter.pSkip = (void*)GetEntity(GetLocalPlayer());
	Source = GetEntityHead(GetEntity(GetLocalPlayer()));
	Dst = GetBonePos(Ent, 8);
	VASource.x = Source.x;
	VASource.y = Source.y;
	VASource.z = Source.z;
	VASource.w = 0;
	VADst.x = Dst.x;
	VADst.y = Dst.y;
	VADst.z = Dst.z;
	VADst.w = 0;
	Ray.Init(VASource, VADst);
	TraceRay(Ray, 0x46004003, &Filter, &Trace);
	if (!Trace.m_pEnt)
		return false;

	UINT EntT = (UINT)Trace.m_pEnt;
	if (GetEntityDormant(EntT) == FALSE && GetEntityHealth(EntT) > 0 && GetEntityTeam(EntT) != GetEntityTeam(GetEntity(GetLocalPlayer()))) {
		return true;
	}
	else {
		return false;
	}
	

}

float SmallestDistance = 999999;
int SmallIndex;
void DoAimBot()
{
	UINT Ent;
	D3DXVECTOR3 Angle,Me,Them, AimAngles;
	float YawDiff, PitchDiff, YawAimDiff, PitchAimDiff;
	for (int i = 0; i < 64; ++i) {
		Ent = GetEntity(i);
		if (!Ent)
			continue;
		if (GetEntityDormant(Ent) == TRUE || GetEntityHealth(Ent) < 1)
			continue;
		D3DXVECTOR3 MyHead = GetEntityHead(GetEntity(GetLocalPlayer()));
		D3DXVECTOR3 TheirHead = GetBonePos(Ent, 8);
		int MyTeam = GetEntityTeam(GetEntity(GetLocalPlayer()));
		int TheirTeam = GetEntityTeam(Ent);
		float Distance = DxMan.GetDistance(MyHead, TheirHead);
		if (Distance < SmallestDistance && MyTeam != TheirTeam ) {
			CalculateAngle(Angle, MyHead, TheirHead);
			YawDiff = GetYawDiff(MyHead, TheirHead);
			PitchDiff = GetPitchDiff(MyHead, TheirHead);
			YawAimDiff = sin(D3DXToRadian(YawDiff)) * Distance;
			PitchAimDiff = sin(D3DXToRadian(PitchDiff)) * Distance;
			if ((DxMan.FovOn == false) || (DxMan.FovOn == true && YawAimDiff <= DxMan.FovAmount && PitchAimDiff <= DxMan.FovAmount && YawDiff < 90)) {
				if (IsVisible(Ent)) {
					ConMsg("Visible\n");
					SmallestDistance = Distance;
					SmallIndex = i;
				}
			}
		}
	}
	Ent = GetEntity(SmallIndex);
	if (GetEntityHealth(Ent) < 1 || GetEntityDormant(Ent) == TRUE)
		SmallestDistance = 999999999;
	Me = GetEntityHead(GetEntity(GetLocalPlayer()));
	Them = GetBonePos(Ent, 8);
	CalculateAngle(AimAngles, Me, Them);

}

void DxDrawEsp()
{
	if (InGame != NULL && DxMan.EspOn) {
		D3DXVECTOR3 PointEyePos, PointAbsPos, Head, Origin, MyHead;
		COLORREF Color = RGB(0, 0, 0);
		D3DCOLOR DColor = D3DCOLOR_RGBA(255, 0, 0, 255);
		UINT Ent = 0;
		int  Width, Height, Team, MyTeam, MyIndex;
		//include draw circle here
		MyIndex = GetLocalPlayer();
		MyTeam = GetEntityTeam(GetEntity(MyIndex));
		for (int i = 1; i < 64; ++i) {
			if (i == MyIndex)
				continue;
			Ent = GetEntity(i);
			if (Ent == 0 || GetEntityHealth(Ent) < 1 || GetEntityDormant(Ent) == TRUE)
				continue;
			Team = GetEntityTeam(Ent);
			GetScreenSize(&Width, &Height);


			const VMatrix * M = GetViewMatrix();
			Head = GetEntityHead(Ent);
			Origin = GetEntityOrigin(Ent);
			MyHead = GetEntityHead(GetEntity(GetLocalPlayer()));

			if (WorldToScreen(Head, PointEyePos, M, Width, Height)) {
				if (WorldToScreen(Origin, PointAbsPos, M, Width, Height)) {

					int xOffset = (PointAbsPos.y - PointEyePos.y) / 3;
					int yOffset = (PointAbsPos.y - PointEyePos.y) / 6;
					int leftX = PointEyePos.x - xOffset;
					int rightX = PointEyePos.x + xOffset;
					int topY = PointEyePos.y - yOffset;
					int botY = PointAbsPos.y - PointEyePos.y + (yOffset * 1.8);
					int owidth = rightX - leftX;
					
					//add crouch check later

					//add team check
					int r, b, g;
					if (GetEntityDormant(Ent) == FALSE && MyTeam == Team) {
						r = DxMan.FriendlyBoxColor.r();
						g = DxMan.FriendlyBoxColor.g();
						b = DxMan.FriendlyBoxColor.b();
						DxMan.BoxFillRGBA(leftX, topY, owidth, botY, r, g, b, 255);
					}
					if (GetEntityDormant(Ent) == FALSE && MyTeam != Team) {
						r = DxMan.EnemyBoxColor.r();
						g = DxMan.EnemyBoxColor.g();
						b = DxMan.EnemyBoxColor.b();
						DxMan.BoxFillRGBA(leftX, topY, owidth, botY, r, g, b, 255);
					}
					if (GetEntityDormant(Ent) == FALSE && DxMan.HealthBarOn) {
						DxMan.HealthBar(leftX, topY, botY,GetEntityHealth(Ent) , DxMan.GetDistance(MyHead, Head));
					}
					if (DxMan.LineOn || DxMan.SkeletonOn) {
						D3DXCreateLine(DxMan.DXDevice, &DxMan.pLine);
						DxMan.LineToBeReleased = true;
					}
					if (GetEntityDormant(Ent) == FALSE && DxMan.LineOn && MyTeam != Team) {
						r = DxMan.EnemyLineColor.r();
						g = DxMan.EnemyLineColor.g();
						b = DxMan.EnemyLineColor.b();
						DxMan.DrawTraceLine(PointEyePos.x,PointEyePos.y, D3DCOLOR_RGBA(r, g, b, 255), Width, Height);
					}
					if (GetEntityDormant(Ent) == FALSE && DxMan.LineOn && MyTeam == Team) {
						r = DxMan.FriendlyLineColor.r();
						g = DxMan.FriendlyLineColor.g();
						b = DxMan.FriendlyLineColor.b();
						DxMan.DrawTraceLine(PointEyePos.x, PointEyePos.y, D3DCOLOR_RGBA(r,g,b,255), Width, Height);
					}
					//	DxMan.DrawDistance(leftX, topY, owidth, botY, DxMan.ParseInt(((int)DxMan.GetDistance(MyHead, Head))/30));
					if (GetEntityDormant(Ent) == FALSE && DxMan.SkeletonOn) {
						for (int i = 1; i < 64; ++i) {
							DrawSkeleton(i);
						}
					}
					if (DxMan.LineToBeReleased) {
						DxMan.pLine->Release();
						DxMan.LineToBeReleased = false;
					}
				}
			}
		}
	}
}


void Glow()
{
	CGlowObjectManager * GlowMan = (CGlowObjectManager*)(ClientBase + Offsets.dwGlowObjectManager);
	for (int i = 0; i < GlowMan->size; ++i) {
		CGlowObjectManager::GlowObjectDefinition_t * glowEntity = &GlowMan->m_GlowObjectDefinitions[i];
		UINT  Ent = (UINT)glowEntity->getEntity();
		if (!Ent || glowEntity->IsEmpty()) 
			continue;
		if (GetEntityDormant(Ent) == TRUE || GetEntity(GetLocalPlayer()) == Ent || GetEntityHealth(Ent) < 1)
			continue;
		int r, g, b;
		if (GetEntityTeam(Ent) != GetEntityTeam(GetEntity(GetLocalPlayer()))) {
			r = DxMan.EnemyGlowColor.r();
			g = DxMan.EnemyGlowColor.g();
			b = DxMan.EnemyGlowColor.b();
			glowEntity->set(Color(r, g, b, 255));
		}
		if (GetEntityTeam(Ent) == GetEntityTeam(GetEntity(GetLocalPlayer()))) {
			r = DxMan.FriendlyGlowColor.r();
			g = DxMan.FriendlyGlowColor.g();
			b = DxMan.FriendlyGlowColor.b();
			glowEntity->set(Color(r, g, b, 255));
		}
	}
}

void main() 
{
	HMODULE mod = GetModuleHandle("client.dll");
	ClientBase = (DWORD)mod;
	mod = GetModuleHandle("engine.dll");
	EngineBase = (DWORD)mod;
	DWORD ClientState = 0;
	
	newThread();
	DxMan.Intitialize();
	DxMan.MenuInit();
	DxHooker.Setup(DxMan.DXDevice);
	DxHooker.HookIndex(16, ResetHook);
	DxHooker.HookIndex(42, EndSceneHook);
	

	Interface.Initialize();
	ClientMode = *(IClientMode**)(PatternScan((void *)ClientBase, "A1 ? ? ? ? 8B 80 ? ? ? ? 5D") + 1);
	ClientModeHooker.Setup(ClientMode);
	ClientModeHooker.HookIndex(24, CreateMoveHook);

	while (1) {
		if (DxMan.Unload)
			FreeLibraryAndExitThread(Library, 1);
		if (GetAsyncKeyState(VK_INSERT) & 0x8000)
			DxMan.MenuVisible = !DxMan.MenuVisible;
		ClientState = *(DWORD*)(EngineBase + Offsets.dwClientState);
		InGame = *(int*)(ClientState + Offsets.dwClientState_State);
		if (InGame == 0) {
			DxMan.SetAllFalse();
		}
		if (InGame) {	
			if (DxMan.GlowOn)
				Glow();
		}
	}
}

INT APIENTRY DllMain(HINSTANCE hinstance, DWORD reason, LPVOID reserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		Library = (HMODULE)hinstance;
		CreateThread(0, 0, LPTHREAD_START_ROUTINE(main), 0, 0, 0);
		return TRUE;
		break;
	case DLL_PROCESS_DETACH:
		return TRUE;
		break;
	}
}