#include <string>
#include <map>
#include <deque>
#include "QueueOfProcesses.h"

class FeedbackScheduler {
private:
    std::map<int, Process> procs;
    std::map<int, std::deque<int>> queues;
    int time;
    void enqueAll();
public:
    FeedbackScheduler();
    FeedbackScheduler(const FeedbackScheduler&);
    const FeedbackScheduler& operator= (const FeedbackScheduler&);
    bool readFromFile(std::string); // filename
    Process& firstActive();
    void start();
    bool done();
    void printQueuesOfProcesses();
    void printStatistics();
    ~FeedbackScheduler();
};