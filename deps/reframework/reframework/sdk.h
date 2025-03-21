#pragma once

#include "reframework/API.hpp"

using API = reframework::API;

struct ValueType {
    std::vector<uint8_t> data{};
    API::TypeDefinition *type{nullptr};

    ValueType(API::TypeDefinition *t) : type(t) {
        if (type != nullptr) {
            data.resize(type->get_size());
        }
    }

    ValueType(API::TypeDefinition *t, void *addr) : type(t) {
        if (t != nullptr && addr != nullptr) {
            uint8_t *raw_data = reinterpret_cast<uint8_t *>(addr);
            data.reserve(t->get_size());
            data.insert(data.begin(), raw_data, raw_data + t->get_size());
        }
    }

    ValueType(const void *raw_data, size_t raw_data_size) {
        if (raw_data_size > 0 && raw_data != nullptr) {
            data.resize(raw_data_size);
            memcpy(data.data(), raw_data, raw_data_size);
        }
    }

    template <typename T> bool is_valid_offset(int32_t offset, T &value) const {
        return offset >= 0 && offset + sizeof(T) <= (int32_t)data.size();
    }

    bool is_valid_offset(int32_t offset) const {
        return offset >= 0 && offset <= (int32_t)data.size();
    }

    template <typename T> void write_memory(int32_t offset, T value) {
        if (!is_valid_offset(offset, value)) {
            return;
        }

        *(T *)((uintptr_t)data.data() + offset) = value;
    }

    template <typename T> T read_memory(int32_t offset) {
        if (!is_valid_offset(offset)) {
            return {};
        }

        return *(T *)((uintptr_t)data.data() + offset);
    }

    uintptr_t address() const { return (uintptr_t)data.data(); }
};
