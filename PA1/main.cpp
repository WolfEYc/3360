#include <pthread.h>
#include <cmath>
#include <sstream>
#include <string>
#include <map>
#include <iostream>
#include <queue>
#include <atomic>
using namespace std;

class Decompressor;
class Resolver;

using ThreadArguments = tuple<Decompressor&, string, char&>;

class Resolver
{
private:
    pthread_t _threadID;
    ThreadArguments* args;
    char output;
public:
    char resolve();
    Resolver(string input, Decompressor& decompressor);
    ~Resolver();
};

class Decompressor
{
private:
    unsigned int _n, bpc;
    map<string, char> alpha;
    queue<Resolver*> resolverQ;
    map<char, pair<string, unsigned int>> store;
    map<char, atomic<unsigned int>> freq;
    string output;
    string order;
    void inputAlpha();
    void readMsg();
public:
    friend void* decode(void* args);
    Decompressor();
    const unsigned int& getBPC() const;
    void decompress();
    void print_results();
    ~Decompressor();
};

string dtob (unsigned int dec, const unsigned int bits)
{
    string result(bits, '0');
    for(int i = 0; i != bits; i++, dec /= 2)
    {
        result[bits - i - 1] = ((char)(dec % 2)) + '0';
    }
    return result;
}

void *decode (void *args)
{
    auto& [decompressor, input, output] = *((ThreadArguments*) args);
    output = decompressor.alpha.at(input);
    decompressor.freq.at(output)++;
    pthread_exit(NULL);
}

Decompressor::Decompressor() : output() {}
Decompressor::~Decompressor() {}

void Decompressor::inputAlpha() {
    cin >> _n;
    char c;
    string s;
    unsigned int code, h = 0, i;
    getline(cin, s);

    for(i = 0; i != _n; i++) {
        getline(cin, s);
        c = s[0];
        code = stoi(s.substr(2));

        auto& [binary, decimal] = store[c];
        decimal = code;
        freq[c];
        order += c;
        
        if (code > h) h = code;
    }
    //bits per char
    bpc = ceil(log2(h));
    //caching phase
    for(auto& [symbol, data] : store)
    {
        auto& [binary, decimal] = data;
        binary = dtob(decimal, bpc);
        alpha[binary] = symbol;
    }
}

void Decompressor::readMsg() {
    bool good = 1;
    string s;
    cin >> s;
    stringstream ss(s);

    while(good) {
        string temp;
        temp.resize(bpc);
        for(int i = 0; i != bpc; i++) {
            if(!(ss >> temp[i])){
                good = 0;
                break;
            }
        }
        if(!good) break;

        resolverQ.push(new Resolver(temp, *this));
    }

    while(!resolverQ.empty()) {
        output += resolverQ.front()->resolve();
        delete resolverQ.front();
        resolverQ.pop();
    }
}

void Decompressor::decompress()
{
    inputAlpha();
    readMsg();
    print_results();
}

void Decompressor::print_results()
{
    cout << "Alphabet:" << endl;
    for(char symbol : order) {
        auto& [binary, decimal] = store[symbol];
        cout << "Character: " << symbol << ", Code: " << binary << ", Frequency: " << freq[symbol] << endl;
    }
    cout << endl
    << "Decompressed message: " << output;
}

Resolver::Resolver(string input, Decompressor& decompressor) 
{
    args = new ThreadArguments(decompressor, input, output);
    pthread_create(&_threadID, NULL, decode, args);
}

char Resolver::resolve() 
{
    pthread_join(_threadID, NULL);
    return output;
}

Resolver::~Resolver() 
{
    delete args;
}

int main () 
{
    Decompressor decompressor;
    decompressor.decompress();
    return EXIT_SUCCESS;
}
