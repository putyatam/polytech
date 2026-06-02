#pragma once
#include <functional>
#include <utility>

template<typename Signature>
class Action {
private:
    std::function<Signature> func;

public:
    Action() = default;
    Action(const Action& other) = default;

    Action(Action&& other) noexcept = default;

    template<typename F>
    requires (!std::is_same_v<std::decay_t<F>, Action<Signature>> && std::is_constructible_v<std::function<Signature>, F&&>)
    Action(F&& other) { 
        func = std::forward<F>(other);
    }

    Action& operator=(Action&& other) noexcept {
        if (this != &other) {
            func = std::move(other.func);
        }
        return *this;
    }

    template<typename... Args>
    auto operator()(Args&&... args) const {
        return func(std::forward<Args>(args)...);
    }

    explicit operator bool() const {
        return static_cast<bool>(func);
    }

    // Сброс
    void reset() {
        func = nullptr;
    }
};