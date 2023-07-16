/*
** EPITECH PROJECT, 2023
** LibsoundioExp
** File description:
** Node
*/

#pragma once

#include "imgui.h"

#include "node/io/IO.hpp"
#include "node/io/Field.hpp"
#include "Utils.hpp"

namespace synthetizer {

    class Node {
        protected:
            const ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
            std::vector<float> frames;

            Field<Node*> instance = true;

        public:
            Node() {
                instance.setValue(this);
            }
            virtual ~Node() = default;

            virtual void render() = 0;

            virtual void inputFrames(const Sample &frames) {};
            virtual void renderFrames(const Sample &frames) {
                this->frames = frames;
            };

    };

}
