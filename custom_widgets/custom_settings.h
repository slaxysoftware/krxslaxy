#include "imgui.h"
#include <d3d11.h>
#include <string>
#include <vector>

class c_settings
{
public:

	DWORD popup_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoFocusOnAppearing;
	DWORD window_flags = ImGuiWindowFlags_NoDecoration;

	ImVec2 window_size = ImVec2(1060, 750);
	ImVec2 window_padding = ImVec2(0, 0);

	ImVec2 widgets_padding = ImVec2(14, 12);
	ImVec2 widgets_spacing = ImVec2(0, 8);

	bool watermark = false;
	bool show_menu = true;
	bool particle_cursor = false;
	bool particle_clicked = false;
	bool spectator_active = false;

	float window_rounding = 26.f;
	float window_border_size = 1.f;
	float window_scrollbar_size = 0.f;
	float progress = 50.f;

	int particle_gravity = 500;
	int particle_num = 10;

	float particle_color[4] = { 37 / 255.f, 99 / 255.f, 235 / 255.f, 1.f };

	int selection_count = 0;
	int sub_selection_count = 0;
	int selection_accept = 0;
	int config_count = 0;

	float selection_alpha = 1;
	float selection_add;
	float selection_width = 0.f;
	float menu_alpha = 1.f;

	char create_config[15] = { "" };
	std::vector<std::string> selection_config = { "Krx Rage Config", "Krx Legit Config", "Krx Semi Rage Config" };

	std::vector<const char*> selection_icons = { "A", "B", "C", "D", "E", "F", "G" };
	std::vector<const char*> selection_labels = { "Rage", "Anti Aim", "Outline", "Visuals", "World", "Misc", "Config" };
	std::vector<const char*> sub_selection = { "General", "Targeting", "Accuracy" };

	std::vector<std::string> watermark_list = { "KRX SLAXY", "PREMIUM", "DX11" };

	std::vector<std::string> spectator_list = { "Raxxys", "Sammill", "Johhnyy", "killstroy", "Anton Var", "Layla1970" };

	ImVec2 child_spacing = ImVec2(16, 12);

	float child_rounding = 14.f;
	float selection_rounding = 10.f;
	float checkbox_rounding = 5.f;
	float input_rounding = 8.f;
	float color_rounding = 4.f;
	float keybind_rounding = 4.f;
	float watermark_rounding = 8.f;
	float combo_rounding = 8.f;
	float content_size = 0;
	float combo_spacing = 10.f;

	ID3D11ShaderResourceView* background_texture = nullptr;
	ID3D11ShaderResourceView* logo = nullptr;

	ImFont* poppins_selection = nullptr;
	ImFont* poppins_widget = nullptr;
	ImFont* inter_logo = nullptr;
	ImFont* icon = nullptr;
	ImFont* icon_cfg = nullptr;
	ImFont* icon_child = nullptr;
	ImFont* icon_micro = nullptr;

};

inline c_settings* set = new c_settings();

class c_function
{
public:
	bool enable = true;
	bool auto_fire = true;
	bool silent_aim = false;
	bool auto_revolver = false;
	bool anti_step = true;
	bool draw_fov = true;
	bool visible_fov = false;
	bool head_safety_if_lethal = true;
	bool auto_step = true;
	bool auto_scope = false;

	bool autofire = true;
	bool hide_shots = true;
	bool double_tap = false;
	bool mode = false;
	bool teleport_boots = true;
    bool body_set[5] = { true, true, true, true, false };
	bool misses_walls = true;

	bool show_in_enable = true;
	bool show_in_fire = true;
	bool show_in_safety = true;
	bool show_in_modem = true;
	bool show_in_step = true;

	float silent_count_size = 0.5f;

	float backtracking = 100.f;
	float visible = 50.f;
	float auto_wall = 50.f;
	float picker_checkbox[4] = { 37 / 255.f, 99 / 255.f, 235 / 255.f, 1.f };

	float accent_color[4] = {
		37 / 255.f,
		99 / 255.f,
		235 / 255.f,
		1.f };

	int first_bullet_delay = 0;
	int after_kill = 500;
	int max_misses = 100;
	int visible_count = 1;
	int force_count = 1;
	int mark_count = 1;
	int conditions_count = 0;
	int body_count = 1;
	int hitchance = 50;

	int enable_key = 0;
	int mode_key = 0;

	int enable_fire = 0;
	int mode_fire = 0;

	int enable_safety = 0;
	int mode_safety = 0;

	int enable_modem = 0;
	int mode_modem = 0;

	int enable_step = 0;
	int mode_step = 0;

	const char* conditions_list[8] = { "Select", "List", "Status", "Test", "Bruh", "Struct", "Pack", "Hack" };
	const char* visible_list[3] = {"Default", "Selection", "Inactive"};
	const char* force_list[3] = { "Select", "Type", "Count" };
	const char* body_list[5] = { "Head", "Body", "Chest", "Neck", "Legs" };

	const char* mark_list[4] = { "Upper Left", "Upper Right", "Bottom Left", "Bottom Right" };

	char input[55] = {""};
};

inline c_function* func = new c_function();

