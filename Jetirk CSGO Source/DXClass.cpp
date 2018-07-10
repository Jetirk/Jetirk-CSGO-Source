#include "stdafx.h"
#include "DXClass.h"
#include "Random.h"


bool DxManager::ProcessMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	auto key = VK_LBUTTON;
	auto state = KeyState::None;
	switch (uMsg) {
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
		state = uMsg == WM_MBUTTONUP ? KeyState::Up : KeyState::Down;
		key = VK_MBUTTON;
		break;
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
		state = uMsg == WM_RBUTTONUP ? KeyState::Up : KeyState::Down;
		key = VK_RBUTTON;
		break;
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
		state = uMsg == WM_LBUTTONUP ? KeyState::Up : KeyState::Down;
		key = VK_LBUTTON;
		break;
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
		state = uMsg == WM_XBUTTONUP ? KeyState::Up : KeyState::Down;
		key = (HIWORD(wParam) == XBUTTON1 ? VK_XBUTTON1 : VK_XBUTTON2);
		break;
	default:
		return false;
	}

	if (state == KeyState::Up && m_iKeyMap[key] == KeyState::Down)
		m_iKeyMap[key] = KeyState::Pressed;
	else
		m_iKeyMap[key] = state;
	return true;
}

bool  DxManager::ProcessKeybdMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	auto key = wParam;
	auto state = KeyState::None;

	switch (uMsg) {
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		state = KeyState::Down;
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		state = KeyState::Up;
		break;
	default:
		return false;
	}

	if (state == KeyState::Up && m_iKeyMap[int(key)] == KeyState::Down) {
		m_iKeyMap[int(key)] = KeyState::Pressed;

		auto& hotkey_callback = m_Hotkeys[key];

		if (hotkey_callback)
			hotkey_callback();

	}
	else {
		m_iKeyMap[int(key)] = state;
	}

	return true;
}

bool  DxManager::ProcessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_LBUTTONDBLCLK:
	case WM_XBUTTONDBLCLK:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONDOWN:
	case WM_XBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
	case WM_LBUTTONUP:
	case WM_XBUTTONUP:
		return ProcessMouseMessage(uMsg, wParam, lParam);
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
		return ProcessKeybdMessage(uMsg, wParam, lParam);
	default:
		return false;
	}
}

LRESULT  DxManager::ImGui_ImplDX9_WndProcHandler(HWND, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ImGuiIO& io = ImGui::GetIO();
	switch (msg) {
	case WM_LBUTTONDOWN:
		io.MouseDown[0] = true;
		return true;
	case WM_LBUTTONUP:
		io.MouseDown[0] = false;
		return true;
	case WM_RBUTTONDOWN:
		io.MouseDown[1] = true;
		return true;
	case WM_RBUTTONUP:
		io.MouseDown[1] = false;
		return true;
	case WM_MBUTTONDOWN:
		io.MouseDown[2] = true;
		return true;
	case WM_MBUTTONUP:
		io.MouseDown[2] = false;
		return true;
	case WM_XBUTTONDOWN:
		if ((GET_KEYSTATE_WPARAM(wParam) & MK_XBUTTON1) == MK_XBUTTON1)
			io.MouseDown[3] = true;
		else if ((GET_KEYSTATE_WPARAM(wParam) & MK_XBUTTON2) == MK_XBUTTON2)
			io.MouseDown[4] = true;
		return true;
	case WM_XBUTTONUP:
		if ((GET_KEYSTATE_WPARAM(wParam) & MK_XBUTTON1) == MK_XBUTTON1)
			io.MouseDown[3] = false;
		else if ((GET_KEYSTATE_WPARAM(wParam) & MK_XBUTTON2) == MK_XBUTTON2)
			io.MouseDown[4] = false;
		return true;
	case WM_MOUSEWHEEL:
		io.MouseWheel += GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? +1.0f : -1.0f;
		return true;
	case WM_MOUSEMOVE:
		io.MousePos.x = (signed short)(lParam);
		io.MousePos.y = (signed short)(lParam >> 16);
		return true;
	case WM_KEYDOWN:
		if (wParam < 256)
			io.KeysDown[wParam] = 1;
		return true;
	case WM_KEYUP:
		if (wParam < 256)
			io.KeysDown[wParam] = 0;
		return true;
	case WM_CHAR:
		// You can also use ToAscii()+GetKeyboardState() to retrieve characters.
		if (wParam > 0 && wParam < 0x10000)
			io.AddInputCharacter((unsigned short)wParam);
		return true;
	}
	return 0;
}



