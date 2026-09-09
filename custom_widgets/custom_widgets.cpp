#define IMGUI_DEFINE_MATH_OPERATORS

#include "custom_widgets.hpp"
#include "imstb_textedit.h"

using namespace ImGui;
using namespace ImStb;

bool c_widgets::begin_popup_w(std::string name, float size_x)
{
    ImGuiWindow* window = GetCurrentWindow();
    const ImGuiID id = window->GetID(name.data());
    ImGuiContext& g = *GImGui;

    ImVec2 content_size;

    struct popup_state
    {
        ImVec2 window_padding = ImVec2(10, 10);
        bool window_opened, hovered;
        float begin_offset, alpha_popup;
    };

    static std::map<ImGuiID, popup_state> anim;
    popup_state& state = anim[id];

    if (IsItemHovered() && g.IO.MouseClicked[1] || state.window_opened && g.IO.MouseClicked[0] && !state.hovered) state.window_opened = !state.window_opened;

    state.begin_offset = ImLerp(state.begin_offset, state.window_opened ? -5.f : 15, gui->fixed_speed(10.f));
    state.alpha_popup = ImClamp(state.alpha_popup + (gui->fixed_speed(5.f) * (state.window_opened ? 1.f : -1.f)), 0.f, 1.f);

    PushStyleVar(ImGuiStyleVar_Alpha, state.alpha_popup);
    PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.f);
    PushStyleVar(ImGuiStyleVar_WindowPadding, state.window_padding);
    PushStyleColor(ImGuiCol_WindowBg, gui->get_clr(clr->combo_color));
    PushStyleColor(ImGuiCol_Border, gui->get_clr(clr->separator));

    SetNextWindowPos(g.LastItemData.Rect.GetBR() - ImVec2(size_x, state.begin_offset));
    SetNextWindowSize(ImVec2(size_x, content_size.y));

    gui->begin((std::stringstream{} << id << "_popup").str().c_str(), NULL, set->popup_flags);

    state.hovered = IsMouseHoveringRect(GetWindowPos(), GetWindowPos() + GetWindowSize());

    content_size = ImGui::GetContentRegionMax() + state.window_padding;

    return false;
}

void c_widgets::end_popup_w()
{
    gui->end();

    PopStyleVar(3);
    PopStyleColor(2);
}

void c_widgets::separator()
{
    ImGuiWindow* window = GetCurrentWindow();

    window->DrawList->AddLine(GetCursorScreenPos(), GetCursorScreenPos() + ImVec2(GetContentRegionMax().x - GetStyle().WindowPadding.x, 0), gui->get_clr(clr->separator), 1.f);
    Spacing();
}

static ImVec2 input_text_calc_text_size_w(ImGuiContext* ctx, const ImWchar* text_begin, const ImWchar* text_end, const ImWchar** remaining = NULL, ImVec2* out_offset = NULL, bool stop_on_new_line = false)
{
    ImGuiContext& g = *ctx;
    ImFont* font = g.Font;
    const float line_height = g.FontSize;
    const float scale = line_height / font->FontSize;

    ImVec2 text_size = ImVec2(0, 0);
    float line_width = 0.0f;

    const ImWchar* s = text_begin;
    while (s < text_end)
    {
        unsigned int c = (unsigned int)(*s++);
        if (c == '\n')
        {
            text_size.x = ImMax(text_size.x, line_width);
            text_size.y += line_height;
            line_width = 0.0f;
            if (stop_on_new_line)
                break;
            continue;
        }
        if (c == '\r')
            continue;

        const float char_width = font->GetCharAdvance((ImWchar)c) * scale;
        line_width += char_width;
    }

    if (text_size.x < line_width)
        text_size.x = line_width;

    if (out_offset)
        *out_offset = ImVec2(line_width, text_size.y + line_height);

    if (line_width > 0 || text_size.y == 0.0f)
        text_size.y += line_height;

    if (remaining)
        *remaining = s;

    return text_size;
}

static const char* items_array_getter(void* data, int idx)
{
    const char* const* items = (const char* const*)data;
    return items[idx];
}

namespace ImStb
{

    static int     STB_TEXTEDIT_STRINGLEN(const ImGuiInputTextState* obj) { return obj->CurLenW; }
    static ImWchar STB_TEXTEDIT_GETCHAR(const ImGuiInputTextState* obj, int idx) { IM_ASSERT(idx <= obj->CurLenW); return obj->TextW[idx]; }
    static float   STB_TEXTEDIT_GETWIDTH(ImGuiInputTextState* obj, int line_start_idx, int char_idx) { ImWchar c = obj->TextW[line_start_idx + char_idx]; if (c == '\n') return IMSTB_TEXTEDIT_GETWIDTH_NEWLINE; ImGuiContext& g = *obj->Ctx; return g.Font->GetCharAdvance(c) * (g.FontSize / g.Font->FontSize); }
    static int     STB_TEXTEDIT_KEYTOTEXT(int key) { return key >= 0x200000 ? 0 : key; }
    static ImWchar STB_TEXTEDIT_NEWLINE = '\n';
    static void    STB_TEXTEDIT_LAYOUTROW(StbTexteditRow* r, ImGuiInputTextState* obj, int line_start_idx)
    {
        const ImWchar* text = obj->TextW.Data;
        const ImWchar* text_remaining = NULL;
        const ImVec2 size = input_text_calc_text_size_w(obj->Ctx, text + line_start_idx, text + obj->CurLenW, &text_remaining, NULL, true);
        r->x0 = 0.0f;
        r->x1 = size.x;
        r->baseline_y_delta = size.y;
        r->ymin = 0.0f;
        r->ymax = size.y;
        r->num_chars = (int)(text_remaining - (text + line_start_idx));
    }

    static bool is_separator(unsigned int c)
    {
        return c == ',' || c == ';' || c == '(' || c == ')' || c == '{' || c == '}' || c == '[' || c == ']' || c == '|' || c == '\n' || c == '\r' || c == '.' || c == '!';
    }

    static int is_word_boundary_from_right(ImGuiInputTextState* obj, int idx)
    {

        if ((obj->Flags & ImGuiInputTextFlags_Password) || idx <= 0)
            return 0;

        bool prev_white = ImCharIsBlankW(obj->TextW[idx - 1]);
        bool prev_separ = is_separator(obj->TextW[idx - 1]);
        bool curr_white = ImCharIsBlankW(obj->TextW[idx]);
        bool curr_separ = is_separator(obj->TextW[idx]);
        return ((prev_white || prev_separ) && !(curr_separ || curr_white)) || (curr_separ && !prev_separ);
    }
    static int is_word_boundary_from_left(ImGuiInputTextState* obj, int idx)
    {
        if ((obj->Flags & ImGuiInputTextFlags_Password) || idx <= 0)
            return 0;

        bool prev_white = ImCharIsBlankW(obj->TextW[idx]);
        bool prev_separ = is_separator(obj->TextW[idx]);
        bool curr_white = ImCharIsBlankW(obj->TextW[idx - 1]);
        bool curr_separ = is_separator(obj->TextW[idx - 1]);
        return ((prev_white) && !(curr_separ || curr_white)) || (curr_separ && !prev_separ);
    }
    static int  STB_TEXTEDIT_MOVEWORDLEFT_IMPL(ImGuiInputTextState* obj, int idx) { idx--; while (idx >= 0 && !is_word_boundary_from_right(obj, idx)) idx--; return idx < 0 ? 0 : idx; }
    static int  STB_TEXTEDIT_MOVEWORDRIGHT_MAC(ImGuiInputTextState* obj, int idx) { idx++; int len = obj->CurLenW; while (idx < len && !is_word_boundary_from_left(obj, idx)) idx++; return idx > len ? len : idx; }
    static int  STB_TEXTEDIT_MOVEWORDRIGHT_WIN(ImGuiInputTextState* obj, int idx) { idx++; int len = obj->CurLenW; while (idx < len && !is_word_boundary_from_right(obj, idx)) idx++; return idx > len ? len : idx; }
    static int  STB_TEXTEDIT_MOVEWORDRIGHT_IMPL(ImGuiInputTextState* obj, int idx) { ImGuiContext& g = *obj->Ctx; if (g.IO.ConfigMacOSXBehaviors) return STB_TEXTEDIT_MOVEWORDRIGHT_MAC(obj, idx); else return STB_TEXTEDIT_MOVEWORDRIGHT_WIN(obj, idx); }
#define STB_TEXTEDIT_MOVEWORDLEFT   STB_TEXTEDIT_MOVEWORDLEFT_IMPL
#define STB_TEXTEDIT_MOVEWORDRIGHT  STB_TEXTEDIT_MOVEWORDRIGHT_IMPL

    static void STB_TEXTEDIT_DELETECHARS(ImGuiInputTextState* obj, int pos, int n)
    {
        ImWchar* dst = obj->TextW.Data + pos;

        obj->Edited = true;
        obj->CurLenA -= ImTextCountUtf8BytesFromStr(dst, dst + n);
        obj->CurLenW -= n;

        const ImWchar* src = obj->TextW.Data + pos + n;
        while (ImWchar c = *src++)
            *dst++ = c;
        *dst = '\0';
    }

    static bool STB_TEXTEDIT_INSERTCHARS(ImGuiInputTextState* obj, int pos, const ImWchar* new_text, int new_text_len)
    {
        const bool is_resizable = (obj->Flags & ImGuiInputTextFlags_CallbackResize) != 0;
        const int text_len = obj->CurLenW;
        IM_ASSERT(pos <= text_len);

        const int new_text_len_utf8 = ImTextCountUtf8BytesFromStr(new_text, new_text + new_text_len);
        if (!is_resizable && (new_text_len_utf8 + obj->CurLenA + 1 > obj->BufCapacityA))
            return false;

        if (new_text_len + text_len + 1 > obj->TextW.Size)
        {
            if (!is_resizable)
                return false;
            IM_ASSERT(text_len < obj->TextW.Size);
            obj->TextW.resize(text_len + ImClamp(new_text_len * 4, 32, ImMax(256, new_text_len)) + 1);
        }

        ImWchar* text = obj->TextW.Data;
        if (pos != text_len)
            memmove(text + pos + new_text_len, text + pos, (size_t)(text_len - pos) * sizeof(ImWchar));
        memcpy(text + pos, new_text, (size_t)new_text_len * sizeof(ImWchar));

        obj->Edited = true;
        obj->CurLenW += new_text_len;
        obj->CurLenA += new_text_len_utf8;
        obj->TextW[obj->CurLenW] = '\0';

        return true;
    }

#define STB_TEXTEDIT_K_LEFT         0x200000
#define STB_TEXTEDIT_K_RIGHT        0x200001
#define STB_TEXTEDIT_K_UP           0x200002
#define STB_TEXTEDIT_K_DOWN         0x200003
#define STB_TEXTEDIT_K_LINESTART    0x200004
#define STB_TEXTEDIT_K_LINEEND      0x200005
#define STB_TEXTEDIT_K_TEXTSTART    0x200006
#define STB_TEXTEDIT_K_TEXTEND      0x200007
#define STB_TEXTEDIT_K_DELETE       0x200008
#define STB_TEXTEDIT_K_BACKSPACE    0x200009
#define STB_TEXTEDIT_K_UNDO         0x20000A
#define STB_TEXTEDIT_K_REDO         0x20000B
#define STB_TEXTEDIT_K_WORDLEFT     0x20000C
#define STB_TEXTEDIT_K_WORDRIGHT    0x20000D
#define STB_TEXTEDIT_K_PGUP         0x20000E
#define STB_TEXTEDIT_K_PGDOWN       0x20000F
#define STB_TEXTEDIT_K_SHIFT        0x400000

#define IMSTB_TEXTEDIT_IMPLEMENTATION
#define IMSTB_TEXTEDIT_memmove memmove
#include "imstb_textedit.h"

    static void stb_textedit_replace(ImGuiInputTextState* str, STB_TexteditState* state, const IMSTB_TEXTEDIT_CHARTYPE* text, int text_len)
    {
        stb_text_makeundo_replace(str, state, 0, str->CurLenW, text_len);
        ImStb::STB_TEXTEDIT_DELETECHARS(str, 0, str->CurLenW);
        state->cursor = state->select_start = state->select_end = 0;
        if (text_len <= 0)
            return;
        if (ImStb::STB_TEXTEDIT_INSERTCHARS(str, 0, text, text_len))
        {
            state->cursor = state->select_start = state->select_end = text_len;
            state->has_preferred_x = 0;
            return;
        }
        IM_ASSERT(0);
    }

}

ImU32 c_widgets::get_clr(const ImVec4& col, float alpha)
{
    ImGuiStyle& style = GImGui->Style;
    ImVec4 c = col;
    c.w *= style.Alpha * alpha;
    return ColorConvertFloat4ToU32(c);
}

void c_widgets::text_colored(const ImU32 col, std::string text)
{
    ImGui::TextColored(ImColor(col), text.data());
}

void c_widgets::render_text_clipped(ImFont* font, const ImVec2& p_min, const ImVec2& p_max, ImU32 col, const char* text, const ImVec2& align)
{
    if (!text || !*text) return;
    const char* text_end = FindRenderedTextEnd(text);
    if (text == text_end) return;
    PushFont(font);
    PushStyleColor(ImGuiCol_Text, col);
    RenderTextClipped(p_min, p_max, text, text_end, NULL, align, NULL);
    PopStyleColor();
    PopFont();
}

void c_widgets::set_cursor_pos(const ImVec2& local_pos)
{
    ImGui::SetCursorPos(local_pos);
}

void c_widgets::begin_group()
{
    ImGui::BeginGroup();
}

void c_widgets::end_group()
{
    ImGui::EndGroup();
}

void c_widgets::push_style_var(ImGuiStyleVar idx, const ImVec2& val)
{
    ImGui::PushStyleVar(idx, val);
}

void c_widgets::push_style_var(ImGuiStyleVar idx, const float val)
{
    ImGui::PushStyleVar(idx, val);
}

void c_widgets::pop_style_var(float num)
{
    ImGui::PopStyleVar(num);
}

void c_widgets::sameline()
{
    ImGui::SameLine(0.f, -1.f);
}

bool c_widgets::begin(std::string_view name, bool* open, ImGuiWindowFlags flags)
{
    return ImGui::Begin(name.data(), open, flags);
}

void c_widgets::end()
{
    ImGui::End();
}

void c_widgets::add_rect_filled_multi_color(ImDrawList* draw, const ImVec2& p_min, const ImVec2& p_max, ImU32 col_upr_left, ImU32 col_upr_right, ImU32 col_bot_right, ImU32 col_bot_left, float rounding, ImDrawFlags flags)
{
    if (((col_upr_left | col_upr_right | col_bot_right | col_bot_left) & IM_COL32_A_MASK) == 0) return;

    auto fix_rect_corner_flags{ [](ImDrawFlags rflags) {    if ((rflags & ImDrawFlags_RoundCornersMask_) == 0)
                                                            rflags |= ImDrawFlags_RoundCornersAll;
                                                            return rflags;
                                                        } };

    flags = fix_rect_corner_flags(flags);
    rounding = ImMin(rounding, ImFabs(p_max.x - p_min.x) * (((flags & ImDrawFlags_RoundCornersTop) == ImDrawFlags_RoundCornersTop) || ((flags & ImDrawFlags_RoundCornersBottom) == ImDrawFlags_RoundCornersBottom) ? 0.5f : 1.0f) - 1.0f);
    rounding = ImMin(rounding, ImFabs(p_max.y - p_min.y) * (((flags & ImDrawFlags_RoundCornersLeft) == ImDrawFlags_RoundCornersLeft) || ((flags & ImDrawFlags_RoundCornersRight) == ImDrawFlags_RoundCornersRight) ? 0.5f : 1.0f) - 1.0f);

    if (rounding > 0.0f)
    {
        const int size_before = draw->VtxBuffer.Size;
        draw->AddRectFilled(p_min, p_max, IM_COL32_WHITE, rounding, flags);
        const int size_after = draw->VtxBuffer.Size;

        for (int i = size_before; i < size_after; i++)
        {
            ImDrawVert* vert = draw->VtxBuffer.Data + i;

            ImVec4 upr_left = ImGui::ColorConvertU32ToFloat4(col_upr_left);
            ImVec4 bot_left = ImGui::ColorConvertU32ToFloat4(col_bot_left);
            ImVec4 up_right = ImGui::ColorConvertU32ToFloat4(col_upr_right);
            ImVec4 bot_right = ImGui::ColorConvertU32ToFloat4(col_bot_right);

            float X = ImClamp((vert->pos.x - p_min.x) / (p_max.x - p_min.x), 0.0f, 1.0f);

            float r1 = upr_left.x + (up_right.x - upr_left.x) * X;
            float r2 = bot_left.x + (bot_right.x - bot_left.x) * X;

            float g1 = upr_left.y + (up_right.y - upr_left.y) * X;
            float g2 = bot_left.y + (bot_right.y - bot_left.y) * X;

            float b1 = upr_left.z + (up_right.z - upr_left.z) * X;
            float b2 = bot_left.z + (bot_right.z - bot_left.z) * X;

            float a1 = upr_left.w + (up_right.w - upr_left.w) * X;
            float a2 = bot_left.w + (bot_right.w - bot_left.w) * X;

            float Y = ImClamp((vert->pos.y - p_min.y) / (p_max.y - p_min.y), 0.0f, 1.0f);
            float r = r1 + (r2 - r1) * Y;
            float g = g1 + (g2 - g1) * Y;
            float b = b1 + (b2 - b1) * Y;
            float a = a1 + (a2 - a1) * Y;
            ImVec4 RGBA(r, g, b, a);

            RGBA = RGBA * ImGui::ColorConvertU32ToFloat4(vert->col);

            vert->col = ImColor(RGBA);
        }
        return;
    }

    const ImVec2 uv = draw->_Data->TexUvWhitePixel;
    draw->PrimReserve(6, 4);
    draw->PrimWriteIdx((ImDrawIdx)(draw->_VtxCurrentIdx)); draw->PrimWriteIdx((ImDrawIdx)(draw->_VtxCurrentIdx + 1)); draw->PrimWriteIdx((ImDrawIdx)(draw->_VtxCurrentIdx + 2));
    draw->PrimWriteIdx((ImDrawIdx)(draw->_VtxCurrentIdx)); draw->PrimWriteIdx((ImDrawIdx)(draw->_VtxCurrentIdx + 2)); draw->PrimWriteIdx((ImDrawIdx)(draw->_VtxCurrentIdx + 3));
    draw->PrimWriteVtx(p_min, uv, col_upr_left);
    draw->PrimWriteVtx(ImVec2(p_max.x, p_min.y), uv, col_upr_right);
    draw->PrimWriteVtx(p_max, uv, col_bot_right);
    draw->PrimWriteVtx(ImVec2(p_min.x, p_max.y), uv, col_bot_left);
}

