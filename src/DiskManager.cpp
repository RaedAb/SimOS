// Raed Abuzaid

#include "DiskManager.hpp"
#include <algorithm>

// Parametized constructor
DiskManager::DiskManager(int numberOfDisks) : numberOfDisks_{numberOfDisks}
{
    for (int i = 0; i < numberOfDisks; i++)
    {
        disks_[i].currentlyServing = FileReadRequest();
        disks_[i].diskQueue_ = std::deque<FileReadRequest>();
    }
}

/**
 * Creates a read request with given parameters sends to disk
 * @param : process pid
 * @param : requested disk number
 * @param : File name
 */
void DiskManager::readRequest(int pid, int diskNumber, std::string fileName)
{
    FileReadRequest request(pid, fileName);
    Disk &disk = disks_[diskNumber];

    if (disk.currentlyServing.PID == 0)
    {
        disk.currentlyServing = request;
    }
    else
    {
        disk.diskQueue_.push_back(request);
    }
}

/**
 * Disk Completes one process
 * @param diskNumber : Disk Number
 * @return : process served
 */
int DiskManager::completeJob(int diskNumber)
{
    Disk &disk = disks_[diskNumber];

    int servedProcess = disk.currentlyServing.PID;
    disk.currentlyServing = FileReadRequest(0, "");

    if (!disk.diskQueue_.empty())
    {
        disk.currentlyServing = disk.diskQueue_.front();
        disk.diskQueue_.pop_front();
    }

    return servedProcess;
}

/**
 * Deletes requests of process from all disk without completing
 * @param pid : process pid
 */
void DiskManager::deleteRequests(int pid)
{
    // Lambda function to check if a request's PID matches the param PID
    auto isRequestedPID = [&](const FileReadRequest &request)
    {
        return request.PID == pid;
    };

    // Iterate over each disk's queue and remove requests with the PID
    for (auto &disk : disks_)
    {
        auto &diskQueue = disk.second.diskQueue_;

        // Move requests with the PID to the end of the queue
        auto newEnd = std::remove_if(diskQueue.begin(), diskQueue.end(), isRequestedPID);

        // Erase requests with the PID from the disk queue
        diskQueue.erase(newEnd, diskQueue.end());
    }
}

/**
 * @param diskNumber : disk number
 * @return : request at front of disk, else empy request
 */
FileReadRequest DiskManager::getDiskStatus(int diskNumber)
{
    return disks_[diskNumber].currentlyServing;
}

/**
 * @param diskNumber : disk number
 * @return : return queue of requested disk number
 */
std::deque<FileReadRequest> DiskManager::getDiskQueue(int diskNumber)
{
    return disks_[diskNumber].diskQueue_;
}

/**
 * @return : number of disks
 */
int DiskManager::getNumberOfDisks()
{
    return numberOfDisks_;
}