bool DxManager::Intitialize()
{
	DXDevice = nullptr;
	auto dx9api = GetModuleHandleW(L"shaderapidx9.dll");
	DXDevice = **(IDirect3DDevice9***)(PatternScan(dx9api, "A1 ? ? ? ? 50 8B 08 FF 51 0C") + 1);
	
//	HRESULT Result = D3DXCreateFont(DXDevice, 18, 0, FW_NORMAL, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Tahoma", &pFont);
//	if (FAILED(Result)) {
//		return false;
//	}
//	 D3DXCreateLine(DXDevice, &pLine);
//	if (FAILED(Result)) {
//		return false;
//	}
	
	if (FAILED(DXDevice->GetCreationParameters(&CreationParameters)))
		return false;
	hTargetWnd = CreationParameters.hFocusWindow;
	OldWndProc = SetWindowLongPtr(hTargetWnd, GWLP_WNDPROC, (long)WndProc);
	sidebar_tabs[0] = "ESP";
	sidebar_tabs[1] = "AIM";
	sidebar_tabs[2] = "MISC";
	sidebar_tabs[3] = "CONFIG";
	Unload = false;
	EspOn = false;
	BoxOn = false;
	TriggerOn = false;
	BunnyHopOn = false;
	fakenews = false;
	HealthBarOn = false;
	LineOn = false;
	SkeletonOn = false;
	LineToBeReleased = false;
	Created = false;
	GlowOn = false;
	FovOn = false;
	FovAmount = 0;
	EnemyBoxColor.SetColor(255,0, 0, 255);
	EnemyLineColor.SetColor(255, 0, 0, 255);
	EnemyGlowColor.SetColor(255, 0, 0, 255);
	FriendlyGlowColor.SetColor(0, 0, 255, 255);
	FriendlyBoxColor.SetColor(0, 0, 255, 255);
	FriendlyLineColor.SetColor(0, 0, 255, 255);

	return true;
}

void DxManager::ShutDown()
{
	return;
}

