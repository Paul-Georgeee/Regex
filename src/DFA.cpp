#include<iostream>
#include<assert.h>
#include<RE.h>
using namespace std;
int Regex::find_node(unordered_set<int> &s)
{
    for(int i = 0; i < DFA.size(); ++i)
        if(s == DFA[i].subset)
            return i;
    return -1;
}

int Regex::epsilon_closure(unordered_set<int> &s, list<int> &handle_list)
{
    list<int> q(s.begin(), s.end());
    while (!q.empty())
    {
        int index = q.front();
        auto re = NFA[index].edge.find(EPSILON);
        if(re != NFA[index].edge.end())
        {
            for(auto i: re->second)
            {
                if(s.count(i) == 0)
                {
                    s.insert(i);
                    q.push_back(i);
                }
            }
        }
        q.pop_front();
    }
    int ret = find_node(s);
    if(ret != -1)
        return ret;
    Node_D n(DFA.size(), s);
    DFA.push_back(n);
    handle_list.push_back(n.index);
    return n.index;
}

void Regex::subset_construct(int NFA_start, int NFA_end)
{
    list<int> q;
    unordered_set<int> tmp({NFA_start});
    epsilon_closure(tmp, q);
    while(!q.empty())
    {
        int index = q.front();
        assert(index < DFA.size());
        for(auto ch: letter_table)
        {
            unordered_set<int> s;
            for(auto nfa: DFA[index].subset)
            {
                auto match = NFA[nfa].edge.find(ch);
                if(match != NFA[nfa].edge.end())
                    s.insert(match->second.begin(), match->second.end());
            }
            if(!s.empty())
                DFA[index].edge[ch] = epsilon_closure(s, q);
            
        }
        q.pop_front();
    }
    for(auto &n: DFA)
        if(n.subset.count(NFA_end) != 0)
            n.is_final_state = true;
}

void Regex::print_DFA()
{
    for(auto &n: DFA)
    {
        cout << n.index << " subset: { ";
        for(auto i: n.subset)
            cout << i << ", ";
        cout << "}" << endl;
        for(auto &e: n.edge)
            cout << e.first << ": " << e.second << endl;
    }
}