/*
** EPITECH PROJECT, 2023
** LibsoundioExp
** File description:
** Utils
*/

#include "Utils.hpp"

ImVec2 operator+(const ImVec2 &lhs, const ImVec2 &rhs) {
    return {lhs.x + rhs.x, lhs.y + rhs.y};
}

ImVec2 operator-(const ImVec2 &lhs, const ImVec2 &rhs) {
    return {lhs.x - rhs.x, lhs.y - rhs.y};
}

ImVec2 operator*(const ImVec2 &lhs, const float &rhs) {
    return {lhs.x * rhs, lhs.y * rhs};
}

ImVec2 operator*(const float &lhs, const ImVec2 &rhs) {
    return {lhs * rhs.x, lhs * rhs.y};
}

ImVec2 operator/(const ImVec2 &lhs, const float &rhs) {
    return {lhs.x / rhs, lhs.y / rhs};
}

ImVec2 operator/(const float &lhs, const ImVec2 &rhs) {
    return {lhs / rhs.x, lhs / rhs.y};
}

template<>
std::unique_ptr<synthetizer::AsyncVector<float>> synthetizer::AsyncVector<float>::instance = nullptr;
