
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
    auto now = gltop::Timer::now();
    mElapsed = (now - mLastTime);

    if(mCallback)
        mCallback(mElapsed.count());

    if(mElapsed > mInterval)
    {
        mElapsed = gltop::Timer::duration(0.f);
        mLastTime = now;
    }

    return mElapsed.count();
}

gltop::Timer::durationRep gltop::Timer::elapse()
{
    auto now = gltop::Timer::now();
    mElapsed = now - mLastTime;
    auto result = mElapsed.count();

    if(mCallback && (mElapsed >= mInterval))
    {
        mCallback(mElapsed.count());
        mElapsed = gltop::Timer::duration(0.f);
        mLastTime = now;
    }

    return result;
}

gltop::Timer::durationRep gltop::Timer::elapseAll()
{
    auto now = gltop::Timer::now();
    mElapsed = (now - mLastTime);
    auto result = mElapsed;

    while(mElapsed >= mInterval)
    {
        if(mCallback)
            mCallback(mInterval.count());
        mElapsed -= mInterval;
    }

    if(result > mInterval)
    {
        mElapsed = gltop::Timer::duration(0.f);
        mLastTime = now;
    }
    return result.count();
}
