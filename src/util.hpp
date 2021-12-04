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
        using callback = std::function<void(float)>;

        // Constant: Unix time since the process started.
        static const inline timePoint START_TIME = sysClock::now();

        // 
        Timer(duration interval,
              callback userCallback = nullptr)
            : mInterval(interval),mLastTime(sysClock::now()),
              mCallback(userCallback)
        {
        }

        ~Timer() = default;

        // Get the elapsed time as a float.
        inline durationRep getElapsed() const
        {
            // TODO duration cast
            return (std::chrono::duration_cast<duration>
                    (sysClock::now()
                     - mLastTime)).count();
        }

        inline float getElapsedTimeToFloat() const
        {
            namespace chron = std::chrono;
            using dDurationRep = double;
            using dDuration = chron::duration<durationRep, std::milli>;
            using dTimePoint = chron::time_point<sysClock, duration>;

            return
                static_cast<float>
                ((chron::time_point_cast<dDuration>(sysClock::now())
                  - chron::time_point_cast<dDuration>(mLastTime))
                 .count());
        }

        inline durationRep getNow() const
        {
            return std::chrono::duration_cast<duration>
                (sysClock::now() - mLastTime).count();
        }

        inline float getElapsedNormalized() const
        {
            // TODO maybe not getElapsed? maybe we dont need mLastTime here.
            auto ms = getNow() % mInterval.count();
            return static_cast<float>(ms) /
                static_cast<float>(mInterval.count());
        }

        inline float getInvervalFloat() const
        {
            return static_cast<float>(mInterval.count());
        }

        // Do the animation. Return how many milliseconds since last call.
        inline durationRep elapseAnimate()
        {
            return internalElapseAnimate(getElapsedTimeToFloat());
        }

        // Call the callback only once and only if mInterval
        // milliseconds have elapsed.
        inline durationRep elapse()
        {
            return internalElapse(getInvervalFloat());
        }

        // Call the callback as many times as mInterval has passed.
        inline durationRep elapseAll()
        {
            return internalElapseAll(getInvervalFloat());
        }

        // Do the animation. Return how many milliseconds since last call.
        inline durationRep elapseAnimateNormalized()
        {
            return internalElapseAnimate(getElapsedNormalized());
        }

        // Call the callback only once and only if mInterval
        // milliseconds have elapsed.
        inline durationRep elapseNormalized()
        {
            return internalElapse(1.f);
        }

        // Call the callback as many times as mInterval has passed.
        inline durationRep elapseAllNormalized()
        {
            return internalElapseAll(1.f);
        }

        inline void setLastTime()
        {
            mLastTime = sysClock::now();
        }

    private:
        // Do the animation. Return how many milliseconds since last call.
        durationRep internalElapseAnimate(float input);

        // Call the callback only once and only if mInterval
        // milliseconds have elapsed.
        durationRep internalElapse(float input);

        // Call the callback as many times as mInterval has passed.
        durationRep internalElapseAll(float input);

        // mElapsed's max value. elapse() and elapseAll() will run if
        // mElapsed exceeds mInterval.
        duration mInterval;
        // The last time any of the elapse() functions ran.
        timePoint mLastTime;
        // Callback called by the elapse() functions. Can be NULL.
        callback mCallback;
    };

    std::vector<float> loadObjFile(const std::filesystem::path &path);
}

#endif /* GLTOP_UTIL_HPP */