bool c_widgets::sub_selection(std::string_view label, int selection_id, int& selection_variable, const ImVec2& size)
{
    struct sub_selection_state
    {
        ImVec4 text = clr->text, icon = clr->accent_color;
        float opticaly = 1.f;
    };

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems) return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label.data());
    const bool selected = selection_id == selection_variable;

    const ImVec2 pos = window->DC.CursorPos;

    static std::map<ImGuiID, sub_selection_state> anim;
    sub_selection_state& state = anim[id];

    const ImRect rect(pos, pos + size);

    ItemSize(rect, 0);
    if (!ItemAdd(rect, id)) return false;

    bool hovered, held, pressed = ButtonBehavior(rect, id, &hovered, &held);
    if (pressed) selection_variable = selection_id;

    state.opticaly = ImLerp(state.opticaly, selected ? 1.f : 0.f, gui->fixed_speed(6.f));
    state.icon = ImLerp(state.icon, selected ? clr->accent_color : clr->text, gui->fixed_speed(8.f));
    state.text = ImLerp(state.text, selected ? clr->text_active : clr->text, gui->fixed_speed(8.f));

    window->DrawList->AddRectFilled(rect.Min, rect.Max, get_clr(clr->selection_active, state.opticaly), set->selection_rounding);
    window->DrawList->AddRectFilled(rect.Min + ImVec2((size.x / 3), (size.y - 3)), rect.Max - ImVec2((size.x / 3), 0), get_clr(clr->accent_color, state.opticaly), set->selection_rounding);

    gui->render_text_clipped(set->poppins_selection, rect.Min, rect.Max, gui->get_clr(state.text), label.data(), { 0.5, 0.5 });

    return pressed;
}

bool c_widgets::selection(std::string_view icon_label, std::string_view label, int selection_id, int& selection_variable, const ImVec2& size)
{
    struct selection_state
    {
        ImVec4 text = clr->text, icon = clr->accent_color;
        float opticaly = 1.f;
    };

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems) return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(icon_label.data());
    const bool selected = selection_id == selection_variable;

    const ImVec2 pos = window->DC.CursorPos;

    static std::map<ImGuiID, selection_state> anim;
    selection_state& state = anim[id];

    const ImRect rect(pos, pos + size);

    ItemSize(rect, 0);
    if (!ItemAdd(rect, id)) return false;

    bool hovered, held, pressed = ButtonBehavior(rect, id, &hovered, &held);
    if (pressed) selection_variable = selection_id;

    float target_alpha = selected ? 1.0f : (hovered ? 0.35f : 0.0f);
    state.opticaly = ImLerp(state.opticaly, target_alpha, gui->fixed_speed(12.f));

    ImVec4 target_icon = selected ? clr->accent_color : (hovered ? clr->text_active : clr->text);
    ImVec4 target_text = selected ? clr->text_active : (hovered ? clr->text_active : clr->text);
    state.icon = ImLerp(state.icon, target_icon, gui->fixed_speed(14.f));
    state.text = ImLerp(state.text, target_text, gui->fixed_speed(14.f));

    window->DrawList->AddRectFilled(rect.Min, rect.Max, get_clr(clr->selection_active, state.opticaly), set->selection_rounding);
    if (selected || state.opticaly > 0.05f)
    {
        window->DrawList->AddRect(rect.Min, rect.Max, get_clr(clr->accent_color, state.opticaly * 0.7f), set->selection_rounding, 0, 1.0f);
        window->DrawList->AddRectFilled(rect.Max - ImVec2(19, (size.y - 15)), rect.Max - ImVec2(15, 15), get_clr(clr->accent_color, state.opticaly), set->selection_rounding);
    }

    gui->render_text_clipped(set->icon, rect.Min, rect.Min + ImVec2(size.y, size.y), gui->get_clr(state.icon), icon_label.data(), {0.5, 0.5});
    gui->render_text_clipped(set->poppins_selection, rect.Min + ImVec2(size.y, 0), rect.Max, gui->get_clr(state.text), label.data(), {0.0, 0.5});

    return pressed;
}

void c_widgets::begin_content()
{
    PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 10));
    PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
    const ImVec2 size = ImVec2(GetWindowSize().x - GetCursorPosX() - 18.0f, GetWindowSize().y - GetCursorPosY() - 18.0f);
    BeginChildEx("content area", GetCurrentWindow()->GetID("content area"), size, ImGuiChildFlags_None, ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
}

void c_widgets::end_content()
{
    EndChild();
    ImGui::PopStyleVar(2);
}

bool begin_child_ex(const char* icon, const char* name, ImGuiID id, const ImVec2& size_arg, ImGuiChildFlags child_flags, ImGuiWindowFlags window_flags)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* parent_window = g.CurrentWindow;
    IM_ASSERT(id != 0);

    const ImGuiChildFlags ImGuiChildFlags_SupportedMask_ = ImGuiChildFlags_Border | ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_ResizeX | ImGuiChildFlags_ResizeY | ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_FrameStyle;
    IM_UNUSED(ImGuiChildFlags_SupportedMask_);
    IM_ASSERT((child_flags & ~ImGuiChildFlags_SupportedMask_) == 0 && "Illegal ImGuiChildFlags value. Did you pass ImGuiWindowFlags values instead of ImGuiChildFlags?");
    IM_ASSERT((window_flags & ImGuiWindowFlags_AlwaysAutoResize) == 0 && "Cannot specify ImGuiWindowFlags_AlwaysAutoResize for BeginChild(). Use ImGuiChildFlags_AlwaysAutoResize!");
    if (child_flags & ImGuiChildFlags_AlwaysAutoResize)
    {
        IM_ASSERT((child_flags & (ImGuiChildFlags_ResizeX | ImGuiChildFlags_ResizeY)) == 0 && "Cannot use ImGuiChildFlags_ResizeX or ImGuiChildFlags_ResizeY with ImGuiChildFlags_AlwaysAutoResize!");
        IM_ASSERT((child_flags & (ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY)) != 0 && "Must use ImGuiChildFlags_AutoResizeX or ImGuiChildFlags_AutoResizeY with ImGuiChildFlags_AlwaysAutoResize!");
    }
#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
    if (window_flags & ImGuiWindowFlags_AlwaysUseWindowPadding)
        child_flags |= ImGuiChildFlags_AlwaysUseWindowPadding;
#endif
    if (child_flags & ImGuiChildFlags_AutoResizeX) child_flags &= ~ImGuiChildFlags_ResizeX;
    if (child_flags & ImGuiChildFlags_AutoResizeY) child_flags &= ~ImGuiChildFlags_ResizeY;

    window_flags |= ImGuiWindowFlags_ChildWindow | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
    window_flags |= (parent_window->Flags & ImGuiWindowFlags_NoMove);

    if (child_flags & (ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysAutoResize)) window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
    if ((child_flags & (ImGuiChildFlags_ResizeX | ImGuiChildFlags_ResizeY)) == 0) window_flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings;

    if (child_flags & ImGuiChildFlags_FrameStyle)
    {
        PushStyleColor(ImGuiCol_ChildBg, g.Style.Colors[ImGuiCol_FrameBg]);
        PushStyleVar(ImGuiStyleVar_ChildRounding, g.Style.FrameRounding);
        PushStyleVar(ImGuiStyleVar_ChildBorderSize, g.Style.FrameBorderSize);
        PushStyleVar(ImGuiStyleVar_WindowPadding, g.Style.FramePadding);
        child_flags |= ImGuiChildFlags_Border | ImGuiChildFlags_AlwaysUseWindowPadding;
        window_flags |= ImGuiWindowFlags_NoMove;
    }

    g.NextWindowData.Flags |= ImGuiNextWindowDataFlags_HasChildFlags;
    g.NextWindowData.ChildFlags = child_flags;

    static std::map<ImGuiID, float> anim;
    float& state = anim[id];

    ImVec2 size = size_arg;
    if (size.x <= 0) size.x = (GetContentRegionMax().x - g.Style.ItemSpacing.x - g.Style.ScrollbarSize - (set->widgets_padding.x + 2)) / 2;
    if (size.y <= 0) size.y = state;

    SetNextWindowSize(size);
    SetNextWindowPos(parent_window->DC.CursorPos + ImVec2(0, 40));

    const char* temp_window_name;

    if (name) ImFormatStringToTempBuffer(&temp_window_name, NULL, "%s/%s_%08X", parent_window->Name, name, id);
    else ImFormatStringToTempBuffer(&temp_window_name, NULL, "%s/%08X", parent_window->Name, id);

    const float backup_border_size = g.Style.ChildBorderSize;
    if ((child_flags & ImGuiChildFlags_Border) == 0) g.Style.ChildBorderSize = 0.0f;

    const ImVec2 c_pos = parent_window->DC.CursorPos;
    const ImVec2 c_max = c_pos + size + ImVec2(0, 40);

    parent_window->DrawList->AddRect(c_pos - ImVec2(1, 0), c_max + ImVec2(1, 2), IM_COL32(0, 0, 0, 45), set->child_rounding + 1.0f, 0, 1.5f);

    parent_window->DrawList->AddRectFilled(c_pos, c_pos + ImVec2(size.x, 40), gui->get_clr(clr->child_top_bar), set->child_rounding, ImDrawFlags_RoundCornersTop);

    parent_window->DrawList->AddLine(c_pos + ImVec2(set->child_rounding, 1), c_pos + ImVec2(size.x - set->child_rounding, 1), IM_COL32(255, 255, 255, 20), 1.0f);

    parent_window->DrawList->AddLine(c_pos + ImVec2(0, 40), c_pos + ImVec2(size.x, 40), gui->get_clr(clr->child_stroke_bar), 1.0f);

    parent_window->DrawList->AddRectFilled(c_pos + ImVec2(0, 40), c_max, gui->get_clr(clr->child_bottom_bar), set->child_rounding, ImDrawFlags_RoundCornersBottom);

    parent_window->DrawList->AddRect(c_pos, c_max, gui->get_clr(clr->child_stroke_bar), set->child_rounding, 0, 1.0f);

    parent_window->DrawList->AddRectFilled(c_pos + ImVec2(12, 13), c_pos + ImVec2(15, 27), gui->get_clr(clr->accent_color, 0.9f), 2.0f);

    gui->render_text_clipped(set->icon_child, c_pos + ImVec2(18, 0), c_pos + ImVec2(48, 40), gui->get_clr(clr->accent_color), icon, { 0.5, 0.5 });
    gui->render_text_clipped(set->poppins_selection, c_pos + ImVec2(50, 0), c_pos + ImVec2(size.x - 16, 40), gui->get_clr(clr->text_active), name, { 0.0, 0.5 });

    const bool ret = Begin(temp_window_name, NULL, window_flags);

    if (child_flags & ImGuiChildFlags_FrameStyle)
    {
        PopStyleVar(3);
        PopStyleColor();
    }

    ImGuiWindow* child_window = g.CurrentWindow;
    child_window->ChildId = id;
    state = child_window->ContentSize.y + (GetStyle().WindowPadding.y * 2);

    if (child_window->BeginCount == 1) parent_window->DC.CursorPos = child_window->Pos;

    const ImGuiID temp_id_for_activation = ImHashStr("##Child", 0, id);
    if (g.ActiveId == temp_id_for_activation) ClearActiveID();
    if (g.NavActivateId == id && !(window_flags & ImGuiWindowFlags_NavFlattened) && (child_window->DC.NavLayersActiveMask != 0 || child_window->DC.NavWindowHasScrollY))
    {
        FocusWindow(child_window);
        NavInitWindow(child_window, false);
        SetActiveID(temp_id_for_activation, child_window);
        g.ActiveIdSource = g.NavInputSource;
    }
    return ret;
}

bool c_widgets::begin_child(std::string_view icon, std::string_view name, const ImVec2& size_arg)
{
    ImGuiID id = GetCurrentWindow()->GetID(name.data());
    PushStyleVar(ImGuiStyleVar_WindowPadding, set->widgets_padding);
    PushStyleVar(ImGuiStyleVar_ItemSpacing, set->widgets_spacing);
    return begin_child_ex(icon.data(), name.data(), id, size_arg, ImGuiChildFlags_None, ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
}

void c_widgets::end_child()
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* child_window = g.CurrentWindow;

    IM_ASSERT(g.WithinEndChild == false);
    IM_ASSERT(child_window->Flags & ImGuiWindowFlags_ChildWindow);

    g.WithinEndChild = true;
    ImVec2 child_size = child_window->Size;
    ImGui::End();
    if (child_window->BeginCount == 1)
    {
        ImGuiWindow* parent_window = g.CurrentWindow;
        ImVec2 total_card_size = ImVec2(child_size.x, child_size.y + 40.0f + set->child_spacing.y);
        ImRect bb(parent_window->DC.CursorPos, parent_window->DC.CursorPos + total_card_size);
        ItemSize(total_card_size);
        if ((child_window->DC.NavLayersActiveMask != 0 || child_window->DC.NavWindowHasScrollY) && !(child_window->Flags & ImGuiWindowFlags_NavFlattened))
        {
            ItemAdd(bb, child_window->ChildId);
            RenderNavHighlight(bb, child_window->ChildId);

            if (child_window->DC.NavLayersActiveMask == 0 && child_window == g.NavWindow) RenderNavHighlight(ImRect(bb.Min - ImVec2(2, 2), bb.Max + ImVec2(2, 2)), g.NavId, ImGuiNavHighlightFlags_Compact);
        }
        else
        {
            ItemAdd(bb, 0);
            if (child_window->Flags & ImGuiWindowFlags_NavFlattened) parent_window->DC.NavLayersActiveMaskNext |= child_window->DC.NavLayersActiveMaskNext;
        }
        if (g.HoveredWindow == child_window)
            g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_HoveredWindow;
    }
    PopStyleVar(2);
    g.WithinEndChild = false;
    g.LogLinePosY = -FLT_MAX;
}

bool c_widgets::checkbox(std::string_view label, bool* callback, bool popup, int* key, int* mode, bool* show_in_binds)
{
    struct checkbox_state
    {
        const char* mode_types[3] = { "Hold", "Toggle", "Always" };
        ImVec4 background = clr->element_background, text = clr->text;
        ImVec2 offset, pump;
        float alpha = 0.f, alpha_mark = 0.f, mark_size;
    };

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems) return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label.data());

    ImVec2 pos = window->DC.CursorPos;
    float width = GetContentRegionMax().x - g.Style.WindowPadding.x, box_size = 18, rect_size = 25;

    ImRect rect(pos, pos + ImVec2(width, rect_size));
    ImRect box(pos + ImVec2(width - box_size, (rect_size - box_size) / 2), pos + ImVec2(width, (rect_size + box_size) / 2));

    ItemSize(rect, 0);
    if (!ItemAdd(rect, id)) return false;

    bool hovered, held, pressed = ButtonBehavior(rect, id, &hovered, &held, ImGuiButtonFlags_PressedOnClick);

    static ImGuiID g_last_dragged_id = 0;
    if (g.IO.MouseDown[0] && hovered && g_last_dragged_id != id)
    {
        *callback = !(*callback);
        g_last_dragged_id = id;
        MarkItemEdited(id);
    }
    if (!g.IO.MouseDown[0])
    {
        g_last_dragged_id = 0;
    }

    static std::map<ImGuiID, checkbox_state> anim;
    checkbox_state& state = anim[id];

    state.background = ImLerp(state.background, *callback ? clr->accent_color : clr->element_background, gui->fixed_speed(12.f));

    state.offset = ImLerp(state.offset, *callback ? state.pump : ImVec2(box_size, box_size) / 2, gui->fixed_speed(12.f));
    state.pump = ImLerp(state.pump, state.alpha >= 0.99f ? ImVec2(0, 0) : -ImVec2(2, 2), gui->fixed_speed(16.f));

    state.alpha_mark = ImLerp(state.alpha_mark, *callback ? 1.f : 0.f, gui->fixed_speed(20.f));
    state.alpha = ImLerp(state.alpha, *callback ? 1.f : 0.f, gui->fixed_speed(10.f));
    state.text = ImLerp(state.text, *callback ? clr->text_active : clr->text, gui->fixed_speed(8.f));

    window->DrawList->AddRectFilled(box.Min, box.Max, gui->get_clr(clr->element_background), set->checkbox_rounding);
    window->DrawList->AddRectFilled(box.Min + state.offset, box.Max - state.offset, gui->get_clr(state.background), set->checkbox_rounding);
    RenderCheckMark(window->DrawList, box.Min + (ImVec2(box_size, box_size) / 2) / 2, gui->get_clr(clr->white_color, state.alpha_mark), box_size / 2);

    if (popup) {
        gui->begin_popup_w((std::stringstream{} << label << "keywindow").str().c_str(), 200);
        {
            if (key) keybind("Key", key);
            if (mode) {
                separator();
                gui->dropdown((std::stringstream{} << "Mode##" << label).str().c_str(), mode, state.mode_types, IM_ARRAYSIZE(state.mode_types), 3);
            }
            if (show_in_binds) {
                separator();
                gui->checkbox("Show In Binds", show_in_binds);
            }
        }
        gui->end_popup_w();

        gui->render_text_clipped(set->icon_child, rect.Min, rect.Max - ImVec2((box_size + 15), 0), gui->get_clr(clr->text), "C", { 1.0, 0.5 });
    }

    gui->render_text_clipped(set->poppins_widget, rect.Min, rect.Max - ImVec2(box_size + 10, 0), gui->get_clr(state.text), label.data(), {0.0, 0.5});

    return pressed;
}