void DxManager::ImGuiCreateStyle()
{
	_style.Alpha = 1.0f;                                // Global alpha applies to everything in ImGui
	_style.WindowPadding = ImVec2(10, 10);                      // Padding within a window
	_style.WindowMinSize = ImVec2(100, 100);                    // Minimum window size
	_style.WindowRounding = 0.0f;                                // Radius of window corners rounding. Set to 0.0f to have rectangular windows
	_style.WindowTitleAlign = ImVec2(0.0f, 0.5f);                  // Alignment for title bar text
	_style.ChildWindowRounding = 0.0f;                                // Radius of child window corners rounding. Set to 0.0f to have rectangular child windows
	_style.FramePadding = ImVec2(5, 5);                        // Padding within a framed rectangle (used by most widgets)
	_style.FrameRounding = 0.0f;                                // Radius of frame corners rounding. Set to 0.0f to have rectangular frames (used by most widgets).
	_style.ItemSpacing = ImVec2(5, 5);                        // Horizontal and vertical spacing between widgets/lines
	_style.ItemInnerSpacing = ImVec2(4, 4);                        // Horizontal and vertical spacing between within elements of a composed widget (e.g. a slider and its label)
	_style.TouchExtraPadding = ImVec2(0, 0);                        // Expand reactive bounding box for touch-based system where touch position is not accurate enough. Unfortunately we don't sort widgets so priority on overlap will always be given to the first widget. So don't grow this too much!
	_style.IndentSpacing = 21.0f;                               // Horizontal spacing when e.g. entering a tree node. Generally == (FontSize + FramePadding.x*2).
	_style.ColumnsMinSpacing = 6.0f;                                // Minimum horizontal spacing between two columns
	_style.ScrollbarSize = 16.0f;                               // Width of the vertical scrollbar, Height of the horizontal scrollbar
	_style.ScrollbarRounding = 9.0f;                                // Radius of grab corners rounding for scrollbar
	_style.GrabMinSize = 10.0f;                               // Minimum width/height of a grab box for slider/scrollbar
	_style.GrabRounding = 0.0f;                                // Radius of grabs corners rounding. Set to 0.0f to have rectangular slider grabs.
	_style.ButtonTextAlign = ImVec2(0.5f, 0.5f);                  // Alignment of button text when button is larger than text.
	_style.DisplayWindowPadding = ImVec2(22, 22);                      // Window positions are clamped to be IsVisible within the display area by at least this amount. Only covers regular windows.
	_style.DisplaySafeAreaPadding = ImVec2(4, 4);                        // If you cannot see the edge of your screen (e.g. on a TV) increase the safe area padding. Covers popups/tooltips as well regular windows.
	_style.AntiAliasedLines = true;                                // Enable anti-aliasing on lines/borders. Disable if you are really short on CPU/GPU.
	_style.AntiAliasedShapes = true;                                // Enable anti-aliasing on filled shapes (rounded rectangles, circles, etc.)
	_style.CurveTessellationTol = 1.25f;             // Tessellation tolerance. Decrease for highly tessellated curves (higher quality, more polygons), increase to reduce quality.
	

//	_style.Colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
	_style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.00f, 1.00f, 1.00f);
	_style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	_style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.40f, 0.80f, 0.35f);
	_style.Colors[ImGuiCol_WindowBg] = ImVec4(.15,.15, .15, 1.00f);
	_style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.40f, 0.40f, 0.40f, 0.00f);
	_style.Colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.05f, 0.10f, 0.90f);
	_style.Colors[ImGuiCol_Border] = ImVec4(0.70f, 0.70f, 0.70f, 0.65f);
	_style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	_style.Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	_style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.90f, 0.80f, 0.80f, 0.40f);
	_style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.90f, 0.65f, 0.65f, 0.45f);
	_style.Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	_style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	_style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	_style.Colors[ImGuiCol_TitleText] = ImVec4(0.80f, 0.80f, 1.00f, 1.00f);
	_style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.40f, 0.40f, 0.55f, 0.80f);
	_style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.25f, 0.30f, 0.60f);
	_style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.40f, 0.40f, 0.80f, 0.30f);
	_style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.80f, 0.40f);
	_style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.80f, 0.50f, 0.50f, 0.40f);
	_style.Colors[ImGuiCol_ComboBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.99f);
	_style.Colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 0.90f, 0.60f, 0.50f);
	_style.Colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
	_style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.80f, 0.50f, 0.50f, 1.00f);
	_style.Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	_style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.40f, 0.00f, 0.00f, 1.00f);
	_style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.00f, 0.20f, 0.70f, 0.83f);
	_style.Colors[ImGuiCol_Header] = ImVec4(0.40f, 0.40f, 0.90f, 0.45f);
	_style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.45f, 0.45f, 0.90f, 0.80f);
	_style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.53f, 0.53f, 0.87f, 0.80f);
	_style.Colors[ImGuiCol_Column] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	_style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.70f, 0.60f, 0.60f, 1.00f);
	_style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.90f, 0.70f, 0.70f, 1.00f);
	_style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
	_style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
	_style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);
	_style.Colors[ImGuiCol_CloseButton] = ImVec4(0.10f, 0.10f, 0.10f, 0.50f);
	_style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.40f, 0.00f, 0.00f, 1.00f);
	_style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.70f, 0.20f, 0.00f, 0.83f);
	_style.Colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	_style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	_style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	_style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	_style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}

