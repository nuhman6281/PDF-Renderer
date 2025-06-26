#include "ps_operators.h"
#include "ps_interpreter.h"
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <cmath>

namespace PostScript {

OperatorRegistry::OperatorRegistry() {
    // Register basic arithmetic operators
    registerOperator("add", Operators::add);
    registerOperator("sub", Operators::sub);
    registerOperator("mul", Operators::mul);
    registerOperator("div", Operators::div);
    
    // Register stack operators
    registerOperator("dup", Operators::dup);
    registerOperator("pop", Operators::pop);
    registerOperator("exch", Operators::exch);
    registerOperator("clear", Operators::clear);
    registerOperator("stack", Operators::stack);
    
    // Register output operators
    registerOperator("show", Operators::show);
    
    // Register comparison operators
    registerOperator("eq", Operators::eq);
    registerOperator("ne", Operators::ne);
    registerOperator("lt", Operators::lt);
    registerOperator("le", Operators::le);
    registerOperator("gt", Operators::gt);
    registerOperator("ge", Operators::ge);
    
    // Register array operators
    registerOperator("array", Operators::array);
    registerOperator("get", Operators::get);
    registerOperator("put", Operators::put);
    registerOperator("length", Operators::length);
    registerOperator("aload", Operators::aload);
    registerOperator("astore", Operators::astore);
    
    // Register dictionary operators
    registerOperator("dict", Operators::dict);
    registerOperator("def", Operators::def);
    registerOperator("load", Operators::load);
    registerOperator("store", Operators::store);
    registerOperator("known", Operators::known);
    registerOperator("keys", Operators::keys);
    
    // Register control flow operators
    registerOperator("exec", Operators::exec);
    registerOperator("forall", Operators::forall);
    registerOperator("if", Operators::if_);
    registerOperator("ifelse", Operators::ifelse);
    registerOperator("repeat", Operators::repeat);
    registerOperator("for", Operators::for_);
    
    // Register graphics operators
    registerOperator("moveto", Operators::moveto);
    registerOperator("lineto", Operators::lineto);
    registerOperator("closepath", Operators::closepath);
    registerOperator("stroke", Operators::stroke);
    registerOperator("fill", Operators::fill);
    registerOperator("newpath", Operators::newpath);
    registerOperator("gsave", Operators::gsave);
    registerOperator("grestore", Operators::grestore);
    registerOperator("translate", Operators::translate);
    registerOperator("scale", Operators::scale);
    registerOperator("rotate", Operators::rotate);
    registerOperator("setrgbcolor", Operators::setrgbcolor);
    registerOperator("setlinewidth", Operators::setlinewidth);
    registerOperator("showpage", Operators::showpage);
}

void OperatorRegistry::registerOperator(const std::string& name, OperatorFunction func) {
    operators_[name] = func;
}

OperatorFunction OperatorRegistry::getOperator(const std::string& name) const {
    auto it = operators_.find(name);
    if (it != operators_.end()) {
        return it->second;
    }
    return nullptr;
}

bool OperatorRegistry::hasOperator(const std::string& name) const {
    return operators_.find(name) != operators_.end();
}

// Operator implementations
namespace Operators {

void add(PSInterpreter& interpreter) {
    PSStack& stack = interpreter.getStack();
    
    if (stack.size() < 2) {
        throw std::runtime_error("Stack underflow: need 2 operands for add");
    }
    
    PSObjectPtr b = stack.pop();
    PSObjectPtr a = stack.pop();
    
    // Handle different type combinations
    if (a->getType() == ObjectType::INTEGER && b->getType() == ObjectType::INTEGER) {
        int result = a->asInteger() + b->asInteger();
        stack.push(std::make_shared<PSObject>(result));
    } else if (a->getType() == ObjectType::REAL || b->getType() == ObjectType::REAL) {
        double aVal = (a->getType() == ObjectType::INTEGER) ? a->asInteger() : a->asReal();
        double bVal = (b->getType() == ObjectType::INTEGER) ? b->asInteger() : b->asReal();
        double result = aVal + bVal;
        stack.push(std::make_shared<PSObject>(result));
    } else {
        throw std::runtime_error("Invalid operands for add operation");
    }
}

void sub(PSInterpreter& interpreter) {
    PSStack& stack = interpreter.getStack();
    
    if (stack.size() < 2) {
        throw std::runtime_error("Stack underflow: need 2 operands for sub");
    }
    
    PSObjectPtr b = stack.pop();
    PSObjectPtr a = stack.pop();
    
    // Handle different type combinations
    if (a->getType() == ObjectType::INTEGER && b->getType() == ObjectType::INTEGER) {
        int result = a->asInteger() - b->asInteger();
        stack.push(std::make_shared<PSObject>(result));
    } else if (a->getType() == ObjectType::REAL || b->getType() == ObjectType::REAL) {
        double aVal = (a->getType() == ObjectType::INTEGER) ? a->asInteger() : a->asReal();
        double bVal = (b->getType() == ObjectType::INTEGER) ? b->asInteger() : b->asReal();
        double result = aVal - bVal;
        stack.push(std::make_shared<PSObject>(result));
    } else {
        throw std::runtime_error("Invalid operands for sub operation");
    }
}

void mul(PSInterpreter& interpreter) {
    PSStack& stack = interpreter.getStack();
    
    if (stack.size() < 2) {
        throw std::runtime_error("Stack underflow: need 2 operands for mul");
    }
    
    PSObjectPtr b = stack.pop();
    PSObjectPtr a = stack.pop();
    
    // Handle different type combinations
    if (a->getType() == ObjectType::INTEGER && b->getType() == ObjectType::INTEGER) {
        int result = a->asInteger() * b->asInteger();
        stack.push(std::make_shared<PSObject>(result));
    } else if (a->getType() == ObjectType::REAL || b->getType() == ObjectType::REAL) {
        double aVal = (a->getType() == ObjectType::INTEGER) ? a->asInteger() : a->asReal();
        double bVal = (b->getType() == ObjectType::INTEGER) ? b->asInteger() : b->asReal();
        double result = aVal * bVal;
        stack.push(std::make_shared<PSObject>(result));
    } else {
        throw std::runtime_error("Invalid operands for mul operation");
    }
}

void div(PSInterpreter& interpreter) {
    PSStack& stack = interpreter.getStack();
    
    if (stack.size() < 2) {
        throw std::runtime_error("Stack underflow: need 2 operands for div");
    }
    
    PSObjectPtr b = stack.pop();
    PSObjectPtr a = stack.pop();
    
    // Check for division by zero
    if ((b->getType() == ObjectType::INTEGER && b->asInteger() == 0) ||
        (b->getType() == ObjectType::REAL && b->asReal() == 0.0)) {
        throw std::runtime_error("Division by zero");
    }
    
    // Handle different type combinations
    if (a->getType() == ObjectType::INTEGER && b->getType() == ObjectType::INTEGER) {
        // Integer division
        int result = a->asInteger() / b->asInteger();
        stack.push(std::make_shared<PSObject>(result));
    } else {
        // Real division
        double aVal = (a->getType() == ObjectType::INTEGER) ? a->asInteger() : a->asReal();
        double bVal = (b->getType() == ObjectType::INTEGER) ? b->asInteger() : b->asReal();
        double result = aVal / bVal;
        stack.push(std::make_shared<PSObject>(result));
    }
}

void dup(PSInterpreter& interpreter) {
    PSStack& stack = interpreter.getStack();
    stack.dup();
}

void pop(PSInterpreter& interpreter) {
    PSStack& stack = interpreter.getStack();
    stack.pop(); // Discard the result
}

void show(PSInterpreter& interpreter) {
    PSStack& stack = interpreter.getStack();
    
    if (stack.empty()) {
        throw std::runtime_error("Stack underflow: need 1 operand for show");
    }
    
    PSObjectPtr obj = stack.pop();
    
    if (obj->getType() == ObjectType::STRING) {
        // Remove the parentheses from the string literal
        std::string str = obj->asString();
        if (str.length() >= 2 && str[0] == '(' && str[str.length()-1] == ')') {
            str = str.substr(1, str.length() - 2);
        }
        std::cout << str;
    } else {
        std::cout << obj->toString();
    }
}

void exch(PSInterpreter& interpreter) {
    PSStack& stack = interpreter.getStack();
    stack.exch();
}

void clear(PSInterpreter& interpreter) {
    PSStack& stack = interpreter.getStack();
    stack.clear();
}

// Output and debugging operators
void stack(PSInterpreter& interpreter) {
    PSStack& stack = interpreter.getStack();
    stack.print();
}

// Comparison operators
void eq(PSInterpreter& interpreter) {
    PSStack& stack = interpreter.getStack();
    
    if (stack.size() < 2) {
        throw std::runtime_error("Stack underflow: need 2 operands for eq");
    }
    
    PSObjectPtr b = stack.pop();
    PSObjectPtr a = stack.pop();
    
    bool result = false;
    if (a->getType() == b->getType()) {
        switch (a->getType()) {
            case ObjectType::INTEGER:
                result = a->asInteger() == b->asInteger();
                break;
            case ObjectType::REAL:
                result = a->asReal() == b->asReal();
                break;
            case ObjectType::STRING:
                result = a->asString() == b->asString();
                break;
            case ObjectType::BOOLEAN:
                result = a->asBoolean() == b->asBoolean();
                break;
            default:
                result = false;
        }
    }
    
    stack.push(std::make_shared<PSObject>(result));
}

void ne(PSInterpreter& interpreter) {
    eq(interpreter);
    PSStack& stack = interpreter.getStack();
    PSObjectPtr result = stack.pop();
    stack.push(std::make_shared<PSObject>(!result->asBoolean()));
}

void lt(PSInterpreter& interpreter) {
    PSStack& stack = interpreter.getStack();
    
    if (stack.size() < 2) {
        throw std::runtime_error("Stack underflow: need 2 operands for lt");
    }
    
    PSObjectPtr b = stack.pop();
    PSObjectPtr a = stack.pop();
    
    bool result = false;
    if (a->getType() == ObjectType::INTEGER && b->getType() == ObjectType::INTEGER) {
        result = a->asInteger() < b->asInteger();
    } else if (a->getType() == ObjectType::REAL || b->getType() == ObjectType::REAL) {
        double aVal = (a->getType() == ObjectType::INTEGER) ? a->asInteger() : a->asReal();
        double bVal = (b->getType() == ObjectType::INTEGER) ? b->asInteger() : b->asReal();
        result = aVal < bVal;
    } else {
        throw std::runtime_error("Invalid operands for lt operation");
    }
    
    stack.push(std::make_shared<PSObject>(result));
}

void le(PSInterpreter& interpreter) {
    PSStack& stack = interpreter.getStack();
    
    if (stack.size() < 2) {
        throw std::runtime_error("Stack underflow: need 2 operands for le");
    }
    
    PSObjectPtr b = stack.pop();
    PSObjectPtr a = stack.pop();
    
    bool result = false;
    if (a->getType() == ObjectType::INTEGER && b->getType() == ObjectType::INTEGER) {
        result = a->asInteger() <= b->asInteger();
    } else if (a->getType() == ObjectType::REAL || b->getType() == ObjectType::REAL) {
        double aVal = (a->getType() == ObjectType::INTEGER) ? a->asInteger() : a->asReal();
        double bVal = (b->getType() == ObjectType::INTEGER) ? b->asInteger() : b->asReal();
        result = aVal <= bVal;
    } else {
        throw std::runtime_error("Invalid operands for le operation");
    }
    
    stack.push(std::make_shared<PSObject>(result));
}

void gt(PSInterpreter& interpreter) {
    PSStack& stack = interpreter.getStack();
    
    if (stack.size() < 2) {
        throw std::runtime_error("Stack underflow: need 2 operands for gt");
    }
    
    PSObjectPtr b = stack.pop();
    PSObjectPtr a = stack.pop();
    
    bool result = false;
    if (a->getType() == ObjectType::INTEGER && b->getType() == ObjectType::INTEGER) {
        result = a->asInteger() > b->asInteger();
    } else if (a->getType() == ObjectType::REAL || b->getType() == ObjectType::REAL) {
        double aVal = (a->getType() == ObjectType::INTEGER) ? a->asInteger() : a->asReal();
        double bVal = (b->getType() == ObjectType::INTEGER) ? b->asInteger() : b->asReal();
        result = aVal > bVal;
    } else {
        throw std::runtime_error("Invalid operands for gt operation");
    }
    
    stack.push(std::make_shared<PSObject>(result));
}

void ge(PSInterpreter& interpreter) {
    PSStack& stack = interpreter.getStack();
    
    if (stack.size() < 2) {
        throw std::runtime_error("Stack underflow: need 2 operands for ge");
    }
    
    PSObjectPtr b = stack.pop();
    PSObjectPtr a = stack.pop();
    
    bool result = false;
    if (a->getType() == ObjectType::INTEGER && b->getType() == ObjectType::INTEGER) {
        result = a->asInteger() >= b->asInteger();
    } else if (a->getType() == ObjectType::REAL || b->getType() == ObjectType::REAL) {
        double aVal = (a->getType() == ObjectType::INTEGER) ? a->asInteger() : a->asReal();
        double bVal = (b->getType() == ObjectType::INTEGER) ? b->asInteger() : b->asReal();
        result = aVal >= bVal;
    } else {
        throw std::runtime_error("Invalid operands for ge operation");
    }
    
    stack.push(std::make_shared<PSObject>(result));
}

// Array operations
void array(PSInterpreter& interpreter) {
    PSStack& stack = interpreter.getStack();
    
    if (stack.empty()) {
        throw std::runtime_error("Stack underflow: need size for array");
    }
    
    PSObjectPtr sizeObj = stack.pop();
    if (sizeObj->getType() != ObjectType::INTEGER) {
        throw std::runtime_error("Array size must be an integer");
    }
    
    int size = sizeObj->asInteger();
    if (size < 0) {
        throw std::runtime_error("Array size cannot be negative");
    }
    
    PSArray array;
    array.resize(size);
    for (int i = 0; i < size; ++i) {
        array[i] = std::make_shared<PSObject>(); // null objects
    }
    
    stack.push(std::make_shared<PSObject>(array));
}

void get(PSInterpreter& interpreter) {
    PSStack& stack = interpreter.getStack();
    
    if (stack.size() < 2) {
        throw std::runtime_error("Stack underflow: need array and index for get");
    }
    
    PSObjectPtr indexObj = stack.pop();
    PSObjectPtr arrayObj = stack.pop();
    
    if (arrayObj->getType() != ObjectType::ARRAY) {
        throw std::runtime_error("First operand must be an array");
    }
    
    if (indexObj->getType() != ObjectType::INTEGER) {
        throw std::runtime_error("Index must be an integer");
    }
    
    int index = indexObj->asInteger();
    if (index < 0 || static_cast<size_t>(index) >= arrayObj->arrayLength()) {
        throw std::runtime_error("Array index out of bounds");
    }
    
    stack.push(arrayObj->arrayGet(index));
}

void put(PSInterpreter& interpreter) {
    PSStack& stack = interpreter.getStack();
    
    if (stack.size() < 3) {
        throw std::runtime_error("Stack underflow: need array, index, and value for put");
    }
    
    PSObjectPtr value = stack.pop();
    PSObjectPtr indexObj = stack.pop();
    PSObjectPtr arrayObj = stack.pop();
    
    if (arrayObj->getType() != ObjectType::ARRAY) {
        throw std::runtime_error("First operand must be an array");
    }
    
    if (indexObj->getType() != ObjectType::INTEGER) {
        throw std::runtime_error("Index must be an integer");
    }
    
    int index = indexObj->asInteger();
    if (index < 0 || static_cast<size_t>(index) >= arrayObj->arrayLength()) {
        throw std::runtime_error("Array index out of bounds");
    }
    
    // Debug output
    std::cout << "DEBUG: Setting array[" << index << "] = " << value->toString() << std::endl;
    
    arrayObj->arraySet(index, value);
    
    // Debug output after setting
    std::cout << "DEBUG: Array after set: " << arrayObj->toString() << std::endl;
    
    // Push the array back onto the stack to preserve reference
    stack.push(arrayObj);
}

void length(PSInterpreter& interpreter) {
    PSStack& stack = interpreter.getStack();
    
    if (stack.empty()) {
        throw std::runtime_error("Stack underflow: need operand for length");
    }
    
    PSObjectPtr obj = stack.pop();
    
    if (obj->getType() == ObjectType::ARRAY) {
        stack.push(std::make_shared<PSObject>(static_cast<int>(obj->arrayLength())));
    } else if (obj->getType() == ObjectType::STRING) {
        std::string str = obj->asString();
        // Remove parentheses for string length
        if (str.length() >= 2 && str[0] == '(' && str[str.length()-1] == ')') {
            str = str.substr(1, str.length() - 2);
        }
        stack.push(std::make_shared<PSObject>(static_cast<int>(str.length())));
    } else {
        throw std::runtime_error("Length operation not supported for this type");
    }
}

void aload(PSInterpreter& interpreter) {
    PSStack& stack = interpreter.getStack();
    
    if (stack.empty()) {
        throw std::runtime_error("Stack underflow: need array for aload");
    }
    
    PSObjectPtr arrayObj = stack.pop();
    
    if (arrayObj->getType() != ObjectType::ARRAY) {
        throw std::runtime_error("Operand must be an array");
    }
    
    PSArray array = arrayObj->asArray();
    
    // Push all array elements onto the stack
    for (const auto& element : array) {
        stack.push(element);
    }
    
    // Push the array back
    stack.push(arrayObj);
}

void astore(PSInterpreter& interpreter) {
    PSStack& stack = interpreter.getStack();
    
    if (stack.empty()) {
        throw std::runtime_error("Stack underflow: need array for astore");
    }
    
    PSObjectPtr arrayObj = stack.pop();
    
    if (arrayObj->getType() != ObjectType::ARRAY) {
        throw std::runtime_error("Operand must be an array");
    }
    
    size_t arraySize = arrayObj->arrayLength();
    
    if (stack.size() < arraySize) {
        throw std::runtime_error("Stack underflow: not enough elements for astore");
    }
    
    // Pop elements from stack and store in array (in reverse order)
    for (size_t i = arraySize; i > 0; --i) {
        PSObjectPtr element = stack.pop();
        arrayObj->arraySet(i - 1, element);
    }
    
    stack.push(arrayObj);
}

// Dictionary operations
void dict(PSInterpreter& interpreter) {
    PSStack& stack = interpreter.getStack();
    
    if (stack.empty()) {
        throw std::runtime_error("Stack underflow: need size for dict");
    }
    
    PSObjectPtr sizeObj = stack.pop();
    if (sizeObj->getType() != ObjectType::INTEGER) {
        throw std::runtime_error("Dictionary size must be an integer");
    }
    
    int size = sizeObj->asInteger();
    if (size < 0) {
        throw std::runtime_error("Dictionary size cannot be negative");
    }
    
    PSDictionary dict;
    stack.push(std::make_shared<PSObject>(dict));
}

void def(PSInterpreter& interpreter) {
    PSStack& stack = interpreter.getStack();
    
    if (stack.size() < 2) {
        throw std::runtime_error("Stack underflow: need key and value for def");
    }
    
    PSObjectPtr value = stack.pop();
    PSObjectPtr keyObj = stack.pop();
    
    if (keyObj->getType() != ObjectType::STRING) {
        throw std::runtime_error("Key must be a string");
    }
    
    std::string key = keyObj->asString();
    // Remove leading slash if present
    if (key.length() > 0 && key[0] == '/') {
        key = key.substr(1);
    }
    
    // Get the current dictionary from the dictionary stack
    PSObjectPtr currentDict = interpreter.getCurrentDictionary();
    if (!currentDict || currentDict->getType() != ObjectType::DICTIONARY) {
        throw std::runtime_error("No current dictionary available");
    }
    
    currentDict->dictPut(key, value);
}

void load(PSInterpreter& interpreter) {
    PSStack& stack = interpreter.getStack();
    
    if (stack.empty()) {
        throw std::runtime_error("Stack underflow: need key for load");
    }
    
    PSObjectPtr keyObj = stack.pop();
    
    if (keyObj->getType() != ObjectType::STRING) {
        throw std::runtime_error("Key must be a string");
    }
    
    std::string key = keyObj->asString();
    // Remove leading slash if present
    if (key.length() > 0 && key[0] == '/') {
        key = key.substr(1);
    }
    
    // Look up the key in the current dictionary
    PSObjectPtr currentDict = interpreter.getCurrentDictionary();
    if (!currentDict || currentDict->getType() != ObjectType::DICTIONARY) {
        throw std::runtime_error("No current dictionary available");
    }
    
    if (!currentDict->dictHas(key)) {
        throw std::runtime_error("Key not found in dictionary: " + key);
    }
    
    stack.push(currentDict->dictGet(key));
}

void store(PSInterpreter& interpreter) {
    PSStack& stack = interpreter.getStack();
    
    if (stack.size() < 3) {
        throw std::runtime_error("Stack underflow: need dictionary, key, and value for store");
    }
    
    PSObjectPtr value = stack.pop();
    PSObjectPtr keyObj = stack.pop();
    PSObjectPtr dictObj = stack.pop();
    
    if (dictObj->getType() != ObjectType::DICTIONARY) {
        throw std::runtime_error("First operand must be a dictionary");
    }
    
    if (keyObj->getType() != ObjectType::STRING) {
        throw std::runtime_error("Key must be a string");
    }
    
    std::string key = keyObj->asString();
    // Remove leading slash if present
    if (key.length() > 0 && key[0] == '/') {
        key = key.substr(1);
    }
    
    dictObj->dictPut(key, value);
}

void known(PSInterpreter& interpreter) {
    PSStack& stack = interpreter.getStack();
    
    if (stack.size() < 2) {
        throw std::runtime_error("Stack underflow: need dictionary and key for known");
    }
    
    PSObjectPtr keyObj = stack.pop();
    PSObjectPtr dictObj = stack.pop();
    
    if (dictObj->getType() != ObjectType::DICTIONARY) {
        throw std::runtime_error("First operand must be a dictionary");
    }
    
    if (keyObj->getType() != ObjectType::STRING) {
        throw std::runtime_error("Key must be a string");
    }
    
    std::string key = keyObj->asString();
    // Remove leading slash if present
    if (key.length() > 0 && key[0] == '/') {
        key = key.substr(1);
    }
    
    bool hasKey = dictObj->dictHas(key);
    stack.push(std::make_shared<PSObject>(hasKey));
}

void keys(PSInterpreter& interpreter) {
    PSStack& stack = interpreter.getStack();
    
    if (stack.empty()) {
        throw std::runtime_error("Stack underflow: need dictionary for keys");
    }
    
    PSObjectPtr dictObj = stack.pop();
    
    if (dictObj->getType() != ObjectType::DICTIONARY) {
        throw std::runtime_error("Operand must be a dictionary");
    }
    
    std::vector<std::string> keyList = dictObj->dictKeys();
    
    // Create an array of key strings
    PSArray keyArray;
    for (const auto& key : keyList) {
        keyArray.push_back(std::make_shared<PSObject>("/" + key));
    }
    
    stack.push(std::make_shared<PSObject>(keyArray));
}

// Procedure operations
void exec(PSInterpreter& interpreter) {
    PSStack& stack = interpreter.getStack();
    
    if (stack.empty()) {
        throw std::runtime_error("Stack underflow: need procedure for exec");
    }
    
    PSObjectPtr procObj = stack.pop();
    
    if (procObj->getType() != ObjectType::PROCEDURE) {
        throw std::runtime_error("Operand must be a procedure");
    }
    
    // Execute the procedure
    PSProcedure proc = procObj->asProcedure();
    for (const auto& token : proc) {
        interpreter.executeToken(token);
    }
}

void forall(PSInterpreter& interpreter) {
    PSStack& stack = interpreter.getStack();
    
    if (stack.size() < 2) {
        throw std::runtime_error("Stack underflow: need array and procedure for forall");
    }
    
    PSObjectPtr procObj = stack.pop();
    PSObjectPtr arrayObj = stack.pop();
    
    if (arrayObj->getType() != ObjectType::ARRAY) {
        throw std::runtime_error("First operand must be an array");
    }
    
    if (procObj->getType() != ObjectType::PROCEDURE) {
        throw std::runtime_error("Second operand must be a procedure");
    }
    
    PSArray array = arrayObj->asArray();
    PSProcedure proc = procObj->asProcedure();
    
    // Execute the procedure for each array element
    for (const auto& element : array) {
        stack.push(element);
        for (const auto& token : proc) {
            interpreter.executeToken(token);
        }
    }
}

// Control flow operations
void if_(PSInterpreter& interpreter) {
    PSStack& stack = interpreter.getStack();
    
    if (stack.size() < 2) {
        throw std::runtime_error("Stack underflow: need condition and procedure for if");
    }
    
    PSObjectPtr procObj = stack.pop();
    PSObjectPtr conditionObj = stack.pop();
    
    if (procObj->getType() != ObjectType::PROCEDURE) {
        throw std::runtime_error("Second operand must be a procedure");
    }
    
    bool condition = false;
    if (conditionObj->getType() == ObjectType::BOOLEAN) {
        condition = conditionObj->asBoolean();
    } else if (conditionObj->getType() == ObjectType::INTEGER) {
        condition = conditionObj->asInteger() != 0;
    } else if (conditionObj->getType() == ObjectType::REAL) {
        condition = conditionObj->asReal() != 0.0;
    }
    
    if (condition) {
        PSProcedure proc = procObj->asProcedure();
        for (const auto& token : proc) {
            interpreter.executeToken(token);
        }
    }
}

void ifelse(PSInterpreter& interpreter) {
    PSStack& stack = interpreter.getStack();
    
    if (stack.size() < 3) {
        throw std::runtime_error("Stack underflow: need condition and two procedures for ifelse");
    }
    
    PSObjectPtr elseProcObj = stack.pop();
    PSObjectPtr thenProcObj = stack.pop();
    PSObjectPtr conditionObj = stack.pop();
    
    if (thenProcObj->getType() != ObjectType::PROCEDURE) {
        throw std::runtime_error("Second operand must be a procedure");
    }
    
    if (elseProcObj->getType() != ObjectType::PROCEDURE) {
        throw std::runtime_error("Third operand must be a procedure");
    }
    
    bool condition = false;
    if (conditionObj->getType() == ObjectType::BOOLEAN) {
        condition = conditionObj->asBoolean();
    } else if (conditionObj->getType() == ObjectType::INTEGER) {
        condition = conditionObj->asInteger() != 0;
    } else if (conditionObj->getType() == ObjectType::REAL) {
        condition = conditionObj->asReal() != 0.0;
    }
    
    PSProcedure proc = condition ? thenProcObj->asProcedure() : elseProcObj->asProcedure();
    for (const auto& token : proc) {
        interpreter.executeToken(token);
    }
}

void repeat(PSInterpreter& interpreter) {
    PSStack& stack = interpreter.getStack();
    
    if (stack.size() < 2) {
        throw std::runtime_error("Stack underflow: need count and procedure for repeat");
    }
    
    PSObjectPtr procObj = stack.pop();
    PSObjectPtr countObj = stack.pop();
    
    if (procObj->getType() != ObjectType::PROCEDURE) {
        throw std::runtime_error("Second operand must be a procedure");
    }
    
    if (countObj->getType() != ObjectType::INTEGER) {
        throw std::runtime_error("Count must be an integer");
    }
    
    int count = countObj->asInteger();
    if (count < 0) {
        throw std::runtime_error("Repeat count cannot be negative");
    }
    
    PSProcedure proc = procObj->asProcedure();
    for (int i = 0; i < count; ++i) {
        for (const auto& token : proc) {
            interpreter.executeToken(token);
        }
    }
}

void for_(PSInterpreter& interpreter) {
    PSStack& stack = interpreter.getStack();
    
    if (stack.size() < 4) {
        throw std::runtime_error("Stack underflow: need start, end, increment, and procedure for for");
    }
    
    PSObjectPtr procObj = stack.pop();
    PSObjectPtr incrementObj = stack.pop();
    PSObjectPtr endObj = stack.pop();
    PSObjectPtr startObj = stack.pop();
    
    if (procObj->getType() != ObjectType::PROCEDURE) {
        throw std::runtime_error("Fourth operand must be a procedure");
    }
    
    if (startObj->getType() != ObjectType::INTEGER || 
        endObj->getType() != ObjectType::INTEGER || 
        incrementObj->getType() != ObjectType::INTEGER) {
        throw std::runtime_error("Start, end, and increment must be integers");
    }
    
    int start = startObj->asInteger();
    int end = endObj->asInteger();
    int increment = incrementObj->asInteger();
    
    if (increment == 0) {
        throw std::runtime_error("Increment cannot be zero");
    }
    
    PSProcedure proc = procObj->asProcedure();
    
    if (increment > 0) {
        for (int i = start; i <= end; i += increment) {
            stack.push(std::make_shared<PSObject>(i));
            for (const auto& token : proc) {
                interpreter.executeToken(token);
            }
        }
    } else {
        for (int i = start; i >= end; i += increment) {
            stack.push(std::make_shared<PSObject>(i));
            for (const auto& token : proc) {
                interpreter.executeToken(token);
            }
        }
    }
}

// Graphics operators
void moveto(PSInterpreter& interpreter) {
    PSStack& stack = interpreter.getStack();
    
    if (stack.size() < 2) {
        throw std::runtime_error("Stack underflow: need x and y for moveto");
    }
    
    PSObjectPtr yObj = stack.pop();
    PSObjectPtr xObj = stack.pop();
    
    if (xObj->getType() != ObjectType::REAL && xObj->getType() != ObjectType::INTEGER) {
        throw std::runtime_error("X coordinate must be a number");
    }
    if (yObj->getType() != ObjectType::REAL && yObj->getType() != ObjectType::INTEGER) {
        throw std::runtime_error("Y coordinate must be a number");
    }
    
    double x = (xObj->getType() == ObjectType::INTEGER) ? xObj->asInteger() : xObj->asReal();
    double y = (yObj->getType() == ObjectType::INTEGER) ? yObj->asInteger() : yObj->asReal();
    
    // Get current graphics state
    GraphicsState& gs = interpreter.getCurrentGraphicsState();
    gs.currentPoint = Point(x, y);
    
    // Add moveto segment to current path
    gs.currentPath.emplace_back(PathSegment::Type::MoveTo, std::vector<Point>{{x, y}});
}

void lineto(PSInterpreter& interpreter) {
    PSStack& stack = interpreter.getStack();
    
    if (stack.size() < 2) {
        throw std::runtime_error("Stack underflow: need x and y for lineto");
    }
    
    PSObjectPtr yObj = stack.pop();
    PSObjectPtr xObj = stack.pop();
    
    if (xObj->getType() != ObjectType::REAL && xObj->getType() != ObjectType::INTEGER) {
        throw std::runtime_error("X coordinate must be a number");
    }
    if (yObj->getType() != ObjectType::REAL && yObj->getType() != ObjectType::INTEGER) {
        throw std::runtime_error("Y coordinate must be a number");
    }
    
    double x = (xObj->getType() == ObjectType::INTEGER) ? xObj->asInteger() : xObj->asReal();
    double y = (yObj->getType() == ObjectType::INTEGER) ? yObj->asInteger() : yObj->asReal();
    
    // Get current graphics state
    GraphicsState& gs = interpreter.getCurrentGraphicsState();
    gs.currentPoint = Point(x, y);
    
    // Add lineto segment to current path
    gs.currentPath.emplace_back(PathSegment::Type::LineTo, std::vector<Point>{{x, y}});
}

void closepath(PSInterpreter& interpreter) {
    GraphicsState& gs = interpreter.getCurrentGraphicsState();
    gs.currentPath.emplace_back(PathSegment::Type::ClosePath, std::vector<Point>{});
}

void stroke(PSInterpreter& interpreter) {
    std::cout << "[DEBUG] stroke called" << std::endl;
    GraphicsState& gs = interpreter.getCurrentGraphicsState();
    
    std::cout << "STROKE: Path with " << gs.currentPath.size() << " segments" << std::endl;
    for (size_t i = 0; i < gs.currentPath.size(); ++i) {
        const auto& segment = gs.currentPath[i];
        switch (segment.type) {
            case PathSegment::Type::MoveTo:
                std::cout << "  " << i << ": moveto(" << segment.points[0].x << ", " << segment.points[0].y << ")" << std::endl;
                break;
            case PathSegment::Type::LineTo:
                std::cout << "  " << i << ": lineto(" << segment.points[0].x << ", " << segment.points[0].y << ")" << std::endl;
                break;
            case PathSegment::Type::ClosePath:
                std::cout << "  " << i << ": closepath" << std::endl;
                break;
            default:
                std::cout << "  " << i << ": unknown segment type" << std::endl;
        }
    }
    std::cout << "  Color: RGB(" << gs.r << ", " << gs.g << ", " << gs.b << ")" << std::endl;
    std::cout << "  Line width: " << gs.lineWidth << std::endl;
    
    // Clear the current path after stroking
    gs.currentPath.clear();
}

void fill(PSInterpreter& interpreter) {
    std::cout << "[DEBUG] fill called" << std::endl;
    GraphicsState& gs = interpreter.getCurrentGraphicsState();
    
    std::cout << "FILL: Path with " << gs.currentPath.size() << " segments" << std::endl;
    for (size_t i = 0; i < gs.currentPath.size(); ++i) {
        const auto& segment = gs.currentPath[i];
        switch (segment.type) {
            case PathSegment::Type::MoveTo:
                std::cout << "  " << i << ": moveto(" << segment.points[0].x << ", " << segment.points[0].y << ")" << std::endl;
                break;
            case PathSegment::Type::LineTo:
                std::cout << "  " << i << ": lineto(" << segment.points[0].x << ", " << segment.points[0].y << ")" << std::endl;
                break;
            case PathSegment::Type::ClosePath:
                std::cout << "  " << i << ": closepath" << std::endl;
                break;
            default:
                std::cout << "  " << i << ": unknown segment type" << std::endl;
        }
    }
    std::cout << "  Fill color: RGB(" << gs.r << ", " << gs.g << ", " << gs.b << ")" << std::endl;
    
    // Clear the current path after filling
    gs.currentPath.clear();
}

void newpath(PSInterpreter& interpreter) {
    GraphicsState& gs = interpreter.getCurrentGraphicsState();
    gs.currentPath.clear();
}

void gsave(PSInterpreter& interpreter) {
    // Push a copy of the current graphics state
    interpreter.graphicsStack_.push_back(interpreter.getCurrentGraphicsState());
}

void grestore(PSInterpreter& interpreter) {
    if (interpreter.graphicsStack_.size() <= 1) {
        throw std::runtime_error("Graphics state stack underflow");
    }
    interpreter.graphicsStack_.pop_back();
}

void translate(PSInterpreter& interpreter) {
    PSStack& stack = interpreter.getStack();
    
    if (stack.size() < 2) {
        throw std::runtime_error("Stack underflow: need tx and ty for translate");
    }
    
    PSObjectPtr tyObj = stack.pop();
    PSObjectPtr txObj = stack.pop();
    
    if (txObj->getType() != ObjectType::REAL && txObj->getType() != ObjectType::INTEGER) {
        throw std::runtime_error("TX must be a number");
    }
    if (tyObj->getType() != ObjectType::REAL && tyObj->getType() != ObjectType::INTEGER) {
        throw std::runtime_error("TY must be a number");
    }
    
    double tx = (txObj->getType() == ObjectType::INTEGER) ? txObj->asInteger() : txObj->asReal();
    double ty = (tyObj->getType() == ObjectType::INTEGER) ? tyObj->asInteger() : tyObj->asReal();
    
    // Get current transformation matrix
    GraphicsState& gs = interpreter.getCurrentGraphicsState();
    auto& ctm = gs.ctm;
    
    // Apply translation: [1 0 0 1 tx ty] * current_matrix
    double new_e = ctm[0] * tx + ctm[2] * ty + ctm[4];
    double new_f = ctm[1] * tx + ctm[3] * ty + ctm[5];
    ctm[4] = new_e;
    ctm[5] = new_f;
}

void scale(PSInterpreter& interpreter) {
    PSStack& stack = interpreter.getStack();
    
    if (stack.size() < 2) {
        throw std::runtime_error("Stack underflow: need sx and sy for scale");
    }
    
    PSObjectPtr syObj = stack.pop();
    PSObjectPtr sxObj = stack.pop();
    
    if (sxObj->getType() != ObjectType::REAL && sxObj->getType() != ObjectType::INTEGER) {
        throw std::runtime_error("SX must be a number");
    }
    if (syObj->getType() != ObjectType::REAL && syObj->getType() != ObjectType::INTEGER) {
        throw std::runtime_error("SY must be a number");
    }
    
    double sx = (sxObj->getType() == ObjectType::INTEGER) ? sxObj->asInteger() : sxObj->asReal();
    double sy = (syObj->getType() == ObjectType::INTEGER) ? syObj->asInteger() : syObj->asReal();
    
    // Get current transformation matrix
    GraphicsState& gs = interpreter.getCurrentGraphicsState();
    auto& ctm = gs.ctm;
    
    // Apply scaling: [sx 0 0 sy 0 0] * current_matrix
    double new_a = sx * ctm[0];
    double new_b = sx * ctm[1];
    double new_c = sy * ctm[2];
    double new_d = sy * ctm[3];
    ctm[0] = new_a;
    ctm[1] = new_b;
    ctm[2] = new_c;
    ctm[3] = new_d;
}

void rotate(PSInterpreter& interpreter) {
    PSStack& stack = interpreter.getStack();
    
    if (stack.empty()) {
        throw std::runtime_error("Stack underflow: need angle for rotate");
    }
    
    PSObjectPtr angleObj = stack.pop();
    
    if (angleObj->getType() != ObjectType::REAL && angleObj->getType() != ObjectType::INTEGER) {
        throw std::runtime_error("Angle must be a number");
    }
    
    double angle = (angleObj->getType() == ObjectType::INTEGER) ? angleObj->asInteger() : angleObj->asReal();
    
    // Convert degrees to radians
    double radians = angle * M_PI / 180.0;
    double cos_a = cos(radians);
    double sin_a = sin(radians);
    
    // Get current transformation matrix
    GraphicsState& gs = interpreter.getCurrentGraphicsState();
    auto& ctm = gs.ctm;
    
    // Apply rotation: [cos sin -sin cos 0 0] * current_matrix
    double new_a = cos_a * ctm[0] - sin_a * ctm[2];
    double new_b = cos_a * ctm[1] - sin_a * ctm[3];
    double new_c = sin_a * ctm[0] + cos_a * ctm[2];
    double new_d = sin_a * ctm[1] + cos_a * ctm[3];
    ctm[0] = new_a;
    ctm[1] = new_b;
    ctm[2] = new_c;
    ctm[3] = new_d;
}

void setrgbcolor(PSInterpreter& interpreter) {
    PSStack& stack = interpreter.getStack();
    
    if (stack.size() < 3) {
        throw std::runtime_error("Stack underflow: need r, g, b for setrgbcolor");
    }
    
    PSObjectPtr bObj = stack.pop();
    PSObjectPtr gObj = stack.pop();
    PSObjectPtr rObj = stack.pop();
    
    if (rObj->getType() != ObjectType::REAL && rObj->getType() != ObjectType::INTEGER) {
        throw std::runtime_error("R must be a number");
    }
    if (gObj->getType() != ObjectType::REAL && gObj->getType() != ObjectType::INTEGER) {
        throw std::runtime_error("G must be a number");
    }
    if (bObj->getType() != ObjectType::REAL && bObj->getType() != ObjectType::INTEGER) {
        throw std::runtime_error("B must be a number");
    }
    
    GraphicsState& gs = interpreter.getCurrentGraphicsState();
    gs.r = (rObj->getType() == ObjectType::INTEGER) ? rObj->asInteger() : rObj->asReal();
    gs.g = (gObj->getType() == ObjectType::INTEGER) ? gObj->asInteger() : gObj->asReal();
    gs.b = (bObj->getType() == ObjectType::INTEGER) ? bObj->asInteger() : bObj->asReal();
}

void setlinewidth(PSInterpreter& interpreter) {
    PSStack& stack = interpreter.getStack();
    
    if (stack.empty()) {
        throw std::runtime_error("Stack underflow: need width for setlinewidth");
    }
    
    PSObjectPtr widthObj = stack.pop();
    
    if (widthObj->getType() != ObjectType::REAL && widthObj->getType() != ObjectType::INTEGER) {
        throw std::runtime_error("Width must be a number");
    }
    
    GraphicsState& gs = interpreter.getCurrentGraphicsState();
    gs.lineWidth = (widthObj->getType() == ObjectType::INTEGER) ? widthObj->asInteger() : widthObj->asReal();
}

void showpage(PSInterpreter& interpreter) {
    std::cout << "[DEBUG] showpage called" << std::endl;
    std::cout << "SHOWPAGE: End of page" << std::endl;
    // In a real implementation, this would trigger page output to the device
}

} // namespace Operators

} // namespace PostScript 