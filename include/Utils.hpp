/*
** EPITECH PROJECT, 2023
** LibsoundioExp
** File description:
** Utils
*/

#pragma once

#include <mutex>
#include <vector>
#include <iostream>
#include <memory>

#include "imgui.h"

#include "node/io/Type.hpp"

ImVec2 operator+(const ImVec2 &lhs, const ImVec2 &rhs);
ImVec2 operator-(const ImVec2 &lhs, const ImVec2 &rhs);

ImVec2 operator*(const ImVec2 &lhs, const float &rhs);
ImVec2 operator*(const float &lhs, const ImVec2 &rhs);

ImVec2 operator/(const ImVec2 &lhs, const float &rhs);
ImVec2 operator/(const float &lhs, const ImVec2 &rhs);

namespace synthetizer {

    template<typename T>
    class AsyncVector : public std::vector<T> {
        using std::vector<T>::vector;
        std::mutex _lock;

        static std::unique_ptr<AsyncVector<T>> instance;

        public:
            AsyncVector() = default;
            ~AsyncVector() = default;

            void lock() {
                _lock.lock();
            }

            void unlock() {
                _lock.unlock();
            }

            void push_back(const T &value) {
                std::vector<T>::push_back(value);
            }

            bool empty() {
                return std::vector<T>::empty();
            }

            void clear() {
                std::vector<T>::clear();
            }

            size_t size() {
                return std::vector<T>::size();
            }

            std::vector<T>::iterator begin() {
                return std::vector<T>::begin();
            }

            std::vector<T>::iterator end() {
                return std::vector<T>::end();
            }

            static AsyncVector<T> &get() {
                if (!instance)
                    instance = std::make_unique<AsyncVector<T>>();
                return *instance;
            }
    };

}
