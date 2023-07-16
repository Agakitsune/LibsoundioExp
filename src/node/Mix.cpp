/*
** EPITECH PROJECT, 2023
** LibsoundioExp
** File description:
** Mix
*/

#include "node/Mix.hpp"

namespace synthetizer {

    float mix(float a, float b, float mix) {
        return a * (1.0f - mix) + b * mix;
    }

    void Mix::render() {
        ImGui::SetNextWindowSize(ImVec2(200, -1));
        std::string name = "Mix " + std::to_string(__id);
        ImGui::Begin(name.c_str(), NULL, flags);
        {
            IO<Sample>("Source", true, &in, &inBack, &inNode);
            IO<Sample>("Source", true, &in2, &in2Back, &in2Node);
            ImGui::SeparatorText("Mix");
            ImGui::SliderFloat("##Mix", &_mix, 0.0f, 1.0f);
            ImGui::Separator();
            IO<Sample>("Output", false, &output, &outBack, &instance);
        }
        ImGui::End();

    }

    void Mix::inputFrames(const std::vector<float> &frames) {
        output->clear();
        if (inNode && in2Node) {
            (*inNode.get())->inputFrames(frames);
            (*in2Node.get())->inputFrames(frames);
            for (int i = 0; i < frames.size(); i++) {
                output->push_back(mix(in->at(i), in2->at(i), _mix));
            }
        }
    }

    int Mix::id = 0;

}