bool c_widgets::checkbox_with_picker(std::string_view label, bool* callback, float col[3], ImGuiColorEditFlags flags)
{
    struct checkbox_state
    {
        ImVec4 background = clr->element_background, text = clr->text;
        ImVec2 offset, pump;
        float alpha = 0.f, alpha_mark = 0.f, mark_size;
    };

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems) return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label.data());

    const ImVec2 pos = window->DC.CursorPos;
    const float width = GetContentRegionMax().x - g.Style.WindowPadding.x, box_size = 18, rect_size = 25;

    const ImRect rect(pos, pos + ImVec2(width, rect_size));
    const ImRect box(pos + ImVec2(width - box_size, (rect_size - box_size) / 2), pos + ImVec2(width, (rect_size + box_size) / 2));

    ItemSize(rect, 0);
    if (!ItemAdd(rect, id)) return false;

    bool hovered, held, pressed = ButtonBehavior(rect, id, &hovered, &held, ImGuiButtonFlags_PressedOnClick);

    static ImGuiID g_last_picker_dragged_id = 0;
    if (g.IO.MouseDown[0] && hovered && g_last_picker_dragged_id != id)
    {
        *callback = !(*callback);
        g_last_picker_dragged_id = id;
        MarkItemEdited(id);
    }
    if (!g.IO.MouseDown[0])
    {
        g_last_picker_dragged_id = 0;
    }

    static std::map<ImGuiID, checkbox_state> anim;
    checkbox_state& state = anim[id];

    state.background = ImLerp(state.background, *callback ? clr->accent_color : clr->element_background, gui->fixed_speed(12.f));

    state.offset = ImLerp(state.offset, *callback ? state.pump : ImVec2(box_size, box_size) / 2, gui->fixed_speed(12.f));
    state.pump = ImLerp(state.pump, state.alpha >= 0.99f ? ImVec2(0, 0) : -ImVec2(1, 1), gui->fixed_speed(16.f));

    state.alpha_mark = ImLerp(state.alpha_mark, *callback ? 1.f : 0.f, gui->fixed_speed(20.f));
    state.alpha = ImLerp(state.alpha, *callback ? 1.f : 0.f, gui->fixed_speed(10.f));
    state.text = ImLerp(state.text, *callback ? clr->text_active : clr->text, gui->fixed_speed(8.f));

    window->DrawList->AddRectFilled(box.Min, box.Max, gui->get_clr(clr->element_background), set->checkbox_rounding);
    window->DrawList->AddRectFilled(box.Min + state.offset, box.Max - state.offset, gui->get_clr(state.background), set->checkbox_rounding);
    RenderCheckMark(window->DrawList, box.Min + (ImVec2(box_size, box_size) / 2) / 2, gui->get_clr(clr->white_color, state.alpha_mark), box_size / 2);

        gui->begin_popup_w((std::stringstream{} << label << "keywindow").str().c_str(), 210);
        {
            gui->color_picker("picker", col, flags, NULL);
        }
        gui->end_popup_w();

    gui->render_text_clipped(set->icon_child, rect.Min, rect.Max - ImVec2((box_size + 15), 0), gui->get_clr(ImColor(col[0], col[1], col[2], 1.f)), "C", {1.0, 0.5});
    gui->render_text_clipped(set->poppins_widget, rect.Min, rect.Max, gui->get_clr(state.text), label.data(), { 0.0, 0.5 });

    return pressed;
}

template<typename TYPE, typename SIGNEDTYPE, typename FLOATTYPE>
bool slider_behavior_t(const ImRect& bb, ImGuiID id, ImGuiDataType data_type, TYPE* v, const TYPE v_min, const TYPE v_max, const char* format, ImGuiSliderFlags flags, ImRect* out_grab_bb)
{
    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;

    const ImGuiAxis axis = (flags & ImGuiSliderFlags_Vertical) ? ImGuiAxis_Y : ImGuiAxis_X;
    const bool is_logarithmic = (flags & ImGuiSliderFlags_Logarithmic) != 0;
    const bool is_floating_point = (data_type == ImGuiDataType_Float) || (data_type == ImGuiDataType_Double);
    const float v_range_f = (float)(v_min < v_max ? v_max - v_min : v_min - v_max);

    const float grab_padding = 2.0f;
    const float slider_sz = (bb.Max[axis] - bb.Min[axis]) - grab_padding * 2.0f;
    float grab_sz = 13.f;
    const float slider_usable_sz = slider_sz - grab_sz;
    const float slider_usable_pos_min = bb.Min[axis] + grab_padding + grab_sz * 0.5f;
    const float slider_usable_pos_max = bb.Max[axis] - grab_padding - grab_sz * 0.5f;

    float logarithmic_zero_epsilon = 0.0f;
    float zero_deadzone_halfsize = 0.0f;
    if (is_logarithmic)
    {

        const int decimal_precision = is_floating_point ? ImParseFormatPrecision(format, 3) : 1;
        logarithmic_zero_epsilon = ImPow(0.1f, (float)decimal_precision);
        zero_deadzone_halfsize = (style.LogSliderDeadzone * 0.5f) / ImMax(slider_usable_sz, 1.0f);
    }

    bool value_changed = false;
    if (g.ActiveId == id)
    {
        bool set_new_value = false;
        float clicked_t = 0.0f;
        if (g.ActiveIdSource == ImGuiInputSource_Mouse)
        {
            if (!g.IO.MouseDown[0])
            {
                ClearActiveID();
            }
            else
            {
                const float mouse_abs_pos = g.IO.MousePos[axis];
                if (g.ActiveIdIsJustActivated)
                {
                    float grab_t = ScaleRatioFromValueT<TYPE, SIGNEDTYPE, FLOATTYPE>(data_type, *v, v_min, v_max, is_logarithmic, logarithmic_zero_epsilon, zero_deadzone_halfsize);
                    if (axis == ImGuiAxis_Y)
                        grab_t = 1.0f - grab_t;
                    const float grab_pos = ImLerp(slider_usable_pos_min, slider_usable_pos_max, grab_t);
                    const bool clicked_around_grab = (mouse_abs_pos >= grab_pos - grab_sz * 0.5f - 1.0f) && (mouse_abs_pos <= grab_pos + grab_sz * 0.5f + 1.0f);
                    g.SliderGrabClickOffset = (clicked_around_grab && is_floating_point) ? mouse_abs_pos - grab_pos : 0.0f;
                }
                if (slider_usable_sz > 0.0f)
                    clicked_t = ImSaturate((mouse_abs_pos - g.SliderGrabClickOffset - slider_usable_pos_min) / slider_usable_sz);
                if (axis == ImGuiAxis_Y)
                    clicked_t = 1.0f - clicked_t;
                set_new_value = true;
            }
        }
        else if (g.ActiveIdSource == ImGuiInputSource_Keyboard || g.ActiveIdSource == ImGuiInputSource_Gamepad)
        {
            if (g.ActiveIdIsJustActivated)
            {
                g.SliderCurrentAccum = 0.0f;
                g.SliderCurrentAccumDirty = false;
            }

            float input_delta = (axis == ImGuiAxis_X) ? GetNavTweakPressedAmount(axis) : -GetNavTweakPressedAmount(axis);
            if (input_delta != 0.0f)
            {
                const bool tweak_slow = IsKeyDown((g.NavInputSource == ImGuiInputSource_Gamepad) ? ImGuiKey_NavGamepadTweakSlow : ImGuiKey_NavKeyboardTweakSlow);
                const bool tweak_fast = IsKeyDown((g.NavInputSource == ImGuiInputSource_Gamepad) ? ImGuiKey_NavGamepadTweakFast : ImGuiKey_NavKeyboardTweakFast);
                const int decimal_precision = is_floating_point ? ImParseFormatPrecision(format, 3) : 0;
                if (decimal_precision > 0)
                {
                    input_delta /= 100.0f;
                    if (tweak_slow)
                        input_delta /= 10.0f;
                }
                else
                {
                    if ((v_range_f >= -100.0f && v_range_f <= 100.0f && v_range_f != 0.0f) || tweak_slow)
                        input_delta = ((input_delta < 0.0f) ? -1.0f : +1.0f) / v_range_f;
                    else
                        input_delta /= 100.0f;
                }
                if (tweak_fast)
                    input_delta *= 10.0f;

                g.SliderCurrentAccum += input_delta;
                g.SliderCurrentAccumDirty = true;
            }

            float delta = g.SliderCurrentAccum;
            if (g.NavActivatePressedId == id && !g.ActiveIdIsJustActivated)
            {
                ClearActiveID();
            }
            else if (g.SliderCurrentAccumDirty)
            {
                clicked_t = ScaleRatioFromValueT<TYPE, SIGNEDTYPE, FLOATTYPE>(data_type, *v, v_min, v_max, is_logarithmic, logarithmic_zero_epsilon, zero_deadzone_halfsize);

                if ((clicked_t >= 1.0f && delta > 0.0f) || (clicked_t <= 0.0f && delta < 0.0f))
                {
                    set_new_value = false;
                    g.SliderCurrentAccum = 0.0f;
                }
                else
                {
                    set_new_value = true;
                    float old_clicked_t = clicked_t;
                    clicked_t = ImSaturate(clicked_t + delta);

                    TYPE v_new = ScaleValueFromRatioT<TYPE, SIGNEDTYPE, FLOATTYPE>(data_type, clicked_t, v_min, v_max, is_logarithmic, logarithmic_zero_epsilon, zero_deadzone_halfsize);
                    if (is_floating_point && !(flags & ImGuiSliderFlags_NoRoundToFormat))
                        v_new = RoundScalarWithFormatT<TYPE>(format, data_type, v_new);
                    float new_clicked_t = ScaleRatioFromValueT<TYPE, SIGNEDTYPE, FLOATTYPE>(data_type, v_new, v_min, v_max, is_logarithmic, logarithmic_zero_epsilon, zero_deadzone_halfsize);

                    if (delta > 0)
                        g.SliderCurrentAccum -= ImMin(new_clicked_t - old_clicked_t, delta);
                    else
                        g.SliderCurrentAccum -= ImMax(new_clicked_t - old_clicked_t, delta);
                }

                g.SliderCurrentAccumDirty = false;
            }
        }

        if (set_new_value)
            if ((g.LastItemData.InFlags & ImGuiItemFlags_ReadOnly) || (flags & ImGuiSliderFlags_ReadOnly))
                set_new_value = false;

        if (set_new_value)
        {
            TYPE v_new = ScaleValueFromRatioT<TYPE, SIGNEDTYPE, FLOATTYPE>(data_type, clicked_t, v_min, v_max, is_logarithmic, logarithmic_zero_epsilon, zero_deadzone_halfsize);

            if (is_floating_point && !(flags & ImGuiSliderFlags_NoRoundToFormat))
                v_new = RoundScalarWithFormatT<TYPE>(format, data_type, v_new);

            if (*v != v_new)
            {
                *v = v_new;
                value_changed = true;
            }
        }
    }

    if (slider_sz < 1.0f)
    {
        *out_grab_bb = ImRect(bb.Min, bb.Min);
    }
    else
    {

        float grab_t = ScaleRatioFromValueT<TYPE, SIGNEDTYPE, FLOATTYPE>(data_type, *v, v_min, v_max, is_logarithmic, logarithmic_zero_epsilon, zero_deadzone_halfsize);
        if (axis == ImGuiAxis_Y)
            grab_t = 1.0f - grab_t;
        const float grab_pos = ImLerp(slider_usable_pos_min, slider_usable_pos_max, grab_t);
        if (axis == ImGuiAxis_X)
            *out_grab_bb = ImRect(grab_pos - grab_sz * 0.5f, bb.Min.y + grab_padding, grab_pos + grab_sz * 0.5f, bb.Max.y - grab_padding);
        else
            *out_grab_bb = ImRect(bb.Min.x + grab_padding, grab_pos - grab_sz * 0.5f, bb.Max.x - grab_padding, grab_pos + grab_sz * 0.5f);
    }

    return value_changed;
}

bool slider_behavior(const ImRect& bb, ImGuiID id, ImGuiDataType data_type, void* p_v, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags, ImRect* out_grab_bb)
{
    IM_ASSERT((flags == 1 || (flags & ImGuiSliderFlags_InvalidMask_) == 0) && "Invalid ImGuiSliderFlags flag!  Has the 'float power' argument been mistakenly cast to flags? Call function with ImGuiSliderFlags_Logarithmic flags instead.");

    switch (data_type)
    {
    case ImGuiDataType_S8: { ImS32 v32 = (ImS32) * (ImS8*)p_v;  bool r = slider_behavior_t<ImS32, ImS32, float>(bb, id, ImGuiDataType_S32, &v32, *(const ImS8*)p_min, *(const ImS8*)p_max, format, flags, out_grab_bb); if (r) *(ImS8*)p_v = (ImS8)v32;  return r; }
    case ImGuiDataType_U8: { ImU32 v32 = (ImU32) * (ImU8*)p_v;  bool r = slider_behavior_t<ImU32, ImS32, float>(bb, id, ImGuiDataType_U32, &v32, *(const ImU8*)p_min, *(const ImU8*)p_max, format, flags, out_grab_bb); if (r) *(ImU8*)p_v = (ImU8)v32;  return r; }
    case ImGuiDataType_S16: { ImS32 v32 = (ImS32) * (ImS16*)p_v; bool r = slider_behavior_t<ImS32, ImS32, float>(bb, id, ImGuiDataType_S32, &v32, *(const ImS16*)p_min, *(const ImS16*)p_max, format, flags, out_grab_bb); if (r) *(ImS16*)p_v = (ImS16)v32; return r; }
    case ImGuiDataType_U16: { ImU32 v32 = (ImU32) * (ImU16*)p_v; bool r = slider_behavior_t<ImU32, ImS32, float>(bb, id, ImGuiDataType_U32, &v32, *(const ImU16*)p_min, *(const ImU16*)p_max, format, flags, out_grab_bb); if (r) *(ImU16*)p_v = (ImU16)v32; return r; }
    case ImGuiDataType_S32:
        IM_ASSERT(*(const ImS32*)p_min >= INT_MIN / 2 && *(const ImS32*)p_max <= INT_MAX / 2);
        return slider_behavior_t<ImS32, ImS32, float >(bb, id, data_type, (ImS32*)p_v, *(const ImS32*)p_min, *(const ImS32*)p_max, format, flags, out_grab_bb);
    case ImGuiDataType_U32:
        IM_ASSERT(*(const ImU32*)p_max <= UINT_MAX / 2);
        return slider_behavior_t<ImU32, ImS32, float >(bb, id, data_type, (ImU32*)p_v, *(const ImU32*)p_min, *(const ImU32*)p_max, format, flags, out_grab_bb);
    case ImGuiDataType_S64:
        IM_ASSERT(*(const ImS64*)p_min >= LLONG_MIN / 2 && *(const ImS64*)p_max <= LLONG_MAX / 2);
        return slider_behavior_t<ImS64, ImS64, double>(bb, id, data_type, (ImS64*)p_v, *(const ImS64*)p_min, *(const ImS64*)p_max, format, flags, out_grab_bb);
    case ImGuiDataType_U64:
        IM_ASSERT(*(const ImU64*)p_max <= ULLONG_MAX / 2);
        return slider_behavior_t<ImU64, ImS64, double>(bb, id, data_type, (ImU64*)p_v, *(const ImU64*)p_min, *(const ImU64*)p_max, format, flags, out_grab_bb);
    case ImGuiDataType_Float:
        IM_ASSERT(*(const float*)p_min >= -FLT_MAX / 2.0f && *(const float*)p_max <= FLT_MAX / 2.0f);
        return slider_behavior_t<float, float, float >(bb, id, data_type, (float*)p_v, *(const float*)p_min, *(const float*)p_max, format, flags, out_grab_bb);
    case ImGuiDataType_Double:
        IM_ASSERT(*(const double*)p_min >= -DBL_MAX / 2.0f && *(const double*)p_max <= DBL_MAX / 2.0f);
        return slider_behavior_t<double, double, double>(bb, id, data_type, (double*)p_v, *(const double*)p_min, *(const double*)p_max, format, flags, out_grab_bb);
    case ImGuiDataType_COUNT: break;
    }
    IM_ASSERT(0);
    return false;
}

bool slider_scalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags)
{
    struct slider_state
    {
        ImVec4 text = clr->text;
        float grab_slide = 0.f, grab_offset = 0.f, circle = 6.f;
    };

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems) return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);

    const float width = GetContentRegionMax().x - g.Style.WindowPadding.x;
    const float rect_size = 25;
    const ImVec2 pos = window->DC.CursorPos;

    const float track_w = 90.0f;
    const float value_w = 48.0f;
    const float gap = 8.0f;

    const ImRect rect(pos, pos + ImVec2(width, rect_size));
    const ImRect slider(pos + ImVec2(width - track_w, (rect_size / 2) - 1.5), pos + ImVec2(width, (rect_size / 2) + 1.5));
    const ImRect clickable(pos + ImVec2(width - track_w, 0), pos + ImVec2(width, rect_size));

    const bool temp_input_allowed = (flags & ImGuiSliderFlags_NoInput) == 0;

    ItemSize(rect, 0);
    if (!ItemAdd(rect, id, &clickable, temp_input_allowed ? ImGuiItemFlags_Inputable : 0)) return false;

    if (format == NULL) format = DataTypeGetInfo(data_type)->PrintFmt;

    bool hovered = ItemHoverable(clickable, id, g.LastItemData.InFlags), held, pressed = ButtonBehavior(clickable, id, &hovered, &held, NULL);
    bool temp_input_is_active = temp_input_allowed && TempInputIsActive(id);

    ImRect grab_bb;
    const bool value_changed = slider_behavior(ImRect(clickable.Min - ImVec2(3, 0), clickable.Max + ImVec2(2, 0)), id, data_type, p_data, p_min, p_max, format, flags, &grab_bb);
    if (value_changed) MarkItemEdited(id);

    char value_buf[64];
    const char* value_buf_end = value_buf + DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, p_data, format);

    static std::map<ImGuiID, slider_state> anim;
    slider_state& state = anim[id];

    state.circle = ImLerp(state.circle, IsItemActive() ? 6.5f : 6.f, gui->fixed_speed(8.f));
    state.text = ImLerp(state.text, IsItemActive() ? clr->text_active : clr->text, gui->fixed_speed(8.f));
    state.grab_slide = ImLerp(state.grab_slide, grab_bb.Min.x - clickable.Min.x, gui->fixed_speed(20.f));

    window->DrawList->AddRectFilled(slider.Min, slider.Max, gui->get_clr(clr->element_background));
    window->DrawList->AddRectFilled(slider.Min, ImVec2(slider.Min.x + state.grab_slide + 5, slider.Max.y), gui->get_clr(clr->accent_color));

    window->DrawList->AddCircleFilled(slider.Min + ImVec2(state.grab_slide + 7, 1), state.circle, gui->get_clr(clr->accent_color), 32);

    gui->render_text_clipped(set->poppins_widget, rect.Min, pos + ImVec2(width - track_w - value_w - gap * 2, rect_size), gui->get_clr(state.text), label, { 0.0, 0.5 });
    gui->render_text_clipped(set->poppins_widget, pos + ImVec2(width - track_w - value_w - gap, 0), pos + ImVec2(width - track_w - gap, rect_size), gui->get_clr(clr->text_active), value_buf, { 1.0, 0.5 });

    return value_changed;
}

bool c_widgets::slider_float(std::string_view label, float* v, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
    return slider_scalar(label.data(), ImGuiDataType_Float, v, &v_min, &v_max, format, flags);
}