void DxManager::MenuInit()
{
	MenuVisible = true;
	ImGui_ImplDX9_Init(hTargetWnd, DXDevice);
	ImGuiCreateStyle();
}

void DxManager::FillRGBA(int x, int y, int w, int h, int r, int g, int b, int a)
{
	D3DCOLOR col = D3DCOLOR_RGBA(r, g, b, a);
	D3DRECT rec = { x, y, x + w, y + h };
	//char * Message = ParseInt(EntList[Index].iHealth);
	//	DxDrawText(Message, x, y, col, Index);
	DXDevice->Clear(1, &rec, D3DCLEAR_TARGET, col, 0, 0);
}

ImVec2 DxManager::get_sidebar_size()
{
	constexpr float padding = 10.0f;
	constexpr auto size_w = padding * 2.0f + 150;
	constexpr auto size_h = padding * 2.0f + (sizeof(sidebar_tabs) / sizeof(char*)) * 50;

	return ImVec2{ size_w, ImMax(325.0f, size_h) };
}

template<size_t N>
void DxManager::render_tabs(char* (&names)[N], int& activetab, float w, float h, bool sameline)
{
	bool values[N] = { false };

	values[activetab] = true;

	for (auto i = 0; i < N; ++i) {
		if (ImGui::ToggleButton(names[i], &values[i], ImVec2{ w, h })) {
			activetab = i;
		}
		if (sameline && i < N - 1)
			ImGui::SameLine();
	}
}

