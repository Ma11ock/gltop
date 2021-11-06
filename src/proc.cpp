
extern "C" {
#include <unistd.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <proc/procps.h>
#include <proc/readproc.h>
}

#include <cerrno>
#include <iostream>
#include <stdexcept>
#include <exception>
#include <vector>
#include <string>
#include <chrono>

#include "gltop.hpp"

using namespace std::chrono_literals;
using namespace std::string_literals;
namespace chron = std::chrono;

gltop::Process gltop::Proctab::getNextProcess()
{
    // TODO search
    return gltop::Process(mTab.get());
}


gltop::Timer::durationRep gltop::Timer::elapseAnimate()
{
    auto now = sysClock::now();
    auto elapsed = chron::duration_cast<duration>(now - mLastTime);

    if(mCallback)
        mCallback(elapsed.count());

    if(elapsed > mInterval)
    {
        elapsed = gltop::Timer::duration(0);
        mLastTime = now;
    }

    return elapsed.count();
}

gltop::Timer::durationRep gltop::Timer::elapse()
{
    auto now = sysClock::now();
    auto elapsed = chron::duration_cast<duration>(now - mLastTime);
    auto result = elapsed.count();

    if(mCallback && (elapsed >= mInterval))
    {
        mCallback(elapsed.count());
        elapsed = gltop::Timer::duration(0);
        mLastTime = now;
    }

    return result;
}

gltop::Timer::durationRep gltop::Timer::elapseAll()
{
    auto now = sysClock::now();
    auto elapsed = chron::duration_cast<duration>(now - mLastTime);
    auto result = elapsed;

    while(elapsed >= mInterval)
    {
        if(mCallback)
            mCallback(mInterval.count());
        elapsed -= mInterval;
    }

    if(result > mInterval)
    {
        elapsed = gltop::Timer::duration(0);
        mLastTime = now;
    }
    return result.count();
}

