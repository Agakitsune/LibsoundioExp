/*
** EPITECH PROJECT, 2023
** LibsoundioExp
** File description:
** Wave
*/

#pragma once

#include <numbers>
#include <functional>

#include "node/Node.hpp"

namespace synthetizer {

    float none(float hertz, float t);
    float wave(float hertz, float t);
    float saw(float hertz, float t);
    float square(float hertz, float t);
    float triangle(float hertz, float t);
    float noise(float hertz, float t);

    class Wave : public Node {
        static int id;
        Field<Sample> data;
        Field<ImVec2> back;

        std::function<float(float, float)> waveFunction = none;
        float hertz = 440.0f;
        int semiTone = 0;

        int select = 0;

        int __id;

        public:
            Wave();
            virtual ~Wave() override = default;

            virtual void render() override;
            virtual void inputFrames(const std::vector<float> &frames) override;

    };

}
