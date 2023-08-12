#pragma once

#include <chrono>
#include <functional>

namespace core {

class AnimationBase {
public:
    AnimationBase(std::chrono::milliseconds animationDuration, std::function<float(float)> scale = nullptr)
        : startPoint(std::chrono::steady_clock::now())
        , duration(animationDuration)
        , scale(std::move(scale))
    {}

protected:
    std::chrono::steady_clock::time_point startPoint;
    std::chrono::milliseconds duration;
    std::function<float(float)> scale;
};

class LoopedAnimation : public AnimationBase {
public:
    using AnimationBase::AnimationBase;

    float progress() {
        using namespace std::chrono;
        auto now = steady_clock::now();
        auto interval = duration_cast<milliseconds>(now - startPoint) % duration;
        auto progress = static_cast<float>(interval.count()) / static_cast<float>(duration.count());
        if (scale)
            progress = scale(progress);
        return progress;
    }
};

} // namespace core
