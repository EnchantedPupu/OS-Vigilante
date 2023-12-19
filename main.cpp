#include <iostream>
#include <fstream>
#include <queue>
#include <vector>

using namespace std;

struct Job{
	int arrivalTime;
	int burstTime;
	int completionTime;
	int turnAroundTime;
	int waitingTime;
};

void openJobFile(ifstream& file, string filename) {
	filename = "jobs/" + filename + ".csv";
	//csv format
	//top first line is Job number, Arrival time
	//the rest contains 2 inputs
	//first input is action - 1 for CPU, 2 for I/O
	//second input is CPU Cycle

	//read file
	file.open(filename.c_str());
	if (file.fail()) {
		cout << "Error opening file" << endl;
		exit(1);
	}
}

 Job readJob(ifstream& file) {
	Job job;
	int CPUcycle = 0;

	string text;
	getline(file, text);

	//Job, Arrival Time
	job.arrivalTime = stoi(text.substr(text.find(",")+1, text.length())); //get arrival time

	while (getline (file, text)) {
		//AS OF NOW, THE PROGRAM COMBINES BOTH IO AND CPU CYCLES
		CPUcycle += stoi(text.substr(text.find(",")+1, text.length()));
		//cout << text.substr(0, text.find(",")) << endl;
	}

	job.burstTime = CPUcycle;

	return job;
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

		int timeSlice = min(currentJob->burstTime, timeQuantum); //ensures that the time slice doesn't exceed the remaining burst time
		currentJob->burstTime -= timeSlice; //subtracts the timeSlice from the remaining burst time of the currentJob
		currentTime += timeSlice;

		// Update waiting times for other jobs
		for (auto &job : jobs) {
			if (&job != currentJob && job.burstTime > 0) {
				job.waitingTime += timeSlice;
			}
		}

		if (currentJob->burstTime > 0) {
			jobQueue.push(currentJob); // Re-queue if job is not finished
		} else {
			currentJob->completionTime = currentTime; // Job completed
			currentJob->turnAroundTime = currentJob->completionTime - currentJob->arrivalTime;
			currentJob->waitingTime = currentJob->turnAroundTime - currentJob->burstTime;
		}
	}
}


int main() {
	const int MAX_JOBS = 2; //MAX IS 30

	double averageTAT;
	double averageWT;
	int interrupt;

	vector<Job> jobs;
	vector<Job> tempJobs;
	ifstream file;

	int timeQuanta[] = {5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60};

	for(int i = 0; i < MAX_JOBS; i++) {
		openJobFile(file, to_string(i+1));
		jobs.push_back(readJob(file)); //read jobs from file and insert into vector
	}
	for(auto &job : jobs) {
		tempJobs.push_back(job);
	}
	for(int timeQuantum: timeQuanta) {
		roundRobin(tempJobs, timeQuantum);
		cout << "Time Quantum: " << timeQuantum << endl;
		for(auto &job : tempJobs) {
			cout << "Job Arrival : " << job.arrivalTime << endl;
			cout << "Completion Time: " << job.completionTime << endl;
			cout << "Turn Around Time: " << job.turnAroundTime << endl;
			cout << "Waiting Time: " << job.waitingTime << endl;
			cout << endl;
		}
		tempJobs = jobs;
	}


    return 0;
}

