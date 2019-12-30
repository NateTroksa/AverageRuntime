// TestingApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <algorithm>
#include <time.h>
#include "windows.h"
using namespace std;

struct listBurst {
	int num = 0;
	int pNum = 0;
	bool IO = false;
};

struct burst {
	bool IO = false;
	int num = 0;
};

struct process {
	vector<burst> bList;
	int burstTime = 0;
	int pNum = -1;
	int endTime = 0;

	bool isDone() {
		if (burstTime == 0)
			return true;

		return false;
	}

	int topBurst() {
		if (bList.empty())
			return INT_MAX;
		else
			return bList[0].num;
	}
};

vector<process> processes;
int IOChance = 50;

//ALL PRINT VARS
bool printRunGraphs = false;
bool printAllAvg = false;
bool printPTable = false;

bool compareByTotalBurst(const process &a, const process &b) {
	return a.burstTime < b.burstTime;
}

bool compareByTopBurst(process& a,  process& b) {
	return a.topBurst() < b.topBurst();
}

//prints the Run-time graph of the given vector of listBursts
void printRunGraph(vector<listBurst> runGraph) {
	for (listBurst b : runGraph) {
		if(!b.IO)
			printf("| %d : P%d |", b.num, b.pNum);
		else
			printf("| %dIO : P%d |", b.num, b.pNum);
	}
	printf("\n\n");
}

//prints the process table for the given process
void printProcTable() {
	for (process p : processes) {
		printf("\n** P%d **", p.pNum);
		for (burst b : p.bList) {
			if (!b.IO)
				printf("| %d ", b.num);
			else
				printf("| %dIO ", b.num);
		}
	}

	printf("\n\n");
}

//gives the total bursttime of the process
int burstTime(process p) {
	int sum = 0;
	for (int i = 0; i < p.bList.size(); i++) {
		if (!p.bList[i].IO)
			sum += p.bList[i].num;
	}

	return sum;
}

//randomly generates a process table
void CreateProcesses() {
	processes.clear();

	int numProcesses = ((rand() % 10) + 3); //create a random amount of processes
	int numBursts =  ((rand() % 15) + 5); //random bursts

	for (int i = 0; i < numProcesses; i++) {
		process newP;

		for (int j = 0; j < numBursts; j++) {
			burst newBurst;

			int IOpercent = rand() % 100;

			if (IOpercent < IOChance)
				newBurst.IO = 1;
			else
				newBurst.IO = 0;
			
			newBurst.num += (rand() % 5 + 1);
			newP.bList.push_back(newBurst);
		}

		newP.burstTime = burstTime(newP);
		newP.pNum = i + 1;

		processes.push_back(newP);
	}

	if (printPTable)
		printProcTable();
}

//checks if the given procList has no more bursts to execute
bool isEmpty(vector<process> procList) {
	for (process p : procList) {
		if (p.bList.size() != 0)
			return false;
	}

	return true;
}

//checks if the given process only has IO remaining
bool allRemainingIO(process procList) {
	for (int i = 0; i < procList.bList.size(); i++) {
		if (!procList.bList[i].IO)
			return false;
	}

	return true;
}


//Checks if all the procedures are in IO
bool allProcInIO(vector<process> procList) {
	for (int i = 0; i < procList.size(); i++) {
		if (procList[i].bList.size() > 0 && !procList[i].bList[0].IO)
			return false;
	}

	return true;
}

//Shortest remaining Burst Time First
float SRBTF(vector<process> procList) {
	float avg;
	int runTime = 0;
	vector<listBurst> runGraph;

	//Executes the graph using SRBTF algorithm
	while (!isEmpty(procList)) {
		sort(procList.begin(), procList.end(), compareByTotalBurst); //re-sort so that the new least total burst time is at the top (main part of SRBTF)

		if (allProcInIO(procList)) { //Checks if all processes are in IO
			int lowestNum = _CRT_INT_MAX;
			int lowestIdx = -1;
			for (int i = 0; i < procList.size(); i++) { //if they are execute the shortest IO for efficiency
				if (procList[i].bList.size() > 0 && procList[i].bList[0].num < lowestNum) {
					lowestNum = procList[i].bList[0].num;
					lowestIdx = i;
				}
			}

			if (lowestIdx != -1) { //prevents OOB on array and removes the shortest IO in the proclist
				listBurst newBurst;
				newBurst.num = procList[lowestIdx].bList[0].num;
				newBurst.pNum = procList[lowestIdx].pNum;
				newBurst.IO = true;
				runGraph.push_back(newBurst);

				runTime += procList[lowestIdx].bList[0].num;
				procList[lowestIdx].bList.erase(procList[lowestIdx].bList.begin());

			}
		}
		else {
			int i = 0;
			for (i; i < procList.size(); i++) { //find the first process not in IO
				if (procList[i].bList.size() > 0 && !procList[i].bList[0].IO)
					break;
			}
			 
			//once found we remove the process from the list
			if (procList[i].bList.size() >= 0) {
				int bNum = procList[i].bList[0].num;
				procList[i].bList.erase(procList[i].bList.begin());
				procList[i].burstTime -= bNum;
				runTime += bNum;

				if (procList[i].burstTime == 0) { //if there is no more burstime in the process (the rest is just IO) Then we may just clear that process
					procList[i].bList.clear();
					procList[i].endTime = runTime;

				}

				//add it to the rungraph
    			listBurst newBurst;
				newBurst.num = bNum;
				newBurst.pNum = procList[i].pNum;
				runGraph.push_back(newBurst);



				//decrement all processes that were in IO
				for (int j = 0; j < procList.size(); j++) {
					if (i != j && procList[j].bList.size() > 0 && procList[j].bList[0].IO) {
						int sCount = bNum;
						do {
							if (procList[j].bList[0].IO) {
								int temp = procList[j].bList[0].num;
								procList[j].bList[0].num -= sCount;
								sCount = sCount - temp;

								if (procList[j].bList[0].num <= 0) {
									procList[j].bList.erase(procList[j].bList.begin());
								}

							}
						} while (sCount > 0  && procList[j].bList.size() > 0 && procList[j].bList[0].IO);
					}
				}
			}
		}
	}

	int sum = 0;
	for (process p : procList) {
		sum += p.endTime;
	}

	if(printRunGraphs)
		printRunGraph(runGraph);

	return (float)sum/ (float)procList.size();
}

