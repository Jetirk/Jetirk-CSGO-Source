#include "stdafx.h"
#include "Random.h"

//struct Offsets_t Offsets;

template< typename T >
T GetVFunc(void* vTable, int iIndex)
{
	return (*(T**)vTable)[iIndex];
}

std::uint8_t* PatternScan(void* module, const char* signature)
{
	static auto pattern_to_byte = [](const char* pattern) {
		auto bytes = std::vector<int>{};
		auto start = const_cast<char*>(pattern);
		auto end = const_cast<char*>(pattern) + strlen(pattern);

		for (auto current = start; current < end; ++current) {
			if (*current == '?') {
				++current;
				if (*current == '?')
					++current;
				bytes.push_back(-1);
			}
			else {
				bytes.push_back(strtoul(current, &current, 16));
			}
		}
		return bytes;
	};

	auto dosHeader = (PIMAGE_DOS_HEADER)module;
	auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)module + dosHeader->e_lfanew);

	auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
	auto patternBytes = pattern_to_byte(signature);
	auto scanBytes = reinterpret_cast<std::uint8_t*>(module);

	auto s = patternBytes.size();
	auto d = patternBytes.data();

	for (auto i = 0ul; i < sizeOfImage - s; ++i) {
		bool found = true;
		for (auto j = 0ul; j < s; ++j) {
			if (scanBytes[i + j] != d[j] && d[j] != -1) {
				found = false;
				break;
			}
		}
		if (found) {
			return &scanBytes[i];
		}
	}
	return nullptr;
}

size_t VMTHook::GetTableLen(uintptr_t * TableStart)
{
	auto Length = size_t{};

	while (TableStart[Length] >= 0x00010000 && TableStart[Length] < 0xFFF00000 && Length < 512) //watch out for hard coded 512
		Length++;
	return Length;
}

bool VMTHook::Setup(void * Base)
{
	if (Base != nullptr)
		ClassBase = Base;
	if (Base == nullptr)
		return false;

	OldTable = *(uintptr_t**)ClassBase; 
	TableLen = GetTableLen(OldTable);

	if (TableLen == 0)
		return false;

	NewTable = new uintptr_t[TableLen]();
	
	memcpy(NewTable, OldTable, TableLen * sizeof(uintptr_t));

	VirtualProtect(ClassBase, sizeof(uintptr_t), PAGE_READWRITE, (PDWORD)&OldProtection);

	*(uintptr_t**)ClassBase = NewTable;

	return true;
}

CreateInterfaceFn Interfaces::GetModuleFactory(HMODULE module)
{
	return reinterpret_cast<CreateInterfaceFn>(GetProcAddress(module, "CreateInterface")); //function pointer to this modules CreateInterface 
}

template<typename T>
T* Interfaces::GetInterface(CreateInterfaceFn Function, const char * InterfaceVersion)
{
	auto Result = reinterpret_cast<T*>(Function(InterfaceVersion, nullptr));
	if (!Result){
		return nullptr;
	}
	return Result;
}

void Interfaces::Initialize()
{
	auto EngineFactory = GetModuleFactory(GetModuleHandle("engine.dll"));
	auto ClientFactory = GetModuleFactory(GetModuleHandle("client.dll"));
	auto vGuiFactory = GetModuleFactory(GetModuleHandle("vguimatsurface.dll"));

	Surface = GetInterface<void>(vGuiFactory, "VGUI_Surface031");
	EngineClient = GetInterface<void>(EngineFactory, "VEngineClient014");
	EntList = GetInterface<void>(ClientFactory, "VClientEntityList003");
	CHLClient = GetInterface<void>(ClientFactory, "VClient018");
	EngineTrace = GetInterface<void>(EngineFactory, "EngineTraceClient004");
}

