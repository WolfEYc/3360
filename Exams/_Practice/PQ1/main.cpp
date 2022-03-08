#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
    
void performFork(int& proc, bool child = false)
{
    // child process because return value zero
    
    pid_t pid = fork();
    
    if (!pid)
    {
        if(child){
            std::cout << "I am a grandchild process from child process " << proc << std::endl;
        } else {
            std::cout << "I am the child process " << proc << std::endl;
            if(proc == 1)
            {
                performFork(proc, true);
            }
        }
        _exit(0);
    }
    // parent process because return value non-zero.
    else if (!child) 
    {
        int status;
        wait(&status);
        proc++;
    }
        
}

int main()
{
    std::cout << "I am the parent process" << std::endl;
    int proc = 0;
    performFork(proc);
    performFork(proc);
    performFork(proc);
    
    
    return 0;
}