bool c_widgets::slider_int(std::string_view label, int* v, int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
{
    return slider_scalar(label.data(), ImGuiDataType_S32, v, &v_min, &v_max, format, flags);
}

static bool input_text_filter_character(ImGuiContext* ctx, unsigned int* p_char, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data, ImGuiInputSource input_source)
{
    IM_ASSERT(input_source == ImGuiInputSource_Keyboard || input_source == ImGuiInputSource_Clipboard);
    unsigned int c = *p_char;

    bool apply_named_filters = true;
    if (c < 0x20)
    {
        bool pass = false;
        pass |= (c == '\n') && (flags & ImGuiInputTextFlags_Multiline) != 0;
        pass |= (c == '\t') && (flags & ImGuiInputTextFlags_AllowTabInput) != 0;
        if (!pass)
            return false;
        apply_named_filters = false;
    }

    if (input_source != ImGuiInputSource_Clipboard)
    {

        if (c == 127)
            return false;

        if (c >= 0xE000 && c <= 0xF8FF)
            return false;
    }

    if (c > IM_UNICODE_CODEPOINT_MAX)
        return false;

    if (apply_named_filters && (flags & (ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_CharsScientific)))
    {

        ImGuiContext& g = *ctx;
        const unsigned c_decimal_point = (unsigned int)g.IO.PlatformLocaleDecimalPoint;
        if (flags & (ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsScientific))
            if (c == '.' || c == ',')
                c = c_decimal_point;

        if (flags & (ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsScientific | ImGuiInputTextFlags_CharsHexadecimal))
            if (c >= 0xFF01 && c <= 0xFF5E)
                c = c - 0xFF01 + 0x21;

        if (flags & ImGuiInputTextFlags_CharsDecimal)
            if (!(c >= '0' && c <= '9') && (c != c_decimal_point) && (c != '-') && (c != '+') && (c != '*') && (c != '/'))
                return false;

        if (flags & ImGuiInputTextFlags_CharsScientific)
            if (!(c >= '0' && c <= '9') && (c != c_decimal_point) && (c != '-') && (c != '+') && (c != '*') && (c != '/') && (c != 'e') && (c != 'E'))
                return false;

        if (flags & ImGuiInputTextFlags_CharsHexadecimal)
            if (!(c >= '0' && c <= '9') && !(c >= 'a' && c <= 'f') && !(c >= 'A' && c <= 'F'))
                return false;

        if (flags & ImGuiInputTextFlags_CharsUppercase)
            if (c >= 'a' && c <= 'z')
                c += (unsigned int)('A' - 'a');

        if (flags & ImGuiInputTextFlags_CharsNoBlank)
            if (ImCharIsBlankW(c))
                return false;

        *p_char = c;
    }

    if (flags & ImGuiInputTextFlags_CallbackCharFilter)
    {
        ImGuiContext& g = *GImGui;
        ImGuiInputTextCallbackData callback_data;
        callback_data.Ctx = &g;
        callback_data.EventFlag = ImGuiInputTextFlags_CallbackCharFilter;
        callback_data.EventChar = (ImWchar)c;
        callback_data.Flags = flags;
        callback_data.UserData = user_data;
        if (callback(&callback_data) != 0)
            return false;
        *p_char = callback_data.EventChar;
        if (!callback_data.EventChar)
            return false;
    }

    return true;
}

static int input_text_calc_text_len_and_line_count(const char* text_begin, const char** out_text_end)
{
    int line_count = 0;
    const char* s = text_begin;
    while (char c = *s++)
        if (c == '\n')
            line_count++;
    s--;
    if (s[0] != '\n' && s[0] != '\r')
        line_count++;
    *out_text_end = s;
    return line_count;
}

static void input_text_reconcile_undo_state_after_user_callback(ImGuiInputTextState* state, const char* new_buf_a, int new_length_a)
{
    ImGuiContext& g = *GImGui;
    const ImWchar* old_buf = state->TextW.Data;
    const int old_length = state->CurLenW;
    const int new_length = ImTextCountCharsFromUtf8(new_buf_a, new_buf_a + new_length_a);
    g.TempBuffer.reserve_discard((new_length + 1) * sizeof(ImWchar));
    ImWchar* new_buf = (ImWchar*)(void*)g.TempBuffer.Data;
    ImTextStrFromUtf8(new_buf, new_length + 1, new_buf_a, new_buf_a + new_length_a);

    const int shorter_length = ImMin(old_length, new_length);
    int first_diff;
    for (first_diff = 0; first_diff < shorter_length; first_diff++)
        if (old_buf[first_diff] != new_buf[first_diff])
            break;
    if (first_diff == old_length && first_diff == new_length)
        return;

    int old_last_diff = old_length - 1;
    int new_last_diff = new_length - 1;
    for (; old_last_diff >= first_diff && new_last_diff >= first_diff; old_last_diff--, new_last_diff--)
        if (old_buf[old_last_diff] != new_buf[new_last_diff])
            break;

    const int insert_len = new_last_diff - first_diff + 1;
    const int delete_len = old_last_diff - first_diff + 1;
    if (insert_len > 0 || delete_len > 0)
        if (IMSTB_TEXTEDIT_CHARTYPE* p = stb_text_createundo(&state->Stb.undostate, first_diff, delete_len, insert_len))
            for (int i = 0; i < delete_len; i++)
                p[i] = ImStb::STB_TEXTEDIT_GETCHAR(state, first_diff + i);
}

bool input_text_ex(const char* label, const char* hint, char* buf, int buf_size, const ImVec2& size_arg, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* callback_user_data)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems) return false;

    IM_ASSERT(buf != NULL && buf_size >= 0);
    IM_ASSERT(!((flags & ImGuiInputTextFlags_CallbackHistory) && (flags & ImGuiInputTextFlags_Multiline)));
    IM_ASSERT(!((flags & ImGuiInputTextFlags_CallbackCompletion) && (flags & ImGuiInputTextFlags_AllowTabInput)));

    ImGuiContext& g = *GImGui;
    ImGuiIO& io = g.IO;
    const ImGuiStyle& style = g.Style;

    const bool RENDER_SELECTION_WHEN_INACTIVE = false;
    const bool is_multiline = (flags & ImGuiInputTextFlags_Multiline) != 0;

    if (is_multiline) BeginGroup();
    const ImGuiID id = window->GetID(label);

    const ImVec2 pos = window->DC.CursorPos;
    const float width = GetContentRegionMax().x - g.Style.WindowPadding.x;

    const ImRect rect(pos, pos + size_arg);
    const ImRect clickable(pos, pos + size_arg);

    ImGuiWindow* draw_window = window;
    ImVec2 inner_size = clickable.GetSize();
    ImGuiLastItemData item_data_backup;
    if (is_multiline)
    {
        ImVec2 backup_pos = window->DC.CursorPos;
        ItemSize(rect, style.FramePadding.y);
        if (!ItemAdd(rect, id, &clickable, ImGuiItemFlags_Inputable))
        {
            EndGroup();
            return false;
        }
        item_data_backup = g.LastItemData;
        window->DC.CursorPos = backup_pos;

        if (g.NavActivateId == id && (g.NavActivateFlags & ImGuiActivateFlags_FromTabbing) && (flags & ImGuiInputTextFlags_AllowTabInput))
            g.NavActivateId = 0;

        const ImGuiID backup_activate_id = g.NavActivateId;
        if (g.ActiveId == id)
            g.NavActivateId = 0;

        PushStyleColor(ImGuiCol_ChildBg, style.Colors[ImGuiCol_FrameBg]);
        PushStyleVar(ImGuiStyleVar_ChildRounding, style.FrameRounding);
        PushStyleVar(ImGuiStyleVar_ChildBorderSize, style.FrameBorderSize);
        PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        bool child_visible = BeginChildEx(label, id, clickable.GetSize(), true, ImGuiWindowFlags_NoMove);
        g.NavActivateId = backup_activate_id;
        PopStyleVar(3);
        PopStyleColor();
        if (!child_visible)
        {
            EndChild();
            EndGroup();
            return false;
        }
        draw_window = g.CurrentWindow;
        draw_window->DC.NavLayersActiveMaskNext |= (1 << draw_window->DC.NavLayerCurrent);
        draw_window->DC.CursorPos += style.FramePadding;
        inner_size.x -= draw_window->ScrollbarSizes.x;
    }
    else
    {

        ItemSize(rect, style.FramePadding.y);
        if (!(flags & ImGuiInputTextFlags_MergedItem))
            if (!ItemAdd(rect, id, &clickable, ImGuiItemFlags_Inputable)) return false;
    }
    const bool hovered = ItemHoverable(clickable, id, g.LastItemData.InFlags);

    PushFont(set->poppins_widget);

    ImGuiInputTextState* state = GetInputTextState(id);

    if (g.LastItemData.InFlags & ImGuiItemFlags_ReadOnly)
        flags |= ImGuiInputTextFlags_ReadOnly;
    const bool is_readonly = (flags & ImGuiInputTextFlags_ReadOnly) != 0;
    const bool is_password = (flags & ImGuiInputTextFlags_Password) != 0;
    const bool is_undoable = (flags & ImGuiInputTextFlags_NoUndoRedo) == 0;
    const bool is_resizable = (flags & ImGuiInputTextFlags_CallbackResize) != 0;
    if (is_resizable)
        IM_ASSERT(callback != NULL);

    const bool input_requested_by_nav = (g.ActiveId != id) && ((g.NavActivateId == id) && ((g.NavActivateFlags & ImGuiActivateFlags_PreferInput) || (g.NavInputSource == ImGuiInputSource_Keyboard)));

    const bool user_clicked = hovered && io.MouseClicked[0];
    const bool user_scroll_finish = is_multiline && state != NULL && g.ActiveId == 0 && g.ActiveIdPreviousFrame == GetWindowScrollbarID(draw_window, ImGuiAxis_Y);
    const bool user_scroll_active = is_multiline && state != NULL && g.ActiveId == GetWindowScrollbarID(draw_window, ImGuiAxis_Y);
    bool clear_active_id = false;
    bool select_all = false;

    float scroll_y = is_multiline ? draw_window->Scroll.y : FLT_MAX;

    const bool init_reload_from_user_buf = (state != NULL && state->ReloadUserBuf);
    const bool init_changed_specs = (state != NULL && state->Stb.single_line != !is_multiline);
    const bool init_make_active = (user_clicked || user_scroll_finish || input_requested_by_nav);
    const bool init_state = (init_make_active || user_scroll_active);
    if ((init_state && g.ActiveId != id) || init_changed_specs || init_reload_from_user_buf)
    {

        state = &g.InputTextState;
        state->CursorAnimReset();
        state->ReloadUserBuf = false;

        InputTextDeactivateHook(state->ID);

        const int buf_len = (int)strlen(buf);
        if (!init_reload_from_user_buf)
        {

            state->InitialTextA.resize(buf_len + 1);
            memcpy(state->InitialTextA.Data, buf, buf_len + 1);
        }

        bool recycle_state = (state->ID == id && !init_changed_specs && !init_reload_from_user_buf);
        if (recycle_state && (state->CurLenA != buf_len || (state->TextAIsValid && strncmp(state->TextA.Data, buf, buf_len) != 0)))
            recycle_state = false;

        const char* buf_end = NULL;
        state->ID = id;
        state->TextW.resize(buf_size + 1);
        state->TextA.resize(0);
        state->TextAIsValid = false;
        state->CurLenW = ImTextStrFromUtf8(state->TextW.Data, buf_size, buf, NULL, &buf_end);
        state->CurLenA = (int)(buf_end - buf);

        if (recycle_state)
        {

            state->CursorClamp();
        }
        else
        {
            state->ScrollX = 0.0f;
            stb_textedit_initialize_state(&state->Stb, !is_multiline);
        }

        if (init_reload_from_user_buf)
        {
            state->Stb.select_start = state->ReloadSelectionStart;
            state->Stb.cursor = state->Stb.select_end = state->ReloadSelectionEnd;
            state->CursorClamp();
        }
        else if (!is_multiline)
        {
            if (flags & ImGuiInputTextFlags_AutoSelectAll)
                select_all = true;
            if (input_requested_by_nav && (!recycle_state || !(g.NavActivateFlags & ImGuiActivateFlags_TryToPreserveState)))
                select_all = true;
            if (user_clicked && io.KeyCtrl)
                select_all = true;
        }

        if (flags & ImGuiInputTextFlags_AlwaysOverwrite)
            state->Stb.insert_mode = 1;
    }

    const bool is_osx = io.ConfigMacOSXBehaviors;
    if (g.ActiveId != id && init_make_active)
    {
        IM_ASSERT(state && state->ID == id);
        SetActiveID(id, window);
        SetFocusID(id, window);
        FocusWindow(window);
    }
    if (g.ActiveId == id)
    {

        if (user_clicked)
            SetKeyOwner(ImGuiKey_MouseLeft, id);
        g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
        if (is_multiline || (flags & ImGuiInputTextFlags_CallbackHistory))
            g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Up) | (1 << ImGuiDir_Down);
        SetKeyOwner(ImGuiKey_Enter, id);
        SetKeyOwner(ImGuiKey_KeypadEnter, id);
        SetKeyOwner(ImGuiKey_Home, id);
        SetKeyOwner(ImGuiKey_End, id);
        if (is_multiline)
        {
            SetKeyOwner(ImGuiKey_PageUp, id);
            SetKeyOwner(ImGuiKey_PageDown, id);
        }
        if (is_osx)
            SetKeyOwner(ImGuiMod_Alt, id);
    }

    if (g.ActiveId == id && state == NULL)
        ClearActiveID();

    if (g.ActiveId == id && io.MouseClicked[0] && !init_state && !init_make_active)
        clear_active_id = true;

    bool render_cursor = (g.ActiveId == id) || (state && user_scroll_active);
    bool render_selection = state && (state->HasSelection() || select_all) && (RENDER_SELECTION_WHEN_INACTIVE || render_cursor);
    bool value_changed = false;
    bool validated = false;

    if (is_readonly && state != NULL && (render_cursor || render_selection))
    {
        const char* buf_end = NULL;
        state->TextW.resize(buf_size + 1);
        state->CurLenW = ImTextStrFromUtf8(state->TextW.Data, state->TextW.Size, buf, NULL, &buf_end);
        state->CurLenA = (int)(buf_end - buf);
        state->CursorClamp();
        render_selection &= state->HasSelection();
    }

    const bool buf_display_from_state = (render_cursor || render_selection || g.ActiveId == id) && !is_readonly && state && state->TextAIsValid;
    const bool is_displaying_hint = (hint != NULL && (buf_display_from_state ? state->TextA.Data : buf)[0] == 0);

    if (is_password && !is_displaying_hint)
    {
        const ImFontGlyph* glyph = g.Font->FindGlyph('*');
        ImFont* password_font = &g.InputTextPasswordFont;
        password_font->FontSize = g.Font->FontSize;
        password_font->Scale = g.Font->Scale;
        password_font->Ascent = g.Font->Ascent;
        password_font->Descent = g.Font->Descent;
        password_font->ContainerAtlas = g.Font->ContainerAtlas;
        password_font->FallbackGlyph = glyph;
        password_font->FallbackAdvanceX = glyph->AdvanceX;
        IM_ASSERT(password_font->Glyphs.empty() && password_font->IndexAdvanceX.empty() && password_font->IndexLookup.empty());
        PushFont(password_font);
    }

    int backup_current_text_length = 0;
    if (g.ActiveId == id)
    {
        IM_ASSERT(state != NULL);
        backup_current_text_length = state->CurLenA;
        state->Edited = false;
        state->BufCapacityA = buf_size;
        state->Flags = flags;

        g.ActiveIdAllowOverlap = !io.MouseDown[0];

        const float mouse_x = (io.MousePos.x - clickable.Min.x - style.FramePadding.x) + state->ScrollX;
        const float mouse_y = (is_multiline ? (io.MousePos.y - draw_window->DC.CursorPos.y) : (g.FontSize * 0.5f));

        if (select_all)
        {
            state->SelectAll();
            state->SelectedAllMouseLock = true;
        }
        else if (hovered && io.MouseClickedCount[0] >= 2 && !io.KeyShift)
        {
            stb_textedit_click(state, &state->Stb, mouse_x, mouse_y);
            const int multiclick_count = (io.MouseClickedCount[0] - 2);
            if ((multiclick_count % 2) == 0)
            {

                const bool is_bol = (state->Stb.cursor == 0) || ImStb::STB_TEXTEDIT_GETCHAR(state, state->Stb.cursor - 1) == '\n';
                if (STB_TEXT_HAS_SELECTION(&state->Stb) || !is_bol)
                    state->OnKeyPressed(STB_TEXTEDIT_K_WORDLEFT);

                if (!STB_TEXT_HAS_SELECTION(&state->Stb))
                    ImStb::stb_textedit_prep_selection_at_cursor(&state->Stb);
                state->Stb.cursor = ImStb::STB_TEXTEDIT_MOVEWORDRIGHT_MAC(state, state->Stb.cursor);
                state->Stb.select_end = state->Stb.cursor;
                ImStb::stb_textedit_clamp(state, &state->Stb);
            }
            else
            {

                const bool is_eol = ImStb::STB_TEXTEDIT_GETCHAR(state, state->Stb.cursor) == '\n';
                state->OnKeyPressed(STB_TEXTEDIT_K_LINESTART);
                state->OnKeyPressed(STB_TEXTEDIT_K_LINEEND | STB_TEXTEDIT_K_SHIFT);
                state->OnKeyPressed(STB_TEXTEDIT_K_RIGHT | STB_TEXTEDIT_K_SHIFT);
                if (!is_eol && is_multiline)
                {
                    ImSwap(state->Stb.select_start, state->Stb.select_end);
                    state->Stb.cursor = state->Stb.select_end;
                }
                state->CursorFollow = false;
            }
            state->CursorAnimReset();
        }
        else if (io.MouseClicked[0] && !state->SelectedAllMouseLock)
        {
            if (hovered)
            {
                if (io.KeyShift)
                    stb_textedit_drag(state, &state->Stb, mouse_x, mouse_y);
                else
                    stb_textedit_click(state, &state->Stb, mouse_x, mouse_y);
                state->CursorAnimReset();
            }
        }
        else if (io.MouseDown[0] && !state->SelectedAllMouseLock && (io.MouseDelta.x != 0.0f || io.MouseDelta.y != 0.0f))
        {
            stb_textedit_drag(state, &state->Stb, mouse_x, mouse_y);
            state->CursorAnimReset();
            state->CursorFollow = true;
        }
        if (state->SelectedAllMouseLock && !io.MouseDown[0])
            state->SelectedAllMouseLock = false;

        if ((flags & ImGuiInputTextFlags_AllowTabInput) && !is_readonly)
        {
            if (Shortcut(ImGuiKey_Tab, id, ImGuiInputFlags_Repeat))
            {
                unsigned int c = '\t';
                if (input_text_filter_character(&g, &c, flags, callback, callback_user_data, ImGuiInputSource_Keyboard))
                    state->OnKeyPressed((int)c);
            }

        }

        const bool ignore_char_inputs = (io.KeyCtrl && !io.KeyAlt) || (is_osx && io.KeySuper);
        if (io.InputQueueCharacters.Size > 0)
        {
            if (!ignore_char_inputs && !is_readonly && !input_requested_by_nav)
                for (int n = 0; n < io.InputQueueCharacters.Size; n++)
                {

                    unsigned int c = (unsigned int)io.InputQueueCharacters[n];
                    if (c == '\t')
                        continue;
                    if (input_text_filter_character(&g, &c, flags, callback, callback_user_data, ImGuiInputSource_Keyboard))
                        state->OnKeyPressed((int)c);
                }

            io.InputQueueCharacters.resize(0);
        }
    }

    bool revert_edit = false;
    if (g.ActiveId == id && !g.ActiveIdIsJustActivated && !clear_active_id)
    {
        IM_ASSERT(state != NULL);

        const int row_count_per_page = ImMax((int)((inner_size.y - style.FramePadding.y) / g.FontSize), 1);
        state->Stb.row_count_per_page = row_count_per_page;

        const int k_mask = (io.KeyShift ? STB_TEXTEDIT_K_SHIFT : 0);
        const bool is_wordmove_key_down = is_osx ? io.KeyAlt : io.KeyCtrl;
        const bool is_startend_key_down = is_osx && io.KeySuper && !io.KeyCtrl && !io.KeyAlt;

        const ImGuiInputFlags f_repeat = ImGuiInputFlags_Repeat;
        const bool is_cut = (Shortcut(ImGuiMod_Shortcut | ImGuiKey_X, id, f_repeat) || Shortcut(ImGuiMod_Shift | ImGuiKey_Delete, id, f_repeat)) && !is_readonly && !is_password && (!is_multiline || state->HasSelection());
        const bool is_copy = (Shortcut(ImGuiMod_Shortcut | ImGuiKey_C, id) || Shortcut(ImGuiMod_Ctrl | ImGuiKey_Insert, id)) && !is_password && (!is_multiline || state->HasSelection());
        const bool is_paste = (Shortcut(ImGuiMod_Shortcut | ImGuiKey_V, id, f_repeat) || Shortcut(ImGuiMod_Shift | ImGuiKey_Insert, id, f_repeat)) && !is_readonly;
        const bool is_undo = (Shortcut(ImGuiMod_Shortcut | ImGuiKey_Z, id, f_repeat)) && !is_readonly && is_undoable;
        const bool is_redo = (Shortcut(ImGuiMod_Shortcut | ImGuiKey_Y, id, f_repeat) || (is_osx && Shortcut(ImGuiMod_Shortcut | ImGuiMod_Shift | ImGuiKey_Z, id, f_repeat))) && !is_readonly && is_undoable;
        const bool is_select_all = Shortcut(ImGuiMod_Shortcut | ImGuiKey_A, id);

        const bool nav_gamepad_active = (io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad) != 0 && (io.BackendFlags & ImGuiBackendFlags_HasGamepad) != 0;
        const bool is_enter_pressed = IsKeyPressed(ImGuiKey_Enter, true) || IsKeyPressed(ImGuiKey_KeypadEnter, true);
        const bool is_gamepad_validate = nav_gamepad_active && (IsKeyPressed(ImGuiKey_NavGamepadActivate, false) || IsKeyPressed(ImGuiKey_NavGamepadInput, false));
        const bool is_cancel = Shortcut(ImGuiKey_Escape, id, f_repeat) || (nav_gamepad_active && Shortcut(ImGuiKey_NavGamepadCancel, id, f_repeat));

        if (IsKeyPressed(ImGuiKey_LeftArrow)) { state->OnKeyPressed((is_startend_key_down ? STB_TEXTEDIT_K_LINESTART : is_wordmove_key_down ? STB_TEXTEDIT_K_WORDLEFT : STB_TEXTEDIT_K_LEFT) | k_mask); }
        else if (IsKeyPressed(ImGuiKey_RightArrow)) { state->OnKeyPressed((is_startend_key_down ? STB_TEXTEDIT_K_LINEEND : is_wordmove_key_down ? STB_TEXTEDIT_K_WORDRIGHT : STB_TEXTEDIT_K_RIGHT) | k_mask); }
        else if (IsKeyPressed(ImGuiKey_UpArrow) && is_multiline) { if (io.KeyCtrl) SetScrollY(draw_window, ImMax(draw_window->Scroll.y - g.FontSize, 0.0f)); else state->OnKeyPressed((is_startend_key_down ? STB_TEXTEDIT_K_TEXTSTART : STB_TEXTEDIT_K_UP) | k_mask); }
        else if (IsKeyPressed(ImGuiKey_DownArrow) && is_multiline) { if (io.KeyCtrl) SetScrollY(draw_window, ImMin(draw_window->Scroll.y + g.FontSize, GetScrollMaxY())); else state->OnKeyPressed((is_startend_key_down ? STB_TEXTEDIT_K_TEXTEND : STB_TEXTEDIT_K_DOWN) | k_mask); }
        else if (IsKeyPressed(ImGuiKey_PageUp) && is_multiline) { state->OnKeyPressed(STB_TEXTEDIT_K_PGUP | k_mask); scroll_y -= row_count_per_page * g.FontSize; }
        else if (IsKeyPressed(ImGuiKey_PageDown) && is_multiline) { state->OnKeyPressed(STB_TEXTEDIT_K_PGDOWN | k_mask); scroll_y += row_count_per_page * g.FontSize; }
        else if (IsKeyPressed(ImGuiKey_Home)) { state->OnKeyPressed(io.KeyCtrl ? STB_TEXTEDIT_K_TEXTSTART | k_mask : STB_TEXTEDIT_K_LINESTART | k_mask); }
        else if (IsKeyPressed(ImGuiKey_End)) { state->OnKeyPressed(io.KeyCtrl ? STB_TEXTEDIT_K_TEXTEND | k_mask : STB_TEXTEDIT_K_LINEEND | k_mask); }
        else if (IsKeyPressed(ImGuiKey_Delete) && !is_readonly && !is_cut)
        {
            if (!state->HasSelection())
            {

                if (is_wordmove_key_down)
                    state->OnKeyPressed(STB_TEXTEDIT_K_WORDRIGHT | STB_TEXTEDIT_K_SHIFT);
            }
            state->OnKeyPressed(STB_TEXTEDIT_K_DELETE | k_mask);
        }
        else if (IsKeyPressed(ImGuiKey_Backspace) && !is_readonly)
        {
            if (!state->HasSelection())
            {
                if (is_wordmove_key_down)
                    state->OnKeyPressed(STB_TEXTEDIT_K_WORDLEFT | STB_TEXTEDIT_K_SHIFT);
                else if (is_osx && io.KeySuper && !io.KeyAlt && !io.KeyCtrl)
                    state->OnKeyPressed(STB_TEXTEDIT_K_LINESTART | STB_TEXTEDIT_K_SHIFT);
            }
            state->OnKeyPressed(STB_TEXTEDIT_K_BACKSPACE | k_mask);
        }
        else if (is_enter_pressed || is_gamepad_validate)
        {

            bool ctrl_enter_for_new_line = (flags & ImGuiInputTextFlags_CtrlEnterForNewLine) != 0;
            if (!is_multiline || is_gamepad_validate || (ctrl_enter_for_new_line && !io.KeyCtrl) || (!ctrl_enter_for_new_line && io.KeyCtrl))
            {
                validated = true;
                if (io.ConfigInputTextEnterKeepActive && !is_multiline)
                    state->SelectAll();
                else
                    clear_active_id = true;
            }
            else if (!is_readonly)
            {
                unsigned int c = '\n';
                if (input_text_filter_character(&g, &c, flags, callback, callback_user_data, ImGuiInputSource_Keyboard))
                    state->OnKeyPressed((int)c);
            }
        }
        else if (is_cancel)
        {
            if (flags & ImGuiInputTextFlags_EscapeClearsAll)
            {
                if (buf[0] != 0)
                {
                    revert_edit = true;
                }
                else
                {
                    render_cursor = render_selection = false;
                    clear_active_id = true;
                }
            }
            else
            {
                clear_active_id = revert_edit = true;
                render_cursor = render_selection = false;
            }
        }
        else if (is_undo || is_redo)
        {
            state->OnKeyPressed(is_undo ? STB_TEXTEDIT_K_UNDO : STB_TEXTEDIT_K_REDO);
            state->ClearSelection();
        }
        else if (is_select_all)
        {
            state->SelectAll();
            state->CursorFollow = true;
        }
        else if (is_cut || is_copy)
        {

            if (io.SetClipboardTextFn)
            {
                const int ib = state->HasSelection() ? ImMin(state->Stb.select_start, state->Stb.select_end) : 0;
                const int ie = state->HasSelection() ? ImMax(state->Stb.select_start, state->Stb.select_end) : state->CurLenW;
                const int clipboard_data_len = ImTextCountUtf8BytesFromStr(state->TextW.Data + ib, state->TextW.Data + ie) + 1;
                char* clipboard_data = (char*)IM_ALLOC(clipboard_data_len * sizeof(char));
                ImTextStrToUtf8(clipboard_data, clipboard_data_len, state->TextW.Data + ib, state->TextW.Data + ie);
                SetClipboardText(clipboard_data);
                MemFree(clipboard_data);
            }
            if (is_cut)
            {
                if (!state->HasSelection())
                    state->SelectAll();
                state->CursorFollow = true;
                stb_textedit_cut(state, &state->Stb);
            }
        }
        else if (is_paste)
        {
            if (const char* clipboard = GetClipboardText())
            {

                const int clipboard_len = (int)strlen(clipboard);
                ImWchar* clipboard_filtered = (ImWchar*)IM_ALLOC((clipboard_len + 1) * sizeof(ImWchar));
                int clipboard_filtered_len = 0;
                for (const char* s = clipboard; *s != 0; )
                {
                    unsigned int c;
                    s += ImTextCharFromUtf8(&c, s, NULL);
                    if (!input_text_filter_character(&g, &c, flags, callback, callback_user_data, ImGuiInputSource_Clipboard))
                        continue;
                    clipboard_filtered[clipboard_filtered_len++] = (ImWchar)c;
                }
                clipboard_filtered[clipboard_filtered_len] = 0;
                if (clipboard_filtered_len > 0)
                {
                    stb_textedit_paste(state, &state->Stb, clipboard_filtered, clipboard_filtered_len);
                    state->CursorFollow = true;
                }
                MemFree(clipboard_filtered);
            }
        }

        render_selection |= state->HasSelection() && (RENDER_SELECTION_WHEN_INACTIVE || render_cursor);
    }

    const char* apply_new_text = NULL;
    int apply_new_text_length = 0;
    if (g.ActiveId == id)
    {
        IM_ASSERT(state != NULL);
        if (revert_edit && !is_readonly)
        {
            if (flags & ImGuiInputTextFlags_EscapeClearsAll)
            {

                IM_ASSERT(buf[0] != 0);
                apply_new_text = "";
                apply_new_text_length = 0;
                value_changed = true;
                IMSTB_TEXTEDIT_CHARTYPE empty_string;
                stb_textedit_replace(state, &state->Stb, &empty_string, 0);
            }
            else if (strcmp(buf, state->InitialTextA.Data) != 0)
            {

                apply_new_text = state->InitialTextA.Data;
                apply_new_text_length = state->InitialTextA.Size - 1;
                value_changed = true;
                ImVector<ImWchar> w_text;
                if (apply_new_text_length > 0)
                {
                    w_text.resize(ImTextCountCharsFromUtf8(apply_new_text, apply_new_text + apply_new_text_length) + 1);
                    ImTextStrFromUtf8(w_text.Data, w_text.Size, apply_new_text, apply_new_text + apply_new_text_length);
                }
                stb_textedit_replace(state, &state->Stb, w_text.Data, (apply_new_text_length > 0) ? (w_text.Size - 1) : 0);
            }
        }

        if (!is_readonly)
        {
            state->TextAIsValid = true;
            state->TextA.resize(state->TextW.Size * 4 + 1);
            ImTextStrToUtf8(state->TextA.Data, state->TextA.Size, state->TextW.Data, NULL);
        }

        const bool apply_edit_back_to_user_buffer = !revert_edit || (validated && (flags & ImGuiInputTextFlags_EnterReturnsTrue) != 0);
        if (apply_edit_back_to_user_buffer)
        {

            if ((flags & (ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory | ImGuiInputTextFlags_CallbackEdit | ImGuiInputTextFlags_CallbackAlways)) != 0)
            {
                IM_ASSERT(callback != NULL);

                ImGuiInputTextFlags event_flag = 0;
                ImGuiKey event_key = ImGuiKey_None;
                if ((flags & ImGuiInputTextFlags_CallbackCompletion) != 0 && Shortcut(ImGuiKey_Tab, id))
                {
                    event_flag = ImGuiInputTextFlags_CallbackCompletion;
                    event_key = ImGuiKey_Tab;
                }
                else if ((flags & ImGuiInputTextFlags_CallbackHistory) != 0 && IsKeyPressed(ImGuiKey_UpArrow))
                {
                    event_flag = ImGuiInputTextFlags_CallbackHistory;
                    event_key = ImGuiKey_UpArrow;
                }
                else if ((flags & ImGuiInputTextFlags_CallbackHistory) != 0 && IsKeyPressed(ImGuiKey_DownArrow))
                {
                    event_flag = ImGuiInputTextFlags_CallbackHistory;
                    event_key = ImGuiKey_DownArrow;
                }
                else if ((flags & ImGuiInputTextFlags_CallbackEdit) && state->Edited)
                {
                    event_flag = ImGuiInputTextFlags_CallbackEdit;
                }
                else if (flags & ImGuiInputTextFlags_CallbackAlways)
                {
                    event_flag = ImGuiInputTextFlags_CallbackAlways;
                }

                if (event_flag)
                {
                    ImGuiInputTextCallbackData callback_data;
                    callback_data.Ctx = &g;
                    callback_data.EventFlag = event_flag;
                    callback_data.Flags = flags;
                    callback_data.UserData = callback_user_data;

                    char* callback_buf = is_readonly ? buf : state->TextA.Data;
                    callback_data.EventKey = event_key;
                    callback_data.Buf = callback_buf;
                    callback_data.BufTextLen = state->CurLenA;
                    callback_data.BufSize = state->BufCapacityA;
                    callback_data.BufDirty = false;

                    ImWchar* text = state->TextW.Data;
                    const int utf8_cursor_pos = callback_data.CursorPos = ImTextCountUtf8BytesFromStr(text, text + state->Stb.cursor);
                    const int utf8_selection_start = callback_data.SelectionStart = ImTextCountUtf8BytesFromStr(text, text + state->Stb.select_start);
                    const int utf8_selection_end = callback_data.SelectionEnd = ImTextCountUtf8BytesFromStr(text, text + state->Stb.select_end);

                    callback(&callback_data);

                    callback_buf = is_readonly ? buf : state->TextA.Data;
                    IM_ASSERT(callback_data.Buf == callback_buf);
                    IM_ASSERT(callback_data.BufSize == state->BufCapacityA);
                    IM_ASSERT(callback_data.Flags == flags);
                    const bool buf_dirty = callback_data.BufDirty;
                    if (callback_data.CursorPos != utf8_cursor_pos || buf_dirty) { state->Stb.cursor = ImTextCountCharsFromUtf8(callback_data.Buf, callback_data.Buf + callback_data.CursorPos); state->CursorFollow = true; }
                    if (callback_data.SelectionStart != utf8_selection_start || buf_dirty) { state->Stb.select_start = (callback_data.SelectionStart == callback_data.CursorPos) ? state->Stb.cursor : ImTextCountCharsFromUtf8(callback_data.Buf, callback_data.Buf + callback_data.SelectionStart); }
                    if (callback_data.SelectionEnd != utf8_selection_end || buf_dirty) { state->Stb.select_end = (callback_data.SelectionEnd == callback_data.SelectionStart) ? state->Stb.select_start : ImTextCountCharsFromUtf8(callback_data.Buf, callback_data.Buf + callback_data.SelectionEnd); }
                    if (buf_dirty)
                    {
                        IM_ASSERT(!is_readonly);
                        IM_ASSERT(callback_data.BufTextLen == (int)strlen(callback_data.Buf));
                        input_text_reconcile_undo_state_after_user_callback(state, callback_data.Buf, callback_data.BufTextLen);
                        if (callback_data.BufTextLen > backup_current_text_length && is_resizable)
                            state->TextW.resize(state->TextW.Size + (callback_data.BufTextLen - backup_current_text_length));
                        state->CurLenW = ImTextStrFromUtf8(state->TextW.Data, state->TextW.Size, callback_data.Buf, NULL);
                        state->CurLenA = callback_data.BufTextLen;
                        state->CursorAnimReset();
                    }
                }
            }

            if (!is_readonly && strcmp(state->TextA.Data, buf) != 0)
            {
                apply_new_text = state->TextA.Data;
                apply_new_text_length = state->CurLenA;
                value_changed = true;
            }
        }
    }

    if (g.InputTextDeactivatedState.ID == id)
    {
        if (g.ActiveId != id && IsItemDeactivatedAfterEdit() && !is_readonly && strcmp(g.InputTextDeactivatedState.TextA.Data, buf) != 0)
        {
            apply_new_text = g.InputTextDeactivatedState.TextA.Data;
            apply_new_text_length = g.InputTextDeactivatedState.TextA.Size - 1;
            value_changed = true;

        }
        g.InputTextDeactivatedState.ID = 0;
    }

    if (apply_new_text != NULL)
    {

        IM_ASSERT(apply_new_text_length >= 0);
        if (is_resizable)
        {
            ImGuiInputTextCallbackData callback_data;
            callback_data.Ctx = &g;
            callback_data.EventFlag = ImGuiInputTextFlags_CallbackResize;
            callback_data.Flags = flags;
            callback_data.Buf = buf;
            callback_data.BufTextLen = apply_new_text_length;
            callback_data.BufSize = ImMax(buf_size, apply_new_text_length + 1);
            callback_data.UserData = callback_user_data;
            callback(&callback_data);
            buf = callback_data.Buf;
            buf_size = callback_data.BufSize;
            apply_new_text_length = ImMin(callback_data.BufTextLen, buf_size - 1);
            IM_ASSERT(apply_new_text_length <= buf_size);
        }

        ImStrncpy(buf, apply_new_text, ImMin(apply_new_text_length + 1, buf_size));
    }

    if (g.ActiveId == id && clear_active_id)
        ClearActiveID();
    else if (g.ActiveId == id)
        g.WantTextInputNextFrame = 1;

    static std::map<ImGuiID, float> anim;
    float& it_anim = anim[id];

    if (!is_multiline)
    {
        window->DrawList->AddRectFilled(clickable.Min, clickable.Max, gui->get_clr(clr->combo_color), set->input_rounding);
    }

    const ImVec4 clip_rect(clickable.Min.x, clickable.Min.y, clickable.Min.x + inner_size.x, clickable.Min.y + inner_size.y);
    ImVec2 draw_pos = is_multiline ? draw_window->DC.CursorPos : clickable.Min + ImVec2((size_arg.y - CalcTextSize(hint).y), (size_arg.y - CalcTextSize(hint).y) - 1) / 2;
    ImVec2 text_size(0.0f, 0.0f);

    const int buf_display_max_length = 2 * 1024 * 1024;
    const char* buf_display = buf_display_from_state ? state->TextA.Data : buf;
    const char* buf_display_end = NULL;
    if (is_displaying_hint)
    {
        buf_display = hint;
        buf_display_end = hint + strlen(hint);
    }

    if (render_cursor || render_selection)
    {
        IM_ASSERT(state != NULL);
        if (!is_displaying_hint)
            buf_display_end = buf_display + state->CurLenA;

        const ImWchar* text_begin = state->TextW.Data;
        ImVec2 cursor_offset, select_start_offset;

        {

            const ImWchar* searches_input_ptr[2] = { NULL, NULL };
            int searches_result_line_no[2] = { -1000, -1000 };
            int searches_remaining = 0;
            if (render_cursor)
            {
                searches_input_ptr[0] = text_begin + state->Stb.cursor;
                searches_result_line_no[0] = -1;
                searches_remaining++;
            }
            if (render_selection)
            {
                searches_input_ptr[1] = text_begin + ImMin(state->Stb.select_start, state->Stb.select_end);
                searches_result_line_no[1] = -1;
                searches_remaining++;
            }

            searches_remaining += is_multiline ? 1 : 0;
            int line_count = 0;

            for (const ImWchar* s = text_begin; *s != 0; s++)
                if (*s == '\n')
                {
                    line_count++;
                    if (searches_result_line_no[0] == -1 && s >= searches_input_ptr[0]) { searches_result_line_no[0] = line_count; if (--searches_remaining <= 0) break; }
                    if (searches_result_line_no[1] == -1 && s >= searches_input_ptr[1]) { searches_result_line_no[1] = line_count; if (--searches_remaining <= 0) break; }
                }
            line_count++;
            if (searches_result_line_no[0] == -1)
                searches_result_line_no[0] = line_count;
            if (searches_result_line_no[1] == -1)
                searches_result_line_no[1] = line_count;

            cursor_offset.x = input_text_calc_text_size_w(&g, ImStrbolW(searches_input_ptr[0], text_begin), searches_input_ptr[0]).x;
            cursor_offset.y = searches_result_line_no[0] * g.FontSize;
            if (searches_result_line_no[1] >= 0)
            {
                select_start_offset.x = input_text_calc_text_size_w(&g, ImStrbolW(searches_input_ptr[1], text_begin), searches_input_ptr[1]).x;
                select_start_offset.y = searches_result_line_no[1] * g.FontSize;
            }

            if (is_multiline)
                text_size = ImVec2(inner_size.x, line_count * g.FontSize);
        }

        if (render_cursor && state->CursorFollow)
        {

            if (!(flags & ImGuiInputTextFlags_NoHorizontalScroll))
            {
                const float scroll_increment_x = inner_size.x * 0.25f;
                const float visible_width = inner_size.x - style.FramePadding.x;
                if (cursor_offset.x < state->ScrollX)
                    state->ScrollX = IM_TRUNC(ImMax(0.0f, cursor_offset.x - scroll_increment_x));
                else if (cursor_offset.x - visible_width >= state->ScrollX)
                    state->ScrollX = IM_TRUNC(cursor_offset.x - visible_width + scroll_increment_x);
            }
            else
            {
                state->ScrollX = 0.0f;
            }

            if (is_multiline)
            {

                if (cursor_offset.y - g.FontSize < scroll_y)
                    scroll_y = ImMax(0.0f, cursor_offset.y - g.FontSize);
                else if (cursor_offset.y - (inner_size.y - style.FramePadding.y * 2.0f) >= scroll_y)
                    scroll_y = cursor_offset.y - inner_size.y + style.FramePadding.y * 2.0f;
                const float scroll_max_y = ImMax((text_size.y + style.FramePadding.y * 2.0f) - inner_size.y, 0.0f);
                scroll_y = ImClamp(scroll_y, 0.0f, scroll_max_y);
                draw_pos.y += (draw_window->Scroll.y - scroll_y);
                draw_window->Scroll.y = scroll_y;
            }

            state->CursorFollow = false;
        }

        const ImVec2 draw_scroll = ImVec2(state->ScrollX, 0.0f);
        if (render_selection)
        {
            const ImWchar* text_selected_begin = text_begin + ImMin(state->Stb.select_start, state->Stb.select_end);
            const ImWchar* text_selected_end = text_begin + ImMax(state->Stb.select_start, state->Stb.select_end);

            ImU32 bg_color = gui->get_clr(clr->accent_color, render_cursor ? 1.0f : 0.6f);
            float bg_offy_up = is_multiline ? 0.0f : -1.0f;
            float bg_offy_dn = is_multiline ? 0.0f : 2.0f;
            ImVec2 rect_pos = draw_pos + select_start_offset - draw_scroll;
            for (const ImWchar* p = text_selected_begin; p < text_selected_end; )
            {
                if (rect_pos.y > clip_rect.w + g.FontSize)
                    break;
                if (rect_pos.y < clip_rect.y)
                {

                    while (p < text_selected_end)
                        if (*p++ == '\n')
                            break;
                }
                else
                {
                    ImVec2 rect_size = input_text_calc_text_size_w(&g, p, text_selected_end, &p, NULL, true);
                    if (rect_size.x <= 0.0f) rect_size.x = IM_TRUNC(g.Font->GetCharAdvance((ImWchar)' ') * 0.50f);
                    ImRect rect(rect_pos + ImVec2(0.0f, bg_offy_up - g.FontSize), rect_pos + ImVec2(rect_size.x, bg_offy_dn));
                    rect.ClipWith(clip_rect);
                    if (rect.Overlaps(clip_rect)) draw_window->DrawList->AddRectFilled(rect.Min, rect.Max, gui->get_clr(clr->accent_color, 0.3f), 2.f);
                }
                rect_pos.x = draw_pos.x - draw_scroll.x;
                rect_pos.y += g.FontSize;
            }
        }

        if (is_multiline || (buf_display_end - buf_display) < buf_display_max_length)
            draw_window->DrawList->AddText(g.Font, g.FontSize, draw_pos - draw_scroll, GetColorU32(is_displaying_hint ? clr->text : clr->text_active), buf_display, buf_display_end, 0.0f, is_multiline ? NULL : &clip_rect);

        if (render_cursor)
        {
            state->CursorAnim += io.DeltaTime;
            bool cursor_is_visible = (!g.IO.ConfigInputTextCursorBlink) || (state->CursorAnim <= 0.0f) || ImFmod(state->CursorAnim, 1.20f) <= 0.80f;
            ImVec2 cursor_screen_pos = ImTrunc(draw_pos + cursor_offset - draw_scroll);
            ImRect cursor_screen_rect(cursor_screen_pos.x, cursor_screen_pos.y - g.FontSize + 0.5f, cursor_screen_pos.x + 1.0f, cursor_screen_pos.y - 1.5f);
            it_anim = ImLerp(it_anim, cursor_screen_rect.Min.x - clickable.Min.x, gui->fixed_speed(20.f));

            if (cursor_is_visible && cursor_screen_rect.Overlaps(clip_rect))
                draw_window->DrawList->AddLine(ImVec2(clickable.Min.x + it_anim, cursor_screen_rect.Min.y), ImVec2(clickable.Min.x + it_anim, cursor_screen_rect.GetBL().y), GetColorU32(clr->white_color));

            if (!is_readonly)
            {
                g.PlatformImeData.WantVisible = true;
                g.PlatformImeData.InputPos = ImVec2(cursor_screen_pos.x - 1.0f, cursor_screen_pos.y - g.FontSize);
                g.PlatformImeData.InputLineHeight = g.FontSize;
            }
        }
    }
    else
    {
        if (is_multiline) text_size = ImVec2(inner_size.x, input_text_calc_text_len_and_line_count(buf_display, &buf_display_end) * g.FontSize);
        else if (!is_displaying_hint && g.ActiveId == id) buf_display_end = buf_display + state->CurLenA;
        else if (!is_displaying_hint) buf_display_end = buf_display + strlen(buf_display);

        if (is_multiline || (buf_display_end - buf_display) < buf_display_max_length)
            draw_window->DrawList->AddText(g.Font, g.FontSize, draw_pos, GetColorU32(is_displaying_hint ? clr->text : clr->text_active), buf_display, buf_display_end, 0.0f, is_multiline ? NULL : &clip_rect);
    }

    if (is_password && !is_displaying_hint) PopFont();

    if (is_multiline)
    {
        Dummy(ImVec2(text_size.x, text_size.y + style.FramePadding.y));
        g.NextItemData.ItemFlags |= ImGuiItemFlags_Inputable | ImGuiItemFlags_NoTabStop;
        EndChild();
        item_data_backup.StatusFlags |= (g.LastItemData.StatusFlags & ImGuiItemStatusFlags_HoveredWindow);

        EndGroup();
        if (g.LastItemData.ID == 0)
        {
            g.LastItemData.ID = id;
            g.LastItemData.InFlags = item_data_backup.InFlags;
            g.LastItemData.StatusFlags = item_data_backup.StatusFlags;
        }
    }

    if (value_changed && !(flags & ImGuiInputTextFlags_NoMarkEdited)) MarkItemEdited(id);

    PopFont();

    if ((flags & ImGuiInputTextFlags_EnterReturnsTrue) != 0) return validated;
    else return value_changed;
}

