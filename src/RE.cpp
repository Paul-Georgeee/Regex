#include<RE.h>
#include<iostream>
using namespace std;

Regex::Regex(std::string _re)
{
    this->re = _re;
    vector<Token> t;
    this->to_tokens(this->re, t);
    auto p = this->to_NFA(t, 0, t.size());
    this->subset_construct(p.first, p.second);
#ifdef DEBUG
    this->print_DFA();
#endif

    this->minimumize_DFA();
    cout << "DFA size: " << DFA.size() << ". min-DFA size: " << min_DFA.size() << endl;

#ifdef DEBUG
    this->print_min_DFA();
#endif
}

bool Regex::match(std::string str)
{
    int current = 0;
    for(auto ch: str)
    {
        auto it = this->min_DFA[current].edge.find(ch);
        if(it != this->min_DFA[current].edge.end())
            current = it->second;
        else
            return false;
    }
    return min_DFA[current].is_final_state == true;
}

bool Regex::match_with_originalDFA(std::string str)
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
