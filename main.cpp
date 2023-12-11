#include <iostream>
#include <fstream>


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


int main() {
	const int JOBS = 1; //MAX IS 30
	double averageTAT;
	double averageWT;
	int interrupt;

	Job job[JOBS];
	ifstream file;

	for(int i = 0; i < JOBS; i++) {
		openJobFile(file, to_string(i+1));
		job[i] = readJob(file);
		cout << "Arrival time = " << job[i].arrivalTime << endl;
		cout << "Burst Time = " << job[i].burstTime << endl;

	}

    return 0;
}

