#include "imgui.h"

class c_colors
{
public:
    ImVec4 white_color = ImColor(255, 255, 255);
    ImVec4 black_color = ImColor(0, 0, 0);

    ImVec4 accent_color = ImColor(37, 99, 235);
    ImVec4 window_background = ImColor(8, 12, 20);

    ImVec4 selection_active = ImColor(18, 29, 52);
    ImVec4 selection_hover = ImColor(14, 22, 38);

    ImVec4 child_top_bar = ImColor(12, 19, 34);
    ImVec4 child_bottom_bar = ImColor(9, 14, 24);
    ImVec4 child_stroke_bar = ImColor(30, 44, 70);

    ImVec4 element_background = ImColor(13, 22, 38);
    ImVec4 keybind_background = ImColor(12, 19, 34);

    ImVec4 combo_color = ImColor(12, 19, 34);
    ImVec4 separator = ImColor(22, 36, 62);

    ImVec4 text_active = ImColor(248, 250, 255);
    ImVec4 text = ImColor(130, 150, 185);

};

inline c_colors* clr = new c_colors();

