#ifndef PS_INTERPRETER_H
#define PS_INTERPRETER_H

#include "ps_stack.h"
#include "ps_operators.h"
#include "ps_parser.h"
#include "ps_types.h"
#include <string>
#include <memory>
#include <vector>

namespace PostScript {

/**
 * @brief Main PostScript interpreter class
 * 
 * This class orchestrates the execution of PostScript programs.
 * It manages the operand stack, operator registry, and execution context.
 */
class PSInterpreter {
public:
    PSInterpreter();
    ~PSInterpreter() = default;

    /**
     * @brief Execute a PostScript program from a string
     * @param program The PostScript program as a string
     */
    void execute(const std::string& program);

    /**
     * @brief Execute a PostScript program from a file
     * @param filename The path to the PostScript file
     */
    void executeFile(const std::string& filename);

    /**
     * @brief Get access to the operand stack
     * @return Reference to the operand stack
     */
    PSStack& getStack() { return stack_; }

    /**
     * @brief Get access to the operand stack (const version)
     * @return Const reference to the operand stack
     */
    const PSStack& getStack() const { return stack_; }

    /**
     * @brief Get access to the operator registry
     * @return Reference to the operator registry
     */
    OperatorRegistry& getOperators() { return operators_; }

    /**
     * @brief Print the current state of the interpreter
     */
    void printState() const;

    /**
     * @brief Execute a single token
     * @param token The token to execute
     */
    void executeToken(const std::string& token);

    /**
     * @brief Get the current dictionary (top of dictionary stack)
     * @return Current dictionary or nullptr if stack is empty
     */
    PSObjectPtr getCurrentDictionary() const;

    /**
     * @brief Push a dictionary onto the dictionary stack
     * @param dict The dictionary to push
     */
    void pushDictionary(const PSObjectPtr& dict);

    /**
     * @brief Pop a dictionary from the dictionary stack
     * @return The popped dictionary
     */
    PSObjectPtr popDictionary();

    /**
     * @brief Get the current graphics state
     * @return Reference to the current graphics state
     */
    GraphicsState& getCurrentGraphicsState() { return graphicsStack_.back(); }

    /**
     * @brief Get the current graphics state (const version)
     * @return Const reference to the current graphics state
     */
    const GraphicsState& getCurrentGraphicsState() const { return graphicsStack_.back(); }

    // Graphics state stack
    std::vector<GraphicsState> graphicsStack_;

private:
    PSStack stack_;
    OperatorRegistry operators_;
    std::unique_ptr<PSParser> parser_;
    std::vector<PSObjectPtr> dictionaryStack_; // Dictionary stack for PostScript
};

} // namespace PostScript

#endif // PS_INTERPRETER_H 