bool c_widgets::text_field(std::string_view label, std::string_view hint, const ImVec2& size, char* buf, size_t buf_size, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
{
    IM_ASSERT(!(flags & ImGuiInputTextFlags_Multiline));
    return input_text_ex(label.data(), hint.data(), buf, (int)buf_size, size, flags, callback, user_data);
}

static void color_edit_restore_hs(const float* col, float* H, float* S, float* V)
{
    ImGuiContext& g = *GImGui;

    if (*S == 0.0f || (*H == 0.0f && g.ColorEditSavedHue == 1))
        *H = g.ColorEditSavedHue;

    if (*V == 0.0f) *S = g.ColorEditSavedSat;
}

bool c_widgets::color_picker(const char* label, float col[4], ImGuiColorEditFlags flags, const float* ref_col)
{
    struct picker_state
    {
        ImVec2 sv;
        float hue, alpha;
    };

    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImDrawList* draw_list = window->DrawList;
    ImGuiStyle& style = g.Style;
    ImGuiIO& io = g.IO;

    const float width = CalcItemWidth();
    g.NextItemData.ClearFlags();

    PushID(label);
    BeginGroup();
    PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 0));

    if (!(flags & ImGuiColorEditFlags_NoSidePreview)) flags |= ImGuiColorEditFlags_NoSmallPreview;

    if (!(flags & ImGuiColorEditFlags_NoOptions)) ColorPickerOptionsPopup(col, flags);

    if (!(flags & ImGuiColorEditFlags_PickerMask_)) flags |= ((g.ColorEditOptions & ImGuiColorEditFlags_PickerMask_) ? g.ColorEditOptions : ImGuiColorEditFlags_DefaultOptions_) & ImGuiColorEditFlags_PickerMask_;
    if (!(flags & ImGuiColorEditFlags_InputMask_)) flags |= ((g.ColorEditOptions & ImGuiColorEditFlags_InputMask_) ? g.ColorEditOptions : ImGuiColorEditFlags_DefaultOptions_) & ImGuiColorEditFlags_InputMask_;

    IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags_PickerMask_));
    IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags_InputMask_));

    if (!(flags & ImGuiColorEditFlags_NoOptions)) flags |= (g.ColorEditOptions & ImGuiColorEditFlags_AlphaBar);
    int components = (flags & ImGuiColorEditFlags_NoAlpha) ? 3 : 4;
    bool alpha_bar = (flags & ImGuiColorEditFlags_AlphaBar) && !(flags & ImGuiColorEditFlags_NoAlpha);
    ImVec2 picker_pos = window->DC.CursorPos + ImVec2(0, 0);
    ImVec2 bar_pos = window->DC.CursorPos + ImVec2(0, 130);

    float picker_size = 190.f;
    float bar_width = 190.f;
    float bar_height = 4.f;

    float backup_initial_col[4];
    memcpy(backup_initial_col, col, components * sizeof(float));

    float H = col[0], S = col[1], V = col[2];
    float R = col[0], G = col[1], B = col[2];
    if (flags & ImGuiColorEditFlags_InputRGB)
    {
        ColorConvertRGBtoHSV(R, G, B, H, S, V);
        color_edit_restore_hs(col, &H, &S, &V);
    }
    else if (flags & ImGuiColorEditFlags_InputHSV)
    {
        ColorConvertHSVtoRGB(H, S, V, R, G, B);
    }

    bool value_changed = false, value_changed_h = false, value_changed_sv = false;

    PushItemFlag(ImGuiItemFlags_NoNav, true);

    SetCursorScreenPos(ImVec2(picker_pos.x, picker_pos.y));
    InvisibleButton("sv", ImVec2(picker_size, picker_size));
    if (IsItemActive())
    {
        S = ImSaturate((io.MousePos.x - picker_pos.x) / (picker_size - 1));
        V = 1.0f - ImSaturate((io.MousePos.y - picker_pos.y) / (picker_size - 1));

        if (g.ColorEditSavedColor == ColorConvertFloat4ToU32(ImVec4(col[0], col[1], col[2], 0))) H = g.ColorEditSavedHue;
        value_changed = value_changed_sv = true;
    }

    SetCursorScreenPos(ImVec2(picker_pos.x, picker_pos.y + 204.f));
    InvisibleButton("hue", ImVec2(bar_width, bar_height + 8));
    if (IsItemActive())
    {
        H = 1.f - ImSaturate((io.MousePos.x - picker_pos.x) / (bar_width - 1));
        value_changed = value_changed_h = true;
    }

    if (alpha_bar)
    {
        SetCursorScreenPos(ImVec2(picker_pos.x, picker_pos.y + 227.f));
        InvisibleButton("alpha", ImVec2(bar_width, bar_height + 8));
        if (IsItemActive())
        {
            col[3] = ImSaturate((io.MousePos.x - picker_pos.x) / (bar_width - 1));
            value_changed = true;
        }
    }
    PopItemFlag();

    if (value_changed_h || value_changed_sv)
    {
        if (flags & ImGuiColorEditFlags_InputRGB)
        {
            ColorConvertHSVtoRGB(H, S, V, col[0], col[1], col[2]);
            g.ColorEditSavedHue = H;
            g.ColorEditSavedSat = S;
            g.ColorEditSavedColor = ColorConvertFloat4ToU32(ImVec4(col[0], col[1], col[2], 0));
        }
        else if (flags & ImGuiColorEditFlags_InputHSV)
        {
            col[0] = H;
            col[1] = S;
            col[2] = V;
        }
    }

    bool value_changed_fix_hue_wrap = false;
    ImGuiColorEditFlags sub_flags_to_forward = ImGuiColorEditFlags_DataTypeMask_ | ImGuiColorEditFlags_InputMask_ | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf;
    ImGuiColorEditFlags sub_flags = (flags & sub_flags_to_forward) | ImGuiColorEditFlags_NoPicker;

    if (flags & ImGuiColorEditFlags_DisplayRGB || (flags & ImGuiColorEditFlags_DisplayMask_) == 0)
        if (gui->color_edit("##rgb", col, sub_flags | ImGuiColorEditFlags_DisplayRGB))
        {
            value_changed_fix_hue_wrap = (g.ActiveId != 0 && !g.ActiveIdAllowOverlap);
            value_changed = true;
        }
    if (flags & ImGuiColorEditFlags_DisplayHSV || (flags & ImGuiColorEditFlags_DisplayMask_) == 0) value_changed |= gui->color_edit("##hsv", col, sub_flags | ImGuiColorEditFlags_DisplayHSV);
    if (flags & ImGuiColorEditFlags_DisplayHex || (flags & ImGuiColorEditFlags_DisplayMask_) == 0) value_changed |= gui->color_edit("##hex", col, sub_flags | ImGuiColorEditFlags_DisplayHex);

    if (value_changed_fix_hue_wrap && (flags & ImGuiColorEditFlags_InputRGB))
    {
        float new_H, new_S, new_V;
        ColorConvertRGBtoHSV(col[0], col[1], col[2], new_H, new_S, new_V);
        if (new_H <= 0 && H > 0)
        {
            if (new_V <= 0 && V != new_V)
                ColorConvertHSVtoRGB(H, S, new_V <= 0 ? V * 0.5f : new_V, col[0], col[1], col[2]);
            else if (new_S <= 0)
                ColorConvertHSVtoRGB(H, new_S <= 0 ? S * 0.5f : new_S, new_V, col[0], col[1], col[2]);
        }
    }

    if (value_changed)
    {
        if (flags & ImGuiColorEditFlags_InputRGB)
        {
            R = col[0];
            G = col[1];
            B = col[2];
            ColorConvertRGBtoHSV(R, G, B, H, S, V);
            color_edit_restore_hs(col, &H, &S, &V);
        }
        else if (flags & ImGuiColorEditFlags_InputHSV)
        {
            H = col[0];
            S = col[1];
            V = col[2];
            ColorConvertHSVtoRGB(H, S, V, R, G, B);
        }
    }

    const int style_alpha8 = IM_F32_TO_INT8_SAT(style.Alpha);
    const ImU32 col_black = IM_COL32(0, 0, 0, style_alpha8);
    const ImU32 col_white = IM_COL32(255, 255, 255, style_alpha8);
    const ImU32 col_midgrey = IM_COL32(128, 128, 128, style_alpha8);
    const ImU32 col_hues[7] = { IM_COL32(255,0,0,style_alpha8), IM_COL32(255,0,255,style_alpha8), IM_COL32(0,0,255,style_alpha8),IM_COL32(0,255,255,style_alpha8), IM_COL32(0,255,0,style_alpha8), IM_COL32(255,255,0,style_alpha8), IM_COL32(255,0,0,style_alpha8) };

    ImVec4 hue_color_f(1, 1, 1, style.Alpha); ColorConvertHSVtoRGB(H, 1, 1, hue_color_f.x, hue_color_f.y, hue_color_f.z);
    ImU32 hue_color32 = ColorConvertFloat4ToU32(hue_color_f);
    ImU32 user_col32_striped_of_alpha = ColorConvertFloat4ToU32(ImVec4(R, G, B, style.Alpha));

    ImVec2 sv_cursor_pos;

    const int vtx_idx_0 = draw_list->VtxBuffer.Size;
    draw_list->AddRectFilled(picker_pos, picker_pos + ImVec2(picker_size, picker_size - 2), col_white, 3.0f);
    const int vtx_idx_1 = draw_list->VtxBuffer.Size;
    ShadeVertsLinearColorGradientKeepAlpha(draw_list, vtx_idx_0, vtx_idx_1, picker_pos, picker_pos + ImVec2(picker_size, 0.0f), col_white, hue_color32);

    gui->add_rect_filled_multi_color(draw_list, picker_pos, picker_pos + ImVec2(picker_size, picker_size), 0, 0, col_black, col_black, 3.f);

    sv_cursor_pos.x = ImClamp(IM_ROUND(picker_pos.x + ImSaturate(S) * picker_size), picker_pos.x + 2, picker_pos.x + picker_size - 2);
    sv_cursor_pos.y = ImClamp(IM_ROUND(picker_pos.y + ImSaturate(1 - V) * (picker_size)), picker_pos.y + 2, picker_pos.y + picker_size - 2);

    static std::map<ImGuiID, picker_state> anim;
    picker_state& state = anim[GetID(label)];

    for (int i = 0; i < 6; ++i)
        gui->add_rect_filled_multi_color(draw_list, ImVec2(picker_pos.x + i * (bar_width / 6) - (i == 5 ? 1 : 0), picker_pos.y + 208), ImVec2(picker_pos.x + (i + 1) * (bar_width / 6) + (i == 0 ? 1 : 0), picker_pos.y + 208 + bar_height), col_hues[i], col_hues[i + 1], col_hues[i + 1], col_hues[i], 2.f, i == 0 ? ImDrawFlags_RoundCornersLeft : i == 5 ? ImDrawFlags_RoundCornersRight : ImDrawFlags_RoundCornersNone);
    float bar0_line_x = IM_ROUND(picker_pos.x + (1.f - H) * bar_width);
    bar0_line_x = ImClamp(bar0_line_x, picker_pos.x + 3.f, picker_pos.x + 186.f);

    state.hue = ImLerp(state.hue, bar0_line_x, g.IO.DeltaTime * 20.f);
    draw_list->AddCircleFilled(ImVec2(state.hue, picker_pos.y + 210), 5, GetColorU32(ImVec4(1.f, 1.f, 1.f, 1.f)));

    state.sv = ImLerp(state.sv, sv_cursor_pos, g.IO.DeltaTime * 10.f);

    draw_list->AddCircle(state.sv, 5.f, col_white, 12, 4.f);
    draw_list->AddCircleFilled(state.sv, 5.f, user_col32_striped_of_alpha, 12);

    if (alpha_bar)
    {
        float alpha = ImSaturate(col[3]);
        gui->add_rect_filled_multi_color(draw_list, picker_pos + ImVec2(0, 231), picker_pos + ImVec2(bar_width, 231 + bar_height), col_black, user_col32_striped_of_alpha, user_col32_striped_of_alpha, col_black, 2.f);
        float bar1_line_x = IM_ROUND(picker_pos.x + alpha * bar_width);
        bar1_line_x = ImClamp(bar1_line_x, picker_pos.x + 3.f, picker_pos.x + 186.f);

        state.alpha = ImLerp(state.alpha, bar1_line_x, g.IO.DeltaTime * 20.f);
        draw_list->AddCircleFilled(ImVec2(state.alpha, picker_pos.y + 233), 5, GetColorU32(ImVec4(1.f, 1.f, 1.f, 1.f)));
    }

    PopStyleVar();
    EndGroup();

    if (value_changed && memcmp(backup_initial_col, col, components * sizeof(float)) == 0) value_changed = false;
    if (value_changed && g.LastItemData.ID != 0)  MarkItemEdited(g.LastItemData.ID);

    PopID();

    return value_changed;
}

