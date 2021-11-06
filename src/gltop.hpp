#ifndef GLTOP_HPP
#define GLTOP_HPP

extern "C" {
#include <unistd.h>
#include <sys/types.h>
#include <sys/types.h>
#include <proc/procps.h>
#include <proc/readproc.h>
}

#include <memory>
#include <vector>
#include <string>
#include <chrono>
#include <functional>

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

    class Process
    {
    public:
        // Construct a new process (NULL).
        Process() : mProc(nullptr, &freeproc)
        {
        }

        // Construct a new process.
        Process(PROCTAB *proctab) : mProc(readproc(proctab, nullptr),
                                          &freeproc)
        {
        }

        // Destroy process.
        ~Process() = default;

        // Get task id (this is the PID for the main task).
        inline int getTID() const
        {
            return mProc->tid;
        }

        // Get Parent's PID.
        inline int getPPID() const
        {
            return mProc->ppid;
        }

        // Get the start time (Unix time stamp).
        inline unsigned long long getStartTime() const
        {
            return mProc->start_time;
        }

        // Get the nice.
        inline long getNice() const
        {
            return mProc->nice;
        }

        // Get virtual memory usage.
        inline unsigned long getVMem() const
        {
            return mProc->vm_size;
        }

        // Get the argument vector used to start the process.
        inline std::vector<std::string> getArgv() const
        {
            // Get the size of the argument vector.
            std::size_t argc = 0;
            auto argv = mProc->cmdline;
            if(argv)
            {
                for(char *tmp = argv[0]; tmp; tmp = *(++argv), argc++);

                return std::vector<std::string>(mProc->cmdline,
                                                mProc->cmdline + argc);
            }
            return std::vector<std::string>();
        }

        // Get the basename of the process.
        inline std::string getBasename() const
        {
            return std::string(mProc->cmd);
        }

        // Get process group ID.
        inline int getProcGID() const
        {
            return mProc->pgrp;
        }

        // Get most recent processor this task was run on.
        inline int getProcessor() const
        {
            return mProc->processor;
        }

        // True if the underlying proc is NULL.
        inline bool isNull() const
        {
            return mProc == nullptr;
        }

        inline operator bool() const
        {
            return !isNull();
        }

    private:
        // The process.
        std::shared_ptr<proc_t> mProc;
    };

    class Proctab
    {
    public:
        Proctab()
            : mUserName(),mProcName(),mProcsOnly(false),
              mTab(openproc(PROC_FILLMEM | PROC_FILLUSR | PROC_FILLGRP | PROC_FILLARG),
                   &closeproc)

        {
        }

        Proctab(std::string_view userName, std::string_view procName,
                bool procsOnly = false)
            : mUserName(userName),mProcName(procName),mProcsOnly(procsOnly),
              mTab(openproc(PROC_FILLMEM | PROC_FILLUSR | PROC_FILLGRP | PROC_FILLARG),
                   &closeproc)
        {
        }

        ~Proctab() = default;

        Process getNextProcess();

    private:
        // User name to search for.
        std::string mUserName;
        // Process name to search for.
        std::string mProcName;
        // Check for processes only (not threads).
        bool mProcsOnly;
        // The PROCTAB object.
        std::shared_ptr<PROCTAB> mTab;
    };
}

#endif /* GLTOP_HPP */
