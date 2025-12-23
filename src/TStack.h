
#ifndef TSTACK_H
#define TSTACK_H

#include "VectorBananov.h"
#include <limits>

template <typename T>
class TStack {
    VectorBananov<T> pMem{};

public:

    void push(const T& el) {
        pMem.push_back(el);
    }

    void pop() {
        if (empty()) throw std::underflow_error("Can't pop from empty stack");
        pMem.pop_back();
    }

    T top() {
        if (empty()) throw std::underflow_error("Can't get any element from empty stack");
        return pMem[pMem.size() - 1];
    }

    bool empty() {
        if (pMem.empty()) return true;
        return false;
    }

    size_t size() {
        return pMem.size();
    }

    void clear() {
        pMem.clear();
    }

    bool operator==(const TStack & other) const {
        return other.pMem == this->pMem;
    }


    bool operator!=(const TStack & other) const {
        return !(*this == other);
    }

};


template <typename T>
class TStackMin {
    VectorBananov<T> pMem{};
    TStack<T> curMinStack{};
    T curMin = std::numeric_limits<T>::max();

public:

    // void push(const T& el) {
    //     pMem.push_back(el);
    //     curMin = std::min(el, curMin);
    //     curMinStack.push(curMin);
    // }
    void push(const T& el) {
        pMem.push_back(el);
        if (curMinStack.empty()) {
            curMin = el;
        } else {
            curMin = std::min(el, curMinStack.top());
        }
        curMinStack.push(curMin);
    }


    // void pop() {
    //     if (empty()) throw std::underflow_error("Can't pop from empty stack");
    //     pMem.pop_back();
    //     curMinStack.pop();
    // }
    void pop() {
        if (empty()) throw std::underflow_error("Can't pop from empty stack");
        pMem.pop_back();
        curMinStack.pop();
        if (curMinStack.empty()) {
            curMin = std::numeric_limits<T>::max();
        } else {
            curMin = curMinStack.top();
        }
    }


    T get_min() {
        if (empty()) throw std::underflow_error("Can't get min from empty stack");
        return curMinStack.top();
    }

    T top() {
        if (empty()) throw std::underflow_error("Can't get any element from empty stack");
        return pMem[pMem.size() - 1];
    }

    bool empty() {
        if (pMem.empty()) return true;
        return false;
    }

    size_t size() {
        return pMem.size();
    }

    void clear() {
        pMem.clear();
        curMin = std::numeric_limits<T>::max();
        curMinStack.clear();
    }

    bool operator==(const TStackMin & other) const {
        return other.pMem == this->pMem;
    }


    bool operator!=(const TStackMin & other) const {
        return !(*this == other);
    }

};

#endif //TSTACK_H
