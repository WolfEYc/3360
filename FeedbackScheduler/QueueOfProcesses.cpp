#include "QueueOfProcesses.h"
using namespace std;

Process::Process() : pid(), servicetime(), remaintime(), queue(0), quantum(1) {}

Process::Process(int pid, int servicetime) : pid(pid), servicetime(servicetime), remaintime(servicetime), arrivaltime(0), endtime(0), queue(0), quantum(1) {}

int Process::getTurnaroundTime() {
    return endtime - arrivaltime;
}

Process& Process::start(const int& time) {
    arrivaltime = time;
    return *this;
}

Process::~Process() {}

int Process::getPid() {
    return pid;
}

int Process::getServiceTime() {
    return servicetime;
}

int Process::getRemainTime() {
    return remaintime;
}

Process& Process::run(int& time) {

    int after = remaintime - quantum, prev = remaintime;
    remaintime = after < 0 ? 0 : after;
    int duration = prev - remaintime;

    cout << "Time " << time << ": Process " << pid << " in queue " << queue << " (quantum = " << quantum << ") runs for " << duration << "UT ";

    time += duration;

    if(!remaintime) {
        queue = -1;
        quantum = 0;
        endtime = time;

        cout << endl 
        << "Time " << time << ": Process " << pid << " ended" << endl;

    } else {
        cout << "(remaining time = " << remaintime << ")";
    }

    cout << endl;

    return *this;
}

const bool Process::done() const {
    return !remaintime;
}

Process& Process::print() {
    cout << "\t\tProcess ID: " << pid << ", servicetime: " << servicetime << ", remaintime: " << remaintime << endl;
    return *this;
}

Process& Process::printStats() {
    int turnaroundTime = this->getTurnaroundTime();
    cout << "Process " << this->getPid() << " Turnaround Time: " << turnaroundTime << ", Turnaround Time / Service time: " << (float)turnaroundTime / servicetime << endl;
    return *this; 
}

Process& Process::setQueue(const int q) {
    queue = q;
    quantum = (int)pow(2, queue);
    return *this;
}

const int Process::getQueue() const {
    return queue;
}
