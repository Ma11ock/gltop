#include "util.hpp"

#include <string>
#include <cstdint>
using namespace std::chrono_literals;
using namespace std::string_literals;
namespace chron = std::chrono;
namespace fs = std::filesystem;

gltop::Timer::durationRep gltop::Timer::internalElapseAnimate(float input)
{
    auto now = sysClock::now();
    auto elapsed = chron::duration_cast<duration>(now - mLastTime);

    if(mCallback)
        mCallback(input);

    if(elapsed > mInterval)
        mLastTime = now;

    return elapsed.count();
}

gltop::Timer::durationRep gltop::Timer::internalElapse(float input)
{
    auto now = sysClock::now();
    auto elapsed = chron::duration_cast<duration>(now - mLastTime);
    auto result = elapsed.count();

    if(mCallback && (elapsed >= mInterval))
    {
        mCallback(input);
        mLastTime = now;
    }

    return result;
}

gltop::Timer::durationRep gltop::Timer::internalElapseAll(float input)
{
    auto now = sysClock::now();
    auto elapsed = chron::duration_cast<duration>(now - mLastTime);
    auto result = elapsed;

    while(elapsed >= mInterval)
    {
        if(mCallback)
            mCallback(input);
        elapsed -= mInterval;
    }

    if(result > mInterval)
        mLastTime = now;

    return result.count();
}


std::vector<float> gltop::loadObjFile(const fs::path &path)
{
    struct vertex
    {
        float x;
        float y;
        float z;
    };

    struct normal
    {
        float x;
        float y;
        float z;
    };
}
