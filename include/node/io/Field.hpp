/*
** EPITECH PROJECT, 2023
** LibsoundioExp
** File description:
** Field
*/

#pragma once

#include <memory>
#include <optional>
#include <vector>

namespace synthetizer {

    template<typename T>
    class Field {
        std::shared_ptr<T> data = nullptr;
        bool original = false;

        std::optional<T> defaultData;

        Field<T> *origin = nullptr;
        std::vector<Field<T>*> clones;

        public:
            Field(bool null = false) : original(true) {
                if (!null)
                    data = std::make_shared<T>();
            };

            Field(Field &field) {
                data = field.data;
                original = false;
                if (field.original)
                    origin = &field;
                else
                    origin = field.origin;
                origin->addClone(*this);
            }

            ~Field() = default;

            Field<T> &operator=(Field<T> &field) {
                data = field.data;
                original = false;
                if (field.original)
                    origin = &field;
                else
                    origin = field.origin;
                origin->addClone(*this);
                return *this;
            }

            void setValue(const T &value) {
                if (original) {
                    data = std::make_shared<T>(value);
                } else {
                    origin->setValue(value);
                }
            }

            void setDefault(T defaultData) {
                this->defaultData = defaultData;
            }

            void addClone(Field<T> &field) {
                clones.push_back(&field);
            }

            void reset() {
                if (original) {
                    for (auto &clone : clones) {
                        clone->data.reset();
                    }
                } else {
                    origin->reset();
                }
            }

            T *get() {
                if (data == nullptr)
                    if (defaultData)
                        return &defaultData.value();
                    else
                        return nullptr;
                else
                    return data.get();
            }

            bool hasValue() {
                if (data == nullptr)
                    return static_cast<bool>(defaultData);
                else
                    return data != nullptr;
            }

            operator bool() {
                return hasValue();
            }

            T *operator->() {
                return get();
            }

    };

}
