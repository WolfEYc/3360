#include "FeedbackSched.h"

int main () {
    FeedbackScheduler FS;
    FS.readFromFile("test1.txt");
    FS.start();
    FS.printStatistics();
}