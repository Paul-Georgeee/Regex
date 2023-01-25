#ifndef NFAH
#define NFAH

#include<vector>
#include<string>
#include<unordered_map>
#include<unordered_set>

enum Kind {LP, RP, CH, STAR, OR};
struct Token
{
    Kind kind;
    char c;
    Token(Kind k):kind(k){}
    Token(Kind k, char _c):kind(k), c(_c){} 
};

#define EPSILON 0
struct Node
{
    int index;
    std::unordered_map<char, std::vector<int>> edge;
    Node(){}
    Node(int _index) {index = _index;}
};


// extern std::vector<Node> NFA;
// extern std::unordered_set<char> letter_table;

// void to_tokens(std::string &re, std::vector<Token> &tokens);
// std::pair<int, int> to_NFA(std::vector<Token> &tokens, int start, int end);
// void print_NFA();

#endif