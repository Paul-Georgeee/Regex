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

static inline void swap_pair(pair<int, int> &p)
{
    if(p.first < p.second)
    {
        int tmp = p.first;
        p.first = p.second;
        p.second = tmp;
    }
}


void Regex::minimumize_DFA()
{
    int n = DFA.size();
    vector<vector<list<pair<int, int>>>> depend(n, vector<list<pair<int, int>>>(n, list<pair<int, int>>())); // used to evaluate for flag, see README
    vector<vector<bool>> flag(n, vector<bool>(n, false)); // used to indicate whther (i, j)(i > j) is distinguished
    list<pair<int, int>> q;
    for(int i = 0; i < n; ++i)
        for(int j = i + 1; j < n; ++j)
            for(auto ch: this->letter_table)
            {
                auto p1 = DFA[i].edge.find(ch);
                auto p2 = DFA[j].edge.find(ch);
                if(p1 != DFA[i].edge.end() && p2 != DFA[j].edge.end() && p1->second != p2->second)
                    depend[p1->second][p2->second].push_back(make_pair(i, j));
                if((p1 == DFA[i].edge.end() && p2 != DFA[j].edge.end()) || (p1 != DFA[i].edge.end() && p2 == DFA[i].edge.end()))
                {
                    auto p = make_pair(i, j);
                    swap_pair(p);
                    if(flag[p.first][p.second] == false)
                    {
                        flag[p.first][p.second] = true;
                        q.push_back(p);
                    }
                    
                }
            }

    for(int i = 0; i < n; ++i)
    {
        if(DFA[i].is_final_state == true)
        {
            for(int j = 0; j < i; ++j)
                if(DFA[j].is_final_state == false && flag[i][j] == false)
                {
                    flag[i][j] = true;
                    q.push_back(make_pair(i, j));
                }
            for(int j = i + 1; j < n; ++j)
                if(DFA[j].is_final_state == false && flag[j][i] == false)
                {
                    flag[j][i] = true;
                    q.push_back(make_pair(j, i));
                }

        }
    }

    while(!q.empty())
    {
        auto p = q.front();
        assert(p.first > p.second);
        for(auto pp: depend[p.first][p.second])
        {
            swap_pair(pp);
            if(flag[pp.first][pp.second] == false)
            {
                flag[pp.first][pp.second] = true;
                q.push_back(pp);
            }
        }
        for(auto pp: depend[p.second][p.first])
        {
            swap_pair(pp);
            if(flag[pp.first][pp.second] == false)
            {
                flag[pp.first][pp.second] = true;
                q.push_back(pp);
            }
        }
        q.pop_front();
    }

    vector<int> tmp(n, -1); 
    //used to record every node in which node in min-DFA. -1 means has not been handled
    for(int i = 0; i < n; ++i)
    {
        if(tmp[i] != -1)
            continue;

        unordered_set<int> ss({i});
        Node_D node(min_DFA.size(), ss);
        tmp[i] = node.index;
        
        for(int j = 0; j < i; ++j)
           if(flag[i][j] == false && tmp[j] == -1)
           {
                tmp[j] = node.index;
                node.subset.insert(j);
           }
        for(int j = i + 1; j < n; ++j)
            if(flag[j][i] == false && tmp[j] == -1)
            {
                tmp[j] = node.index;
                node.subset.insert(j);
            }
        
        min_DFA.push_back(node);
    }

    for(auto &node: min_DFA)
    {
        for(auto &i: node.subset)
            if(DFA[i].is_final_state)
            {
                node.is_final_state = true;
                break;
            }
        
        int src = *(node.subset.begin());
        for(auto ch: letter_table)
        {
            auto p = DFA[src].edge.find(ch);
            if(p != DFA[src].edge.end())
            {
                assert(tmp[p->second] != -1);
                node.edge[ch] = tmp[p->second];
            }
        }
    }
}

void Regex::print_DFA()
{
    for(auto &n: DFA)
    {
        cout << n.index << " subset: { ";
        for(auto i: n.subset)
            cout << i << ", ";
        cout << "}" ;
        if(n.is_final_state)
            cout << " final node" << endl;
        else
            cout << endl;
        for(auto &e: n.edge)
            cout << e.first << ": " << e.second << endl;
    }
}

void Regex::print_min_DFA()
{
    for(auto &n: min_DFA)
    {
        cout << n.index << " subset: { ";
        for(auto i: n.subset)
            cout << i << ", ";
        cout << "}";

        if(n.is_final_state)
            cout << " final node" << endl;
        else
            cout << endl;

        for(auto &e: n.edge)
            cout << e.first << ": " << e.second << endl;
    }
}

