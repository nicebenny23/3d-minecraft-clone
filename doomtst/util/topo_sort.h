#pragma once

#include <unordered_map>
#include <unordered_set>
#include <stdexcept>
#include "dynamicarray.h"  // Assume Cont::array<T>
#include "stack.h"         // Assume Cont::stack<T>
#include "Option.h"        // Assume stn::Option<T>
#include "queue.h"

// Templated DAG node with static npos for "none"
template<typename T>
struct DagNode {
    static constexpr size_t npos = static_cast<size_t>(-1);
    size_t index = npos;
    T value;
    stn::array<size_t> successors;

    DagNode() = default;
    DagNode(size_t nodeIndex, const T& nodeValue)
        : index(nodeIndex), value(nodeValue) {}
};

// Templated DAG storing nodes and edges
template<typename T>
struct Dag {
    stn::array<DagNode<T>> nodes;
    void destroy() {


        nodes.destroy();



    }
    Dag() = default;
    Dag(const Dag& other) : nodes(other.nodes) {}

    // Add a node returning its index
    size_t addNode(const T& nodeValue) {
        size_t newIndex = nodes.length();
        nodes.push(DagNode<T>(newIndex, nodeValue));
        return newIndex;
    }

    // Add edge from "fromIndex" to "toIndex"
    void addEdge(size_t fromIndex, size_t toIndex) {
        auto& list = nodes[fromIndex].successors;
        if (!list.contains(toIndex)) list.push(toIndex);
    } 
    
    void EnsureEdge(const T& start, const T& end) {
        auto& list = nodes[reach_find(start)].successors;
        size_t end_ind = reach_find(end);
        if (!list.contains(end_ind)) list.push(end_ind);
        
    }
    size_t find(const T& SearchValue) const {
        for (DagNode<T> node:nodes)
        {
            if (node.id == SearchValue) {
                return node.index;
            }
        }

    }
    size_t find_create(const T& SearchValue){
        for (DagNode<T> node : nodes)
        {
            if (node.id == SearchValue) {
                return node.index;
            }
        }
        push(SearchValue);

    }

    bool contains(const T& SearchValue) const {
        for (DagNode<T> node : nodes)
        {
            if (node.value == SearchValue) {
                return true;
            }
        }
        return false;
    }

    size_t length() const { return nodes.length(); }
    DagNode<T>& operator[](size_t nodeIndex) { return nodes[nodeIndex]; }
    const DagNode<T>& operator[](size_t nodeIndex) const { return nodes[nodeIndex]; }

    using iterator = typename stn::array<DagNode<T>>::iterator;
    iterator begin() { return nodes.begin(); }
    iterator end() { return nodes.end(); }
};

// Topological sort returning values in order
template<typename T>
inline stn::array<T> dag_sort(const Dag<T>& graph) {
    size_t nodeCount = graph.length();
    stn::array<int> inDegree(nodeCount, 0);
    stn::queue<size_t> zeroQueue;

    // compute in-degrees
    for (size_t i = 0; i < nodeCount; i++) {
        for (auto succIndex : graph.nodes[i].successors) {
            ++inDegree[succIndex];
        }
    }
    // enqueue zeros
    for (size_t i = 0; i < nodeCount; ++i) {
        if (inDegree[i] == 0) zeroQueue.push(i);
    }
    // process
    stn::array<T> sorted;
   
    while (!zeroQueue.empty()) {
        size_t currentIndex = zeroQueue.pop();
        sorted.push(graph.nodes[currentIndex].value);
        for (auto succIndex : graph.nodes[currentIndex].successors) {
            if (--inDegree[succIndex] == 0) zeroQueue.push(succIndex);
        }
    }
    if (sorted.length() != nodeCount) throw std::logic_error("Cycle detected in DAG");
    return sorted;
}
template<typename T>
 bool is_acyclic(const Dag<T>& graph) {
    size_t nodeCount = graph.length()();
    stn::array<int> inDegree(nodeCount, 0);
    stn::stack<size_t> zeroQueue;

    // compute in-degrees
    for (size_t i = 0; i < nodeCount; i++) {
        for (auto succIndex : graph.nodes[i].successors) {
            ++inDegree[succIndex];
        }
    }
    // enqueue zeros
    for (size_t i = 0; i < nodeCount; ++i) {
        if (inDegree[i] == 0) zeroQueue.push(i);
    }
    // process
    size_t proccesed=0;

    while (!zeroQueue.empty()) {
        size_t currentIndex = zeroQueue.pop();
        proccesed++;
        for (auto succIndex : graph.nodes[currentIndex].successors) {
            if (--inDegree[succIndex] == 0) zeroQueue.push(succIndex);
        }
    }
    return (proccesed == nodeCount);

}
// Builder with clear names
template<typename T, typename Hash = std::hash<T>>
struct DagBuilder {
    enum class PushType { Normal, First, Last };

