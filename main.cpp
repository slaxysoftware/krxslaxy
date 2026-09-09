#define IMGUI_DEFINE_MATH_OPERATORS

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_freetype.h"
#include "custom_widgets/custom_widgets.hpp"
#include "custom_widgets/fonts.h"
#include "Globals.h"

#include <d3d11.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#include <tchar.h>

static ID3D11Device*            g_pd3dDevice = nullptr;
static ID3D11DeviceContext*     g_pd3dDeviceContext = nullptr;
static IDXGISwapChain*          g_pSwapChain = nullptr;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static ID3D11RenderTargetView*  g_mainRenderTargetView = nullptr;

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static const char* const kHitboxOptionsArr[]      = { "Head", "Neck", "Chest", "Random" };
static const char* const kTargetOptionsArr[]       = { "Distance", "Crosshair", "Combined" };
static const char* const kFireModeOptionsArr[]     = { "Manual", "Automatic" };
static const char* const kAAModeOptionsArr[]       = { "Normal", "Random", "Backwards", "3-Way", "Lag-jitter" };
static const char* const kAtomicModeOptionsArr[]   = { "Normal", "Inverter", "Flicker" };
static const char* const kGalaxyPresetOptionsArr[] = { "Galaxy", "Green Fresnel", "Purple Fresnel", "Blue Fresnel" };
static const char* const kCrystalPresetOptsArr[]   = { "Option 1", "Option 2", "Option 3", "Option 4", "Option 5" };
static const char* const kDynFresnelOptsArr[]      = { "Green", "Danger", "Purple Burn", "Yellow", "Red", "Burn" };
static const char* const kVisiblePresetsArr[]      = { "Galaxy", "Quantum Flux", "Neon Mirage", "Solar Flare", "Crystal Aura", "Cyber Matrix", "Arc Pulse", "Magma Rush", "Bloom", "Nova Blaze", "Cosmic Dream", "Stellar Burst" };
static const char* const kInvisiblePresetsArr[]    = { "Shadow Veil", "Inferno Rush", "Volt Surge", "Glacial Peak", "Crimson Moon", "Ghost Shade", "Wraith Flame", "Venom Chill", "Crimson Default", "Volcanic Rage", "Electric Surge", "Arctic Blast", "Blood Eclipse", "Dark Lightning" };
static const char* const kHandChamsOptsArr[]       = { "Glow", "Ghost", "Red Devil", "Holographic", "Hell" };
static const char* const kSkyboxPresetsArr[]       = { "Default", "Night Blue", "Galaxy", "Neon", "Cyberpunk", "Sunset", "Emerald", "Midnight" };

static void CaptureBackbufferToBmp(const char* filepath)
{
    ID3D11Texture2D* pBackBuffer = nullptr;
    if (FAILED(g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer)))
        return;

    D3D11_TEXTURE2D_DESC desc;
    pBackBuffer->GetDesc(&desc);

    D3D11_TEXTURE2D_DESC stagingDesc = desc;
    stagingDesc.Usage = D3D11_USAGE_STAGING;
    stagingDesc.BindFlags = 0;
    stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    stagingDesc.MiscFlags = 0;

    ID3D11Texture2D* pStaging = nullptr;
    if (SUCCEEDED(g_pd3dDevice->CreateTexture2D(&stagingDesc, nullptr, &pStaging)))
    {
        g_pd3dDeviceContext->CopyResource(pStaging, pBackBuffer);
        D3D11_MAPPED_SUBRESOURCE mapped;
        if (SUCCEEDED(g_pd3dDeviceContext->Map(pStaging, 0, D3D11_MAP_READ, 0, &mapped)))
        {
            FILE* f = nullptr;
            fopen_s(&f, filepath, "wb");
            if (f)
            {
                BITMAPFILEHEADER bfh = { 0 };
                BITMAPINFOHEADER bih = { 0 };
                bfh.bfType = 0x4D42;
                bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
                bfh.bfSize = bfh.bfOffBits + desc.Width * desc.Height * 4;

                bih.biSize = sizeof(BITMAPINFOHEADER);
                bih.biWidth = (LONG)desc.Width;
                bih.biHeight = -((LONG)desc.Height);
                bih.biPlanes = 1;
                bih.biBitCount = 32;
                bih.biCompression = BI_RGB;

                fwrite(&bfh, sizeof(bfh), 1, f);
                fwrite(&bih, sizeof(bih), 1, f);

                const unsigned char* src = (const unsigned char*)mapped.pData;
                for (UINT y = 0; y < desc.Height; y++)
                {
                    const unsigned char* row = src + y * mapped.RowPitch;
                    std::vector<unsigned char> bgra(desc.Width * 4);
                    for (UINT x = 0; x < desc.Width; x++)
                    {
                        bgra[x * 4 + 0] = row[x * 4 + 2];
                        bgra[x * 4 + 1] = row[x * 4 + 1];
                        bgra[x * 4 + 2] = row[x * 4 + 0];
                        bgra[x * 4 + 3] = row[x * 4 + 3];
                    }
                    fwrite(bgra.data(), desc.Width * 4, 1, f);
                }
                fclose(f);
            }
            g_pd3dDeviceContext->Unmap(pStaging, 0);
        }
        pStaging->Release();
    }
    pBackBuffer->Release();
}