void DxManager::RenderEspTab()
{
	static char* esp_tab_names[] = { "ESP", "AIM", "MISC" };
	static int   active_esp_tab = 0;

	bool placeholder_true = true;

	auto& style = ImGui::GetStyle();
	float group_w = ImGui::GetCurrentWindow()->Size.x - style.WindowPadding.x * 2;
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2::Zero);
	{
		render_tabs(esp_tab_names, active_esp_tab, group_w / _countof(esp_tab_names), 25.0f, true);
	}
	ImGui::PopStyleVar();
	ImGui::BeginGroupBox("##body_content");
	{
		if (active_esp_tab == 0) {
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ style.WindowPadding.x, style.ItemSpacing.y });
			ImGui::Columns(3, NULL, false);
			ImGui::SetColumnOffset(1, group_w / 3.0f);
			ImGui::SetColumnOffset(2, 2 * group_w / 3.0f);
			ImGui::SetColumnOffset(3, group_w);

			
			ImGui::Checkbox("Box", &EspOn);
	//		ImGui::Checkbox("Team check", &fakenews);
	//		ImGui::Checkbox("Boxes", &fakenews);
	//		ImGui::Checkbox("Names", &fakenews);
			ImGui::Checkbox("Healthbar", &HealthBarOn);
			ImGui::Checkbox("Skeleton", &SkeletonOn);
			ImGui::Checkbox("Snaplines", &LineOn);
			
			ImGui::NextColumn();
			
			ImGui::Checkbox("Glow", &GlowOn);
	//		ImGui::Checkbox("Dropped Weapons", &fakenews);
	//		ImGui::Checkbox("Defuse Kit", &fakenews);
	//		ImGui::Checkbox("Planted C4", &fakenews);
			
			ImGui::NextColumn();

			ImGui::PushItemWidth(100);
			ImGui::ColorEditMode(ImGuiColorEditMode_HEX);
			
			ImGuiEx::ColorEdit3("Box Team", &FriendlyBoxColor);
			
			ImGuiEx::ColorEdit3("Box Enemies", &EnemyBoxColor);
			ImGuiEx::ColorEdit3("Line Allies", &FriendlyLineColor);
			ImGuiEx::ColorEdit3("Line Enemies", &EnemyLineColor);
			ImGuiEx::ColorEdit3("Glow Team", &FriendlyGlowColor);
			ImGuiEx::ColorEdit3("Glow Enemy", &EnemyGlowColor);
	//		ImGuiEx::ColorEdit3("Defuse Kit", &g_Options.);
	//		ImGuiEx::ColorEdit3("Planted C4", &g_Options.color_esp_c4);
			ImGui::PopItemWidth();
			
			ImGui::Columns(1, NULL, false);
			ImGui::PopStyleVar();
		}
		
		else if (active_esp_tab == 1) {
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ style.WindowPadding.x, style.ItemSpacing.y });
			ImGui::Columns(3, NULL, false);
			ImGui::SetColumnOffset(1, group_w / 3.0f);
			ImGui::SetColumnOffset(2, 2 * group_w / 3.0f);
			ImGui::SetColumnOffset(3, group_w);

		
			ImGui::Checkbox("FOV", &FovOn);
			ImGui::InputInt("Fov Amount", &FovAmount);
	//		ImGui::Checkbox("Players", &g_Options.glow_players);
	//		ImGui::Checkbox("Chickens", &g_Options.glow_chickens);
	//		ImGui::Checkbox("C4 Carrier", &g_Options.glow_c4_carrier);
	//		ImGui::Checkbox("Planted C4", &g_Options.glow_planted_c4);
	//		ImGui::Checkbox("Defuse Kits", &g_Options.glow_defuse_kits);
	//		ImGui::Checkbox("Weapons", &g_Options.glow_weapons);
			
			ImGui::NextColumn();

			ImGui::PushItemWidth(100);
			ImGui::ColorEditMode(ImGuiColorEditMode_HEX);
	//		ImGuiEx::ColorEdit3("Ally", &FriendlyGlowColor);
	//		ImGuiEx::ColorEdit3("Enemy", &EnemyGlowColor);
	//		ImGuiEx::ColorEdit3("Chickens", &g_Options.color_glow_chickens);
	//		ImGuiEx::ColorEdit3("C4 Carrier", &g_Options.color_glow_c4_carrier);
	//		ImGuiEx::ColorEdit3("Planted C4", &g_Options.color_glow_planted_c4);
	//		ImGuiEx::ColorEdit3("Defuse Kits", &g_Options.color_glow_defuse);
	//		ImGuiEx::ColorEdit3("Weapons", &g_Options.color_glow_weapons);
			ImGui::PopItemWidth();

			ImGui::NextColumn();
			
			ImGui::Columns(1, NULL, false);
			ImGui::PopStyleVar();
		}
		
		else if (active_esp_tab == 2) {
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ style.WindowPadding.x, style.ItemSpacing.y });
			ImGui::Columns(3, NULL, false);
			ImGui::SetColumnOffset(1, group_w / 3.0f);
			ImGui::SetColumnOffset(2, 2 * group_w / 3.0f);
			ImGui::SetColumnOffset(3, group_w);

			
				ImGui::Checkbox("TriggerBot", &TriggerOn); ImGui::SameLine();
				ImGui::Checkbox("BunnyHop", &BunnyHopOn);
				/*
				ImGui::Checkbox("Wireframe", &g_Options.chams_player_wireframe);
				ImGui::Checkbox("Flat", &g_Options.chams_player_flat);
				ImGui::Checkbox("Ignore-Z", &g_Options.chams_player_ignorez);
				ImGui::ColorEditMode(ImGuiColorEditMode_HEX);
				ImGui::PushItemWidth(110);
				ImGuiEx::ColorEdit4("Ally (Visible)", &g_Options.color_chams_player_ally_visible);
				ImGuiEx::ColorEdit4("Ally (Occluded)", &g_Options.color_chams_player_ally_occluded);
				ImGuiEx::ColorEdit4("Enemy (Visible)", &g_Options.color_chams_player_enemy_visible);
				ImGuiEx::ColorEdit4("Enemy (Occluded)", &g_Options.color_chams_player_enemy_occluded);
				*/
				ImGui::PopItemWidth();
		

			ImGui::NextColumn();

			
				/*
				ImGui::Checkbox("Enabled", &g_Options.chams_arms_enabled);
				ImGui::Checkbox("Wireframe", &g_Options.chams_arms_wireframe);
				ImGui::Checkbox("Flat", &g_Options.chams_arms_flat);
				ImGui::Checkbox("Ignore-Z", &g_Options.chams_arms_ignorez);
				ImGui::ColorEditMode(ImGuiColorEditMode_HEX);
				ImGui::PushItemWidth(110);
				ImGuiEx::ColorEdit4("Color (Visible)", &g_Options.color_chams_arms_visible);
				ImGuiEx::ColorEdit4("Color (Occluded)", &g_Options.color_chams_arms_occluded);
				*/
				ImGui::PopItemWidth();
				
			

			ImGui::Columns(1, NULL, false);
			ImGui::PopStyleVar();
		}
		
	}
	ImGui::EndGroupBox();
}


