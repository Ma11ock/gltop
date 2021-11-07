#ifndef GLTOP_UTIL_HPP
#define GLTOP_UTIL_HPP

#include <chrono>
#include <cstdint>
#include <functional>
#include <filesystem>

namespace gltop
{
    // Timer class. All values in milliseconds, represented by floats.
    class Timer
    {
    public:
        using sysClock = std::chrono::high_resolution_clock;
        using duration = std::chrono::milliseconds;
        // Probably uint64_t.
        using durationRep = duration::rep;
        using timePoint = sysClock::time_point;

        // Constant: Unix time since the process started.
        static const inline timePoint START_TIME = sysClock::now();

        // 
        Timer(duration interval,
              std::function<void(durationRep)> callback = nullptr)
            : mInterval(interval),mLastTime(sysClock::now()),
              mCallback(callback)
        {
        }

        ~Timer() = default;

        // Get the elapsed time as a float.
        inline durationRep getElapsed() const
        {
            return (sysClock::now() - mLastTime).count();
        }

        // Do the animation. Return how many milliseconds since last call.
        durationRep elapseAnimate();

        // Call the callback only once and only if mInterval
        // milliseconds have elapsed.
        durationRep elapse();

        // Call the callback as many times as mInterval has passed.
        durationRep elapseAll();

        float elapsedTimeToFloat() const
        {
            namespace chron = std::chrono;
            using dDurationRep = double;
            using dDuration = std::chrono::duration<durationRep, std::milli>;
            using dTimePoint = std::chrono::time_point<sysClock, duration>;

            return
                static_cast<float>
                ((chron::time_point_cast<dDuration>(sysClock::now())
                  - chron::time_point_cast<dDuration>(mLastTime))
                 .count());
        }

    private:
        // mElapsed's max value. elapse() and elapseAll() will run if
        // mElapsed exceeds mInterval.
        duration mInterval;
        // The last time any of the elapse() functions ran.
        timePoint mLastTime;
        // Callback called by the elapse() functions. Can be NULL.
        std::function<void(durationRep)> mCallback;
    };

    std::vector<float> loadObjFile(const std::filesystem &path);
}

#endif /* GLTOP_UTIL_HPP */