    Dag<T> fullGraph;                       // complete graph
    std::unordered_map<T, uint32_t, Hash> valueIndex; // map value to index
    stn::array<T> pushed;          // values in push order
    stn::Option<uint32_t> firstNode, lastNode;
    Dag<T> filteredGraph;                   // filtered graph subset
    bool dirty = false;                     // lazy rebuild flag

    void init() {
        fullGraph.nodes.clear();
        valueIndex.clear();
        pushed.clear();
        firstNode = {};
        lastNode = {};
        filteredGraph.nodes.clear();
        dirty = false;
    }

    size_t ensureNode(const T& nodeValue) {
        auto it = valueIndex.find(nodeValue);
        if (it != valueIndex.end()) return it->second;
		uint32_t newIdx =static_cast<uint32_t>(fullGraph.addNode(nodeValue));
        valueIndex[nodeValue] = newIdx;
        return newIdx;
    }

    DagBuilder& push(const T& nodeValue, PushType type = PushType::Normal) {
        size_t idx = ensureNode(nodeValue);
        if (type == PushType::First) {
            if (firstNode) throw std::logic_error("Only one first node");
            firstNode = idx;
        }
        if (type == PushType::Last) {
            if (lastNode) throw std::logic_error("Only one last node");
            lastNode = idx;
        }
        if (!pushed.contains(nodeValue)) {
            pushed.push(nodeValue);
            dirty = true;
        }
        return *this;
    }

    DagBuilder& push(T nodeValue, const stn::array<T>& dependents,const stn::array<T>& dependencies){

        size_t idx = ensureNode(nodeValue);
        for (auto depValue : dependents) {
            size_t depIndex = ensureNode(depValue);
            if (depIndex == lastNode)
                throw std::logic_error("Cannot depend on last node");
            fullGraph.addEdge(depIndex, idx);
        }
        for (auto depValue : dependencies) {
            size_t depIndex = ensureNode(depValue);
            if (depIndex == lastNode)
                throw std::logic_error("Cannot depend on last node");
            fullGraph.addEdge(idx, depIndex);
        }
        if (!pushed.contains(nodeValue)) {
            pushed.push(nodeValue);
            dirty = true;
        }
        return *this;
    }

    const Dag<T>& getFiltered() {
        if (dirty) rebuild();
        return filteredGraph;
    }

private:
    void rebuild() {
       filteredGraph = filter(fullGraph,pushed);
        dirty = false;
    }
   
};

template<typename T>
stn::array<T> filter(const stn::array<T>& whole, const stn::array<T>& keep) {
    stn::array<T> result;
    for (const auto & elem: whole)
    {
        if (keep.contains(elem))
        {
            result.push(elem);
        }
    }
    return result;
}
template<typename T>
Dag<T> filter(Dag<T> full, const stn::array<T>& keep) {
    Dag<T> new_dag;
    std::unordered_set<size_t> added;
    stn::array<size_t> remap;
    stn::array<size_t> filter_ind;
    for (size_t i = 0; i < full.length(); i++)
    {
        if (keep.contains(full[i].value))
        {
            //?
            remap.reach(i) = new_dag.length();
            new_dag.addNode(full[i].value);
            filter_ind.push(i);
        
        }

    }
    for (size_t i = 0; i < full.length(); i++)
    {
        if (keep.contains(full[i].value))
        {
           
            DagNode<T>& new_element = new_dag[remap[i]];
            new_element.successors = filter(full[i].successors,filter_ind);
            for (std::uint32_t j = 0; j < new_element.successors.length(); j++)
            {

                new_element.successors[j]= remap[new_element.successors[j]];

            } 
        }
    }
    return new_dag;
}