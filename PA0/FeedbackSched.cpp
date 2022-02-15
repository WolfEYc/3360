#include "FeedbackSched.h"
#include <fstream>
#include <iomanip>
using namespace std;

FeedbackScheduler::FeedbackScheduler() : time(0) {
    cout << setprecision(2) << fixed;
}

FeedbackScheduler::FeedbackScheduler(const FeedbackScheduler& other) {
    *this = other;
}

const FeedbackScheduler& FeedbackScheduler::operator= (const FeedbackScheduler& other) {
    if(this != &other){
        procs = other.procs;
        queues = other.queues;
        time = other.time;
    }
    return *this;
}

bool FeedbackScheduler::readFromFile(std::string filename) {
    try{
        ifstream in(filename);

        int pid, servicetime;

        while(in >> pid >> servicetime) {
            procs[pid] = Process(pid, servicetime);
        }


    } catch (exception& e) {
        return false;
    }
    return true;
}

bool FeedbackScheduler::done() {
    for(auto const& [pid, process] : procs) {
        if(!process.done()) {
            return false;
        }
    }
    return true;
}

Process& FeedbackScheduler::firstActive() {
    if(done()){
        throw;
    }

    for(auto& [number, queue] : queues) {
        if(!queue.empty()){
            int procid = queue.front();
            queue.pop_front();
            return procs[procid];
        }
    }

    throw;
}

void FeedbackScheduler::enqueAll() {
    cout << "Initial ready queue:" << endl << endl;
    for(auto& [pid, process] : procs) {
        process.start(time);
        queues[0].push_back(pid);
    }
    printQueuesOfProcesses();
}

void FeedbackScheduler::start() {

    enqueAll();

    while(!done()){
        Process& p = firstActive().run(time);
        if(p.done()) {
            printQueuesOfProcesses();
        } else {
            queues[p.setQueue(p.getQueue() + 1).getQueue()].push_back(p.getPid());
        }
    }
}

void FeedbackScheduler::printQueuesOfProcesses() {
    for(auto& [number, queue] : queues) {
        if(!queue.empty()){
            cout << "\tQueue " << number << ", quantum: " << (int)pow(2, number) << endl << endl;
            for(auto const& pid : queue) {
                procs[pid].print();
            }
            cout << endl;
        }
    }
}

void FeedbackScheduler::printStatistics() {
    cout << "Scheduler statistics" << endl << endl;
    
    float avgTurn = 0, avgServ = 0;
    for(auto& [pid, proc] : procs) {
        proc.printStats();
        avgTurn += proc.getTurnaroundTime();
        avgServ += (float)proc.getTurnaroundTime() / proc.getServiceTime(); 
    }

    avgServ = avgServ / procs.size();
    avgTurn = avgTurn / procs.size();

    cout << endl
    << "Average Turnaround Time = " << avgTurn << endl << endl
    << "Average Turnaround Time / Service time = " << avgServ << endl;
}

FeedbackScheduler::~FeedbackScheduler(){}

