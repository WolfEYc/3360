#include <cmath>
#include <iostream>

class Process {
private:
    int pid;
    int servicetime;
    int remaintime;
    int arrivaltime;
    int endtime;
    int queue;
    int quantum;
public:
    Process();
    Process(int pid, int servicetime);
    Process& print();
    Process& printStats();
    ~Process();
    Process& start(const int& time);
    Process& run(int& time);
    int getPid();
    int getServiceTime();
    int getRemainTime();
    int getTurnaroundTime();
    Process& setQueue(const int queue);
    const int getQueue() const;
    const bool done() const;
};
