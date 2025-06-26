#ifndef PS_OPERATORS_H
#define PS_OPERATORS_H

#include "ps_stack.h"
#include <functional>
#include <unordered_map>
#include <string>

namespace PostScript {

/**
 * @brief Forward declaration of the interpreter class
 */
class PSInterpreter;

/**
 * @brief Type definition for operator functions
 * 
 * Each operator is a function that takes a reference to the interpreter
 * and performs the operation using the interpreter's stack and context.
 */
using OperatorFunction = std::function<void(PSInterpreter&)>;

/**
 * @brief Operator registry that maps operator names to their implementations
 */
class OperatorRegistry {
public:
    OperatorRegistry();
    ~OperatorRegistry() = default;

    /**
     * @brief Register an operator function
     * @param name The name of the operator (e.g., "add", "sub", "show")
     * @param func The function that implements the operator
     */
    void registerOperator(const std::string& name, OperatorFunction func);

    /**
     * @brief Get an operator function by name
     * @param name The name of the operator
     * @return The operator function, or nullptr if not found
     */
    OperatorFunction getOperator(const std::string& name) const;

    /**
     * @brief Check if an operator exists
     * @param name The name of the operator
     * @return true if the operator exists, false otherwise
     */
    bool hasOperator(const std::string& name) const;

private:
    std::unordered_map<std::string, OperatorFunction> operators_;
};

// Operator function declarations
namespace Operators {
    // Basic arithmetic
    void add(PSInterpreter& interpreter);
    void sub(PSInterpreter& interpreter);
    void mul(PSInterpreter& interpreter);
    void div(PSInterpreter& interpreter);
    
    // Stack manipulation
    void dup(PSInterpreter& interpreter);
    void pop(PSInterpreter& interpreter);
    void exch(PSInterpreter& interpreter);
    void clear(PSInterpreter& interpreter);
    
    // Output and debugging
    void show(PSInterpreter& interpreter);
    void stack(PSInterpreter& interpreter);
    
    // Comparison operators
    void eq(PSInterpreter& interpreter);
    void ne(PSInterpreter& interpreter);
    void lt(PSInterpreter& interpreter);
    void le(PSInterpreter& interpreter);
    void gt(PSInterpreter& interpreter);
    void ge(PSInterpreter& interpreter);
    
    // Array operations
    void array(PSInterpreter& interpreter);
    void get(PSInterpreter& interpreter);
    void put(PSInterpreter& interpreter);
    void length(PSInterpreter& interpreter);
    void aload(PSInterpreter& interpreter);
    void astore(PSInterpreter& interpreter);
    
    // Dictionary operations
    void dict(PSInterpreter& interpreter);
    void def(PSInterpreter& interpreter);
    void load(PSInterpreter& interpreter);
    void store(PSInterpreter& interpreter);
    void known(PSInterpreter& interpreter);
    void keys(PSInterpreter& interpreter);
    
    // Procedure operations
    void exec(PSInterpreter& interpreter);
    void forall(PSInterpreter& interpreter);
    
    // Control flow
    void if_(PSInterpreter& interpreter);
    void ifelse(PSInterpreter& interpreter);
    void repeat(PSInterpreter& interpreter);
    void for_(PSInterpreter& interpreter);
    
    // Graphics operators
    void moveto(PSInterpreter& interpreter);
    void lineto(PSInterpreter& interpreter);
    void closepath(PSInterpreter& interpreter);
    void stroke(PSInterpreter& interpreter);
    void fill(PSInterpreter& interpreter);
    void newpath(PSInterpreter& interpreter);
    void gsave(PSInterpreter& interpreter);
    void grestore(PSInterpreter& interpreter);
    void translate(PSInterpreter& interpreter);
    void scale(PSInterpreter& interpreter);
    void rotate(PSInterpreter& interpreter);
    void setrgbcolor(PSInterpreter& interpreter);
    void setlinewidth(PSInterpreter& interpreter);
    void showpage(PSInterpreter& interpreter);
}

} // namespace PostScript

#endif // PS_OPERATORS_H 