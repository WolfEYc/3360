#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <map>
#include <cmath>
#include <iostream>
using namespace std;

namespace MyServer 
{
    #define BUF_SIZE 1024
    #define DEFAULT_BACKLOG 5
    const int opt = 1;
    const int domain = AF_INET;
    const int type = SOCK_STREAM;
    const int hostaddress = INADDR_ANY;
    const int protocol = 0;

    enum EventType
    {
        GET_CODELEN,
        TRANSLATE_CODE
    };

    string dec2bi (int dec)
    {
        string res = "";
        while(dec)
        {
            if(dec & 1)
                res += "1";
            else
                res += "0";
            dec >>= 1;
        }
        return res;
    }

    void EndChild(int signum)
    {
        wait(NULL);
    }

    class Socket
    {
    private:
        int id;
        ssize_t bytesRead;
        char buffer[BUF_SIZE];
        sockaddr_in address;
        int addrlen;
        Server* server;
    public:
        //Passive Socket   
        Socket(Server* server) : server(server)
        {
            addrlen = sizeof(address);
            if(!(id = socket(domain, type, protocol)))
            {
                cerr << "socket creation failed" << endl;
            }
            if (setsockopt(id, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
            {
                cerr << "setting socket options failed" << endl;
            }
            address.sin_family = domain;
            address.sin_addr.s_addr = hostaddress;
            address.sin_port = htons(server->port);
            if(bind(id, (sockaddr*)&address, sizeof(address))<0)
            {
                cerr << "socket port binding failed" << endl;
            }
            if(listen(id, DEFAULT_BACKLOG) < 0)
            {
                cerr << "socket listen failed" << endl;
            }
        }

        //Active Socket
        Socket(const Socket& passive) : server(passive.server)
        {
            if((id = accept(passive.id, (sockaddr*)&passive.address, (socklen_t*)&passive.addrlen)) < 0)
            {
                cerr << "socket accept failed" << endl;
            }
            address = passive.address;
            addrlen = passive.addrlen;
            if((bytesRead = read(id, buffer, BUF_SIZE)) <= 0)
            {
                cerr << "could not receive message properly" << endl;
            }
            else
            {
                handleEvent();
            }
        }

        void handleEvent()
        {
            int pid = fork();
            if(!pid)
            {
                EventType event = (EventType)(buffer[0] - '0');
                string msg(buffer);
                if(bytesRead != 1)
                {
                    msg = msg.substr(1);
                }
                on(event, msg);
            } 
            else
            {
                signal(SIGCHLD, EndChild);
            }
        }

        void on(EventType event, string msg)
        {
            switch(event)
            {
                case GET_CODELEN:
                    send(id, (void*)server->codeLen, sizeof(int), 0);
                    break;
                case TRANSLATE_CODE:
                    send(id, (void*)server->rossetta[msg], sizeof(char), 0);
                    break;
                default:
                    cerr << "event unhandled!" << endl;
                    break;
            }
        }
    };

    class Server
    {
    private:
        int port;
        Socket passive;
        int codeLen;
        map<string, char> rossetta;
        void ReadFromFile ()
        {
            int n, dec, max = 0;
            char c;
            for(int i = 0; i != n; i++) 
            {
                cin >> c >> dec;
                max = dec > max ? dec : max;
                rossetta[dec2bi(dec)] = c;
            }
            codeLen = ceil(log2(max + 1));
        }
    public:
        friend class Socket;
        Server(int port) : port(port), passive(this)
        {
            ReadFromFile();
            while(true)
            {
                Socket active(passive);
            }
        }
    };
}


int main (int argc, char** argv)
{
    if(argc < 2)
    {
        cerr << "./exec_filename port_no < input_filename" << endl;
        return EXIT_FAILURE;
    }
    int port = stoi(argv[1]);
    MyServer::Server server(port);
    return EXIT_SUCCESS;
}
