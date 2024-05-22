// Raed Abuzaid

#ifndef DISK_MANAGER_HPP_
#define DISK_MANAGER_HPP_

#include <deque>
#include <string>
#include <unordered_map>

struct FileReadRequest
{
    int PID{0};
    std::string fileName{""};

    // Param Constructor
    FileReadRequest(int pid = 0, std::string name = "") : PID{pid}, fileName{name} {}
};

struct Disk
{
    std::deque<FileReadRequest> diskQueue_;
    FileReadRequest currentlyServing;

    // Default constructor
    Disk() : diskQueue_(), currentlyServing(FileReadRequest(0, "")) {}

    // Param constructor
    Disk(FileReadRequest currentlyServing)
        : diskQueue_(), currentlyServing(currentlyServing) {}
};

class DiskManager
{
private:
    std::unordered_map<int, Disk> disks_;
    int numberOfDisks_;

public:
    // Parametized constructor
    DiskManager(int numberOfDisks);

    /**
     * Creates a read request with given parameters sends to disk
     * @param : process pid
     * @param : requested disk number
     * @param : File name
     */
    void readRequest(int pid, int diskNumber, std::string fileName);

    /**
     * Disk Completes one process
     * @param diskNumber : Disk Number
     * @return : process served
     */
    int completeJob(int diskNumber);

    /**
     * Deletes requests of process from all disk without completing
     * @param pid : process pid
     */
    void deleteRequests(int pid);

    /**
     * @param diskNumber : disk number
     * @return : request at front of disk, else empy request
     */
    FileReadRequest getDiskStatus(int diskNumber);

    /**
     * @param diskNumber : disk number
     * @return : return queue of requested disk number
     */
    std::deque<FileReadRequest> getDiskQueue(int diskNumber);

    /**
     * @return : number of disks
     */
    int getNumberOfDisks();
};

#endif // DISK_MANAGER_HPP_