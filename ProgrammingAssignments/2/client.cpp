#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>
#include <queue>
using namespace std;

namespace MyClient
{
#define BUFF_SIZE 1024

    enum EventType
    {
        GET_CODELEN,
        TRANSLATE_CODE
    };

    struct Query
    {
        EventType eventType;
        string msg;
        const char *ToString() const
        {
            return (to_string(eventType) + msg).c_str();
        }
    };

    class Socket
    {
    private:
        int id;
        int bytesRead;
        sockaddr_in server_address;
        Query query;

    public:
        Socket(string hostname, int port)
        {
            if ((id = socket(AF_INET, SOCK_STREAM, 0)) < 0)
            {
                cerr << "Socket creation error" << endl;
                return;
            }
            server_address.sin_family = AF_INET;
            server_address.sin_port = htons(port);
            if (inet_pton(AF_INET, hostname.c_str(), &server_address.sin_addr) <= 0)
            {
                cerr << "Invalid address" << endl;
                return;
            }
            if (connect(id, (sockaddr *)&server_address, sizeof(server_address)) < 0)
            {
                cerr << "Connection Failed" << endl;
                return;
            }
        }    

        int GetCodeLength()
        {
            int response;
            query.eventType = GET_CODELEN;
            send(id, query.ToString(), sizeof(query.ToString()), 0);
            bytesRead = read(id, (void *)&response, sizeof(int));
            return response;
        }

        char TranslateMessage(string code)
        {
            char response;
            query.eventType = TRANSLATE_CODE;
            query.msg = code;
            send(id, query.ToString(), sizeof(query.ToString()), 0);
            bytesRead = read(id, (void *)&response, sizeof(char));
            return response;
        }
    };

    typedef void *(*THREADFUNCPTR)(void *);

    class Translator
    {
    private:
        bool resolved;
        char output;
        string input;
        string hostname;
        int port;
        pthread_t tid;
        void *decode()
        {
            Socket socket(hostname, port);
            output = socket.TranslateMessage(input);
        }

    public:
        Translator(string input, string hostname, int port) : input(input), hostname(hostname), port(port), resolved(false)
        {
            pthread_create(&tid, NULL, (THREADFUNCPTR)&Translator::decode, this);
        }
        char Output()
        {
            if (!resolved)
            {
                pthread_join(tid, NULL);
                resolved = true;
            }
            return output;
        }
        ~Translator()
        {
            if (!resolved)
            {
                pthread_join(tid, NULL);
            }
        }
    };

    class Decompressor
    {
    private:
        string hostname;
        int port;
        queue<Translator> translators;
        void ReadFromInput()
        {
        }

    public:
        Decompressor(string hostname, int port) : hostname(hostname), port(port)
        {
            ReadFromInput();
        }
    };

}
