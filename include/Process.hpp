#ifndef PROCESS_HPP_
#define PROCESS_HPP_

#include <vector>

struct Process
{
    int PID;
    int parentPID;
    std::vector<int> childrenPIDs;
    bool isZombie;
    bool isWaiting;

    // Default constructor
    Process() : PID(-1), parentPID(-1), isZombie(false), isWaiting(false) {}

    Process(int pid, int parentPid = -1)
        : PID(pid), parentPID(parentPid), isZombie(0), isWaiting(0) {}
};

#endif