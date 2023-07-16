/*
** EPITECH PROJECT, 2023
** LibsoundioExp
** File description:
** Micro
*/

#include "node/Micro.hpp"

namespace synthetizer {

    void Micro::render() {
        ImGui::SetNextWindowSize(ImVec2(250, -1));
        std::string name = "Micro " + std::to_string(__id);
        Sample __copy = __cache;
        ImGui::Begin(name.c_str(), NULL, flags);
        {
            IO<Sample>("Output", false, &data, &back, &instance);
            ImGui::SeparatorText("Preview");
            std::vector<float> subFrames;
            for (auto &f : __copy) {
                subFrames.push_back(f);
            }
            while (subFrames.size() < 4096) {
                subFrames.push_back(0);
            }
            while (subFrames.size() > 4096) {
                subFrames.erase(subFrames.begin());
            }
            ImGui::PlotLines("##Micro", subFrames.data(), subFrames.size(), 0, NULL, -1.0f, 1.0f, ImVec2(-1, 50));
        }
        ImGui::End();
    }

    void Micro::inputFrames(const std::vector<float> &frames) {
        data->clear();
        for (auto &_ : frames) {
            if (__cache.empty()) {
                data->push_back(0);
                continue;
            }
            data->push_back(__cache.front());
            __cache.erase(__cache.begin());
        }
    }

    void Micro::feedInput(const std::vector<float> &frames) {
        __cache.insert(__cache.end(), frames.begin(), frames.end());
        while (__cache.size() > 4096) {
            __cache.erase(__cache.begin());
        }
    }

    int Micro::id = 0;

}
