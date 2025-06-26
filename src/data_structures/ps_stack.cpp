#include "ps_stack.h"
#include <iostream>
#include <stdexcept>

namespace PostScript {

void PSStack::push(const PSObjectPtr& obj) {
    stack_.push_back(obj);
}

PSObjectPtr PSStack::pop() {
    if (stack_.empty()) {
        throw std::runtime_error("Stack underflow: cannot pop from empty stack");
    }
    
    PSObjectPtr top = stack_.back();
    stack_.pop_back();
    return top;
}

PSObjectPtr PSStack::peek() const {
    if (stack_.empty()) {
        throw std::runtime_error("Stack underflow: cannot peek empty stack");
    }
    
    return stack_.back();
}

void PSStack::dup() {
    if (stack_.empty()) {
        throw std::runtime_error("Stack underflow: cannot dup empty stack");
    }
    
    // Create a new copy of the top element
    PSObjectPtr top = stack_.back();
    PSObjectPtr copy = std::make_shared<PSObject>(*top);
    stack_.push_back(copy);
}

void PSStack::exch() {
    if (stack_.size() < 2) {
        throw std::runtime_error("Stack underflow: need at least 2 elements for exch");
    }
    
    // Exchange the top two elements
    std::swap(stack_[stack_.size() - 1], stack_[stack_.size() - 2]);
}

void PSStack::clear() {
    stack_.clear();
}

void PSStack::print() const {
    std::cout << "Stack (" << stack_.size() << " elements): ";
    
    if (stack_.empty()) {
        std::cout << "empty";
    } else {
        for (size_t i = 0; i < stack_.size(); ++i) {
            if (i > 0) std::cout << " ";
            std::cout << stack_[i]->toString();
        }
    }
    std::cout << std::endl;
}

} // namespace PostScript 