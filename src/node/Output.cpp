/*
** EPITECH PROJECT, 2023
** LibsoundioExp
** File description:
** Output
*/

#include "node/Output.hpp"

namespace synthetizer {

    void Output::render() {
        ImGui::SetNextWindowSize(ImVec2(150, 150));
        ImGui::Begin("Output", NULL, flags);
        {
            ImGui::Text("Output");
            IO<Sample>("Input", true, &data, &back, &src);
        }
        ImGui::End();
    }

    void Output::inputFrames(const std::vector<float> &frames) {
        if (src) {
            (*src.get())->inputFrames(frames);
        }
    }

    Sample *Output::getSample() {
        if (data.hasValue()) {
            Sample *sample = data.get();
            return sample;
        } else {
            return nullptr;
        }
    }

}
