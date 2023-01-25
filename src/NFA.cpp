#include<iostream>
#include<RE.h>
#include<assert.h>
#include<stack>

using namespace std;

static void err(string s)
{
    cerr << s << endl;
    exit(-1);
}

static void check(bool b, string s)
{
    if(b)
        return;
    err(s);
}

void Regex::to_tokens(std::string &re, std::vector<Token> &tokens)
{
    //check parentheses match
    stack<char> s;
    //meet '\' flag
    bool flag = false;

    for(auto &c: re)
    {
        if(flag)
        {
            flag = false;
            if(c != '(' && c != ')' && c != '|' && c != '*' && c != '\\')
                err("Unkonw use escape sequence");
            tokens.push_back(Token(CH, c));
            letter_table.insert(c);
        }
        else
        {
            switch (c)
            {
            case '(':
                s.push('(');
                tokens.push_back(Token(LP));
                break;
            case ')':
                if(s.empty())
                    err("Parentheses mismatches");
                s.pop();
                tokens.push_back(Token(RP));
                break;
            case '|':
                tokens.push_back(Token(OR));
                break;
            case '*':
                tokens.push_back(Token(STAR));
                break;
            case '\\':
                flag = true;
                break;
            default:
                tokens.push_back(Token(CH, c));
                letter_table.insert(c);
                break;
            }
        }
    }

    if(!s.empty())
        err("Parentheses mismatches");
}


void Regex::insert_edge(Node &n, char c, int index)
{
    auto it = n.edge.find(c);
    if(it != n.edge.end())
        it->second.push_back(index);
    else
        n.edge.insert(make_pair(c, vector<int>(1, index)));
}

int Regex::find_next_matched_RP(vector<Token> &tokens, int lp, int end)
{
    assert(tokens[lp].kind == LP);
    stack<int> s;
    s.push(1);
    for(int i = lp + 1; i < end; ++i)
    {
        if(tokens[i].kind == LP)
            s.push(1);
        else if(tokens[i].kind == RP)
            s.pop();
        
        if(s.empty())
            return i;
    }
    err("Should never reach here in func \"find_next_matched_RP\"");
    return -1;
}

pair<int, int> Regex::gen_sequence(vector<Token> &tokens, int start, int end)
{
    int first = NFA.size();
    for(int i = start; i < end; ++i)
    {
        Node n1;
        n1.index = NFA.size();
        insert_edge(n1, tokens[i].c, n1.index + 1);
        NFA.push_back(n1);
    }
    Node n2(NFA.size());
    NFA.push_back(n2);
    return make_pair(first, n2.index);
}

pair<int, int> Regex::merge(pair<int, int> &p1, pair<int, int> &p2)
{
    check(p1.first != -1, "p1.first should not be -1 in merge func");
    if(p2.first == -1)
        return p1;
    insert_edge(NFA[p1.second], EPSILON, p2.first);
    return make_pair(p1.first, p2.second);
}

void Regex::add_star(pair<int, int> &p1)
{
    Node n1(NFA.size()), n2(NFA.size() + 1);
    vector<int> e({p1.first, p1.second});
    n1.edge.insert(make_pair(EPSILON, e));
    insert_edge(NFA[p1.second], EPSILON, p1.first);
    insert_edge(NFA[p1.second], EPSILON, n2.index);
    NFA.push_back(n1);
    NFA.push_back(n2);
    p1.first = n1.index;
    p1.second = n2.index;
}

pair<int, int> Regex::to_NFA(vector<Token> &tokens, int start, int end)
{
    if(start >= end)
        return make_pair(-1, -1);
    if(tokens[start].kind == LP && find_next_matched_RP(tokens, start, end) == end - 1)
        return to_NFA(tokens, start + 1, end - 1);

    stack<int> s;
    for(int i = end - 1; i >= start; --i)
    {
        if(tokens[i].kind == RP)
            s.push(0);
        else if(tokens[i].kind == LP)
            s.pop();

        if(tokens[i].kind == OR && s.empty() == true)
        {
            if(i == start || i == end - 1)
                err("error use '|'");
            auto p1 = to_NFA(tokens, start, i);
            auto p2 = to_NFA(tokens, i + 1, end);
            assert(p1.first != -1 && p2.first != -1);
            Node n1(NFA.size()), n2(NFA.size() + 1);
            vector<int> e({p1.first, p2.first});
            n1.edge.insert(make_pair(EPSILON, e));
            insert_edge(NFA[p1.second], EPSILON, n2.index);
            insert_edge(NFA[p2.second], EPSILON, n2.index);
            
            NFA.push_back(n1);
            NFA.push_back(n2);
            return make_pair(n1.index, n2.index);
        }
    }

    if(tokens[start].kind == LP)
    {
        int matched_index = find_next_matched_RP(tokens, start, end);
        check(matched_index != end - 1, "match index can not be \'end - 1\' here");
        auto p1 = to_NFA(tokens, start + 1, matched_index);
        check(p1.first != -1, "Error usage of \'(\'");
        if(tokens[matched_index + 1].kind == STAR)
        {
            add_star(p1);
            ++matched_index;
        }
        auto p2 = to_NFA(tokens, matched_index + 1, end);
        return merge(p1, p2);
    }
    else if(tokens[start].kind == CH)
    {
        int index = start + 1;
        for(; index < end; ++index)
            if(tokens[index].kind != CH)
                break; 
        
        pair<int, int> p1;

        if(index == end)
            return gen_sequence(tokens, start, index);
        else if(tokens[index].kind == STAR)
        {
            p1 = gen_sequence(tokens, index - 1, index);
            add_star(p1);
            if(index - 1 != start)
            {
                auto p2 = gen_sequence(tokens, start, index - 1);
                p1 = merge(p2, p1);
            }
            ++index;
        }
        else
            p1 = gen_sequence(tokens, start, index);
            
        if(index == end)
            return p1;

        auto p2 = to_NFA(tokens, index, end);
        return merge(p1, p2);
    }
    else
        err("Error");
}

void Regex::print_NFA()
{
    for(auto &n: NFA)
    {
        cout << n.index << ":\n";
        for(auto &e: n.edge)
        {
            string s = e.first == EPSILON ? "epsilon" : e.first + string(6, ' ');
            cout << "\t" << s << ":";
            for(auto &i: e.second)
                cout << i << " ";
            cout << endl;
        }
    }
}