bool color_button(const char* desc_id, const ImVec4& col, ImGuiColorEditFlags flags, const ImVec2& size_arg, bool popup = false)
{
    struct cbutton_state
    {
        ImVec4 text;
    };

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems) return false;

    ImGuiContext& g = *GImGui;
    const ImGuiID id = window->GetID(desc_id);
    const float default_size = GetFrameHeight();

    const float width = GetWindowWidth() - g.Style.WindowPadding.x * 2;
    const float box_size = 18, rect_correct_size = 25;

    const ImVec2 rect_size = ImVec2(box_size, box_size);
    const ImVec2 pos = window->DC.CursorPos;
    const ImRect rect(pos, pos + ImVec2(width, rect_correct_size));

    ItemSize(ImRect(rect.Min, rect.Max - ImVec2(0, 0)));
    if (!ItemAdd((flags & ImGuiColorEditFlags_NoLabel) ? rect : rect, id)) return false;

    bool hovered, held, pressed = ButtonBehavior(rect, id, &hovered, &held);

    if (flags & ImGuiColorEditFlags_NoAlpha) flags &= ~(ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf);

    ImVec4 col_rgb = col;
    if (flags & ImGuiColorEditFlags_InputHSV) ColorConvertHSVtoRGB(col_rgb.x, col_rgb.y, col_rgb.z, col_rgb.x, col_rgb.y, col_rgb.z);

    static std::map<ImGuiID, cbutton_state> anim;
    cbutton_state& state = anim[id];

    gui->render_text_clipped(set->poppins_widget, rect.Min, rect.Max, gui->get_clr(clr->text), desc_id, { 0.0, 0.5 });

    RenderColorRectWithAlphaCheckerboard(window->DrawList, ImVec2(rect.Max.x - rect_size.x, rect.Min.y + (rect_correct_size - box_size) / 2), rect.Max - ImVec2(0, (rect_correct_size - box_size) / 2), gui->get_clr(col_rgb), ImMin(20, 20) / 2.99f, ImVec2(0.f, 0.f), set->color_rounding);

    return pressed;
}

