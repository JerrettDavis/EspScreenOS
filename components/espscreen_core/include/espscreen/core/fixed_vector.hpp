#pragma once
#include <array>
#include <cstddef>
#include <utility>

namespace espscreen::core {

template <typename T, std::size_t Capacity>
class FixedVector final {
public:
    using value_type = T;
    using iterator = T*;
    using const_iterator = const T*;

    [[nodiscard]] constexpr std::size_t size() const { return size_; }
    [[nodiscard]] static constexpr std::size_t capacity() { return Capacity; }
    [[nodiscard]] constexpr bool empty() const { return size_ == 0; }
    [[nodiscard]] constexpr bool full() const { return size_ == Capacity; }

    constexpr bool push_back(const T& value) {
        if (full()) return false;
        storage_[size_++] = value;
        return true;
    }

    constexpr bool push_back(T&& value) {
        if (full()) return false;
        storage_[size_++] = std::move(value);
        return true;
    }

    template <typename... Args>
    constexpr T* emplace_back(Args&&... args) {
        if (full()) return nullptr;
        storage_[size_] = T{std::forward<Args>(args)...};
        return &storage_[size_++];
    }

    constexpr bool erase(std::size_t index) {
        if (index >= size_) return false;
        for (std::size_t i = index; i + 1 < size_; ++i) storage_[i] = std::move(storage_[i + 1]);
        --size_;
        return true;
    }

    constexpr bool pop_back() {
        if (empty()) return false;
        --size_;
        return true;
    }

    constexpr void clear() { size_ = 0; }
    constexpr T& operator[](std::size_t index) { return storage_[index]; }
    constexpr const T& operator[](std::size_t index) const { return storage_[index]; }
    constexpr T& front() { return storage_[0]; }
    constexpr const T& front() const { return storage_[0]; }
    constexpr T& back() { return storage_[size_ - 1]; }
    constexpr const T& back() const { return storage_[size_ - 1]; }
    constexpr iterator begin() { return storage_.data(); }
    constexpr iterator end() { return storage_.data() + size_; }
    constexpr const_iterator begin() const { return storage_.data(); }
    constexpr const_iterator end() const { return storage_.data() + size_; }

private:
    std::array<T, Capacity> storage_{};
    std::size_t size_{0};
};

} // namespace espscreen::core