//Shortest Next Burst time First
float SNBTF(vector<process> procList) {
	float avg;
	int runTime = 0;
	vector<listBurst> runGraph;

	while (!isEmpty(procList)) {
		sort(procList.begin(), procList.end(), compareByTopBurst); //re-sort so that new least burst time is at the top

		if (allProcInIO(procList)) { //Checks if all processes are in IO
			int lowestNum = _CRT_INT_MAX;
			int lowestIdx = -1;
			for (int i = 0; i < procList.size(); i++) { //if they are execute the shortest IO for efficiency
				if (procList[i].bList.size() > 0 && procList[i].bList[0].num < lowestNum) {
					lowestNum = procList[i].bList[0].num;
					lowestIdx = i;
				}
			}

			if (lowestIdx != -1) { //prevents OOB on array and removes the shortest IO in the proclist
				listBurst newBurst;
				newBurst.num = procList[lowestIdx].bList[0].num;
				newBurst.pNum = procList[lowestIdx].pNum;
				newBurst.IO = true;
				runGraph.push_back(newBurst);

				runTime += procList[lowestIdx].bList[0].num;
				procList[lowestIdx].bList.erase(procList[lowestIdx].bList.begin());

			}
		}
		else {
			int i = 0;
			for (i; i < procList.size(); i++) { //find the first process not in IO
				if (procList[i].bList.size() > 0 && !procList[i].bList[0].IO)
					break;
			}

			//once found we remove the process from the list
			if (procList[i].bList.size() >= 0) {
				int bNum = procList[i].bList[0].num;
				procList[i].bList.erase(procList[i].bList.begin());
				procList[i].burstTime -= bNum;
				runTime += bNum;

				if (procList[i].burstTime == 0) { //if there is no more burstime in the process (the rest is just IO) Then we may just clear that process
					procList[i].bList.clear();
					procList[i].endTime = runTime;

				}

				//add it to the rungraph
				listBurst newBurst;
				newBurst.num = bNum;
				newBurst.pNum = procList[i].pNum;
				runGraph.push_back(newBurst);



				//decrement all processes that were in IO
				for (int j = 0; j < procList.size(); j++) {
					if (i != j && procList[j].bList.size() > 0 && procList[j].bList[0].IO) {
						int sCount = bNum;
						do {
							if (procList[j].bList[0].IO) {
								int temp = procList[j].bList[0].num;
								procList[j].bList[0].num -= sCount;
								sCount = sCount - temp;

								if (procList[j].bList[0].num <= 0) {
									procList[j].bList.erase(procList[j].bList.begin());
								}

							}
						} while (sCount > 0 && procList[j].bList.size() > 0 && procList[j].bList[0].IO);
					}
				}
			}
		}
	}

	int sum = 0;
	for (process p : procList) {
		sum += p.endTime;
	}

	if (printRunGraphs)
		printRunGraph(runGraph);

	return (float)sum / (float)procList.size();
}

