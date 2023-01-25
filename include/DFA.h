#ifndef DFAH
#define DFAH

#include<unordered_set>
#include<vector>
#include<unordered_map>
#include<list>

struct Node_D{
    int index;
    bool is_final_state = false;
    std::unordered_set<int> subset;
    std::unordered_map<char, int> edge;

    Node_D(int _index, std::unordered_set<int> &_set) {
        index = _index;
        subset = _set;
    }
};

#endif