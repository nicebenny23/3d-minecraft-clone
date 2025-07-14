#pragma once

#include <unordered_map>
#include <unordered_set>
#include <stdexcept>
#include "dynamicarray.h"  // Assume Cont::array<T>
#include "stack.h"         // Assume Cont::stack<T>
#include "Option.h"        // Assume Opt::Option<T>

// Templated DAG node with static npos for "none"
template<typename T>
struct DagNode {
    static constexpr size_t npos = static_cast<size_t>(-1);
    size_t index = npos;
    T value;
    Cont::array<size_t> successors;

    DagNode() = default;
    DagNode(size_t nodeIndex, const T& nodeValue)
        : index(nodeIndex), value(nodeValue) {}
};

// Templated DAG storing nodes and edges
template<typename T>
struct Dag {
    Cont::array<DagNode<T>> nodes;

    Dag() = default;
    Dag(const Dag& other) : nodes(other.nodes) {}

    // Add a node returning its index
    size_t addNode(const T& nodeValue) {
        size_t newIndex = nodes.length;
        nodes.push(DagNode<T>(newIndex, nodeValue));
        return newIndex;
    }

    // Add edge from "fromIndex" to "toIndex"
    void addEdge(size_t fromIndex, size_t toIndex) {
        auto& list = nodes[fromIndex].successors;
        if (!list.contains(toIndex)) list.push(toIndex);
    }

    size_t length() const { return nodes.length; }
    DagNode<T>& operator[](size_t nodeIndex) { return nodes[nodeIndex]; }
    const DagNode<T>& operator[](size_t nodeIndex) const { return nodes[nodeIndex]; }

    using iterator = typename Cont::array<DagNode<T>>::iterator;
    iterator begin() { return nodes.begin(); }
    iterator end() { return nodes.end(); }
};

// Topological sort returning values in order
template<typename T>
inline Cont::array<T> dag_sort(const Dag<T>& graph) {
    size_t nodeCount = graph.length();
    Cont::array<int> inDegree(nodeCount, 0);
    Cont::stack<size_t> zeroQueue;

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
    Cont::array<T> sorted;
   
    while (!zeroQueue.empty()) {
        size_t currentIndex = zeroQueue.pop();
        sorted.push(graph.nodes[currentIndex].value);
        for (auto succIndex : graph.nodes[currentIndex].successors) {
            if (--inDegree[succIndex] == 0) zeroQueue.push(succIndex);
        }
    }
    if (sorted.length != nodeCount) throw std::logic_error("Cycle detected in DAG");
    return sorted;
}

// Builder with clear names
template<typename T, typename Hash = std::hash<T>>
struct DagBuilder {
    enum class PushType { Normal, First, Last };

    Dag<T> fullGraph;                       // complete graph
    std::unordered_map<T, size_t, Hash> valueIndex; // map value to index
    Cont::array<T> pushed;          // values in push order
    Opt::Option<size_t> firstNode, lastNode;
    Dag<T> filteredGraph;                   // filtered graph subset
    bool dirty = false;                     // lazy rebuild flag

    void init() {
        fullGraph.nodes.destroy();
        valueIndex.clear();
        pushed.destroy();
        firstNode = {};
        lastNode = {};
        filteredGraph.nodes.destroy();
        dirty = false;
    }

    size_t ensureNode(const T& nodeValue) {
        auto it = valueIndex.find(nodeValue);
        if (it != valueIndex.end()) return it->second;
        size_t newIdx = fullGraph.addNode(nodeValue);
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

    DagBuilder& push(T nodeValue, Cont::array<T>& dependents,Cont::array<T>& dependencies){

        size_t idx = ensureNode(nodeValue);
        for (auto depValue : dependents) {
            size_t depIndex = ensureNode(depValue);
            if (lastNode && depIndex == lastNode.unwrap())
                throw std::logic_error("Cannot depend on last node");
            fullGraph.addEdge(depIndex, idx);
        }
        for (auto depValue : dependencies) {
            size_t depIndex = ensureNode(depValue);
            if (lastNode && depIndex == lastNode.unwrap())
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
        // Determine all active indices
        std::unordered_set<size_t> activeSet;
        for (auto& entry : valueIndex) activeSet.insert(entry.second);

        // Clear previous filtered graph
        filteredGraph.nodes.destroy();

        // Build ordered list: first insertionOrder, then remaining active nodes
        Cont::array<size_t> orderedIndices;
        std::unordered_set<size_t> added;
        // Add in insertion order
        for (auto& val : pushed) {
            size_t idx = valueIndex[val];
            orderedIndices.push(idx);
            added.insert(idx);
        }
        // Append any other active nodes
        for (size_t idx = 0; idx < fullGraph.length(); idx++) {
            if (activeSet.count(idx) && !added.count(idx)) {
                orderedIndices.push(idx);
                added.insert(idx);
            }
        }

        // Add nodes to filtered graph; addNode sets correct index
        for (size_t oldIdx : orderedIndices) {
            filteredGraph.addNode(fullGraph.nodes[oldIdx].value);
        }

        // Prepare remap from old->new
        Cont::array<size_t> remap(fullGraph.length());
        
        for (size_t newIdx = 0; newIdx < orderedIndices.length; newIdx++) {
            remap[orderedIndices[newIdx]] = newIdx;
        }

        // Add edges within filtered graph
        for (size_t newIdx = 0; newIdx < orderedIndices.length; newIdx++) {
            size_t oldIdx = orderedIndices[newIdx];
            for (auto succIdx : fullGraph.nodes[oldIdx].successors) {
                if (activeSet.count(succIdx)) {
                    filteredGraph.addEdge(newIdx, remap[succIdx]);
                }
            }
        }

        dirty = false;
    }
};
