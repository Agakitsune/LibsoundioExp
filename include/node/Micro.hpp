/*
** EPITECH PROJECT, 2023
** LibsoundioExp
** File description:
** Micro
*/

#pragma once

#include "node/Node.hpp"

namespace synthetizer {

    class Micro : public Node {
        static int id;
        Sample __cache;

        Field<Sample> data;
        Field<ImVec2> back;

        int __id = 0;

        public:
            Micro() {
                __id = id++;
            }
            ~Micro() = default;

            virtual void render() override;
            virtual void inputFrames(const std::vector<float> &frames) override;

            void feedInput(const std::vector<float> &frames);

    };

}
