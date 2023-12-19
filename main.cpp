#include <iostream>
#include <fstream>
#include <queue>
#include <vector>
using namespace std;

struct Job{
	int jobNumber;
	int arrivalTime;
	int burstTime;
	int tempBurstTime;
	int completionTime;
	int turnAroundTime;
	int waitingTime;
};

struct FinishedJob{
	vector<Job> jobs;
	int timeQuantum;
	double averageTAT;
	double averageWT;
	int intterupts;
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
	}

	job.tempBurstTime = CPUcycle; //get job number

	return job;
} 

void roundRobin(vector<Job> &jobs, int timeQuantum, vector<Job> &finishedJobs) {
	queue<Job*> jobQueue;
    int currentTime = 1;

    // Initial load of jobs to queue
    for (auto &job : jobs) {
		job.burstTime = job.tempBurstTime; //resets burst time to original value
        jobQueue.push(&job);
    }

	while (!jobQueue.empty()) {
        Job *currentJob = jobQueue.front();
		if(currentJob->arrivalTime > currentTime) { //verify if job not supposed to be executed yet
			jobQueue.push(currentJob);
			jobQueue.pop();
			continue;
		}

		int timeSlice = min(currentJob->burstTime, timeQuantum); //ensures that the time slice doesn't exceed the remaining burst time
		currentJob->burstTime -= timeSlice; //subtracts the timeSlice from the remaining burst time of the currentJob
		currentTime += timeSlice;

		if (currentJob->burstTime > 0) {
			jobQueue.push(currentJob); // Re-queue if job is not finished
		} else {
			currentJob->completionTime = currentTime; // Job completed
			currentJob->turnAroundTime = currentJob->completionTime - currentJob->arrivalTime;
			currentJob->waitingTime = currentJob->turnAroundTime - currentJob->tempBurstTime;
			finishedJobs.push_back(*currentJob);
		}
		jobQueue.pop();
	}
}


int main() {
	const int MAX_JOBS = 30; //MAX IS 30

	double averageTAT;
	double averageWT;
	int interrupt;

	vector<Job> jobs;
	vector<FinishedJob> finishedJobsList;
	ifstream file;

	int timeQuanta[] = {5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60};

	for(int i = 0; i < MAX_JOBS; i++) {
		openJobFile(file, to_string(i+1));
		Job job = readJob(file);
		job.jobNumber = i+1;
		jobs.push_back(job); //read jobs from file and insert into vector
		file.close();
	}

	for(int timeQuantum: timeQuanta) {
		vector<Job> finishedJobs;
		roundRobin(jobs, timeQuantum, finishedJobs);
		FinishedJob finishedJob;
		finishedJob.jobs = finishedJobs;
		finishedJob.timeQuantum = timeQuantum;
		finishedJobsList.push_back(finishedJob);
	}

	//print results
	for(FinishedJob finishedJob: finishedJobsList) {
		cout << "Time Quantum: " << finishedJob.timeQuantum << endl;
		cout << "Job Number\tArrival Time\tCompletion Time\tTurnaround Time\tWaiting Time" << endl;
		for(Job job: finishedJob.jobs) {
			cout << job.jobNumber << "\t\t" << job.arrivalTime << "\t\t" << job.completionTime << "\t\t" << job.turnAroundTime << "\t\t" << job.waitingTime << endl;
		}
		cout << endl;
	}
	cout << endl;


    return 0;
}

