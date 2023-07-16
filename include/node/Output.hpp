/*
** EPITECH PROJECT, 2023
** LibsoundioExp
** File description:
** Output
*/

#pragma once

#include <mutex>

#include "node/Node.hpp"

namespace synthetizer {

    class Output : public Node {
        Field<Sample> data = true;
        Field<ImVec2> back = true;

        std::mutex lock;

        Field<Node*> src = true;

        public:
            Output() = default;
            virtual ~Output() override = default;

            virtual void render() override;
            virtual void inputFrames(const std::vector<float> &frames) override;

            Sample *getSample();

    };

}
