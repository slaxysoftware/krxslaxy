#pragma once
#include <Windows.h>
#include <string>
#include <vector>

namespace globals
{
    inline bool Watermark = true;

    namespace il {
        inline bool enable = false;
        inline int lock_agent = 0;
    }

    namespace aimbot {
        inline bool a1mbot = false;
        inline bool draw_f0v = false;
        inline bool reco1l_contr0l = false;
        inline bool spread_comp = false;
        inline bool autoshot = false;
        inline bool enable_360_fov = false;
        inline bool spreadempty_comp = false;
        inline bool pvc = false;
        inline bool v1sh_ch3ck = false;
        inline int  v1sh_ch3ck_k3y = 0;
        inline int  a1m_k3y = 0;
        inline float a1m_sm00th = 15.0f;
        inline float a1m_f0v = 70.0f;
        inline bool silent = false;
        inline bool auto_wall = false;
        inline bool auto_scope = false;
        inline bool aim_assist = false;
        inline bool vsr = false;
        inline float spread_comp_delay = 100.0f;
        inline float autoshoot_delay = 20.0f;
        inline float min_damage = 0.0f;
        inline float max_aim_distance = 50000.0f;
        inline int  target_selection = 0;
        inline int  hitbox = 0;
        inline int  auto_fire_mode = 0;
        inline bool ht23 = false;
    }

    namespace triggerbot {
        inline bool onne = false;
        inline bool v1sh_ch3ck = false;
        inline float tr1g_f0v = 5.0f;
    }

    namespace visuals {
        inline bool dormant = false;
        inline bool visiblecheck = false;
        inline bool box3d = false;
        inline bool box2d = false;
        inline bool dstc = false;
        inline bool snapl1ne = false;
        inline bool sk3let0n = false;
        inline bool agenticon = false;
        inline bool h3althbar = false;
        inline bool HealthText = false;
        inline bool agent_name = false;
        inline bool weaponesp = false;
        inline bool chinese_hat = false;
        inline bool chinese_hat_self = false;
        inline bool headb0x = false;
        inline bool chamsvsbd = false;
        inline bool chams = false;
        inline bool bullet_tracers = false;
        inline bool wireframe_weapon = false;
        inline bool rainbow_chams = false;
        inline bool outline_enabled = false;
        inline bool self_galaxy_enabled = false;
        inline bool dynamic_fresnel_outline = false;
        inline bool crystal_chams_enabled = false;
        inline bool galaxy_gun = false;
        inline bool self_wireframe = false;
        inline bool wireframe_hands = false;
        inline bool usepresetedoutlines = false;
        inline bool visible_check_ch = false;
        inline bool hand_outline = false;
        inline bool handchamsd2 = false;
        inline bool handchams2 = false;
        inline bool trail_effect_enabled = false;
        inline bool clone_enabled = false;
        inline float outlineintensityvisibleoutline = 200.0f;
        inline float outlineintensityinvisbleoutline = 200.0f;
        inline float intensityvisibleoutline = 18.2f;
        inline int  self_galaxy_preset = 0;
        inline int  crystal_chams_preset = 0;
        inline int  dynamic_fresnel_preset = 0;
        inline int  visiblepreset = 0;
        inline int  invisiblepreset = 0;
        inline bool nmpl = false;
        inline bool nmpl34 = false;
        inline bool nmpl2 = false;
        inline bool pclp = false;
        inline int  handchams_material_index1 = 0;
        inline float handchams_intensity = 1.0f;
        inline float handchams_color_r = 255.0f;
        inline float handchams_color_g = 255.0f;
        inline float handchams_color_b = 255.0f;
    }

    namespace misc {

        inline bool spinner = false;
        inline bool aa = false;
        inline bool fakeduck = false;
        inline bool jitter_move = false;
        inline float spinvalue = 15.0f;
        inline float fake_lag_ticks = 0.0f;
        inline float jitter_range = 45.0f;
        inline float desync_range = 60.0f;
        inline bool pitch_enabled = false;
        inline float pitch_value = 89.0f;
        inline float yaw_add = 0.0f;
        inline bool jitter_on_back = false;
        inline bool jitter_enabled = false;
        inline bool aa_lean_left = false;
        inline bool aa_lean_right = false;
        inline bool manual_aa = false;
        inline bool freestanding = false;
        inline bool atomic_aa = false;
        inline float atomic_speed = 1.0f;
        inline int  atomic_mode = 0;
        inline bool prediction_breaker = false;
        inline float breaker_intensity = 1.0f;
        inline int  aa_mode = 0;
        inline bool aa_spin = false;
        inline bool aa_jitter = false;
        inline bool aa_threeway = false;
        inline bool aa_desync = false;

