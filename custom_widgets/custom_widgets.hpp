#include "imgui.h"
#include "imgui_internal.h"
#include "custom_colors.h"
#include "custom_settings.h"
#include <map>
#include <cmath>
#include <ctime>
#include <string>
#include <sstream>
#include <vector>
#include <chrono>
#include <vector>
#include <algorithm>

#include <random>

#define IMGUI_DEFINE_MATH_OPERATORS

class c_widgets
{
public:
    float                   fixed_speed(float speed) { return speed / ImGui::GetIO().Framerate; };

    ImU32                   get_clr(const ImVec4& col, float alpha = 1.f);

    void                    text_colored(const ImU32 col, std::string text);
    void                    set_cursor_pos(const ImVec2& local_pos);
    void                    begin_group();
    void                    end_group();
    void                    push_style_var(ImGuiStyleVar idx, const ImVec2& val);
    void                    push_style_var(ImGuiStyleVar idx, const float val);
    void                    pop_style_var(float num = 1);

    void                    sameline();
    void                    render_text_clipped(ImFont* font, const ImVec2& p_min, const ImVec2& p_max, ImU32 col, const char* text, const ImVec2& align);
    void                    end();
    void                    end_child();
    void                    begin_content();
    void                    end_content();
    void					add_rect_filled_multi_color(ImDrawList* draw, const ImVec2& p_min, const ImVec2& p_max, ImU32 col_upr_left, ImU32 col_upr_right, ImU32 col_bot_right, ImU32 col_bot_left, float rounding = 0.f, ImDrawFlags flags = 0);

    bool                    keybind(const char* label, int* key);

    bool                    dropdown(std::string_view label, int* current_item, const char* const items[], int items_count, int max_count = 5);
    void                    multi_dropdown(std::string_view label, bool variable[], const char* labels[], int count, int max_count = 5);

    void					water_mark(std::string name, std::vector<std::string> function, int corner_pos, bool* visible);
    void					spectators(std::string name, std::vector<std::string> players, bool* visible);

    void                    separator();

    bool                    begin_popup_w(std::string name, float size_x);
    void                    end_popup_w();

    bool                    selectable_ex(const char* label, bool active);
    bool                    sub_selection(std::string_view label, int selection_id, int& selection_variable, const ImVec2& size);
    bool                    button(std::string_view label, const ImVec2& size_arg);
    bool                    checkbox(std::string_view label, bool* callback, bool popup = false, int* key = 0, int* mode = 0, bool* show_in_binds = nullptr);
    bool                    checkbox_with_picker(std::string_view label, bool* callback, float col[3], ImGuiColorEditFlags flags);
    bool                    text_field(std::string_view label, std::string_view hint, const ImVec2& size, char* buf, size_t buf_size, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = 0, void* user_data = 0);
    bool                    color_edit(std::string_view label, float col[4], ImGuiColorEditFlags flags = 0, bool popup = false);
    bool                    begin(std::string_view name, bool* open, ImGuiWindowFlags flags = ImGuiWindowFlags_None);
    bool                    begin_child(std::string_view icon, std::string_view name, const ImVec2& size_arg = ImVec2(0, 0));
    bool                    slider_float(std::string_view label, float* v, float v_min, float v_max, const char* format = "%.2f", ImGuiSliderFlags flags = 0);
    bool                    slider_int(std::string_view label, int* v, int v_min, int v_max, const char* format = "%d", ImGuiSliderFlags flags = 0);
    bool                    selection(std::string_view icon_label, std::string_view label, int selection_id, int& selection_variable, const ImVec2& size);
    bool                    color_picker(const char* label, float col[4], ImGuiColorEditFlags flags, const float* ref_col);
    bool                    config_panel(std::string_view name, std::string_view modified, std::string_view author, int config_id, int& config_variable, const ImVec2& size);
};

inline c_widgets* gui = new c_widgets();

struct notify_struct {
    int id;
    std::string message, information;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point endTime;
    ImU32 color;
};

class c_notify {
public:
    c_notify() : notificationIdCounter(0) {}

    void AddNotification(const std::string& information, const std::string& message, int durationMs, ImU32 color) {
        auto now = std::chrono::steady_clock::now();
        auto endTime = now + std::chrono::milliseconds(durationMs);
        notifications.push_back({ notificationIdCounter++, information, message, now, endTime, color });
    }