void DxManager::Render()
{
	ImGui_ImplDX9_NewFrame();

	ImGui::GetIO().MouseDrawCursor = MenuVisible;
	//ImGuiIO& io = ImGui::GetIO();
	//io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Ruda-Bold.ttf", 12);
	//io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Ruda-Bold.ttf", 10);
	//io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Ruda-Bold.ttf", 14);
	//io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Ruda-Bold.ttf", 18);
	if (!MenuVisible)
		return;



	const auto sidebar_size = get_sidebar_size();
	static int active_sidebar_tab = 0;

	ImGui::PushStyle(_style);

	ImGui::SetNextWindowPos(ImVec2{ 0, 0 }, ImGuiSetCond_Once);
	ImGui::SetNextWindowSize(ImVec2{ 1000, 0 }, ImGuiSetCond_Once);

	if (ImGui::Begin("ViralCheats Private",
		&MenuVisible,
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_ShowBorders |
		ImGuiWindowFlags_NoResize))
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2::Zero);
	//	{
	//		ImGui::BeginGroupBox("##sidebar", sidebar_size);
	//		{
	//		ImGui::GetCurrentWindow()->Flags &= ~ImGuiWindowFlags_ShowBorders;

	//		render_tabs(sidebar_tabs, active_sidebar_tab, 150, 50, false);
	//		}
	//		ImGui::EndGroupBox();
	//	}
		ImGui::PopStyleVar();
		ImGui::SameLine();

		// Make the body the same vertical size as the sidebar
		// except for the width, which we will set to auto
		auto size = ImVec2{ 0.0f, sidebar_size.y };

		ImGui::BeginGroupBox("##body", size);
	//	if (active_sidebar_tab == 0) {
			RenderEspTab();
	//	}
		
//		else if (active_sidebar_tab == 1) {
//			RenderEmptyTab();
//		}
//		else if (active_sidebar_tab == 2) {
//			RenderMiscTab();
//		}
//		else if (active_sidebar_tab == 3) {
//			RenderEmptyTab();
//		}
//		
		ImGui::EndGroupBox();

		ImGui::TextColored(ImVec4{ 0.0f, 0.5f, 0.0f, 1.0f }, "FPS: %03d", 666);//get_fps());
		ImGui::SameLine(ImGui::GetWindowWidth() - 150 - ImGui::GetStyle().WindowPadding.x);
		if (ImGui::Button("Unload", ImVec2{ 150, 25 })) {
			Unload = true;
		}
		ImGui::End();
	}

	ImGui::PopStyle();

	ImGui::Render();
}

Color Color::Black(0, 0, 0, 255);
Color Color::White(255, 255, 255, 255);
Color Color::Red(255, 0, 0, 255);
Color Color::Green(0, 128, 0, 255);
Color Color::Blue(0, 0, 255, 255);

