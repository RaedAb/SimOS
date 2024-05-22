
// Raed Abuzaid

#ifndef PROCESS_MANAGER_HPP_
#define PROCESS_MANAGER_HPP_

#include <unordered_map>
#include <unordered_set>
#include "CPU.hpp"
#include "DiskManager.hpp"
#include "MemoryManager.hpp"

struct Process
{
    int PID;
    int parentPID;
    std::vector<int> childrenPIDs;
    bool isZombie;
    bool isWaiting;
    bool requestedReading;

    // Default constructor
    Process() : PID(-1), parentPID(-1), isZombie(false), isWaiting(false) {}

    Process(int pid, int parentPid = -1)
        : PID(pid), parentPID(parentPid), isZombie(0), isWaiting(0) {}
};

class ProcessManager
{
private:
    int nextPID_;
    std::unordered_map<int, Process> processes_;

    /**
     * Cascading terminate to prevent orphans when a process is terminated
     * @param pid : process pid
     * @param cpu : refrence to cpu
     * @param diskManager : refrence to disk manager
     * @param memoryManager : refrence to memory manager
     */
    void cascadingTerminate(int pid, CPU &cpu, MemoryManager &memoryManager, DiskManager &diskManager);

public:
    /**
     * Creates PRocess Manager object. Sets lowest PID to 1
     */
    ProcessManager();

    /**
     * Creates a process, adds it to processes map, increments pid
     * @return : PID of new process
     */
    int createProcess();

    /**
     * Forks currently running process
     * @param parentPID : PID of child
     * @return : child pid
     */
    int forkProcess(int parentPID);

    /**
     * Terminates process, as well as all decendent processes
     * If parent is waiting automatically terminate, else process becomes zombie
     * @param pid : PID of process
     * @param cpu : refrence to cpu
     * @param diskManager : refrence to disk manager
     * @param memoryManager : refrence to memory manager
     */
    void terminateProcess(int pid, CPU &cpu, MemoryManager &memoryManager, DiskManager &diskManager);

    /**
     * Process pauses and waits for any child process to terminate
     * If child process is already a zombie, process resumes without stopping
     * If more than one zombie-child exists, the system uses one of them (any!) to immediately resume the parent,
     * while other zombies keep waiting for the next wait from the parent.
     * @param pid : PID of Process
     * @param cpu : Refrence to cpu
     */
    void waitProcess(int pid, CPU &cpu);
};

#endif // PROCESS_MANAGER_HPP_