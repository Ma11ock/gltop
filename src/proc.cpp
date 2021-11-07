
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

using namespace std::string_literals;

gltop::Process gltop::Proctab::getNextProcess()
{
    // TODO search
    return gltop::Process(mTab.get());
}


