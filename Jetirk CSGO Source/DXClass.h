#pragma once
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
#include <functional>


LRESULT __stdcall  WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

enum class KeyState
{
	None = 1,
	Down,
	Up,
	Pressed /*Down and then up*/
};

class Color
{
public:
	Color();
	Color(int _r, int _g, int _b);
	Color(int _r, int _g, int _b, int _a);
	Color(float _r, float _g, float _b) : Color(_r, _g, _b, 1.0f) {}
	Color(float _r, float _g, float _b, float _a)
		: Color(
			static_cast<int>(_r * 255.0f),
			static_cast<int>(_g * 255.0f),
			static_cast<int>(_b * 255.0f),
			static_cast<int>(_a * 255.0f))
	{
	}
	explicit Color(float* rgb) : Color(rgb[0], rgb[1], rgb[2], 1.0f) {}
	explicit Color(unsigned long argb)
	{
		_CColor[2] = (unsigned char)((argb & 0x000000FF) >> (0 * 8));
		_CColor[1] = (unsigned char)((argb & 0x0000FF00) >> (1 * 8));
		_CColor[0] = (unsigned char)((argb & 0x00FF0000) >> (2 * 8));
		_CColor[3] = (unsigned char)((argb & 0xFF000000) >> (3 * 8));
	}

	void    SetRawColor(int color32);
	int     GetRawColor() const;
	void    SetColor(int _r, int _g, int _b, int _a = 0);
	void    SetColor(float _r, float _g, float _b, float _a = 0);
	void    GetColor(int &_r, int &_g, int &_b, int &_a) const;

	int r() const { return _CColor[0]; }
	int g() const { return _CColor[1]; }
	int b() const { return _CColor[2]; }
	int a() const { return _CColor[3]; }

	unsigned char &operator[](int index)
	{
		return _CColor[index];
	}
	const unsigned char &operator[](int index) const
	{
		return _CColor[index];
	}

	bool operator==(const Color &rhs) const;
	bool operator!=(const Color &rhs) const;
	Color &operator=(const Color &rhs);

	static Color Black;
	static Color White;
	static Color Red;
	static Color Green;
	static Color Blue;

private:
	unsigned char _CColor[4];
};

namespace ImGuiEx
{
	inline bool ColorEdit4(const char* label, Color* v, bool show_alpha = true)
	{
		auto clr = ImVec4{
			v->r() / 255.0f,
			v->g() / 255.0f,
			v->b() / 255.0f,
			v->a() / 255.0f
		};

		if (ImGui::ColorEdit4(label, &clr.x, show_alpha)) {
			v->SetColor(clr.x, clr.y, clr.z, clr.w);
			return true;
		}
		return false;
	}
	inline bool ColorEdit3(const char* label, Color* v)
	{
		return ColorEdit4(label, v, false);
	}
}


class DxManager
{
public:
	IDirect3D9* DXObject;
	IDirect3DDevice9* DXDevice;
	D3DPRESENT_PARAMETERS DXParameters;
	D3DDEVICE_CREATION_PARAMETERS CreationParameters;
	LPD3DXFONT pFont;
	ID3DXLine * pLine;
	HWND hTargetWnd;
	LONG_PTR OldWndProc;
	KeyState       m_iKeyMap[256];
	std::function<void(void)> m_Hotkeys[256];
	ImGuiStyle _style;
	Color FriendlyBoxColor, EnemyBoxColor, FriendlyLineColor, EnemyLineColor, FriendlyGlowColor, EnemyGlowColor;

	bool MenuVisible;
	char * sidebar_tabs[4];
	bool fakenews;
	bool Unload;
	bool EspOn;
	bool BoxOn;
	bool TriggerOn;
	bool BunnyHopOn;
	bool HealthBarOn;
	bool LineOn;
	bool Created;
	bool SkeletonOn;
	bool GlowOn;
	bool LineToBeReleased;
	bool FovOn;
	int FovAmount;
	//IDirect3DVertexBuffer9* g_pVB;
	bool Intitialize();
	void ShutDown();
	void ImGuiCreateStyle();
	void MenuInit();
	void FillRGBA(int x, int y, int w, int h, int r, int g, int b, int a);
	void Render();
	ImVec2 get_sidebar_size();
	template<size_t N>
	void render_tabs(char* (&names)[N], int& activetab, float w, float h, bool sameline);
	void RenderEspTab();
	void BoxFillRGBA(int x, int y, int x2, int y2, int r, int g, int b, int a);
	void RenderEmptyTab();
	void RenderMiscTab();
	LRESULT ImGui_ImplDX9_WndProcHandler(HWND, UINT msg, WPARAM wParam, LPARAM lParam);
	bool ProcessKeybdMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	bool ProcessMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	bool ProcessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void SetAllFalse();
	void HealthBar(float x, float y, float y2, float Health, float Dist);
	float GetDistance(D3DXVECTOR3 Me, D3DXVECTOR3 Them);
	void DrawTraceLine(int x, int y, D3DCOLOR Color,int Width, int Height);
	void OnDeviceLost();
	void OnDeviceReset();
	void DrawDistance(int x, int y, int x2, int y2, const char * Distance);
	char* ParseInt(int x);

	
};

class CGlowObjectManager
{
public:
	class GlowObjectDefinition_t
	{
	public:
		void set(Color color)
		{
			m_vGlowColor = D3DXVECTOR3(color.r(), color.g(), color.b());
			m_flGlowAlpha = color.a();
			m_bRenderWhenOccluded = true;
			m_bRenderWhenUnoccluded = false;
			m_flBloomAmount = 1.f;
		}

		UINT* getEntity()
		{
			return m_hEntity;
		}

		bool IsEmpty() const
		{
			return m_nNextFreeSlot != GlowObjectDefinition_t::ENTRY_IN_USE;
		}

	public:
		UINT* m_hEntity;
		D3DXVECTOR3 m_vGlowColor;
		float m_flGlowAlpha;

		char unknown[4];
		float flUnk;
		float m_flBloomAmount;
		float localplayeriszeropoint3;

		bool m_bRenderWhenOccluded;
		bool m_bRenderWhenUnoccluded;
		bool m_bFullBloomRender;
		char unknown1[1];

		int m_nFullBloomStencilTestValue;
		int iUnk;
		int m_nSplitScreenSlot;
		int m_nNextFreeSlot;

		// Special values for GlowObjectDefinition_t::m_nNextFreeSlot
		static const int END_OF_FREE_LIST = -1;
		static const int ENTRY_IN_USE = -2;
	};

	GlowObjectDefinition_t* m_GlowObjectDefinitions;
	int max_size;
	int pad;
	int size;
	GlowObjectDefinition_t* m_GlowObjectDefinitions2;
	int currentObjects;
};