bool c_widgets::color_edit(std::string_view label, float col[4], ImGuiColorEditFlags flags, bool popup)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems) return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const float square_sz = GetFrameHeight();
    const float w_full = CalcItemWidth();
    const float w_button = (flags & ImGuiColorEditFlags_NoSmallPreview) ? 0.0f : (square_sz + style.ItemInnerSpacing.x);
    const float w_inputs = w_full - w_button;
    const char* label_display_end = FindRenderedTextEnd(label.data());
    g.NextItemData.ClearFlags();

    BeginGroup();
    PushID(label.data());

    const ImGuiColorEditFlags flags_untouched = flags;
    if (flags & ImGuiColorEditFlags_NoInputs) flags = (flags & (~ImGuiColorEditFlags_DisplayMask_)) | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoOptions;

    if (!(flags & ImGuiColorEditFlags_NoOptions)) ColorEditOptionsPopup(col, flags);

    if (!(flags & ImGuiColorEditFlags_DisplayMask_)) flags |= (g.ColorEditOptions & ImGuiColorEditFlags_DisplayMask_);
    if (!(flags & ImGuiColorEditFlags_DataTypeMask_)) flags |= (g.ColorEditOptions & ImGuiColorEditFlags_DataTypeMask_);
    if (!(flags & ImGuiColorEditFlags_PickerMask_)) flags |= (g.ColorEditOptions & ImGuiColorEditFlags_PickerMask_);
    if (!(flags & ImGuiColorEditFlags_InputMask_)) flags |= (g.ColorEditOptions & ImGuiColorEditFlags_InputMask_);

    flags |= (g.ColorEditOptions & ~(ImGuiColorEditFlags_DisplayMask_ | ImGuiColorEditFlags_DataTypeMask_ | ImGuiColorEditFlags_PickerMask_ | ImGuiColorEditFlags_InputMask_));

    IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags_DisplayMask_));
    IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags_InputMask_));

    const bool alpha = (flags & ImGuiColorEditFlags_NoAlpha) == 0;
    const bool hdr = (flags & ImGuiColorEditFlags_HDR) != 0;
    const int components = alpha ? 4 : 3;

    float f[4] = { col[0], col[1], col[2], alpha ? col[3] : 1.0f };
    if ((flags & ImGuiColorEditFlags_InputHSV) && (flags & ImGuiColorEditFlags_DisplayRGB))
        ColorConvertHSVtoRGB(f[0], f[1], f[2], f[0], f[1], f[2]);
    else if ((flags & ImGuiColorEditFlags_InputRGB) && (flags & ImGuiColorEditFlags_DisplayHSV))
    {
        ColorConvertRGBtoHSV(f[0], f[1], f[2], f[0], f[1], f[2]);
        color_edit_restore_hs(col, &f[0], &f[1], &f[2]);
    }
    int i[4] = { IM_F32_TO_INT8_UNBOUND(f[0]), IM_F32_TO_INT8_UNBOUND(f[1]), IM_F32_TO_INT8_UNBOUND(f[2]), IM_F32_TO_INT8_UNBOUND(f[3]) };

    bool value_changed = false;
    bool value_changed_as_float = false;

    const ImVec2 pos = window->DC.CursorPos;
    const float inputs_offset_x = (style.ColorButtonPosition == ImGuiDir_Left) ? w_button : 0.0f;

    ImGuiWindow* picker_active_window = NULL;
    if (!(flags & ImGuiColorEditFlags_NoSmallPreview))
    {
        const float button_offset_x = ((flags & ImGuiColorEditFlags_NoInputs) || (style.ColorButtonPosition == ImGuiDir_Left)) ? 0.0f : w_inputs + style.ItemInnerSpacing.x;
        const ImVec4 col_v4(col[0], col[1], col[2], alpha ? col[3] : 1.0f);

        PushFont(set->poppins_widget);
        if (color_button(label.data(), col_v4, flags, ImVec2(0, 0), popup))
        {
            if (!(flags & ImGuiColorEditFlags_NoPicker))
            {
                g.ColorPickerRef = col_v4;
                OpenPopup("picker");
                SetNextWindowPos(g.LastItemData.Rect.GetBL() + ImVec2(0.0f, 0.f));
            }
        }
        PopFont();

        gui->begin_popup_w((std::stringstream{} << label << "keywindow").str().c_str(), 210);
        {
            picker_active_window = g.CurrentWindow;

            ImGuiColorEditFlags picker_flags_to_forward = ImGuiColorEditFlags_DataTypeMask_ | ImGuiColorEditFlags_PickerMask_ | ImGuiColorEditFlags_InputMask_ | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_AlphaBar;
            ImGuiColorEditFlags picker_flags = (flags_untouched & picker_flags_to_forward) | ImGuiColorEditFlags_DisplayMask_ | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreviewHalf;
            SetNextItemWidth(square_sz * 12.0f);
            value_changed |= color_picker("##picker", col, picker_flags, &g.ColorPickerRef.x);
        }
        gui->end_popup_w();
    }

    if (value_changed && picker_active_window == NULL)
    {
        if (!value_changed_as_float)
            for (int n = 0; n < 4; n++)
                f[n] = i[n] / 255.0f;
        if ((flags & ImGuiColorEditFlags_DisplayHSV) && (flags & ImGuiColorEditFlags_InputRGB))
        {
            g.ColorEditSavedHue = f[0];
            g.ColorEditSavedSat = f[1];
            ColorConvertHSVtoRGB(f[0], f[1], f[2], f[0], f[1], f[2]);
            g.ColorEditSavedColor = ColorConvertFloat4ToU32(ImVec4(f[0], f[1], f[2], 0));
        }
        if ((flags & ImGuiColorEditFlags_DisplayRGB) && (flags & ImGuiColorEditFlags_InputHSV))
            ColorConvertRGBtoHSV(f[0], f[1], f[2], f[0], f[1], f[2]);

        col[0] = f[0];
        col[1] = f[1];
        col[2] = f[2];
        if (alpha)
            col[3] = f[3];
    }

    PopID();
    EndGroup();

    if (picker_active_window && g.ActiveId != 0 && g.ActiveIdWindow == picker_active_window) g.LastItemData.ID = g.ActiveId;

    if (value_changed && g.LastItemData.ID != 0)  MarkItemEdited(g.LastItemData.ID);

    return value_changed;
}

bool c_widgets::button(std::string_view label, const ImVec2& size_arg)
{

    struct button_state
    {
        ImVec4 label_clr = clr->text_active, background = clr->combo_color;
        float offset, alpha;
        bool press = false;
    };

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems) return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label.data());

    const float width = GetContentRegionMax().x - g.Style.WindowPadding.x;
    ImVec2 pos = window->DC.CursorPos;

    const ImRect rect(pos, pos + size_arg);
    ItemSize(rect, 0);
    if (!ItemAdd(rect, id)) return false;

    bool hovered, held, pressed = ButtonBehavior(rect, id, &hovered, &held, 0);

    static std::map<ImGuiID, button_state> anim;
    button_state& state = anim[id];

    if (IsItemClicked()) state.press = true;
    if (state.offset >= 1.98f) state.press = false;

    state.offset = ImLerp(state.offset, state.press ? 2.f : 0.f, gui->fixed_speed(12.f));
    ImVec4 target_bg = state.press ? clr->accent_color : (hovered ? clr->selection_active : clr->combo_color);
    state.background = ImLerp(state.background, target_bg, gui->fixed_speed(14.f));

    ImVec2 b_min = rect.Min - ImVec2(state.offset, state.offset);
    ImVec2 b_max = rect.Max + ImVec2(state.offset, state.offset);
    window->DrawList->AddRectFilled(b_min, b_max, gui->get_clr(state.background), set->input_rounding);
    window->DrawList->AddRect(b_min, b_max, gui->get_clr(hovered || state.press ? clr->accent_color : clr->child_stroke_bar, hovered ? 0.85f : 0.4f), set->input_rounding, 0, 1.0f);
    gui->render_text_clipped(set->poppins_widget, rect.Min, rect.Max, gui->get_clr(clr->text_active), label.data(), {0.5, 0.5});

    return pressed;
}

void c_widgets::water_mark(std::string name, std::vector<std::string> function, int corner_pos, bool* visible)
{
    static ImVec2 content_size, pos, current_pos;

    struct spectator_state
    {
        float alpha;
    };

    static std::map<ImGuiID, spectator_state> anim;
    spectator_state& state = anim[ImGui::GetID(name.c_str())];

    state.alpha = ImLerp(state.alpha, *visible ? 1.f : 0.f, fixed_speed(12.f));

    push_style_var(ImGuiStyleVar_Alpha, state.alpha);

    if (state.alpha >= 0.01f) {

        switch (corner_pos)
        {
        case 0:
            pos = ImVec2(10, 10);
            break;
        case 1:
            pos = ImVec2(ImGui::GetIO().DisplaySize.x - content_size.x, 10);
            break;
        case 2:
            pos = ImVec2(10, ImGui::GetIO().DisplaySize.y - content_size.y);
            break;
        case 3:
            pos = ImVec2(ImGui::GetIO().DisplaySize.x - content_size.x, ImGui::GetIO().DisplaySize.y - content_size.y);
            break;
        }

        current_pos = ImLerp(current_pos, pos, fixed_speed(25.f));

        PushStyleVar(ImGuiStyleVar_WindowRounding, set->watermark_rounding);
        PushStyleVar(ImGuiStyleVar_WindowPadding, { 10, 10 });
        PushStyleVar(ImGuiStyleVar_ItemSpacing, { 20, 10 });

        SetNextWindowPos(current_pos);
        gui->begin("watermark", nullptr, set->window_flags | ImGuiWindowFlags_AlwaysAutoResize);
        {
            ImDrawList* draw = ImGui::GetWindowDrawList();

            for (int i = 0; i < function.size(); i++)
            {
                gui->text_colored(gui->get_clr(clr->text_active), function[i]);
                gui->sameline();
                draw->AddRectFilled(ImGui::GetCursorScreenPos() - ImVec2(12, -4), ImGui::GetCursorScreenPos() + ImVec2(-9, 15), gui->get_clr(clr->text), 10.f);
                gui->sameline();
            }
            content_size = ImGui::GetContentRegionMax() + ImVec2(20, 20);
        }
        gui->end();
        PopStyleVar(3);
    }
    pop_style_var();
}

static float calc_combo_size(int items_count, float item_size)
{
    ImGuiContext& g = *GImGui;
    if (items_count <= 0)
        return FLT_MAX;
    return item_size * items_count + g.Style.ItemSpacing.y * (items_count - 1);
}

bool c_widgets::selectable_ex(const char* label, bool active)
{
    struct selectable_state
    {
        float alpha = 0.f, offset = 0.f;
        ImVec4 rect = clr->element_background, text = clr->text_active;
    };

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems) return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);

    const float width = GetContentRegionMax().x - 2 * set->combo_spacing;
    const ImVec2 pos = window->DC.CursorPos;
    const ImRect rect(pos, pos + ImVec2(width, 31));
    ItemSize(rect, 0);
    if (!ItemAdd(rect, id)) return false;

    bool hovered = IsItemHovered(), pressed = hovered && g.IO.MouseClicked[0];

    if (pressed) MarkItemEdited(id);

    static std::map<ImGuiID, selectable_state> anim;
    selectable_state& state = anim[id];

    state.alpha = ImClamp(state.alpha + (gui->fixed_speed(6.f) * (active ? 1.f : -1.f)), 0.f, 1.f);
    state.rect = ImLerp(state.rect, active ? clr->accent_color : clr->text, gui->fixed_speed(12.f));
    state.text = ImLerp(state.text, active ? clr->accent_color : clr->text_active, gui->fixed_speed(12.f));
    state.offset = ImLerp(state.offset, active ? 7.f : 9.f, gui->fixed_speed(12.f));

    window->DrawList->AddRectFilled(rect.Max - ImVec2(5, 31 - state.offset), rect.Max - ImVec2(0, 0 + state.offset), gui->get_clr(state.rect), 30.f);
    gui->render_text_clipped(set->poppins_widget, rect.Min, rect.Max, gui->get_clr(state.text), label, { 0.0, 0.5 });

    return pressed;
}

bool selectable(const char* label, bool* p_selected)
{
    if (gui->selectable_ex(label, *p_selected))
    {
        *p_selected = !*p_selected;
        return true;
    }
    return false;
}

