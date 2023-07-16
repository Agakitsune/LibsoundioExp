/*
** EPITECH PROJECT, 2023
** LibsoundioExp
** File description:
** IO
*/

#pragma once

#include <iostream>
#include <algorithm>

#include "imgui.h"
#include "imgui_internal.h"

#include "Utils.hpp"
#include "node/io/Field.hpp"
#include "node/io/Type.hpp"

namespace synthetizer {

    class Node;

    template<typename T>
    struct IOPack {
        Field<T> *data;
        Field<ImVec2> *link;
        Field<Node*> *node;
    };

    static void curveLink(ImVec2 src, ImVec2 dst, ImU32 color, bool dynamic = false) {
        ImVec2 offset = {100, 0};

        if (dynamic) {
            offset.y = 0;
            if (src.x > dst.x)
                offset.x = std::clamp<float>(src.x - dst.x, 0, 100);
            else
                offset.x = std::clamp<float>(src.x - dst.x, -100, 0);
        } else {
            if (src.x > dst.x) {
                offset.x = 100;
                offset.y = 0;
            } else {
                offset.x = 100 + (dst.x - src.x);
                offset.y = 0;
            }
        }

        ImGui::GetForegroundDrawList()->AddBezierCubic(src, src - offset, dst + offset, dst, color, 3);
    }

    template<typename T>
    struct IOColor;

    template<>
    struct IOColor<float> {
        static const ImU32 color = IM_COL32(217, 87, 99, 255);
    };

    template<>
    struct IOColor<Sample> {
        static const ImU32 color = IM_COL32(158, 161, 212, 255);
    };

    template<typename T>
    concept IOColorable = requires { IOColor<T>::color; };

    template<typename T>
    requires IOColorable<T>
    void IO(const char *label,
            bool input,
            Field<T> *data,
            Field<ImVec2> *link,
            Field<Node*> *node
    ) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems)
            return;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);
        const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

        const float square_sz = ImGui::GetFrameHeight();
        ImVec2 pos = window->DC.CursorPos;
        if (!input)
            pos.x = pos.x + (ImGui::GetWindowWidth() - (label_size.x + square_sz + style.FramePadding.x * 2.0f + style.ItemInnerSpacing.x * 2.0));
        const ImRect total_bb(pos, pos + ImVec2(square_sz + (label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f), label_size.y + style.FramePadding.y * 2.0f));
        ImGui::ItemSize(total_bb, style.FramePadding.y);
        if (!ImGui::ItemAdd(total_bb, id))
        {
            IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
            return;
        }

        ImGuiIO &guiIo = ImGui::GetIO();

        bool hovered, held;
        bool pressed = ImGui::ButtonBehavior(total_bb, id, &hovered, &held);
        if (pressed) {
            ImGui::MarkItemEdited(id);
        }

        if (input) {
            const ImRect check_bb(pos, pos + ImVec2(square_sz, square_sz));
            ImVec2 label_pos = ImVec2(check_bb.Max.x + style.ItemInnerSpacing.x, check_bb.Min.y + style.FramePadding.y);
            if (label_size.x > 0.0f)
                ImGui::RenderText(label_pos, label);

            window->DrawList->AddCircleFilled(check_bb.GetCenter(), square_sz / 3, IOColor<T>::color, 12);

            if (ImGui::IsItemActive()) {
                curveLink(check_bb.GetCenter(), guiIo.MousePos, IOColor<T>::color, true);
            }

            if (link->hasValue()) {
                curveLink(check_bb.GetCenter(), *link->get(), IOColor<T>::color);
            }
        } else {
            ImVec2 tmp = ImVec2(pos.x + label_size.x + style.ItemInnerSpacing.x, pos.y);
            const ImRect check_bb(tmp, tmp + ImVec2(square_sz, square_sz));
            ImVec2 label_pos = ImVec2(pos.x, check_bb.Min.y + style.FramePadding.y);
            if (label_size.x > 0.0f)
                ImGui::RenderText(label_pos, label);
            window->DrawList->AddCircleFilled(check_bb.GetCenter(), square_sz / 3, IOColor<T>::color, 12);

            if (ImGui::IsItemActive()) {
                curveLink(check_bb.GetCenter(), guiIo.MousePos, IOColor<T>::color, true);
            }

            *link->get() = check_bb.GetCenter();
        }

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
        {
            IOPack<T> pack = {data, link, node};
            ImGui::SetDragDropPayload("IO", &pack, sizeof(IOPack<T>));
            data->reset();
            link->reset();
            node->reset();
            ImGui::EndDragDropSource();
        }
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("IO"))
            {
                IM_ASSERT(payload->DataSize == sizeof(IOPack<T>));
                IOPack<T> pack = *(IOPack<T>*)payload->Data;
                if (input) {
                    *data = *pack.data;
                    *link = *pack.link;
                    *node = *pack.node;
                } else {
                    *pack.data = *data;
                    *pack.link = *link;
                    *pack.node = *node;
                }
            }
            ImGui::EndDragDropTarget();
        }
    }

}
