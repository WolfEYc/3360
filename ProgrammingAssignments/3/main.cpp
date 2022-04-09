#include <map>
#include <iostream>
#include <pthread.h>
#include <cmath>
using namespace std;

namespace ProgrammingAssignments
{
    static pthread_mutex_t translationSem;
    static pthread_cond_t oneTranslationFinished;
    static pthread_cond_t printResult;

    struct DecimalTranslationContext
    {
        char symbol;
        int decimal;
        int codeLength;
        int index;
        int* currentIndex;
        map<string, char>* decodedAlphabet;
        map<char, int>* frequency;

        DecimalTranslationContext(int index, int* currentIndex, char symbol, int decimal, int codeLength, map<string, char>* decodedAlphabet, map<char, int>* frequency) :
           index(index), currentIndex(currentIndex), symbol(symbol), decimal(decimal), codeLength(codeLength), decodedAlphabet(decodedAlphabet), frequency(frequency) {}
    };

    struct BinaryTranslationContext
    {
        int index;
        int codeLength;
        string* encodedmsg;
        string* decodedmsg;
        map<string, char>* decodedAlphabet;
        map<char, int>* frequency;
        
        BinaryTranslationContext(int index, int codeLength, string* encodedmsg, string* decodedmsg, map<string, char>* decodedAlphabet, map<char, int>* frequency) :
           index(index), codeLength(codeLength), encodedmsg(encodedmsg), decodedmsg(decodedmsg), decodedAlphabet(decodedAlphabet), frequency(frequency) {}
    };

    string dectobi(DecimalTranslationContext ctx)
    {
        string binary(ctx.codeLength, ' ');
        for(int i = binary.length() - 1; i >= 0; i--)
        {
            binary[i] = ctx.decimal & 1 ? '1' : '0';
            ctx.decimal >>= 1;
        }
        return binary;
    }

    void* translateDecimal(void* ctx)
    {
        DecimalTranslationContext* context = (DecimalTranslationContext*)ctx;
        string binary = dectobi(*context);
        
        pthread_mutex_lock(&translationSem);
        (*(context->decodedAlphabet))[binary] = context->symbol;
        pthread_cond_broadcast(&oneTranslationFinished);
        
        while(*context->currentIndex != context->index){
            pthread_cond_wait(&printResult, &translationSem);
        }
        cout << "Character: " << context->symbol << ", Code: " << binary << ", Frequency: " << context->frequency->at(context->symbol) << endl;
        (*(context->currentIndex))++;
        pthread_cond_broadcast(&printResult);
        pthread_mutex_unlock(&translationSem);

        delete context;
        return NULL;
    }

    void* translateBinary(void* ctx)
    {
        BinaryTranslationContext* context = (BinaryTranslationContext*)ctx;
        string binary = context->encodedmsg->substr(context->index * context->codeLength, context->codeLength);
             
        //wait for this binary to be translated from the decimal input and stored in our decoded alphabet
        pthread_mutex_lock(&translationSem);
        auto it = context->decodedAlphabet->find(binary);
        while(it == context->decodedAlphabet->end())
        {
            pthread_cond_wait(&oneTranslationFinished, &translationSem);
            it = context->decodedAlphabet->find(binary);
        }
        pthread_mutex_unlock(&translationSem);

        char symbol = it->second;
        (*(context->decodedmsg))[context->index] = symbol;
        
        pthread_mutex_lock(&translationSem);
        (*(context->frequency))[symbol]++;
        pthread_mutex_unlock(&translationSem);
        
        delete context;
        return NULL;
    }

    class Decompressor
    {
    private:
        int alphabetSize;
        int codeSize;
        int decodedMessageLength;
        int currentIndex;
        map<char, int> encodedAlphabet;
        map<string, char> decodedAlphabet;
        map<char, int> frequency;
        pthread_t* alphabetRecordGenerators;
        pthread_t* messageDecoders;
        string order;
        string encodedMessage;
        string decodedMessage;

        void createAlphabetRecord(int i, char c, int dec)
        {
            pthread_create(&alphabetRecordGenerators[i], NULL, translateDecimal, new DecimalTranslationContext(i, &currentIndex, c, dec, codeSize, &decodedAlphabet, &frequency));
        }
        void translateMsgSeg(int i)
        {
            pthread_create(&messageDecoders[i], NULL, translateBinary, new BinaryTranslationContext(i, codeSize, &encodedMessage, &decodedMessage, &decodedAlphabet, &frequency));
        }
        void printResults()
        {
            currentIndex = 0;
            cout << "Alphabet:\n";                
            pthread_cond_broadcast(&printResult);
        }
        void joinGenerators()
        {
            printResults();
            for(int i = 0; i != alphabetSize; i++)
            {
                pthread_join(alphabetRecordGenerators[i], NULL);
            }
            cout << endl
            << "Decompressed message: " << decodedMessage;
        }
        void joinDecoders()
        {
            for(int i = 0; i < decodedMessageLength; i++)
            {
                pthread_join(messageDecoders[i], NULL);
            }
        }
    public:
        void readInput()
        {
            cin >> alphabetSize;
            alphabetRecordGenerators = new pthread_t[alphabetSize];
            order = string(alphabetSize, ' ');

            string input;
            int i, dec, max = 0;
            getline(cin, input);
            for(i = 0; i != alphabetSize; i++)
            {
                getline(cin, input);
                dec = stoi(input.substr(2));
                max = dec > max ? dec : max;
                encodedAlphabet[input[0]] = dec;
                frequency[input[0]] = 0;
                order[i] = input[0];
            }
            codeSize = ceil(log2(max));
            
            cin >> encodedMessage;
            decodedMessageLength = encodedMessage.length() / codeSize;
            messageDecoders = new pthread_t[decodedMessageLength];
            decodedMessage = string(decodedMessageLength, ' ');
        }

        void generateAlphabet()
        {
            pthread_mutex_init(&translationSem, NULL);
            pthread_cond_init(&oneTranslationFinished, NULL);
            pthread_cond_init(&printResult, NULL);
            for(int i = 0; i != alphabetSize; i++)
            { 
                createAlphabetRecord(i, order[i], encodedAlphabet[order[i]]);
            }
        }
        
        void translateMsg()
        {
            for(int i = 0; i != decodedMessageLength; i++)
            {
                translateMsgSeg(i);
            }
            joinDecoders();
            joinGenerators();
        }

        ~Decompressor()
        {
            pthread_mutex_destroy(&translationSem);
            pthread_cond_destroy(&oneTranslationFinished);
            pthread_cond_destroy(&printResult);
        
            delete[] alphabetRecordGenerators;
            delete[] messageDecoders;
        }
    };
}

using namespace ProgrammingAssignments;
int main()
{
    Decompressor decompressor;
    decompressor.readInput();
    decompressor.generateAlphabet();
    decompressor.translateMsg();

    return EXIT_SUCCESS;
}