bool begin_combo(const char* label, const char* preview_value, int val, int max_count, ImGuiComboFlags flags, bool multi)
{
    struct combo_state
    {
        ImVec4 text = clr->text;
        bool combo_opened = false, hovered = false;
        float alpha = 0.f, offset = 0.f;
    };

    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();

    ImGuiNextWindowDataFlags backup_next_window_data_flags = g.NextWindowData.Flags;
    g.NextWindowData.ClearFlags();
    if (window->SkipItems) return false;

    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    IM_ASSERT((flags & (ImGuiComboFlags_NoArrowButton | ImGuiComboFlags_NoPreview)) != (ImGuiComboFlags_NoArrowButton | ImGuiComboFlags_NoPreview));
    if (flags & ImGuiComboFlags_WidthFitPreview) IM_ASSERT((flags & (ImGuiComboFlags_NoPreview | ImGuiComboFlags_CustomPreview)) == 0);

    if (val > max_count) val = max_count;
    const ImVec2 pos = window->DC.CursorPos;
    const float width = GetContentRegionMax().x - g.Style.WindowPadding.x;
    const ImRect rect(pos, pos + ImVec2(width, 25));

    const float combo_box_w = 145.0f;
    const ImRect combo(pos + ImVec2(0, (25 - 18) / 2), pos + ImVec2(width, (25 + 18) / 2));
    const ImRect open_combo(pos + ImVec2(width - combo_box_w, 0), pos + ImVec2(width, 25));

    ItemSize(rect, 0.f);
    if (!ItemAdd(rect, id)) return false;

    bool hovered, held, pressed = ButtonBehavior(rect, id, &hovered, &held);

    static std::map<ImGuiID, combo_state> anim;
    combo_state& state = anim[id];

    if ((hovered && g.IO.MouseClicked[0]) || (state.combo_opened && g.IO.MouseClicked[0] && !state.hovered)) state.combo_opened = !state.combo_opened;

    state.text = ImLerp(state.text, (state.combo_opened || hovered) ? clr->text_active : clr->text, gui->fixed_speed(14.f));
    state.alpha = ImClamp(state.alpha + (gui->fixed_speed(10.f) * (state.combo_opened ? 1.f : -1.f)), 0.f, 1.f);
    state.offset = ImLerp(state.offset, state.combo_opened ? 18.f : 0.f, gui->fixed_speed(12.f));

    if (!IsRectVisible(rect.Min, rect.Max + ImVec2(0, 2)))
    {
        state.combo_opened = false;
        state.alpha = 0.f;
    }

    gui->render_text_clipped(set->poppins_widget, open_combo.Min, open_combo.Max - ImVec2(18, 0), gui->get_clr(clr->text_active), preview_value ? preview_value : "None", {1.0, 0.5});
    gui->render_text_clipped(set->icon_micro, open_combo.Max - ImVec2(16, 25), open_combo.Max, gui->get_clr(clr->accent_color), "K", { 0.5, 0.5 });

    gui->render_text_clipped(set->poppins_widget, rect.Min, ImVec2(open_combo.Min.x - 8, rect.Max.y), gui->get_clr(state.text), label, {0.0, 0.5});

    if (!state.combo_opened && state.alpha < 0.1f) return false;

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoFocusOnAppearing;

    PushStyleVar(ImGuiStyleVar_Alpha, state.alpha);
    PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 0));
    PushStyleVar(ImGuiStyleVar_WindowRounding, set->combo_rounding);
    PushStyleColor(ImGuiCol_WindowBg, gui->get_clr(clr->combo_color));
    const float popup_w = ImMax(open_combo.GetWidth(), 210.0f);
    SetNextWindowPos(open_combo.GetBL() - ImVec2(popup_w - open_combo.GetWidth(), (state.offset + calc_combo_size(val, 31.f)) / 2));
    SetNextWindowSize(ImVec2(popup_w, calc_combo_size(val, 31.f)));
    Begin(label, NULL, window_flags);
    {
        SetCursorPosX(GetCursorPosX() + set->combo_spacing);
        BeginGroup();
        state.hovered = IsWindowHovered();

        if (!multi)
            if (IsWindowHovered() && g.IO.MouseClicked[0]) state.combo_opened = false;
    }
    return true;
}

void end_combo()
{
    EndGroup();
    PopStyleColor();
    PopStyleVar(4);
    ImGui::End();
}

bool combo(const char* label, int* current_item, const char* (*getter)(void* user_data, int idx), void* user_data, int items_count, int popup_max_height_in_items, int max_count)
{
    if (!current_item || items_count <= 0) return false;
    ImGuiContext& g = *GImGui;

    if (*current_item < 0) *current_item = 0;
    if (*current_item >= items_count) *current_item = items_count - 1;

    const char* preview_value = getter(user_data, *current_item);
    if (!preview_value) preview_value = "None";

    if (popup_max_height_in_items != -1 && !(g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint))
        SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, calc_combo_size(popup_max_height_in_items, 31.f)));

    if (!begin_combo(label, preview_value, items_count, max_count, ImGuiComboFlags_None, false)) return false;

    bool value_changed = false;
    for (int i = 0; i < items_count; i++)
    {
        const char* item_text = getter(user_data, i);
        if (item_text == NULL) item_text = "*Unknown item*";

        PushID(i);
        const bool item_selected = (i == *current_item);
        if (gui->selectable_ex(item_text, item_selected) && *current_item != i)
        {
            value_changed = true;
            *current_item = i;
        }
        if (item_selected) SetItemDefaultFocus();
        PopID();
    }

    end_combo();

    if (value_changed) MarkItemEdited(g.LastItemData.ID);

    return value_changed;
}

bool c_widgets::dropdown(std::string_view label, int* current_item, const char* const items[], int items_count, int max_count)
{
    const bool value_changed = combo(label.data(), current_item, items_array_getter, (void*)items, items_count, -1, max_count);
    return value_changed;
}

void c_widgets::multi_dropdown(std::string_view label, bool variable[], const char* labels[], int count, int max_count)
{
    ImGuiContext& g = *GImGui;

    std::string preview = "Select";

    for (auto i = 0, j = 0; i < count; i++)
    {
        if (variable[i])
        {
            if (j)
                preview += (", ") + (std::string)labels[i];
            else
                preview = labels[i];

            j++;
        }
    }

    if (begin_combo(label.data(), preview.c_str(), count, max_count, 0, true))
    {
        for (auto i = 0; i < count; i++)
            selectable(labels[i], &variable[i]);
        end_combo();
    }

    preview = ("Select");
}

const char* keys[] =
{
    "-",
    "Mouse 1",
    "Mouse 2",
    "CN",
    "Mouse 3",
    "Mouse 4",
    "Mouse 5",
    "-",
    "Back",
    "Tab",
    "-",
    "-",
    "CLR",
    "Enter",
    "-",
    "-",
    "Shift",
    "CTL",
    "Menu",
    "Pause",
    "Caps Lock",
    "KAN",
    "-",
    "JUN",
    "FIN",
    "KAN",
    "-",
    "Escape",
    "CON",
    "NCO",
    "ACC",
    "MAD",
    "Space",
    "PGU",
    "PGD",
    "End",
    "Home",
    "Left",
    "Up",
    "Right",
    "Down",
    "SEL",
    "PRI",
    "EXE",
    "PRI",
    "INS",
    "Delete",
    "HEL",
    "0",
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "A",
    "B",
    "C",
    "D",
    "E",
    "F",
    "G",
    "H",
    "I",
    "J",
    "K",
    "L",
    "M",
    "N",
    "O",
    "P",
    "Q",
    "R",
    "S",
    "T",
    "U",
    "V",
    "W",
    "X",
    "Y",
    "Z",
    "WIN",
    "WIN",
    "APP",
    "-",
    "SLE",
    "Numpad 0",
    "Numpad 1",
    "Numpad 2",
    "Numpad 3",
    "Numpad 4",
    "Numpad 5",
    "Numpad 6",
    "Numpad 7",
    "Numpad 8",
    "Numpad 9",
    "MUL",
    "ADD",
    "SEP",
    "MIN",
    "Delete",
    "DIV",
    "F1",
    "F2",
    "F3",
    "F4",
    "F5",
    "F6",
    "F7",
    "F8",
    "F9",
    "F10",
    "F11",
    "F12",
    "F13",
    "F14",
    "F15",
    "F16",
    "F17",
    "F18",
    "F19",
    "F20",
    "F21",
    "F22",
    "F23",
    "F24",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "NUM",
    "SCR",
    "EQU",
    "MAS",
    "TOY",
    "OYA",
    "OYA",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "Shift",
    "Shift",
    "Ctrl",
    "Ctrl",
    "Alt",
    "Alt"
};

bool c_widgets::keybind(const char* label, int* key)
{
    struct keybind_state
    {
        ImVec4 text = clr->text;
        float slow = 0;
    };

    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return false;

    ImGuiContext& g = *GImGui;
    ImGuiIO& io = g.IO;
    const ImGuiStyle& style = g.Style;

    const ImGuiID id = window->GetID(label);

    const ImVec2 pos = window->DC.CursorPos;
    const float width = GetContentRegionMax().x - g.Style.WindowPadding.x;
    const ImRect rect(pos, pos + ImVec2(width, 25));

    ImGui::ItemSize(rect, 0.f);
    if (!ImGui::ItemAdd(rect, id, &rect))  return false;

    char buf_display[64] = "None";

    bool value_changed = false;
    int k = *key;

    std::string active_key = "";
    active_key += keys[*key];

    if (*key != 0 && g.ActiveId != id) {
        strcpy_s(buf_display, active_key.c_str());
    }
    else if (g.ActiveId == id) {
        strcpy_s(buf_display, "...");
    }

    bool hovered = ItemHoverable(rect, id, 0);

    if (hovered && io.MouseClicked[0])
    {
        if (g.ActiveId != id) {
            memset(io.MouseDown, 0, sizeof(io.MouseDown));
            memset(io.KeysDown, 0, sizeof(io.KeysDown));
            *key = 0;
        }
        ImGui::SetActiveID(id, window);
        ImGui::FocusWindow(window);
    }
    else if (io.MouseClicked[0]) {

        if (g.ActiveId == id) ImGui::ClearActiveID();
    }

    if (g.ActiveId == id) {
        for (auto i = 0; i < 5; i++) {
            if (io.MouseDown[i]) {
                switch (i) {
                case 0:
                    k = 0x01;
                    break;
                case 1:
                    k = 0x02;
                    break;
                case 2:
                    k = 0x04;
                    break;
                case 3:
                    k = 0x05;
                    break;
                case 4:
                    k = 0x06;
                    break;
                }
                value_changed = true;
                ImGui::ClearActiveID();
            }
        }
        if (!value_changed) {
            for (auto i = 0x08; i <= 0xA5; i++) {
                if (io.KeysDown[i]) {
                    k = i;
                    value_changed = true;
                    ImGui::ClearActiveID();
                }
            }
        }

        if (IsKeyPressedMap(ImGuiKey_Escape)) {
            *key = 0;
            ImGui::ClearActiveID();
        }
        else {
            *key = k;
        }
    }

    static std::map<ImGuiID, keybind_state> anim;
    keybind_state& state = anim[id];

    ImRect clickable(ImVec2(rect.Max.x - (CalcTextSize(buf_display).x + 20), rect.Min.y), rect.Max);

    state.slow = ImLerp(state.slow, clickable.Min.x - rect.Min.x, fixed_speed(20.f));
    state.text = ImLerp(state.text, g.ActiveId == id ? clr->text_active : clr->text, g.IO.DeltaTime * 6.f);

    window->DrawList->AddRectFilled(ImVec2(state.slow + rect.Min.x, clickable.Min.y), clickable.Max, GetColorU32(clr->keybind_background), set->keybind_rounding);

    gui->render_text_clipped(set->poppins_widget, clickable.Min, clickable.Max, gui->get_clr(clr->text_active), buf_display, { 0.5, 0.5 });
    gui->render_text_clipped(set->poppins_widget, rect.Min, ImVec2(clickable.Min.x - 8, rect.Max.y), gui->get_clr(state.text), label, { 0.0, 0.5 });

    return value_changed;
}

bool c_widgets::config_panel(std::string_view name, std::string_view modified, std::string_view author, int config_id, int& config_variable, const ImVec2& size)
{
    struct config_state
    {
        ImVec4 background = clr->combo_color;
        float opticaly = 1.f;
    };

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems) return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(name.data());
    const ImVec2 pos = window->DC.CursorPos;
    const bool selected = config_id == config_variable;

    static std::map<ImGuiID, config_state> anim;
    config_state& state = anim[id];

    const ImRect rect(pos, pos + size);

    ItemSize(rect, 0);
    if (!ItemAdd(rect, id)) return false;

    bool hovered, held, pressed = ButtonBehavior(rect, id, &hovered, &held);
    if (pressed) config_variable = config_id;

    state.background = ImLerp(state.background, selected ? clr->accent_color : clr->element_background, fixed_speed(15.f));

    window->DrawList->AddRectFilled(rect.Min, rect.Max, get_clr(clr->child_top_bar), set->selection_rounding);
    window->DrawList->AddRect(rect.Min, rect.Max, get_clr(selected ? clr->accent_color : clr->child_stroke_bar), set->selection_rounding, 0, selected ? 1.5f : 1.0f);

    if (selected)
    {

        window->DrawList->AddRectFilled(rect.Min + ImVec2(0, 10), rect.Min + ImVec2(4, size.y - 10), get_clr(clr->accent_color), 2.0f);
    }

    const ImVec2 badge_min = rect.Min + ImVec2(10, (size.y - 36) * 0.5f);
    const ImVec2 badge_max = badge_min + ImVec2(36, 36);
    window->DrawList->AddRectFilled(badge_min, badge_max, get_clr(clr->element_background), 8.0f);
    window->DrawList->AddRect(badge_min, badge_max, get_clr(clr->child_stroke_bar), 8.0f, 0, 1.0f);
    gui->render_text_clipped(set->icon_child, badge_min, badge_max, get_clr(clr->accent_color), "G", { 0.5, 0.5 });

    const float text_max_x = rect.Max.x - 136.0f;
    gui->render_text_clipped(set->poppins_selection, rect.Min + ImVec2(54, 9), ImVec2(text_max_x, rect.Min.y + 34), gui->get_clr(clr->text_active), name.data(), { 0.0, 0.5 });

    const ImU32 dot_clr = selected ? get_clr(clr->accent_color) : get_clr(clr->text, 0.6f);
    window->DrawList->AddCircleFilled(rect.Min + ImVec2(58, 44), 3.0f, dot_clr);
    gui->render_text_clipped(set->poppins_widget, rect.Min + ImVec2(66, 33), ImVec2(text_max_x, rect.Max.y), gui->get_clr(selected ? clr->accent_color : clr->text), selected ? "Active Profile" : "Ready to load", { 0.0, 0.5 });

    const ImVec2 del_min = ImVec2(rect.Max.x - 128, rect.Min.y + (size.y - 28) * 0.5f);
    const ImVec2 del_max = del_min + ImVec2(28, 28);
    const bool del_hover = ImRect(del_min, del_max).Contains(g.IO.MousePos);
    window->DrawList->AddRectFilled(del_min, del_max, del_hover ? IM_COL32(239, 68, 68, 40) : get_clr(clr->element_background), 6.0f);
    window->DrawList->AddRect(del_min, del_max, del_hover ? IM_COL32(239, 68, 68, 180) : get_clr(clr->child_stroke_bar), 6.0f, 0, 1.0f);
    gui->render_text_clipped(set->icon_cfg, del_min, del_max, del_hover ? IM_COL32(239, 68, 68, 255) : gui->get_clr(clr->text), "C", { 0.5, 0.5 });

    const ImVec2 save_min = ImVec2(rect.Max.x - 94, rect.Min.y + (size.y - 28) * 0.5f);
    const ImVec2 save_max = save_min + ImVec2(28, 28);
    const bool save_hover = ImRect(save_min, save_max).Contains(g.IO.MousePos);
    window->DrawList->AddRectFilled(save_min, save_max, save_hover ? get_clr(clr->selection_active) : get_clr(clr->element_background), 6.0f);
    window->DrawList->AddRect(save_min, save_max, save_hover ? get_clr(clr->accent_color) : get_clr(clr->child_stroke_bar), 6.0f, 0, 1.0f);
    gui->render_text_clipped(set->icon_cfg, save_min, save_max, save_hover ? gui->get_clr(clr->accent_color) : gui->get_clr(clr->text), "A", { 0.5, 0.5 });

    const ImVec2 load_min = ImVec2(rect.Max.x - 60, rect.Min.y + (size.y - 28) * 0.5f);
    const ImVec2 load_max = load_min + ImVec2(52, 28);
    const bool load_hover = ImRect(load_min, load_max).Contains(g.IO.MousePos);
    window->DrawList->AddRectFilled(load_min, load_max, selected ? get_clr(clr->accent_color) : (load_hover ? get_clr(clr->selection_active) : get_clr(clr->element_background)), 6.0f);
    window->DrawList->AddRect(load_min, load_max, (selected || load_hover) ? get_clr(clr->accent_color) : get_clr(clr->child_stroke_bar), 6.0f, 0, 1.0f);
    gui->render_text_clipped(set->poppins_widget, load_min, load_max, gui->get_clr(clr->text_active), "LOAD", { 0.5, 0.5 });

    if (g.IO.MouseClicked[0])
    {
        if (del_hover)
        {
            if (config_id < (int)set->selection_config.size())
            {
                notify->AddNotification("Profile Deleted", set->selection_config[config_id].c_str(), 2500, IM_COL32(239, 68, 68, 255));
                set->selection_config.erase(set->selection_config.begin() + config_id);
                if (config_variable >= (int)set->selection_config.size()) config_variable = (int)set->selection_config.size() - 1;
            }
        }
        else if (save_hover)
        {
            notify->AddNotification("Configuration", "Profile saved successfully.", 2500, gui->get_clr(clr->accent_color));
        }
        else if (load_hover)
        {
            config_variable = config_id;
            notify->AddNotification("Configuration", "Profile loaded.", 2500, gui->get_clr(clr->accent_color));
        }
    }

    return pressed;
}

void c_widgets::spectators(std::string name, std::vector<std::string> players, bool* visible)
{
    struct spectator_state
    {
        float alpha;
    };

    static std::map<ImGuiID, spectator_state> anim;
    spectator_state& state = anim[ImGui::GetID(name.c_str())];

    state.alpha = ImLerp(state.alpha, *visible ? 1.f : 0.f, fixed_speed(12.f));

    push_style_var(ImGuiStyleVar_Alpha, state.alpha);

    if (state.alpha >= 0.01f) {
        ImVec2 content_size;

        ImGui::SetNextWindowSize(ImVec2(250, content_size.y));
        gui->begin(name.c_str(), nullptr, set->window_flags);
        {

            gui->begin_child("D", "Spectators", ImVec2(GetContentRegionMax().x, 0));
            {
                for (int i = 0; i < players.size(); i++)
                {
                    gui->text_colored(gui->get_clr(clr->text_active), players[i]);
                    if (i < players.size() - 1) gui->separator();
                }
            }
            gui->end_child();

            content_size = ImGui::GetContentRegionMax();
        }
        gui->end();
    }
    pop_style_var();
}

