#include <thread>
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

class Resolver
{
private:
    thread* _th;
public:
    Resolver(string input, char& output, Decompressor& decompressor);
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
    void inputAlpha();
    void readMsg();
public:
    void decode(string input, char& output);
    Decompressor();
    const unsigned int& getBPC() const;
    void decompress();
    void print_results();
    ~Decompressor();
};

string dtob (unsigned int dec, const unsigned int bits)
{
    string result(bits, '0');
    for(int i = 0; dec > 0; i++, dec /= 2)
    {
        result[bits - i - 1] = ((char)dec % 2 + '0');
    }
    return result;
}

void Decompressor::decode (string input, char& output)
{
    output = alpha[input];
    freq.at(output)++;
}

Decompressor::Decompressor() : output() {}
Decompressor::~Decompressor() {}

void Decompressor::inputAlpha() {
    cin >> _n;
    char c;
    unsigned int code, h = 0, i;
    for(i = 0; i != _n; i++) {
        cin >> c >> code;
        //symbol -> dec, freq
        auto& [binary, decimal] = store[c];
        decimal = code;
        freq[c];
        
        if (code > h) {
            h = code;
        }
    }
    //bits per char
    bpc = ceil(log2(h));
    //caching phase
    for(auto& [symbol, data] : store)
    {
        auto& [binary, decimal] = data;
        //extract code
        binary = dtob(decimal, bpc);
        //binary -> symbol
        alpha[binary] = symbol;
    }
}

void Decompressor::readMsg() {
    bool good = 1;
    string s;
    cin >> s;
    stringstream ss(s);

    while(good) {
        output.resize(output.size() + 1);
        string temp;
        temp.resize(bpc);
        for(int i = 0; i != bpc; i++) {
            if(!(ss >> temp[i])){
                good = 0;
                break;
            }
        }
        if(!good) break;

        resolverQ.push(new Resolver(temp, output.back(), *this));
    }

    while(!resolverQ.empty()) {
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
    cout << endl << "Alphabet:" << endl;
    for(auto& [symbol, data] : store) {
        auto& [binary, decimal] = data;
        cout << "Character: " << symbol << ", Code: " << binary << ", Frequency: " << freq[symbol] << endl;
    }
    cout << endl
    << "Decompressed message: " << output << endl;
}

Resolver::Resolver(string input, char& output, Decompressor& decompressor) 
{
    _th = new thread(&Decompressor::decode, &decompressor, input, ref(output));
}

Resolver::~Resolver() {
    _th->join();
    delete _th;
}

int main () 
{
    Decompressor decompressor;
    decompressor.decompress();

    return 0;
}


