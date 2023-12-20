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
	int interupts;
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

void roundRobin(vector<Job> &jobs, int timeQuantum, vector<Job> &finishedJobs, int &interrupts) {
    queue<Job*> jobQueue;
    int currentTime = 1;
	interrupts = 0;

    // Add jobs to queue based on arrival time
    auto jobIt = jobs.begin();
    while (jobIt != jobs.end() || !jobQueue.empty()) {
        // Load jobs that have arrived
        while (jobIt != jobs.end() && jobIt->arrivalTime <= currentTime) {
            jobIt->burstTime = jobIt->tempBurstTime;
            jobQueue.push(&(*jobIt));
            ++jobIt;
        }

        if (!jobQueue.empty()) {
            Job *currentJob = jobQueue.front();
            jobQueue.pop();

            int timeSlice = min(currentJob->burstTime, timeQuantum);
            currentJob->burstTime -= timeSlice;
            currentTime += timeSlice;

			if (currentJob->arrivalTime > currentTime) {
				jobQueue.push(currentJob);
				currentTime++; // Advance time if no job is ready
				continue;
        	}

            if (currentJob->burstTime > 0) {
                jobQueue.push(currentJob);
				interrupts++;
            } else {
                currentJob->completionTime = currentTime;
                currentJob->turnAroundTime = currentJob->completionTime - currentJob->arrivalTime;
                currentJob->waitingTime = currentJob->turnAroundTime - currentJob->tempBurstTime;
                finishedJobs.push_back(*currentJob);
            }
        } else {
            // If no jobs are in the queue, advance time
            currentTime++;
        }
    }
}

FinishedJob getLowestTAT(vector<FinishedJob> finishedJobsList) {
	FinishedJob lowestTAT = finishedJobsList[0];
	for(FinishedJob finishedJob: finishedJobsList) {
		if(finishedJob.averageTAT < lowestTAT.averageTAT) {
			lowestTAT = finishedJob;
		}
	}
	return lowestTAT;
}

FinishedJob getLowestWT(vector<FinishedJob> finishedJobsList) {
	FinishedJob lowestWT = finishedJobsList[0];
	for(FinishedJob finishedJob: finishedJobsList) {
		if(finishedJob.averageWT < lowestWT.averageWT) {
			lowestWT = finishedJob;
		}
	}
	return lowestWT;
}

FinishedJob getLowestInterrupts(vector<FinishedJob> finishedJobsList) {
	FinishedJob lowestInterrupts = finishedJobsList[0];
	for(FinishedJob finishedJob: finishedJobsList) {
		if(finishedJob.interupts < lowestInterrupts.interupts) {
			lowestInterrupts = finishedJob;
		}
	}
	return lowestInterrupts;
}


int main() {
	const int MAX_JOBS = 30; //MAX IS 30

	double averageTAT;
	double averageWT;
	int interrupts;

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

    for (int timeQuantum : timeQuanta) {
        vector<Job> finishedJobs; //temporary vector before putting in finishedJobsList
        roundRobin(jobs, timeQuantum, finishedJobs, interrupts);

        double totalTAT = 0, totalWT = 0;
        for (const Job& job : finishedJobs) {
            totalTAT += job.turnAroundTime;
            totalWT += job.waitingTime;
        }
        double averageTAT = totalTAT / finishedJobs.size();
        double averageWT = totalWT / finishedJobs.size();

        FinishedJob finishedJob;
        finishedJob.jobs = finishedJobs;
        finishedJob.timeQuantum = timeQuantum;
        finishedJob.averageTAT = averageTAT;
        finishedJob.averageWT = averageWT;
		finishedJob.interupts = interrupts;
        finishedJobsList.push_back(finishedJob);
    }

	//print results
 	for(FinishedJob finishedJob: finishedJobsList) {
		cout << "+------------------+" << endl;
		cout << "| Time Quantum: " << finishedJob.timeQuantum << " |" << endl;
		cout << "+---------------+---------------+---------------+---------------+---------------+" << endl; 
		cout << "| Job Number\t| Burst Time\t| Comp. Time\t| Turn. Time\t| Wait. Time\t|" << endl;
		cout << "+---------------+---------------+---------------+---------------+---------------+" << endl;
		for(Job job: finishedJob.jobs) {
			cout << "| " << job.jobNumber << "\t\t| " << job.tempBurstTime << "\t\t| " << job.completionTime << "\t\t| " << job.turnAroundTime << "\t\t| " << job.waitingTime <<"\t\t|" << endl;
		}
		cout << "+---------------+---------------+---------------+---------------+---------------+" << endl;
		cout << "Average Turnaround Time: " << finishedJob.averageTAT << endl;
		cout << "Average Waiting Time: " << finishedJob.averageWT << endl;
		cout << "Number of Interrupts: " << finishedJob.interupts << endl;
		cout << endl;
	}
	cout << endl; 

	FinishedJob data;

	cout << "Statistic: " << endl;
	cout << "Lowest Turnaround Time: ";
	data = getLowestTAT(finishedJobsList);
	cout << data.averageTAT << endl;
	cout << "Time Quantum: " << data.timeQuantum << endl;
	cout << endl;
	data = getLowestWT(finishedJobsList);
	cout << "Lowest Waiting Time: ";
	cout << data.averageWT << endl;
	cout << "Time Quantum: " << data.timeQuantum << endl;
	cout << endl;
	data = getLowestInterrupts(finishedJobsList);
	cout << "Lowest Interrupts: ";
	cout << data.interupts << endl;
	cout << "Time Quantum: " << data.timeQuantum << endl;



    return 0;
}