int main(int argc, char** argv)
{
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, (HBRUSH)GetStockObject(WHITE_BRUSH), nullptr, L"KrxLeyawinMain", nullptr };
    ::RegisterClassExW(&wc);

    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"KRX SLAXY", WS_OVERLAPPEDWINDOW, 50, 50, 1140, 830, nullptr, nullptr, wc.hInstance, nullptr);

    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

        ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    BOOL darkMode = FALSE;
    DwmSetWindowAttribute(hwnd, 20, &darkMode, sizeof(darkMode));
    DWM_WINDOW_CORNER_PREFERENCE corner_pref = DWMWCP_ROUND;
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &corner_pref, sizeof(corner_pref));

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImFontConfig cfg;
    cfg.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_ForceAutoHint | ImGuiFreeTypeBuilderFlags_Bitmap;

    set->poppins_selection = io.Fonts->AddFontFromMemoryTTF(poppins_semibold, sizeof(poppins_semibold), 17.0f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    set->poppins_widget    = io.Fonts->AddFontFromMemoryTTF(poppins_semibold, sizeof(poppins_semibold), 15.0f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    set->icon              = io.Fonts->AddFontFromMemoryTTF(glypher_icon, sizeof(glypher_icon), 18.0f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    set->inter_logo        = io.Fonts->AddFontFromMemoryTTF(inter_bold, sizeof(inter_bold), 30.0f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    set->icon_child        = io.Fonts->AddFontFromMemoryTTF(glypher_icon, sizeof(glypher_icon), 14.0f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    set->icon_cfg          = io.Fonts->AddFontFromMemoryTTF(cfg_icons, sizeof(cfg_icons), 16.0f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    set->icon_micro        = io.Fonts->AddFontFromMemoryTTF(glypher_icon, sizeof(glypher_icon), 7.0f, &cfg, io.Fonts->GetGlyphRangesCyrillic());

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    ImVec4 clear_color = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);

    set->show_menu = true;
    set->menu_alpha = 1.0f;
    set->particle_cursor = false;

    bool done = false;
    while (!done)
    {
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done) break;

        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            g_ResizeWidth = g_ResizeHeight = 0;
            CreateRenderTarget();
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        {
            ImGuiStyle* style = &ImGui::GetStyle();

            notify->DrawNotifications();

            bool toggle_key_pressed = false;
            if (ImGui::IsKeyPressed(ImGuiKey_Insert, false))
                toggle_key_pressed = true;

            if (globals::misc::kky != 0 && globals::misc::kky != VK_INSERT)
            {
                static bool s_last_custom_down = false;
                bool s_cur_custom_down = (GetAsyncKeyState(globals::misc::kky) & 0x8000) != 0;
                if (s_cur_custom_down && !s_last_custom_down)
                    toggle_key_pressed = true;
                s_last_custom_down = s_cur_custom_down;
            }

            static bool s_last_ins_down = false;
            bool s_cur_ins_down = (GetAsyncKeyState(VK_INSERT) & 0x8000) != 0;
            if (s_cur_ins_down && !s_last_ins_down)
                toggle_key_pressed = true;
            s_last_ins_down = s_cur_ins_down;

            if (toggle_key_pressed)
            {
                set->show_menu = !set->show_menu;
            }

            static ImVec2 g_window_pos = ImVec2(-1.0f, -1.0f);
            if (g_window_pos.x < 0.0f)
            {
                g_window_pos = ImVec2((io.DisplaySize.x - set->window_size.x) * 0.5f, (io.DisplaySize.y - set->window_size.y) * 0.5f);
                if (g_window_pos.x < 10.0f) g_window_pos.x = 10.0f;
                if (g_window_pos.y < 10.0f) g_window_pos.y = 10.0f;
            }

            static float s_anim_progress = 1.0f;
            const float target_progress = set->show_menu ? 1.0f : 0.0f;
            const float anim_rate = 13.5f;
            s_anim_progress += (target_progress - s_anim_progress) * (1.0f - expf(-anim_rate * io.DeltaTime));
            if (fabsf(target_progress - s_anim_progress) < 0.001f)
                s_anim_progress = target_progress;

            const float ease_t = 1.0f - powf(1.0f - s_anim_progress, 3.0f);
            set->menu_alpha = s_anim_progress * s_anim_progress * (3.0f - 2.0f * s_anim_progress);

            if (s_anim_progress < 0.05f)
            {
                ImDrawList* fg = ImGui::GetForegroundDrawList();
                if (fg)
                {
                    float pulse = (sinf((float)ImGui::GetTime() * 2.5f) + 1.0f) * 0.5f;
                    ImU32 text_col   = IM_COL32(37, 99, 235, (int)(180 + 75 * pulse));
                    ImU32 border_col = IM_COL32(37, 99, 235, (int)(100 + 80 * pulse));
                    ImU32 bg_col     = IM_COL32(8, 12, 20, 230);
                    ImVec2 badge_size(230.0f, 32.0f);
                    ImVec2 hint_pos(24.0f, io.DisplaySize.y - badge_size.y - 20.0f);
                    fg->AddRectFilled(hint_pos, hint_pos + badge_size, bg_col, 8.0f);
                    fg->AddRect(hint_pos, hint_pos + badge_size, border_col, 8.0f, 0, 1.2f);
                    fg->AddText(hint_pos + ImVec2(18.0f, 8.0f), text_col, "KRX SLAXY  [INSERT] Open");
                }
            }

            if (s_anim_progress > 0.001f)
            {
                const float scale_factor = 0.91f + 0.09f * ease_t;
                ImVec2 cur_size = ImVec2(set->window_size.x * scale_factor, set->window_size.y * scale_factor);
                const float slide_y = (1.0f - ease_t) * 18.0f;

                static bool g_is_dragging = false;
                static ImVec2 g_drag_offset = ImVec2(0, 0);

                ImVec2 cur_pos = g_window_pos;
                ImRect drag_area_header(cur_pos, cur_pos + ImVec2(cur_size.x, 70.0f));
                ImRect drag_area_sidebar(cur_pos, cur_pos + ImVec2(236.0f, cur_size.y));

                if (io.MouseClicked[0])
                {
                    if ((drag_area_header.Contains(io.MousePos) || drag_area_sidebar.Contains(io.MousePos)) &&
                        !ImGui::IsAnyItemHovered() && !ImGui::IsAnyItemActive())
                    {
                        g_is_dragging = true;
                        g_drag_offset = io.MousePos - g_window_pos;
                    }
                }
                if (g_is_dragging)
                {
                    if (io.MouseDown[0])
                    {
                        g_window_pos = io.MousePos - g_drag_offset;
                        if (g_window_pos.x < -set->window_size.x + 120.0f) g_window_pos.x = -set->window_size.x + 120.0f;
                        if (g_window_pos.y < 0.0f) g_window_pos.y = 0.0f;
                        if (g_window_pos.x > io.DisplaySize.x - 120.0f) g_window_pos.x = io.DisplaySize.x - 120.0f;
                        if (g_window_pos.y > io.DisplaySize.y - 70.0f) g_window_pos.y = io.DisplaySize.y - 70.0f;
                    }
                    else
                    {
                        g_is_dragging = false;
                    }
                }

                ImVec2 render_pos = g_window_pos + ImVec2(
                    (set->window_size.x - cur_size.x) * 0.5f,
                    (set->window_size.y - cur_size.y) * 0.5f + slide_y
                );
                ImGui::SetNextWindowPos(render_pos);
                ImGui::SetNextWindowSize(cur_size);

                style->WindowPadding = ImVec2(0, 0);
                style->WindowBorderSize = 0.0f;
                style->WindowRounding = 26.0f;
                style->ChildRounding = 14.0f;
                style->FrameRounding = 8.0f;
                style->PopupRounding = 10.0f;
                style->ScrollbarRounding = 8.0f;
                style->ScrollbarSize = 0.0f;
                style->Colors[ImGuiCol_WindowBg] = ImVec4(0, 0, 0, 0);

                gui->push_style_var(ImGuiStyleVar_Alpha, set->menu_alpha);
                gui->begin("window", nullptr, set->window_flags | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
                {
                    const ImVec2 pos = ImGui::GetWindowPos();
                    const ImVec2 size = ImGui::GetWindowSize();
                    ImDrawList* draw = ImGui::GetWindowDrawList();

                    clr->accent_color = { func->accent_color[0], func->accent_color[1], func->accent_color[2], 1.f };

                    for (int s = 1; s <= 5; s++)
                    {
                        float expand = (float)s * 1.5f;
                        int shadow_alpha = (int)((45 - s * 8) * set->menu_alpha);
                        if (shadow_alpha > 0)
                            draw->AddRect(pos - ImVec2(expand, expand), pos + size + ImVec2(expand, expand), IM_COL32(0, 0, 0, shadow_alpha), 26.0f + expand, 0, 1.5f);
                    }

                    draw->AddRectFilled(pos, pos + size, gui->get_clr(clr->window_background), 26.0f);

                    draw->AddRect(pos, pos + size, gui->get_clr(clr->child_stroke_bar), 26.0f, 0, 1.2f);

                    draw->AddLine(pos + ImVec2(40, 1), pos + ImVec2(size.x - 40, 1), IM_COL32(255, 255, 255, (int)(35 * set->menu_alpha)), 1.0f);

                    draw->AddLine(pos + ImVec2(236, 18), pos + ImVec2(236, size.y - 18), gui->get_clr(clr->child_stroke_bar, 0.75f * set->menu_alpha), 1.0f);

                    {
                        ImVec2 center = pos + ImVec2(38, 42);

                        float anim_time = (float)ImGui::GetTime();
                        float pulse = (sinf(anim_time * 2.8f) + 1.0f) * 0.5f;

                        ImU32 navy_accent  = gui->get_clr(clr->accent_color);
                        ImU32 headshot_red = IM_COL32(255, 35, 65, (int)(255 * set->menu_alpha));
                        ImU32 reticle_dim  = gui->get_clr(clr->text, (0.65f + 0.15f * pulse) * set->menu_alpha);

                        draw->AddCircle(center, 15.5f + pulse * 1.5f, gui->get_clr(clr->accent_color, (0.15f + 0.18f * pulse) * set->menu_alpha), 32, 1.2f);

                        draw->AddCircle(center, 13.5f, navy_accent, 32, 1.8f);

                        draw->AddCircle(center, 6.5f, reticle_dim, 24, 1.0f);

                        draw->AddLine(center + ImVec2(0, -18), center + ImVec2(0, -9), navy_accent, 2.0f);
                        draw->AddLine(center + ImVec2(0, 9),   center + ImVec2(0, 18), navy_accent, 2.0f);
                        draw->AddLine(center + ImVec2(-18, 0), center + ImVec2(-9, 0), navy_accent, 2.0f);
                        draw->AddLine(center + ImVec2(9, 0),   center + ImVec2(18, 0), navy_accent, 2.0f);

                        draw->AddCircleFilled(center, 3.8f + pulse * 1.2f, IM_COL32(255, 35, 65, (int)((55 + 65 * pulse) * set->menu_alpha)));
                        draw->AddCircleFilled(center, 2.5f, headshot_red);
                        draw->AddCircleFilled(center, 1.0f, IM_COL32(255, 255, 255, (int)(255 * set->menu_alpha)));

                        ImVec2 text_start = pos + ImVec2(66, 28);
                        gui->render_text_clipped(set->inter_logo, text_start, text_start + ImVec2(62, 28), gui->get_clr(clr->text_active), "KRX", { 0.0f, 0.5f });
                        gui->render_text_clipped(set->inter_logo, text_start + ImVec2(62, 0), text_start + ImVec2(185, 28), navy_accent, "SLAXY", { 0.0f, 0.5f });
                    }

                    {
                        gui->set_cursor_pos(ImVec2(10, 85));
                        gui->begin_group();
                        {
                            gui->push_style_var(ImGuiStyleVar_ItemSpacing, { 10, 6 });
                            for (int i = 0; i < (int)set->selection_labels.size(); i++)
                            {
                                switch (i)
                                {
                                case 0:
                                    gui->text_colored(gui->get_clr(clr->text), "COMBAT");
                                    break;
                                case 2:
                                    gui->text_colored(gui->get_clr(clr->text), "VISUALS");
                                    break;
                                case 5:
                                    gui->text_colored(gui->get_clr(clr->text), "SETTINGS");
                                    break;
                                }

                                gui->selection(set->selection_icons[i], set->selection_labels[i], i, set->selection_count, ImVec2(220, 44));
                            }
                            gui->pop_style_var();
                        }
                        gui->end_group();
                    }

                    {
                        set->selection_alpha = ImClamp(set->selection_alpha + (6.f * ImGui::GetIO().DeltaTime * (set->selection_count == set->selection_accept ? 1.f : -1.f)), 0.f, 1.f);
                        if (set->selection_alpha == 0.f && set->selection_add == 0.f) set->selection_accept = set->selection_count;

                        gui->set_cursor_pos(ImVec2(246, 16));
                        gui->begin_content();
                        {
                            gui->push_style_var(ImGuiStyleVar_Alpha, set->selection_alpha * style->Alpha);

                            if (set->selection_accept == 0)
                            {

                                gui->begin_group();
                                {
                                    gui->begin_child("A", "Aimbot");
                                    {
                                        gui->checkbox("Enable Aim", &globals::aimbot::a1mbot);
                                        gui->checkbox("Show target fov", &globals::aimbot::draw_f0v);
                                        gui->checkbox("Recoil Control", &globals::aimbot::reco1l_contr0l);
                                        gui->checkbox("Perfect NoSpread", &globals::aimbot::spread_comp);
                                        gui->checkbox("Perfect Automatic Fire", &globals::aimbot::autoshot);
                                        gui->checkbox("Perfect 360 FOV", &globals::aimbot::enable_360_fov);
                                        gui->checkbox("No Spread Delay", &globals::aimbot::spreadempty_comp);
                                        gui->checkbox("Prediction", &globals::aimbot::pvc);
                                        gui->checkbox("Visibility Check", &globals::aimbot::v1sh_ch3ck);
                                        gui->keybind("Aim Key", &globals::aimbot::a1m_k3y);
                                    }
                                    gui->end_child();

                                    gui->begin_child("A", "Configuration");
                                    {
                                        gui->slider_float("Smooth", &globals::aimbot::a1m_sm00th, 1.0f, 100.0f, "%.1f");
                                        gui->slider_float("FOV", &globals::aimbot::a1m_f0v, 1.0f, 500.0f, "%.0f");
                                        gui->checkbox("Silent Aim", &globals::aimbot::silent);
                                        gui->checkbox("Closest Target", &globals::test);
                                        gui->checkbox("Hitbox Selector", &globals::test2);
                                        gui->dropdown("Hitbox", &globals::aimbot::hitbox, kHitboxOptionsArr, IM_ARRAYSIZE(kHitboxOptionsArr));
                                    }
                                    gui->end_child();
                                }
                                gui->end_group();

                                gui->sameline();

                                gui->begin_group();
                                {
                                    gui->begin_child("A", "Target Selection");
                                    {
                                        gui->dropdown("Target Priority", &globals::aimbot::target_selection, kTargetOptionsArr, IM_ARRAYSIZE(kTargetOptionsArr));
                                        gui->checkbox("Hitsound", &globals::test2);
                                        gui->checkbox("Thru Smoke", &globals::misc::rpl);
                                        gui->checkbox("No Smoke", &globals::misc::no_smoke);
                                        gui->checkbox("Auto Wall", &globals::aimbot::auto_wall);
                                        gui->checkbox("Spans Check", &globals::test2);
                                        gui->checkbox("Target Selector", &globals::test);
                                        gui->dropdown("Fire Mode", &globals::aimbot::auto_fire_mode, kFireModeOptionsArr, IM_ARRAYSIZE(kFireModeOptionsArr));
                                        gui->slider_float("Spread Delay", &globals::aimbot::spread_comp_delay, 0.0f, 500.0f, "%.0f");
                                        gui->slider_float("Autoshoot Delay", &globals::aimbot::autoshoot_delay, 0.0f, 100.0f, "%.0f");
                                    }
                                    gui->end_child();

                                    gui->begin_child("A", "Other");
                                    {
                                        gui->checkbox("Hit Priority", &globals::misc::ht23);
                                        gui->checkbox("Automatic Scope", &globals::aimbot::auto_scope);
                                        gui->checkbox("Aim assist", &globals::aimbot::aim_assist);
                                        gui->slider_float("Minimum Damage", &globals::aimbot::min_damage, 0.0f, 100.0f, "%.0f");
                                        gui->slider_float("Max Distance", &globals::aimbot::max_aim_distance, 0.0f, 100000.0f, "%.0f");
                                    }
                                    gui->end_child();
                                }
                                gui->end_group();
                            }

                            else if (set->selection_accept == 1)
                            {

                                gui->begin_group();
                                {
                                    gui->begin_child("B", "Anti-Aim");
                                    {
                                        gui->checkbox("Enable Anti-Aim", &globals::misc::spinner);
                                        if (globals::misc::spinner) {
                                            globals::misc::aa = true;
                                            gui->checkbox("Server Anti-Aim", &globals::misc::aa);
                                        } else {
                                            globals::misc::aa = false;
                                        }
                                        gui->checkbox("Fast Duck", &globals::misc::fakeduck);
                                        gui->checkbox("Desync Move", &globals::misc::jitter_move);
                                    }
                                    gui->end_child();

                                    gui->begin_child("B", "Anti-Aim Settings");
                                    {
                                        gui->slider_float("Spin value", &globals::misc::spinvalue, 0.0f, 140.0f, "%.1f");
                                        gui->slider_float("Fake lag", &globals::misc::fake_lag_ticks, 0.0f, 64.0f, "%.0f");
                                        gui->slider_float("Jitter range", &globals::misc::jitter_range, 0.0f, 360.0f, "%.1f");
                                        gui->slider_float("Desync range", &globals::misc::desync_range, 0.0f, 360.0f, "%.1f");
                                        gui->checkbox("Anti Aim Control", &globals::misc::pitch_enabled);
                                        if (globals::misc::pitch_enabled) {
                                            gui->slider_float("Pitch Value", &globals::misc::pitch_value, -90.0f, 90.0f, "%.1f");
                                            gui->slider_float("Yaw Value", &globals::misc::yaw_add, -180.0f, 180.0f, "%.1f");
                                        }
                                        gui->checkbox("Jitter on back", &globals::misc::jitter_on_back);
                                        gui->checkbox("Jitter Enabled", &globals::misc::jitter_enabled);
                                    }
                                    gui->end_child();
                                }
                                gui->end_group();

                                gui->sameline();

                                gui->begin_group();
                                {
                                    gui->begin_child("B", "Advanced Anti-Aim");
                                    {
                                        gui->checkbox("Wall Standing", &globals::misc::manual_aa);
                                        gui->checkbox("Freestanding", &globals::misc::freestanding);
                                        gui->checkbox("Center Jitter", &globals::misc::atomic_aa);
                                        if (globals::misc::atomic_aa) {
                                            gui->slider_float("Center Speed", &globals::misc::atomic_speed, 0.1f, 3.0f, "%.2f");
                                            gui->dropdown("Jitter Mode", &globals::misc::atomic_mode, kAtomicModeOptionsArr, IM_ARRAYSIZE(kAtomicModeOptionsArr));
                                            gui->checkbox("Prediction Resolver", &globals::misc::prediction_breaker);
                                            if (globals::misc::prediction_breaker)
                                                gui->slider_float("Resolver Force", &globals::misc::breaker_intensity, 0.5f, 5.0f, "%.2f");
                                        }
                                        gui->dropdown("AA Mode", &globals::misc::aa_mode, kAAModeOptionsArr, IM_ARRAYSIZE(kAAModeOptionsArr));

                                        static int last_aa_mode = -1;
                                        if (last_aa_mode != globals::misc::aa_mode) {
                                            if (globals::misc::aa_mode == 2) {
                                                globals::misc::pitch_enabled  = true;
                                                globals::misc::pitch_value    = -90.0f;
                                                globals::misc::yaw_add        = -180.0f;
                                                globals::misc::jitter_enabled = true;
                                                globals::misc::jitter_range   = 25.0f;
                                            } else if (last_aa_mode == 2) {
                                                globals::misc::pitch_enabled  = false;
                                                globals::misc::jitter_enabled = false;
                                            }
                                            last_aa_mode = globals::misc::aa_mode;
                                        }
                                        switch (globals::misc::aa_mode) {
                                            case 1: globals::misc::aa_spin=true;  globals::misc::aa_jitter=false; globals::misc::aa_threeway=false; globals::misc::aa_desync=false; break;
                                            case 2: globals::misc::aa_spin=false; globals::misc::aa_jitter=true;  globals::misc::aa_threeway=false; globals::misc::aa_desync=false; break;
                                            case 3: globals::misc::aa_spin=false; globals::misc::aa_jitter=false; globals::misc::aa_threeway=true;  globals::misc::aa_desync=false; break;
                                            case 4: globals::misc::aa_spin=false; globals::misc::aa_jitter=false; globals::misc::aa_threeway=true;  globals::misc::aa_desync=false; break;
                                            case 5: globals::misc::aa_spin=false; globals::misc::aa_jitter=false; globals::misc::aa_threeway=false; globals::misc::aa_desync=true;  break;
                                            default: globals::misc::aa_spin=false; globals::misc::aa_jitter=false; globals::misc::aa_threeway=false; globals::misc::aa_desync=false; break;
                                        }
                                    }
                                    gui->end_child();
                                }
                                gui->end_group();
                            }

                            else if (set->selection_accept == 2)
                            {

                                gui->begin_group();
                                {
                                    gui->begin_child("C", "Outline Settings");
                                    {
                                        gui->checkbox("Outline Enemy", &globals::visuals::outline_enabled);
                                        gui->checkbox("Galaxy Self / Enemy", &globals::visuals::self_galaxy_enabled);
                                        gui->checkbox("Dynamic Fresnel", &globals::visuals::dynamic_fresnel_outline);
                                        gui->checkbox("Crystal Hand & Self", &globals::visuals::crystal_chams_enabled);
                                        gui->checkbox("Galaxy Gun", &globals::visuals::galaxy_gun);
                                        gui->checkbox("Self Wireframe", &globals::visuals::self_wireframe);
                                        gui->checkbox("Wireframe hand", &globals::visuals::wireframe_hands);
                                        if (globals::visuals::outline_enabled)
                                            globals::visuals::usepresetedoutlines = true;
                                        if (globals::visuals::self_galaxy_enabled) {
                                            globals::visuals::outline_enabled     = true;
                                            globals::visuals::usepresetedoutlines = true;
                                            gui->dropdown("Galaxy Preset", &globals::visuals::self_galaxy_preset, kGalaxyPresetOptionsArr, IM_ARRAYSIZE(kGalaxyPresetOptionsArr));
                                        }
                                        if (globals::visuals::crystal_chams_enabled) {
                                            gui->dropdown("Crystal Preset", &globals::visuals::crystal_chams_preset, kCrystalPresetOptsArr, IM_ARRAYSIZE(kCrystalPresetOptsArr));
                                        }
                                        if (globals::visuals::dynamic_fresnel_outline) {
                                            gui->dropdown("Dynamic Fresnel", &globals::visuals::dynamic_fresnel_preset, kDynFresnelOptsArr, IM_ARRAYSIZE(kDynFresnelOptsArr));
                                        }
                                    }
                                    gui->end_child();

                                    gui->begin_child("C", "Outline Hand");
                                    {
                                        gui->checkbox("Outline Hand Chams", &globals::visuals::hand_outline);
                                        gui->checkbox("Hand Fresnel", &globals::visuals::handchamsd2);
                                        gui->checkbox("Rainbow Fresnel", &globals::visuals::rainbow_chams);
                                    }
                                    gui->end_child();
                                }
                                gui->end_group();

                                gui->sameline();

                                gui->begin_group();
                                {
                                    gui->begin_child("C", "Intensity Settings");
                                    {
                                        gui->slider_float("Visible Chams", &globals::visuals::outlineintensityvisibleoutline, 100.1f, 500.0f, "%.1f");
                                        gui->slider_float("Invisible Chams", &globals::visuals::outlineintensityinvisbleoutline, 100.1f, 500.0f, "%.1f");
                                        gui->slider_float("Hand & Weapon", &globals::visuals::intensityvisibleoutline, 10.1f, 30.0f, "%.1f");
                                    }
                                    gui->end_child();

                                    gui->begin_child("C", "Chams Presets");
                                    {
                                        gui->dropdown("Visible Preset", &globals::visuals::visiblepreset, kVisiblePresetsArr, IM_ARRAYSIZE(kVisiblePresetsArr));
                                        gui->dropdown("Invisible Preset", &globals::visuals::invisiblepreset, kInvisiblePresetsArr, IM_ARRAYSIZE(kInvisiblePresetsArr));
                                    }
                                    gui->end_child();
                                }
                                gui->end_group();
                            }

                            else if (set->selection_accept == 3)
                            {

                                gui->begin_group();
                                {
                                    gui->begin_child("D", "Players (ESP)");
                                    {
                                        gui->checkbox("Visible Check", &globals::visuals::visiblecheck);
                                        gui->checkbox("Ignore Dormants", &globals::visuals::dormant);
                                        gui->checkbox("2d Box", &globals::visuals::box2d);
                                        gui->checkbox("3d Box", &globals::visuals::box3d);
                                        gui->checkbox("Head Box", &globals::visuals::headb0x);
                                        gui->checkbox("Skeleton", &globals::visuals::sk3let0n);
                                        gui->checkbox("Snaplines", &globals::visuals::snapl1ne);
                                    }
                                    gui->end_child();

                                    gui->begin_child("D", "Render & Info");
                                    {
                                        gui->checkbox("Distance ESP", &globals::visuals::dstc);
                                        gui->checkbox("Health Bar", &globals::visuals::h3althbar);
                                        gui->checkbox("Health Text", &globals::visuals::HealthText);
                                        gui->checkbox("Weapon ESP", &globals::visuals::weaponesp);
                                        gui->checkbox("Chinese Hat", &globals::visuals::chinese_hat);
                                        gui->checkbox("Chinese Hat (Self)", &globals::visuals::chinese_hat_self);
                                    }
                                    gui->end_child();
                                }
                                gui->end_group();

                                gui->sameline();

                                gui->begin_group();
                                {
                                    gui->begin_child("D", "Player Info");
                                    {
                                        gui->checkbox("Player Name", &globals::visuals::nmpl);
                                        gui->checkbox("Platform Info", &globals::visuals::nmpl34);
                                        gui->checkbox("Rank Info", &globals::visuals::nmpl2);
                                        gui->checkbox("Rank Label", &globals::visuals::pclp);
                                        gui->checkbox("Agent Name", &globals::visuals::agent_name);
                                        gui->checkbox("Agent Icon", &globals::visuals::agenticon);
                                    }
                                    gui->end_child();

                                    gui->begin_child("D", "Visuals");
                                    {
                                        gui->checkbox("Normal Chams", &globals::visuals::chamsvsbd);
                                        if (globals::visuals::chamsvsbd) {
                                            globals::visuals::visible_check_ch = true;
                                            gui->checkbox("Self Refresh (P)", &globals::visuals::chams);
                                        } else {
                                            globals::visuals::visible_check_ch = false;
                                        }
                                        gui->checkbox("Bullet Tracers", &globals::visuals::bullet_tracers);
                                        gui->checkbox("Wireframe weapon", &globals::visuals::wireframe_weapon);
                                        gui->checkbox("Hand Chams", &globals::visuals::handchams2);
                                        if (globals::visuals::handchams2) {
                                            gui->dropdown("Material", &globals::visuals::handchams_material_index1, kHandChamsOptsArr, IM_ARRAYSIZE(kHandChamsOptsArr));
                                            gui->slider_float("Glow Intensity", &globals::visuals::handchams_intensity, 0.1f, 20.0f, "%.1f");
                                            gui->slider_float("Red", &globals::visuals::handchams_color_r, 0.0f, 255.0f, "%.0f");
                                            gui->slider_float("Green", &globals::visuals::handchams_color_g, 0.0f, 255.0f, "%.0f");
                                            gui->slider_float("Blue", &globals::visuals::handchams_color_b, 0.0f, 255.0f, "%.0f");
                                        }
                                        gui->checkbox("Trail Effect", &globals::visuals::trail_effect_enabled);
                                        gui->checkbox("Backtrack", &globals::visuals::clone_enabled);
                                    }
                                    gui->end_child();
                                }
                                gui->end_group();
                            }

                            else if (set->selection_accept == 4)
                            {

                                gui->begin_group();
                                {
                                    gui->begin_child("E", "Skybox Controls");
                                    {
                                        gui->checkbox("Enable Skybox", &globals::misc::skybox);
                                        gui->checkbox("Rainbow Skybox", &globals::misc::skyboxrgb);
                                    }
                                    gui->end_child();

                                    gui->begin_child("E", "Traps & World");
                                    {
                                        gui->checkbox("Spike ESP", &globals::misc::gdg);
                                        gui->checkbox("Spike Timer", &globals::misc::spktimer);
                                        gui->checkbox("World ESP", &globals::misc::cpp);
                                        gui->checkbox("Cypher Traps", &globals::misc::world_esp);
                                        gui->checkbox("Gadgets", &globals::misc::gadgets);
                                    }
                                    gui->end_child();
                                }
                                gui->end_group();

                                gui->sameline();

                                gui->begin_group();
                                {
                                    gui->begin_child("E", "Skybox Presets");
                                    {
                                        static int last_preset_idx = -1;
                                        gui->dropdown("Preset", &globals::misc::skybox_preset_index, kSkyboxPresetsArr, IM_ARRAYSIZE(kSkyboxPresetsArr));
                                        if (last_preset_idx != globals::misc::skybox_preset_index) {
                                            switch (globals::misc::skybox_preset_index) {
                                                case 0: globals::misc::CloudSpeed=1.0f;  globals::misc::StarsBrightness=1.0f;  globals::misc::CloudOpacity=1.0f;  globals::misc::SkySunRadius=1.0f;  globals::misc::SkySunBrightness=1.0f;  globals::misc::SkySunHeight=0.0f;  break;
                                                case 1: globals::misc::CloudSpeed=1.0f;  globals::misc::StarsBrightness=4.0f;  globals::misc::CloudOpacity=1.0f;  globals::misc::SkySunRadius=0.5f;  globals::misc::SkySunBrightness=0.5f;  globals::misc::SkySunHeight=-1.0f; break;
                                                case 2: globals::misc::CloudSpeed=10.0f; globals::misc::StarsBrightness=5.0f;  globals::misc::CloudOpacity=2.0f;  globals::misc::SkySunRadius=5.0f;  globals::misc::SkySunBrightness=0.0f;  globals::misc::SkySunHeight=-5.0f; break;
                                                case 3: globals::misc::CloudSpeed=10.0f; globals::misc::StarsBrightness=3.65f; globals::misc::CloudOpacity=2.0f;  globals::misc::SkySunRadius=4.51f; globals::misc::SkySunBrightness=0.0f;  globals::misc::SkySunHeight=5.0f;  break;
                                                case 4: globals::misc::CloudSpeed=5.0f;  globals::misc::StarsBrightness=4.0f;  globals::misc::CloudOpacity=2.0f;  globals::misc::SkySunRadius=1.0f;  globals::misc::SkySunBrightness=1.0f;  globals::misc::SkySunHeight=0.0f;  break;
                                                case 5: globals::misc::CloudSpeed=2.0f;  globals::misc::StarsBrightness=1.0f;  globals::misc::CloudOpacity=1.5f;  globals::misc::SkySunRadius=1.0f;  globals::misc::SkySunBrightness=1.0f;  globals::misc::SkySunHeight=0.0f;  break;
                                                case 6: globals::misc::CloudSpeed=4.0f;  globals::misc::StarsBrightness=3.0f;  globals::misc::CloudOpacity=1.8f;  globals::misc::SkySunRadius=1.0f;  globals::misc::SkySunBrightness=1.0f;  globals::misc::SkySunHeight=0.0f;  break;
                                                case 7: globals::misc::CloudSpeed=1.0f;  globals::misc::StarsBrightness=8.0f;  globals::misc::CloudOpacity=0.8f;  globals::misc::SkySunRadius=1.0f;  globals::misc::SkySunBrightness=1.0f;  globals::misc::SkySunHeight=0.0f;  break;
                                            }
                                            last_preset_idx = globals::misc::skybox_preset_index;
                                        }
                                    }
                                    gui->end_child();

                                    gui->begin_child("E", "Skybox Customize");
                                    {
                                        gui->slider_float("Cloud Speed", &globals::misc::CloudSpeed, 0.0f, 10.0f, "%.2f");
                                        gui->slider_float("Stars Brightness", &globals::misc::StarsBrightness, 0.0f, 5.0f, "%.2f");
                                        gui->slider_float("Cloud Opacity", &globals::misc::CloudOpacity, 0.0f, 2.0f, "%.2f");
                                        gui->slider_float("Sun Radius", &globals::misc::SkySunRadius, 0.0f, 5.0f, "%.2f");
                                        gui->slider_float("Sun Brightness", &globals::misc::SkySunBrightness, 0.0f, 10.0f, "%.2f");
                                        gui->slider_float("Noise Power 1", &globals::misc::SkyNoisePower1, 0.0f, 10.0f, "%.2f");
                                        gui->slider_float("Noise Power 2", &globals::misc::SkyNoisePower2, 0.0f, 10.0f, "%.2f");
                                    }
                                    gui->end_child();
                                }
                                gui->end_group();
                            }

                            else if (set->selection_accept == 5)
                            {

                                gui->begin_group();
                                {
                                    gui->begin_child("F", "Skins");
                                    {
                                        gui->checkbox("Apply custom skins", &globals::misc::playerchamsself);
                                        gui->checkbox("Unlock all", &globals::misc::sk1n_chang3r);
                                        gui->checkbox("Gun Chams", &globals::misc::customgun);
                                        gui->checkbox("Gun Materials", &globals::misc::rdy);
                                    }
                                    gui->end_child();

                                    gui->begin_child("F", "Misc Features");
                                    {
                                        gui->checkbox("Advanced Resolver", &globals::aimbot::vsr);
                                        gui->checkbox("View model changer", &globals::misc::rmsdw);
                                        gui->checkbox("Big Gun", &globals::misc::BigGun3p);
                                        gui->checkbox("Self Resizer", &globals::misc::BigSelf);
                                        gui->checkbox("Finisher", &globals::misc::finisher);
                                        if (globals::misc::finisher)
                                            gui->checkbox("Only Last Kill", &globals::misc::onlylastkill);
                                        gui->checkbox("Killsay", &globals::misc::killsays);
                                        gui->checkbox("Chat Spammer F2", &globals::misc::chat_spammer);

                                        static char chatBuf[128] = "KRX on top";
                                        if (gui->text_field("Chat Message", "Enter spam message", ImVec2(ImGui::GetContentRegionMax().x - style->WindowPadding.x, 35), chatBuf, sizeof(chatBuf)))
                                        {
                                            globals::misc::chat_message = chatBuf;
                                        }
                                    }
                                    gui->end_child();
                                }
                                gui->end_group();

                                gui->sameline();

                                gui->begin_group();
                                {
                                    gui->begin_child("F", "Exploits");
                                    {
                                        gui->checkbox("Remove flash", &globals::misc::antiflash);
                                        gui->checkbox("Bunny hop", &globals::misc::bunnyhop);
                                        gui->checkbox("Fast crouch", &globals::misc::fastcrouch);
                                        gui->checkbox("Skip Tutorial", &globals::misc::disconnect_server);
                                        gui->checkbox("Unload", &globals::misc::nld);
                                    }
                                    gui->end_child();

                                    gui->begin_child("F", "Future & Camera");
                                    {
                                        gui->checkbox("Custom Aspect Ratio", &globals::misc::aspect_ratio_enabled);
                                        gui->checkbox("Third Person (H)", &globals::misc::tperson);
                                        gui->checkbox("Fov Changer", &globals::misc::FovChangor);
                                        if (globals::misc::FovChangor)
                                            gui->slider_float("FOV", &globals::misc::Fovchangerfloat, 70.0f, 120.0f, "%.0f");
                                        gui->slider_float("Aspect ratio", &globals::misc::aspect_ratio_value, 0.5f, 3.0f, "%.2f");
                                        gui->slider_float("Camera Distance", &globals::misc::PlayerDistance, 10.0f, 1000.0f, "%.0f");
                                    }
                                    gui->end_child();
                                }
                                gui->end_group();
                            }

                            else if (set->selection_accept == 6)
                            {

                                gui->begin_group();
                                {
                                    gui->begin_child("G", "Config Actions");
                                    {
                                        static bool save_conf = false;
                                        if (gui->button(save_conf ? "Are you sure?" : "Save Config", ImVec2(ImGui::GetContentRegionMax().x - style->WindowPadding.x, 38)))
                                        {
                                            if (save_conf)
                                            {
                                                save_config();
                                                notify->AddNotification("Configuration", "Config successfully saved.", 3000, gui->get_clr(clr->accent_color));
                                                save_conf = false;
                                            }
                                            else save_conf = true;
                                        }

                                        static bool load_conf = false;
                                        if (gui->button(load_conf ? "Confirm Load?" : "Load Config", ImVec2(ImGui::GetContentRegionMax().x - style->WindowPadding.x, 38)))
                                        {
                                            if (load_conf)
                                            {
                                                load_config();
                                                notify->AddNotification("Configuration", "Config loaded successfully.", 3000, gui->get_clr(clr->accent_color));
                                                load_conf = false;
                                            }
                                            else load_conf = true;
                                        }
                                    }
                                    gui->end_child();

                                    gui->begin_child("G", "Menu Settings");
                                    {
                                        gui->keybind("Menu Key", &globals::misc::kky);
                                        gui->slider_float("UI Scale", &globals::misc::ui_scale, 0.25f, 2.5f, "%.1f");
                                        gui->checkbox("Stream Proof", &globals::misc::stream_proof);
                                        gui->color_edit("Accent Color", func->accent_color);
                                    }
                                    gui->end_child();
                                }
                                gui->end_group();

                                gui->sameline();

                                gui->begin_group();
                                {
                                    gui->begin_child("G", "Profiles");
                                    {
                                        float btn_w = 78.0f;
                                        float field_w = ImGui::GetContentRegionMax().x - style->WindowPadding.x - btn_w - style->ItemSpacing.x;
                                        gui->text_field("Config Name", "Enter name", ImVec2(field_w, 36), set->create_config, IM_ARRAYSIZE(set->create_config));
                                        gui->sameline();
                                        if (gui->button("Create", ImVec2(btn_w, 36)))
                                        {
                                            if (strlen(set->create_config) > 0) {
                                                set->selection_config.push_back(set->create_config);
                                                notify->AddNotification("Profile Created", set->create_config, 3000, gui->get_clr(clr->accent_color));
                                                set->create_config[0] = 0;
                                            }
                                        }

                                        gui->separator();

                                        for (int i = 0; i < (int)set->selection_config.size(); i++)
                                        {
                                            gui->config_panel(set->selection_config[i], "", "KRX", i, set->config_count, ImVec2(ImGui::GetContentRegionMax().x - style->WindowPadding.x, 62));
                                        }
                                    }
                                    gui->end_child();
                                }
                                gui->end_group();
                            }

                            gui->pop_style_var();
                        }
                        gui->end_content();
                    }
                }
                gui->end();
                gui->pop_style_var();
            }

        if (s_anim_progress > 0.05f)
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_None);
            ImVec2 mouse = ImGui::GetMousePos();
            ImDrawList* fg = ImGui::GetForegroundDrawList();
            if (fg && (mouse.x >= 0 && mouse.y >= 0)) {
                fg->AddRect(ImVec2(mouse.x - 3, mouse.y - 3), ImVec2(mouse.x + 4, mouse.y + 4), IM_COL32(0, 0, 0, 200));
                fg->AddRectFilled(ImVec2(mouse.x - 2, mouse.y - 2), ImVec2(mouse.x + 3, mouse.y + 3), IM_COL32(255, 255, 255, 255));
            }
        }
        else
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
        }
        }
        ImGui::Render();
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        g_pSwapChain->Present(0, 0);

        if (wcsstr(::GetCommandLineW(), L"-screenshot") != nullptr)
        {
            static int s_frames = 0;
            if (++s_frames == 25)
            {
                CaptureBackbufferToBmp("preview.bmp");
                exit(0);
            }
        }
        if (GetAsyncKeyState(VK_F12) & 0x01)
        {
            CaptureBackbufferToBmp("preview.bmp");
        }

        Sleep(1);
    }

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}

bool CreateDeviceD3D(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED)
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_KEYDOWN && (wParam == VK_INSERT || (globals::misc::kky != 0 && (int)wParam == globals::misc::kky)))
    {
        set->show_menu = !set->show_menu;
    }

    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_ERASEBKGND:
        return 1;
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_ResizeWidth = (UINT)LOWORD(lParam);
        g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