Color::Color()
{
	*((int *)this) = 0;
}
Color::Color(int _r, int _g, int _b)
{
	SetColor(_r, _g, _b, 255);
}
Color::Color(int _r, int _g, int _b, int _a)
{
	SetColor(_r, _g, _b, _a);
}
void Color::SetRawColor(int color32)
{
	*((int *)this) = color32;
}
int Color::GetRawColor() const
{
	return *((int *)this);
}
void Color::SetColor(int _r, int _g, int _b, int _a)
{
	_CColor[0] = (unsigned char)_r;
	_CColor[1] = (unsigned char)_g;
	_CColor[2] = (unsigned char)_b;
	_CColor[3] = (unsigned char)_a;
}
void Color::SetColor(float _r, float _g, float _b, float _a)
{
	_CColor[0] = static_cast<unsigned char>(_r * 255.0f);
	_CColor[1] = static_cast<unsigned char>(_g * 255.0f);
	_CColor[2] = static_cast<unsigned char>(_b * 255.0f);
	_CColor[3] = static_cast<unsigned char>(_a * 255.0f);
}
void Color::GetColor(int &_r, int &_g, int &_b, int &_a) const
{
	_r = _CColor[0];
	_g = _CColor[1];
	_b = _CColor[2];
	_a = _CColor[3];
}
bool Color::operator== (const Color &rhs) const
{
	return (*((int *)this) == *((int *)&rhs));
}
bool Color::operator!= (const Color &rhs) const
{
	return !(operator==(rhs));
}
Color& Color::operator=(const Color &rhs)
{
	SetRawColor(rhs.GetRawColor());
	return *this;
}

void DxManager::BoxFillRGBA(int x, int y, int x2, int y2, int r, int g, int b, int a)
{
	D3DCOLOR col = D3DCOLOR_RGBA(r, g, b, a);
	FillRGBA(x - 1, y - 1, x2 + 2, 1, r, g, b, a);
	FillRGBA(x - 1, y, 1, y2 - 1, r, g, b, a);
	FillRGBA(x + x2, y, 1, y2 - 1, r, g, b, a);
	FillRGBA(x - 1, y + y2 - 1, x2 + 2, 1, r, g, b, a);
}
void DxManager::RenderEmptyTab()
{
	auto& style = ImGui::GetStyle();
	float group_w = ImGui::GetCurrentWindow()->Size.x - style.WindowPadding.x * 2;

	bool placeholder_true = true;

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2::Zero);
	ImGui::ToggleButton("AIM", &placeholder_true, ImVec2{ group_w, 25.0f });
	ImGui::PopStyleVar();

	ImGui::BeginGroupBox("##body_content");
	{
		auto message = "There's nothing here yet. Add something you want!";

		auto pos = ImGui::GetCurrentWindow()->Pos;
		auto wsize = ImGui::GetCurrentWindow()->Size;

		pos.x = pos.x + wsize.x / 2.0f;
		pos.y = pos.y + wsize.y / 2.0f;
		pos.x -= ImGui::CalcTextSize(message).x / 2.0f;
		pos.y -= ImGui::CalcTextSize(message).y / 2.0f;

		ImGui::RenderText(pos, message);
	}
	ImGui::EndGroupBox();
}
void DxManager::RenderMiscTab()
{
	bool placeholder_true = true;

	auto& style = ImGui::GetStyle();
	float group_w = ImGui::GetCurrentWindow()->Size.x - style.WindowPadding.x * 2;

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2::Zero);
	ImGui::ToggleButton("MISC", &placeholder_true, ImVec2{ group_w, 25.0f });
	ImGui::PopStyleVar();

	ImGui::BeginGroupBox("##body_content");
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ style.WindowPadding.x, style.ItemSpacing.y });
		ImGui::Columns(3, NULL, false);
		ImGui::SetColumnOffset(1, group_w / 3.0f);
		ImGui::SetColumnOffset(2, 2 * group_w / 3.0f);
		ImGui::SetColumnOffset(3, group_w);

		ImGui::Checkbox("Bunny hop", &BunnyHopOn);
		ImGui::Checkbox("TriggerBot", &TriggerOn);
		ImGui::PushItemWidth(-1.0f);
