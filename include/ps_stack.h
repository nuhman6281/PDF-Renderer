#ifndef PS_STACK_H
#define PS_STACK_H

#include "ps_types.h"
#include <vector>
#include <stdexcept>

namespace PostScript {

/**
 * @brief PostScript operand stack implementation
 * 
 * The operand stack is the primary data structure in PostScript.
 * All operations consume and produce values on this stack.
 */
class PSStack {
public:
    PSStack() = default;
    ~PSStack() = default;

    // Stack operations
    void push(const PSObjectPtr& obj);
    PSObjectPtr pop();
    PSObjectPtr peek() const;
    
    // Stack manipulation
    void dup();  // Duplicate top element
    void exch(); // Exchange top two elements
    void clear(); // Clear the stack
    
    // Stack information
    size_t size() const { return stack_.size(); }
    bool empty() const { return stack_.empty(); }
    
    // Debugging
    void print() const;

private:
    std::vector<PSObjectPtr> stack_;
};

} // namespace PostScript

#endif // PS_STACK_H 