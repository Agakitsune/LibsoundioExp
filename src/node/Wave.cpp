/*
** EPITECH PROJECT, 2023
** LibsoundioExp
** File description:
** Wave
*/

#include "imgui_internal.h"

#include "node/Wave.hpp"

namespace synthetizer {

    float none(float hertz, float t) {
        return 0.0f;
    }

    float wave(float hertz, float t) {
        return sinf(t * hertz * 2.0f * std::numbers::pi_v<float>);
    }

    float saw(float hertz, float t) {
        float period = 1.0f / hertz;
        float saw_t = fmodf(t, period) / period;
        return saw_t * 2.0f - 1.0f;
    }

    float square(float hertz, float t) {
        float period = 1.0f / hertz;
        float saw_t = fmodf(t, period) / period;
        return saw_t < 0.5f ? 1.0f : -1.0f;
    }

    float triangle(float hertz, float t) {
        float period = 1.0f / hertz;
        float saw_t = fmodf(t, period) / period;
        return saw_t < 0.5f ? saw_t * 4.0f - 1.0f : 3.0f - saw_t * 4.0f;
    }

    float noise(float hertz, float t) {
        return (float)rand() / (float)RAND_MAX * 2.0f - 1.0f;
    }

    Wave::Wave() {
        __id = id;
        ++id;
    }

    void Wave::render() {
        static std::vector<std::function<float(float, float)>> func = {
            std::function{none},
            std::function{saw},
            std::function{square},
            std::function{triangle},
            std::function{wave},
            std::function{noise}
        };

        static const char* items[] = {
            "none",
            "saw",
            "square",
            "triangle",
            "wave",
            "noise",
        };
        const char* combo_preview_value = items[select];

        ImGui::SetNextWindowSize(ImVec2(250, -1));
        std::string name = "Wave " + std::to_string(__id);
        ImGui::Begin(name.c_str(), NULL, flags);
        {
            if (ImGui::BeginCombo("Function", combo_preview_value)) {
                for (int n = 0; n < IM_ARRAYSIZE(items); ++n) {
                    const bool is_selected = (select == n);
                    if (ImGui::Selectable(items[n], is_selected)) {
                        select = n;
                        waveFunction = func[n];
                    }

                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            ImGui::Text("Hertz: %.2f", hertz); ImGui::SameLine();
            float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
            const float button_size = ImGui::GetFrameHeight();
            ImGui::PushButtonRepeat(true);
            {
                bool update = false;
                if (ImGui::Button("-12", ImVec2(button_size * 1.5, button_size))) {
                    semiTone -= 12;
                    update = true;
                }
                ImGui::SameLine(0.0f, spacing);
                if (ImGui::Button("-1", ImVec2(button_size, button_size))) {
                    semiTone--;
                    update = true;
                }
                ImGui::SameLine(0.0f, spacing);
                if (ImGui::Button("+1", ImVec2(button_size, button_size))) {
                    semiTone++;
                    update = true;
                }
                ImGui::SameLine(0.0f, spacing);
                if (ImGui::Button("+12", ImVec2(button_size * 1.5, button_size))) {
                    semiTone += 12;
                    update = true;
                }
                if (update)
                    hertz = 440.0f * powf(2.0f, semiTone / 12.0f);
            }
            ImGui::PopButtonRepeat();
            ImGui::SeparatorText("Preview");
            std::vector<float> subFrames;
            for (auto &frame : frames) {
                subFrames.push_back(waveFunction(hertz, frame));
            }
            ImGui::PlotLines("", subFrames.data(), subFrames.size(), 0, NULL, -1.0f, 1.0f, ImVec2(-1, 50));
            ImGui::Separator();
            IO<Sample>("Sample", false, &data, &back, &instance);
        }
        ImGui::End();

    }

    void Wave::inputFrames(const std::vector<float> &frames) {
        data->clear();
        for (auto &frame : frames) {
            data->push_back(waveFunction(hertz, frame));
        }
    }

    int Wave::id = 0;

}