bool WorldToScreen(D3DXVECTOR3 from, D3DXVECTOR3& to, const VMatrix * M,int  width, int height)
{
	
	float w = 0.0f;
	to.x = M->m[0][0] * from.x + M->m[0][1] * from.y + M->m[0][2] * from.z + M->m[0][3];
	to.y = M->m[1][0] * from.x + M->m[1][1] * from.y + M->m[1][2] * from.z + M->m[1][3];
	w = M->m[3][0] * from.x + M->m[3][1] * from.y + M->m[3][2] * from.z + M->m[3][3];
	if (w < 0.01f)
		return false;
	float invw = 1.0f / w;
	to.x *= invw;
	to.y *= invw;

	

	float x = width / 2;
	float y = height / 2;

	x += 0.5 * to.x * width + 0.5;
	y -= 0.5 * to.y * height + 0.5;

	to.x = x; //+ height;
		to.y = y; //+ width;
	to.z = 0;

	return true;
}

/*
bool WorldToScreen(const D3DXVECTOR3 &vecOrigin, D3DXVECTOR3 &vecScreen, float *pflViewMatrix, int width, int height)
{
	vecScreen[0] = pflViewMatrix[0] * vecOrigin[0] + pflViewMatrix[1] * vecOrigin[1] + pflViewMatrix[2] * vecOrigin[2] + pflViewMatrix[3];
	vecScreen[1] = pflViewMatrix[4] * vecOrigin[0] + pflViewMatrix[5] * vecOrigin[1] + pflViewMatrix[6] * vecOrigin[2] + pflViewMatrix[7];

	auto flTemp = pflViewMatrix[12] * vecOrigin[0] + pflViewMatrix[13] * vecOrigin[1] + pflViewMatrix[14] * vecOrigin[2] + pflViewMatrix[15];

	if (flTemp < 0.01f)
		return false;

	auto invFlTemp = 1.f / flTemp;
	vecScreen[0] *= invFlTemp;
	vecScreen[1] *= invFlTemp;

	static int iResolution[2] = { 0 };
	iResolution[0] = width;
	iResolution[1] = height;
	auto x = (float)iResolution[0] / 2.f;
	auto y = (float)iResolution[1] / 2.f;

	x += 0.5f * vecScreen[0] * (float)iResolution[0] + 0.5f;
	y -= 0.5f * vecScreen[1] * (float)iResolution[1] + 0.5f;

	vecScreen[0] = x;
	vecScreen[1] = y;

	return true;
}
*/
void CalculateAngle(D3DXVECTOR3& to, D3DXVECTOR3 Me, D3DXVECTOR3 Them)
{
	double delta[3] = { (Me.x - Them.x), (Me.y - Them.y), (Me.z - Them.z) };
	double hyp = sqrt(delta[0] * delta[0] + delta[1] * delta[1]);
	to.x = (float)(asinf(delta[2] / hyp) * 57.295779513082f);
	to.y = (float)(atanf(delta[1] / delta[0]) * 57.295779513082f);
	to.z = 0.0f;
	if (delta[0] >= 0.0)
		to.y += 180.0f;
}

float GetYawDiff(D3DXVECTOR3 Me, D3DXVECTOR3 Them)
{
	if (Me.y > 0 && Them.y > 0) {
		return (abs(Me.y - Them.y));
	}
	if (Me.y < 0)
		Me.y += 360;
	if (Them.y < 0)
		Them.y += 360;
	return (abs(Me.y - Them.y));
}