//Shortest Total Burst time First
float STBTF(vector<process> procList) {
	float avg;
	int runTime = 0;
	vector<listBurst> runGraph;
	sort(procList.begin(), procList.end(), compareByTotalBurst); //re-sort so that the total least burst time is at the top

	while (!isEmpty(procList)) {
		if (allProcInIO(procList)) { //Checks if all processes are in IO
			int lowestNum = _CRT_INT_MAX;
			int lowestIdx = -1;
			for (int i = 0; i < procList.size(); i++) { //if they are execute the shortest IO for efficiency
				if (procList[i].bList.size() > 0 && procList[i].bList[0].num < lowestNum) {
					lowestNum = procList[i].bList[0].num;
					lowestIdx = i;
				}
			}

			if (lowestIdx != -1) { //prevents OOB on array and removes the shortest IO in the proclist
				listBurst newBurst;
				newBurst.num = procList[lowestIdx].bList[0].num;
				newBurst.pNum = procList[lowestIdx].pNum;
				newBurst.IO = true;
				runGraph.push_back(newBurst);

				runTime += procList[lowestIdx].bList[0].num;
				procList[lowestIdx].bList.erase(procList[lowestIdx].bList.begin());

			}
		}
		else {
			int i = 0;
			for (i; i < procList.size(); i++) { //find the first process not in IO
				if (procList[i].bList.size() > 0 && !procList[i].bList[0].IO)
					break;
			}

			//once found we remove the process from the list
			if (procList[i].bList.size() >= 0) {
				int bNum = procList[i].bList[0].num;
				procList[i].bList.erase(procList[i].bList.begin());
				procList[i].burstTime -= bNum;
				runTime += bNum;

				if (procList[i].burstTime == 0) { //if there is no more burstime in the process (the rest is just IO) Then we may just clear that process
					procList[i].bList.clear();
					procList[i].endTime = runTime;

				}

				//add it to the rungraph
				listBurst newBurst;
				newBurst.num = bNum;
				newBurst.pNum = procList[i].pNum;
				runGraph.push_back(newBurst);



				//decrement all processes that were in IO
				for (int j = 0; j < procList.size(); j++) {
					if (i != j && procList[j].bList.size() > 0 && procList[j].bList[0].IO) {
						int sCount = bNum;
						do {
							if (procList[j].bList[0].IO) {
								int temp = procList[j].bList[0].num;
								procList[j].bList[0].num -= sCount;
								sCount = sCount - temp;

								if (procList[j].bList[0].num <= 0) {
									procList[j].bList.erase(procList[j].bList.begin());
								}

							}
						} while (sCount > 0 && procList[j].bList.size() > 0 && procList[j].bList[0].IO);
					}
				}
			}
		}
	}

	int sum = 0;
	for (process p : procList) {
		sum += p.endTime;
	}

	if (printRunGraphs)
		printRunGraph(runGraph);

	return (float)sum / (float)procList.size();
}


int main()
{
	//print vars
	printRunGraphs = true;
	printAllAvg = true;
	printPTable = true;

	IOChance = 90; //percent chance for a burst in the process to be IO

	int RUNAMOUNT = 5;
	int TRIALS_PER_RUN = 10;

	float overAllAVGtimes[3] = { 0, 0, 0 };
	vector<vector<float>> overAllAVG(RUNAMOUNT);

	srand(time(0));
	for (int runs = 0; runs < RUNAMOUNT; runs++) {
		float averageTimes[3] = { 0, 0, 0 };
		vector<vector<float>> turnaroundTimes;

		for (int i = 0; i < TRIALS_PER_RUN; i++) {
			vector<float> averages;
			//Sleep(10);
			CreateProcesses();
			averages.push_back(SRBTF(processes));
			averages.push_back(SNBTF(processes));
			averages.push_back(STBTF(processes));

			if (printAllAvg)
				printf("SRBTF: %4.3f \t\t\t SNBTF: %4.3f \t\t\t STBTF: %4.3f\n", averages.at(0), averages.at(1), averages.at(2));
			else if(printPTable || printRunGraphs)
				printf("\n\n");

			turnaroundTimes.push_back(averages);
		}

		for (int i = 0; i < turnaroundTimes.size(); i++) {
			averageTimes[0] += turnaroundTimes[i][0];
			averageTimes[1] += turnaroundTimes[i][1];
			averageTimes[2] += turnaroundTimes[i][2];
		}

		averageTimes[0] /= TRIALS_PER_RUN;
		averageTimes[1] /= TRIALS_PER_RUN;
		averageTimes[2] /= TRIALS_PER_RUN;

		overAllAVG[runs].push_back(averageTimes[0]);
		overAllAVG[runs].push_back(averageTimes[1]);
		overAllAVG[runs].push_back(averageTimes[2]);

		printf("\n\nAVERAGES: SRBTF = %4.5f \t\t SNBTF = %4.5f \t\t STBTF = %4.5f\n\n\n", averageTimes[0], averageTimes[1], averageTimes[2]);
	}

	for (int i = 0; i < overAllAVG.size(); i++) {
		overAllAVGtimes[0] += overAllAVG[i][0];
		overAllAVGtimes[1] += overAllAVG[i][1];
		overAllAVGtimes[2] += overAllAVG[i][2];
	}

	overAllAVGtimes[0] /= RUNAMOUNT;
	overAllAVGtimes[1] /= RUNAMOUNT;
	overAllAVGtimes[2] /= RUNAMOUNT;

	printf("\n\n*******OVERALL AVERAGES: SRBTF = %4.7f \t\t SNBTF = %4.7f \t\t STBTF = %4.7f*******\n\n\n", overAllAVGtimes[0], overAllAVGtimes[1], overAllAVGtimes[2]);
}
