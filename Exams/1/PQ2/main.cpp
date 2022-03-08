#include <pthread.h>
#include <pthread.h>
#include <iostream>
#include <unistd.h>
#include <string>
#include <cmath>

struct arguments
{
    char ch;
    int *asciiPTR;
};

void *chToascii(void *void_ptr)
{
	struct arguments *arg_ptr = (arguments*)void_ptr; // cast the void pointer to a struct arguments pointer
	// store the ASCII value of the received character into the memory location provided in the arguments (asciiPTR). 
	*arg_ptr->asciiPTR = arg_ptr->ch;

    return nullptr;
}

int main()
{

    std::string word;
    std::cin >> word;
    int size = word.length();
    pthread_t *tid = new pthread_t[size];
    struct arguments * arg= new arguments[size];
    int *asciiValues = new int[size];

	for (int i = 0; i < size; i++)
	{
        arg[i].ch = word[i]; // assign a character from the input word.
        arg[i].asciiPTR = &asciiValues[i]; // assign the address from the asciiValues array where you want to store the ASCII value.   
        if (pthread_create(&tid[i], NULL, chToascii, &arg[i]))
		{
			    fprintf(stderr, "Error creating thread\n");
			    return 1;
		}
	}
    // call pthread_join here
    std::cout << "The word \"" << word << "\" is represented with the following ASCII values:" << std::endl;
    for (int i=0; i < size; i++)
    {
        pthread_join(tid[i], NULL);
        std::cout << asciiValues[i];
        if (i < size -1)
            std::cout << " ";
        else
            std::cout << std::endl;
    }
	delete [] tid;
    delete [] arg;
    delete [] asciiValues;
    return 0;
}