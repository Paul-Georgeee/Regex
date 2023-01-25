#include<RE.h>
using namespace std;

Regex::Regex(std::string _re)
{
    this->re = _re;
    vector<Token> t;
    this->to_tokens(this->re, t);
    auto p = to_NFA(t, 0, t.size());
    subset_construct(p.first, p.second);
}

bool Regex::match(std::string str)
{
    int current = 0;
    for(auto ch: str)
    {
        auto it = this->DFA[current].edge.find(ch);
        if(it != this->DFA[current].edge.end())
            current = it->second;
        else
            return false;
    }
    return DFA[current].is_final_state == true;
}