    void DrawNotifications() {
        auto now = std::chrono::steady_clock::now();

        std::sort(notifications.begin(), notifications.end(),
            [now](const notify_struct& a, const notify_struct& b) -> bool {
                float durationA = std::chrono::duration_cast<std::chrono::milliseconds>(a.endTime - a.startTime).count();
                float elapsedA = std::chrono::duration_cast<std::chrono::milliseconds>(now - a.startTime).count();
                float percentageA = (durationA - elapsedA) / durationA;

                float durationB = std::chrono::duration_cast<std::chrono::milliseconds>(b.endTime - b.startTime).count();
                float elapsedB = std::chrono::duration_cast<std::chrono::milliseconds>(now - b.startTime).count();
                float percentageB = (durationB - elapsedB) / durationB;

                return percentageA < percentageB;
            }
        );

        int currentNotificationPosition = 0;

        for (auto& notification : notifications) {
            if (now < notification.endTime) {
                float duration = std::chrono::duration_cast<std::chrono::milliseconds>(notification.endTime - notification.startTime).count();
                float elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - notification.startTime).count();
                float percentage = (duration - elapsed) / duration * 100.0f;

                ShowNotification(currentNotificationPosition, notification.information, notification.message, percentage, notification.color);
                currentNotificationPosition++;
            }
        }

        notifications.erase(std::remove_if(notifications.begin(), notifications.end(),
            [now](const notify_struct& notification) { return now >= notification.endTime; }),
            notifications.end());
    }

private:
    std::vector<notify_struct> notifications;
    int notificationIdCounter;

    void ShowNotification(int position, const std::string& information, const std::string& message, float percentage, ImU32 color) {

        struct notifi_state
        {
            float offset;
        };

        static std::map<ImGuiID, notifi_state> anim;
        notifi_state& state = anim[position];

        float duePercentage = 100.0f - percentage;
        float alpha = percentage > 10.0f ? 1.0f : percentage / 10.0f;

        ImGui::SetNextWindowPos(ImVec2(5.f, 10 + (position * 80)), ImGuiCond_Always);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6.f);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);

        ImGui::Begin(("##NOTIFY" + std::to_string(position)).c_str(), nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground);
        {
            const ImVec2 pos = ImGui::GetWindowPos();
            const ImVec2 size = ImGui::GetWindowSize();
            ImDrawList* draw = ImGui::GetWindowDrawList();
            ImGuiStyle* style = &ImGui::GetStyle();

            {
                style->WindowPadding = set->window_padding;
                style->WindowBorderSize = set->window_border_size;
                style->WindowRounding = set->watermark_rounding;
            }

            {
                draw->AddRectFilled(pos + ImVec2(2, 0), pos + size, gui->get_clr(clr->combo_color, alpha), 4.f);

                draw->AddRectFilled(pos + ImVec2(0, size.y - 2), pos + ImVec2(size.x * (duePercentage / 100.0f), size.y), gui->get_clr(clr->accent_color, alpha), 0);

                gui->text_colored(gui->get_clr(clr->text_active), information.c_str());
                gui->text_colored(gui->get_clr(clr->text), message.c_str());
            }
        }
        ImGui::End();

        ImGui::PopStyleVar(3);
    }
};

inline c_notify* notify = new c_notify();

class c_particle {
public:
    struct Particle
    {
        float position[2];
        float velocity[2];
        float lifetime;
    };
    const float M_PI = 3.14159265359f;

    std::vector<Particle> particles;

    void EmitParticles(const ImVec2& cursorPos)
    {
        const float minSpeed = 100.0f;
        const float maxSpeed = 150.0f;
        const float minLifetime = 1.0f;
        const float maxLifetime = 2.0f;

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> speedDist(minSpeed, maxSpeed);
        std::uniform_real_distribution<float> lifetimeDist(minLifetime, maxLifetime);

        for (int i = 0; i < set->particle_num; ++i)
        {
            Particle particle;
            particle.position[0] = cursorPos.x;
            particle.position[1] = cursorPos.y;

            float angle = 2 * M_PI * (i / static_cast<float>(set->particle_num));
            particle.velocity[0] = cos(angle) * speedDist(gen);
            particle.velocity[1] = sin(angle) * speedDist(gen);

            particle.lifetime = lifetimeDist(gen);

            particles.push_back(particle);
        }
    }

    void UpdateParticles(float deltaTime)
    {
        const float damping = 0.98f;

        for (auto& particle : particles)
        {
            particle.velocity[1] += set->particle_gravity * deltaTime;
            particle.velocity[0] *= damping;
            particle.velocity[1] *= damping;

            particle.position[0] += particle.velocity[0] * deltaTime;
            particle.position[1] += particle.velocity[1] * deltaTime;

            float alpha = particle.lifetime / 2.0f;
            ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2(particle.position[0], particle.position[1]), 1.0f, gui->get_clr(ImColor(set->particle_color[0], set->particle_color[1], set->particle_color[2], alpha)));

            particle.lifetime -= deltaTime;
        }

        particles.erase(std::remove_if(particles.begin(), particles.end(),
            [](const Particle& p) { return p.lifetime <= 0.0f; }), particles.end());
    }
};

inline c_particle* particle = new c_particle();