//		ImGui::SliderInt("viewmodel_fov:", &fakenews, 68, 120);
//		ImGui::SliderFloat("mat_ambient_light_r:", &g_Options, 0, 1);
//		ImGui::SliderFloat("mat_ambient_light_g:", &g_Options, 0, 1);
//		ImGui::SliderFloat("mat_ambient_light_b:", &g_Options, 0, 1);
		ImGui::PopItemWidth();

		ImGui::Columns(1, NULL, false);
		ImGui::PopStyleVar();
	}
	ImGui::EndGroupBox();
}

void DxManager::SetAllFalse()
{
	EspOn = false;
	TriggerOn = false;
	BunnyHopOn = false;
	HealthBarOn = false;
	LineOn = false;
	GlowOn = false;
}

void DxManager::HealthBar(float x, float y, float y2, float Health, float Dist)
{
	float Red = 255 - (Health*2.55);
	float Green = Health*2.55;

	if (Health>0)
	{
		if (Dist < 75.0) {
			FillRGBA(x , y , ((-10.0 / 100.0) * Dist) + 10, y2, 20, 255, 20, 20);//background +border
																								   //Box(x,y,100,10,D3DCOLOR_ARGB( 255 ,20, 20, 20));//background  no border
			FillRGBA(x, y, ((-10.0 / 100.0) * Dist) + 10, ((-Health + 100) / 100) * y2, 255, 0, 0, 0);//Healthbar
		}
		if (Dist > 75.0) {
			FillRGBA(x - 1, y - 1, 3, y2, 20, 255, 20, 20);//background +border
																	   //Box(x,y,100,10,D3DCOLOR_ARGB( 255 ,20, 20, 20));//background  no border
			FillRGBA(x, y, 3, ((-Health + 100) / 100) * y2, 255, 0, 0, 0);//Healthbar
		}
	}
}

float DxManager::GetDistance(D3DXVECTOR3 Me, D3DXVECTOR3 Them)
{
	float Distance = sqrtf((Them.x - Me.x)*(Them.x - Me.x) + (Them.y - Me.y)*(Them.y - Me.y) + (Them.z - Me.z)*(Them.z - Me.z));
	return Distance;
}



void DxManager::DrawTraceLine(int x, int y, D3DCOLOR Color, int Width, int Height)
{

	D3DXVECTOR2 Line[2];
	Line[0].x = (Width) / 2;
	Line[0].y = (Height) / 2;
	Line[1].x = x;
	Line[1].y = y;
	pLine->Begin();
	pLine->SetWidth(1);
	pLine->Draw(Line, 2, Color);
	pLine->End();
	
}

void DxManager::OnDeviceLost()
{
	pLine->OnLostDevice();
	pFont->OnLostDevice();
	ImGui_ImplDX9_InvalidateDeviceObjects();
}

void DxManager::OnDeviceReset()
{
	pLine->OnResetDevice();
	pFont->OnResetDevice();
	ImGui_ImplDX9_CreateDeviceObjects();
}

void DxManager::DrawDistance(int x, int y, int x2, int y2, const char * Distance)
{
	if (!Created) {
		D3DXCreateFont(DXDevice, 18, 0, FW_NORMAL, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Tahoma", &pFont);
		Created = true;
	}
	RECT Rect = { x - 1, y + y2 - 1, x + 120, y + 666 };
	pFont->DrawTextA(NULL, "Dist: ", -1, &Rect, DT_LEFT, D3DCOLOR_RGBA(0, 255, 0, 255));
	Rect = { x - 1 + 30, y + y2 - 1 , x + 120,y + 666 };
	pFont->DrawTextA(NULL, Distance, -1, &Rect, DT_LEFT, D3DCOLOR_RGBA(0, 255, 0, 255));
//	pFont->Release();
}

char* DxManager::ParseInt(int x)
{
	std::stringstream StringStream;
	StringStream << x;
	char* BufferObj = new char[StringStream.str().size() + 1];
	strcpy(BufferObj, StringStream.str().c_str());
	char * ReturnVal = BufferObj;
	delete BufferObj;
	return ReturnVal;
}