        inline bool skybox = false;
        inline bool skyboxrgb = false;
        inline bool gdg = false;
        inline bool spktimer = false;
        inline bool cpp = false;
        inline bool world_esp = false;
        inline bool gadgets = false;
        inline int  skybox_preset_index = 0;
        inline float CloudSpeed = 1.0f;
        inline float StarsBrightness = 1.0f;
        inline float CloudOpacity = 1.0f;
        inline float SkySunRadius = 1.0f;
        inline float SkySunBrightness = 1.0f;
        inline float SkyNoisePower1 = 1.0f;
        inline float SkyNoisePower2 = 1.0f;
        inline float SkySunHeight = 0.0f;

        inline bool playerchamsself = false;
        inline bool sk1n_chang3r = false;
        inline bool customgun = false;
        inline bool rdy = false;
        inline bool rmsdw = false;
        inline bool BigGun3p = false;
        inline bool BigSelf = false;
        inline bool finisher = false;
        inline bool onlylastkill = false;
        inline bool killsays = false;
        inline bool chat_spammer = false;
        inline bool antiflash = false;
        inline bool bunnyhop = false;
        inline bool fastcrouch = false;
        inline bool disconnect_server = false;
        inline bool nld = false;
        inline bool aspect_ratio_enabled = false;
        inline bool tperson = false;
        inline bool FovChangor = false;
        inline float Fovchangerfloat = 100.0f;
        inline float aspect_ratio_value = 1.0f;
        inline float PlayerDistance = 100.0f;
        inline float ui_scale = 1.0f;
        inline int  kky = VK_INSERT;
        inline bool no_smoke = false;
        inline bool rpl = false;
        inline bool ht23 = false;
        inline bool stream_proof = false;
        inline std::string chat_message = "KRX on top";
    }

    inline bool test = false;
    inline bool test2 = false;
}

inline const std::vector<std::wstring> kHitboxOptions      = {L"Head", L"Neck", L"Chest", L"Random"};
inline const std::vector<std::wstring> kTargetOptions       = {L"Distance", L"Crosshair", L"Combined"};
inline const std::vector<std::wstring> kFireModeOptions     = {L"Manual", L"Automatic"};
inline const std::vector<std::wstring> kAAModeOptions       = {L"Normal", L"Random", L"Backwards", L"3-Way", L"Lag-jitter"};
inline const std::vector<std::wstring> kAtomicModeOptions   = {L"Normal", L"Inverter", L"Flicker"};
inline const std::vector<std::wstring> kGalaxyPresetOptions = {L"Galaxy", L"Green Fresnel", L"Purple Fresnel", L"Blue Fresnel"};
inline const std::vector<std::wstring> kCrystalPresetOpts   = {L"Option 1", L"Option 2", L"Option 3", L"Option 4", L"Option 5"};
inline const std::vector<std::wstring> kDynFresnelOpts      = {L"Green", L"Danger", L"Purple Burn", L"Yellow", L"Red", L"Burn"};
inline const std::vector<std::wstring> kVisiblePresets      = {L"Galaxy", L"Quantum Flux", L"Neon Mirage", L"Solar Flare",
                                                                L"Crystal Aura", L"Cyber Matrix", L"Arc Pulse", L"Magma Rush",
                                                                L"Bloom", L"Nova Blaze", L"Cosmic Dream", L"Stellar Burst"};
inline const std::vector<std::wstring> kInvisiblePresets    = {L"Shadow Veil", L"Inferno Rush", L"Volt Surge", L"Glacial Peak",
                                                                L"Crimson Moon", L"Ghost Shade", L"Wraith Flame", L"Venom Chill",
                                                                L"Crimson Default", L"Volcanic Rage", L"Electric Surge",
                                                                L"Arctic Blast", L"Blood Eclipse", L"Dark Lightning"};
inline const std::vector<std::wstring> kHandChamsOpts       = {L"Glow", L"Ghost", L"Red Devil", L"Holographic", L"Hell"};
inline const std::vector<std::wstring> kSkyboxPresets       = {L"Default", L"Night Blue", L"Galaxy", L"Neon",
                                                                L"Cyberpunk", L"Sunset", L"Emerald", L"Midnight"};

inline void save_config() {  }
inline void load_config() {  }

