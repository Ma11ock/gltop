
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

#include "gltop.hpp"

static PROCTAB *proctab = nullptr;
static bool isInit = false;

gltop::Process gltop::Process::GetNextProcess()
{
    return GetNextProcess(proctab);
}

void gltop::initProc()
{
    isInit = true;
    proctab = openproc(PROC_FILLMEM | PROC_FILLUSR | PROC_FILLGRP | PROC_FILLARG);
    if(!proctab)
        throw std::runtime_error("Could not create a proctab");
}


void gltop::closeProc()
{
    closeproc(proctab);
}


bool gltop::wasInit()
{
    return isInit;
}