float GetPitchDiff(D3DXVECTOR3 Me, D3DXVECTOR3 Them)
{
	if (Me.x > 0) {
		Me.x = 90 - Me.x;
	}
	if (Them.x > 0) {
		Them.x = 90 - Them.x;
	}
	if (Me.x < 0)
		Me.x = abs(Me.x - 90);
	if (Them.x < 0)
		Them.x = abs(Them.x - 90);
	return (abs(Me.x - Them.x));
}
/*
int& CBaseCombatWeapon::GetWeaponID()
{
	return *(int*)((DWORD)this + Offsets.m_iWeaponID);
}

float& CBaseCombatWeapon::GetNextPrimaryAttack()
{
	return *(float*)((DWORD)this + Offsets.m_flNextPrimaryAttack);
}

float& CBaseCombatWeapon::GetAccuracyPenalty()
{
	return *(float*)((DWORD)this + Offsets.m_fAccuracyPenalty);
}

int& CBaseCombatWeapon::GetXUIDLow()
{
	return *(int*)((DWORD)this + Offsets.m_OriginalOwnerXuidLow);
}

int& CBaseCombatWeapon::GetXUIDHigh()
{
	return *(int*)((DWORD)this + Offsets.m_OriginalOwnerXuidHigh);
}

int& CBaseCombatWeapon::GetEntityQuality()
{
	return *(int*)((DWORD)this + Offsets.m_iEntityQuality);
}

int& CBaseCombatWeapon::GetAccountID()
{
	return *(int*)((DWORD)this + Offsets.m_iAccountID);
}

int& CBaseCombatWeapon::GetItemIDHigh()
{
	return *(int*)((DWORD)this + Offsets.m_iItemIDHigh);
}

int& CBaseCombatWeapon::GetItemDefinitionIndex()
{
	return *(int*)((DWORD)this + Offsets.m_iItemDefinitionIndex);
}

int& CBaseCombatWeapon::GetFallbackPaintKit()
{
	return *(int*)((DWORD)this + Offsets.m_nFallbackPaintKit);
}

int& CBaseCombatWeapon::GetFallbackStatTrak()
{
	return *(int*)((DWORD)this + Offsets.m_nFallbackStatTrak);
}

float& CBaseCombatWeapon::GetFallbackWear()
{
	return *(float*)((DWORD)this + Offsets.m_flFallbackWear);
}
*/
WeaponInfo_t* CBaseCombatWeapon::GetCSWpnData()
{
	if (!this)
		return nullptr;

	typedef WeaponInfo_t*(__thiscall* OriginalFn)(void*);
	return GetVFunc< OriginalFn >(this, 456)(this);
}
/*
bool CBaseCombatWeapon::IsEmpty()
{
	int clip = *(int*)((DWORD)this + Offsets.m_iClip1);
	return clip == 0;
}

bool CBaseCombatWeapon::IsReloading()
{
	return *(bool*)((DWORD)this + 0x3235);
}

void CBaseCombatWeapon::UpdateAccuracyPenalty()
{
	typedef void(__thiscall *FunctionFn)(void*);
	reinterpret_cast< FunctionFn >(*reinterpret_cast< PDWORD >(*reinterpret_cast< PDWORD >(this) + 0x748))((void*)this);
}

float CBaseCombatWeapon::GetWeaponCone()
{
	if (!this)
		return 0.f;

	typedef float(__thiscall* OriginalFn)(void*);
	return U::GetVFunc< OriginalFn >(this, 478)(this);
}

float CBaseCombatWeapon::GetWeaponSpread()
{
	if (!this)
		return 0.f;

	typedef float(__thiscall* OriginalFn)(void*);
	return U::GetVFunc< OriginalFn >(this, 479)(this);
}

bool CBaseCombatWeapon::IsGun()
{
	if (!this)
		return false;

	int id = this->GetWeaponID();
	//If your aimbot is broken, this is the reason. Just an FYI.

	switch (id)
	{
	case WEAPON_DEAGLE:
	case WEAPON_ELITE:
	case WEAPON_FIVESEVEN:
	case WEAPON_GLOCK:
	case WEAPON_AK47:
	case WEAPON_AUG:
	case WEAPON_AWP:
	case WEAPON_FAMAS:
	case WEAPON_G3SG1:
	case WEAPON_GALILAR:
	case WEAPON_M249:
	case WEAPON_M4A1:
	case WEAPON_MAC10:
	case WEAPON_P90:
	case WEAPON_UMP45:
	case WEAPON_XM1014:
	case WEAPON_BIZON:
	case WEAPON_MAG7:
	case WEAPON_NEGEV:
	case WEAPON_SAWEDOFF:
	case WEAPON_TEC9:
		return true;
	case WEAPON_TASER:
		return false;
	case WEAPON_HKP2000:
	case WEAPON_MP7:
	case WEAPON_MP9:
	case WEAPON_NOVA:
	case WEAPON_P250:
	case WEAPON_SCAR20:
	case WEAPON_SG556:
	case WEAPON_SSG08:
		return true;
	case WEAPON_KNIFE:
	case WEAPON_FLASHBANG:
	case WEAPON_HEGRENADE:
	case WEAPON_SMOKEGRENADE:
	case WEAPON_MOLOTOV:
	case WEAPON_DECOY:
	case WEAPON_INCGRENADE:
	case WEAPON_C4:
	case WEAPON_KNIFE_T:
		return false;
	case WEAPON_M4A1_SILENCER:
	case WEAPON_USP_SILENCER:
	case WEAPON_CZ75A:
	case WEAPON_REVOLVER:
		return true;
	default:
		return false;
	}
}

std::string CBaseCombatWeapon::GetWeaponName()
{
	if (!this)
		return "";

	int id = this->GetWeaponID();

	switch (id)
	{
	case WEAPON_DEAGLE:
		return strenc("Desert Eagle");
	case WEAPON_ELITE:
		return strenc("Dual Berettas");
	case WEAPON_FIVESEVEN:
		return strenc("Five-SeveN");
	case WEAPON_GLOCK:
		return strenc("Glock-18");
	case WEAPON_AK47:
		return strenc("AK-47");
	case WEAPON_AUG:
		return strenc("AUG");
	case WEAPON_AWP:
		return strenc("AWP");
	case WEAPON_FAMAS:
		return strenc("FAMAS");
	case WEAPON_G3SG1:
		return strenc("G3SG1");
	case WEAPON_GALILAR:
		return strenc("Galil");
	case WEAPON_M249:
		return strenc("M249");
	case WEAPON_M4A1:
		return strenc("M4A1");
	case WEAPON_MAC10:
		return strenc("MAC-10");
	case WEAPON_P90:
		return strenc("P90");
	case WEAPON_UMP45:
		return strenc("UMP-45");
	case WEAPON_XM1014:
		return strenc("XM1014");
	case WEAPON_BIZON:
		return strenc("PP-Bizon");
	case WEAPON_MAG7:
		return strenc("MAG-7");
	case WEAPON_NEGEV:
		return strenc("Negev");
	case WEAPON_SAWEDOFF:
		return strenc("Sawed-Off");
	case WEAPON_TEC9:
		return strenc("Tec-9");
	case WEAPON_TASER:
		return strenc("Taser");
	case WEAPON_HKP2000:
		return strenc("P2000");
	case WEAPON_MP7:
		return strenc("MP7");
	case WEAPON_MP9:
		return strenc("MP9");
	case WEAPON_NOVA:
		return strenc("Nova");
	case WEAPON_P250:
		return strenc("P250");
	case WEAPON_SCAR20:
		return strenc("SCAR-20");
	case WEAPON_SG556:
		return strenc("SG 553");
	case WEAPON_SSG08:
		return strenc("SSG 08");
	case WEAPON_KNIFE:
		return strenc("Knife");
	case WEAPON_FLASHBANG:
		return strenc("Flashbang");
	case WEAPON_HEGRENADE:
		return strenc("HE Grenade");
	case WEAPON_SMOKEGRENADE:
		return strenc("Smoke Grenade");
	case WEAPON_MOLOTOV:
		return strenc("Molotov");
	case WEAPON_DECOY:
		return strenc("Decoy");
	case WEAPON_INCGRENADE:
		return strenc("Incendiary Grenade");
	case WEAPON_C4:
		return strenc("C4");
	case WEAPON_KNIFE_T:
		return strenc("Knife");
	case WEAPON_M4A1_SILENCER:
		return strenc("M4A1-S");
	case WEAPON_USP_SILENCER:
		return strenc("USP-S");
	case WEAPON_CZ75A:
		return strenc("CZ75-Auto");
	case WEAPON_REVOLVER:
		return strenc("R8 Revolver");
	default:
		return strenc("Knife");
	}

	return "";
}
*/