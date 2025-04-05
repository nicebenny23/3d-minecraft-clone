#include "chainpool.h"
#include <stdexcept>
#include <cstring> // For std::memset

namespace chainutil {
    metalist::metalist() : start(nullptr), end(nullptr), length(0) {}

    bool metalist::empty() {
        return length == 0;
    }

    void metalist::shrink()
    {
        length--;
        if (empty())
        {
            reset();
        }

    }

    void metalist::remove(blockmetadata* block)
    {

        if (block == nullptr) {

            throw std::invalid_argument("cannot remove a nullptr from a list");
        }
        if (block == end)
        {
            end = block->prev;
        }
        else
        {

            block->next->prev = block->prev;
        }
        if (block == start)
        {
            start = block->next;
        }
        else
        {
            block->prev->next = block->next;
        }
        block->next = nullptr;
        block->prev = nullptr;
        shrink();
    }
    blockmetadata* metalist::pop() {
        if (empty()) {
            throw std::runtime_error("Cannot pop from an empty queue");
        }
        blockmetadata* node = start;
        remove(start);
        return node;
    }


    void metalist::push(blockmetadata* node) {
        if (empty()) {

            start = node;
            start->prev = nullptr;
        }
        else {

            end->next = node;
            node->prev = end;
        }

        end = node;
        end->next = nullptr;

        length++;
    }
    void metalist::reset()
    {

        *this = metalist();
    }
}