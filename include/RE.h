#ifndef REH
#define REH

#include<NFA.h>
#include<DFA.h>

class Regex{
private:
    std::string re;
    std::unordered_set<char> letter_table;
    std::vector<Node> NFA;
    std::vector<Node_D> DFA;

    void insert_edge(Node &n, char c, int index);
    int find_next_matched_RP(std::vector<Token> &tokens, int lp, int end);
    std::pair<int, int> gen_sequence(std::vector<Token> &tokens, int start, int end);
    std::pair<int, int> merge(std::pair<int, int> &p1, std::pair<int, int> &p2);
    void add_star(std::pair<int, int> &p1);
    void to_tokens(std::string &re, std::vector<Token> &tokens);
    std::pair<int, int> to_NFA(std::vector<Token> &tokens, int start, int end);
    void print_NFA();

    int find_node(std::unordered_set<int> &s);
    int epsilon_closure(std::unordered_set<int> &s, std::list<int> &handle_list);
    void subset_construct(int NFA_start, int NFA_end);
    void print_DFA();

public:
    Regex(std::string _re);
    bool match(std::string str);
};

#endif