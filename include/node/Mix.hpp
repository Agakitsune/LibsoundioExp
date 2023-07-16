/*
** EPITECH PROJECT, 2023
** LibsoundioExp
** File description:
** Mix
*/

#pragma once

#include "node/Node.hpp"

namespace synthetizer {

    class Mix : public Node {
        static int id;
        Field<Sample> in = true;
        Field<Sample> in2 = true;
        Field<Sample> output;

        Field<ImVec2> inBack = true;
        Field<ImVec2> in2Back = true;
        Field<ImVec2> outBack;

        Field<Node*> inNode = true;
        Field<Node*> in2Node = true;

        int __id = 0;

        float _mix = 0.5f;

        public:
            Mix() {
                __id = id++;
            }
            ~Mix() = default;

            virtual void render() override;
            virtual void inputFrames(const std::vector<float> &frames) override;

    };

}
