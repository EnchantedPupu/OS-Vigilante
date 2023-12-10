#include <iostream>
#include <fstream>

using namespace std;


struct Job{
	int arrivalTime;
	int burstTime;
	int completionTime;
	int turnAroundTime;
	int waitingTime;
	double averageTAT;
	double averageWT;
	int interrupt;
};

ifstream openFile(string filename) {
	filename = filename + ".csv";
	//csv format
	//top 2 lines are Job number, Arrival time
	//the rest contains 2 inputs
	//first input is action - 1 for CPU, 2 for I/O
	//second input is CPU Cycle

	//read file
	ifstream file;
	file.open(filename.c_str());
	if (file.fail()) {
		cout << "Error opening file" << endl;
		exit(1);
	}
	return file;
}

void readJob(ifstream file){
	string text;
	while (getline (file, text)) {
		// Output the text from the file
		cout << text;
		cout << endl;
}
}

int main() {
	const int JOBS = 1; //MAX IS 30
	Job job[JOBS];

	//read file
	readJob(openFile("1"));
    return 0;
}

