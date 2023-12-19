#include <iostream>
#include <fstream>
#include <queue>
#include <vector>

using namespace std;

struct Job {
    int id;
    int cpuCycle;
    int remainingCycle;
    int waitingTime;
    int completionTime;
    // Add more attributes if needed
};

vector<Job> readJobsFromFile(const string &fileName) {
    vector<Job> jobs;
    ifstream file(fileName);

    int id = 1;
    int cycle;
    while (file >> cycle) {
        jobs.push_back({id++, cycle, cycle, 0, 0});
    }

    return jobs;
}

void roundRobin(vector<Job> &jobs, int timeQuantum) {
    queue<Job*> jobQueue;
    int currentTime = 0;

    // Initial load of jobs to queue
    for (auto &job : jobs) {
        jobQueue.push(&job);
    }

    while (!jobQueue.empty()) {
        Job *currentJob = jobQueue.front();
        jobQueue.pop();

        int timeSlice = min(currentJob->remainingCycle, timeQuantum);
        currentJob->remainingCycle -= timeSlice;
        currentTime += timeSlice;

        // Update waiting times for other jobs
        for (auto &job : jobs) {
            if (&job != currentJob && job.remainingCycle > 0) {
                job.waitingTime += timeSlice;
            }
        }

        if (currentJob->remainingCycle > 0) {
            jobQueue.push(currentJob); // Re-queue if job is not finished
        } else {
            currentJob->completionTime = currentTime; // Job completed
        }
    }

    // Calculate additional metrics here...
}

void roundRobinScheduling(int timeQuantum) {
    queue<Job*> jobQueue;
    int currentTime = 0;
    std::queue<Job> waitingQueue;

    while (!jobQueue.empty() || !waitingQueue.empty()) {
        if (!waitingQueue.empty()) {
            Job currentJob = waitingQueue.front();
            waitingQueue.pop();

            int processingTime = std::min(currentJob.remainingCycles, timeQuantum);
            currentJob.remainingCycles -= processingTime;
            currentTime += processingTime;
            currentJob.interrupts++;

            // Update waiting time for all jobs in the queue
            std::queue<Job> tempQueue;
            while (!jobQueue.empty()) {
                Job tempJob = jobQueue.front();
                jobQueue.pop();
                tempJob.waitingTime += processingTime;
                tempQueue.push(tempJob);
            }
            std::swap(jobQueue, tempQueue);

            if (currentJob.remainingCycles > 0) {
                waitingQueue.push(currentJob);
            } else {
                currentJob.finishTime = currentTime;
                std::cout << "Job completed: Arrival Time = " << currentJob.arrivalTime
                          << ", Finish Time = " << currentJob.finishTime
                          << ", Waiting Time = " << currentJob.waitingTime
                          << ", Total Interrupts = " << currentJob.interrupts << std::endl;
            }
        } else if (!jobQueue.empty()) {
            waitingQueue.push(jobQueue.front());
            jobQueue.pop();
        }
    }
}


int main() {
    vector<Job> jobs = readJobsFromFile("jobs.txt");

    int timeQuanta[] = {5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60};
    for (int quantum : timeQuanta) {
        roundRobin(jobs, quantum);
        // Calculate and display performance metrics here...
    }

    return 0;
}
