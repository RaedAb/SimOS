// Raed Abuzaid

#ifndef CPU_HPP_
#define CPU_HPP_

#include <deque>
#include <algorithm>

constexpr int NO_PROCESS{0};

class CPU
{
private:
    int runningProcess_;
    std::deque<int> readyQueue_;

public:
    // Default constructor
    CPU();

    /**
     * Starts a process
     */
    void startProcess();

    /**
     * @param: process pid
     * Adds process to CPU Ready queue
     */
    void addProcess(int pid);

    /**
     * Sets running process to no process
     */
    void removeRunningProcess();

    /**
     * Handles Timer interrupt from OS by pushing running process to back of ready queue
     */
    void handleTimerInterrupt();

    /**
     * @return: currently running process
     */
    int getRunningProcess();

    /**
     * @return: the ready queue
     */
    std::deque<int> getReadyQueue();

    /** // for std::remove, std::remove_if
     * Removed process from cpu ready queue
     */
    void removeFromReadyQueue(int pid);
};

#endif // CPU_HPP_