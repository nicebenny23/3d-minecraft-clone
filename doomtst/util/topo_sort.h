#pragma once

#include "dynamicarray.h"
#include "stack.h"
#include <unordered_map>

struct Edge {
    Edge() :pred() {


    }
    Edge(const Edge& oth) {
        pred = oth.pred;
    }
    Cont::array<size_t> pred;
    Edge(const Cont::array<size_t>& preds) :pred(preds) {}
};

struct Dag {
    Cont::array<Edge> list;
    Edge& operator[](size_t ind) {
        return list[ind];
    }
    size_t length() {
        return list.length;
    }

    Dag(const Dag& oth)
        : list(oth.list)
    {
    }
    Dag() {
        
    }
};

inline Cont::array<size_t> dag_sort_indices(Dag dag) {
    Cont::array<int> num_front;
    Cont::stack<size_t> zero_front;
    for (size_t i = 0; i < num_front.length; i++)
    {
        num_front[i] = 0;
    }
    for (int i = 0; i < dag.list.length; i++)
    {
        for (size_t bk : dag[i].pred)
        {
            num_front[bk]++;
            int l = 1;
        }
    }
    for (int i = 0; i < dag.length(); i++)
    {
        if (num_front[i] == 0)
        {
            zero_front.push(i);
        }
    }
    Cont::array<size_t> result;
    while (!zero_front.empty())
    {
        size_t freed = zero_front.pop();
        result.push(freed);
        size_t len = dag[freed].pred.length;
        for (size_t i = 0; i < len; i++)
        {
            size_t back_ind = dag[freed].pred[i];
            num_front[back_ind]--;
            if (num_front[back_ind] == 0)
            {
                zero_front.push(back_ind);
            }
        }
    }
    if (result.length != dag.length())
    {
        throw std::logic_error("Attempted to topoligical sort an cyclic graph");
    }
    for (size_t i = 0; i < result.length / 2; i++)
    {
        std::swap(result[i], result[result.length - 1 - i]);
    }
    return result;
}

template <typename T, typename GetPredecessors, typename Hash = std::hash<T>>
Cont::array<T> topo_sort(Cont::array<T>& items, GetPredecessors get_predecessors) {
    std::unordered_map<T, size_t, Hash> map;
    Dag graph;
    for (size_t i = 0; i < items.length; i++)
    {

        map[items[i]] = i;
    }
    for (size_t i = 0; i < items.length; i++)
    {
        Cont::array<T> preds = get_predecessors(items[i]);
        Cont::array<size_t> nebs;
        for (size_t k = 0; k < preds.length; k++)
        {
            nebs[k] = map[preds[k]];

        }
        graph.list.push(Edge(nebs));

    }
    Cont::array<size_t> res = dag_sort_indices(graph);
    Cont::array<T> fin;
    for (size_t i = 0; i < res.length; i++)
    {
        fin[i] = items[res[i]];
    }
    return fin;
}