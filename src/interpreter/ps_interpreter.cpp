#include "ps_interpreter.h"
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace PostScript {

PSInterpreter::PSInterpreter() : parser_(std::make_unique<PSParser>()) {
    // Initialize with a default dictionary
    PSDictionary defaultDict;
    dictionaryStack_.push_back(std::make_shared<PSObject>(defaultDict));
    // Initialize graphics state stack with one default state
    graphicsStack_.push_back(GraphicsState());
}

void PSInterpreter::execute(const std::string& program) {
    try {
        std::vector<std::string> tokens = parser_->parse(program);
        std::cout << "[DEBUG] Parsed tokens (execute): ";
        for (const auto& t : tokens) std::cout << "'" << t << "' ";
        std::cout << std::endl;
        for (const auto& token : tokens) {
            executeToken(token);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error executing PostScript program: " << e.what() << std::endl;
        throw;
    }
}

void PSInterpreter::executeFile(const std::string& filename) {
    try {
        std::vector<std::string> tokens = parser_->parseFile(filename);
        std::cout << "[DEBUG] Parsed tokens (executeFile): ";
        for (const auto& t : tokens) std::cout << "'" << t << "' ";
        std::cout << std::endl;
        for (const auto& token : tokens) {
            executeToken(token);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error executing PostScript file '" << filename << "': " << e.what() << std::endl;
        throw;
    }
}

void PSInterpreter::executeToken(const std::string& token) {
    std::cout << "[DEBUG] Executing token: '" << token << "'" << std::endl;
    // Skip empty tokens
    if (token.empty()) {
        return;
    }
    
    // Check if it's an operator
    if (operators_.hasOperator(token)) {
        OperatorFunction op = operators_.getOperator(token);
        if (op) {
            op(*this);
        }
        return;
    }
    
    // Check if it's a dictionary key (starts with /)
    if (token.length() > 1 && token[0] == '/') {
        std::string key = token.substr(1); // Remove the leading slash
        stack_.push(std::make_shared<PSObject>(token)); // Push the full token including /
        return;
    }
    
    // Check if it's a procedure name (look it up in the current dictionary)
    PSObjectPtr currentDict = getCurrentDictionary();
    if (currentDict && currentDict->getType() == ObjectType::DICTIONARY) {
        if (currentDict->dictHas(token)) {
            PSObjectPtr proc = currentDict->dictGet(token);
            if (proc->getType() == ObjectType::PROCEDURE) {
                // Execute the procedure
                PSProcedure procedure = proc->asProcedure();
                for (const auto& procToken : procedure) {
                    executeToken(procToken);
                }
                return;
            }
        }
    }
    
    // Check if it's a number (integer or real)
    if (token.find('.') != std::string::npos) {
        // Try to parse as real number
        try {
            double value = std::stod(token);
            stack_.push(std::make_shared<PSObject>(value));
            return;
        } catch (const std::exception&) {
            // Not a valid real number, continue to other checks
        }
    } else {
        // Try to parse as integer
        try {
            int value = std::stoi(token);
            stack_.push(std::make_shared<PSObject>(value));
            return;
        } catch (const std::exception&) {
            // Not a valid integer, continue to other checks
        }
    }
    
    // Check if it's a string literal (enclosed in parentheses)
    if (token.length() >= 2 && token[0] == '(' && token[token.length()-1] == ')') {
        stack_.push(std::make_shared<PSObject>(token));
        return;
    }
    
    // Check if it's an array literal (enclosed in square brackets)
    if (token.length() >= 2 && token[0] == '[' && token[token.length()-1] == ']') {
        // Parse array contents
        std::string arrayContent = token.substr(1, token.length() - 2);
        std::vector<std::string> elementTokens = parser_->parse(arrayContent);
        
        PSArray array;
        for (const auto& elementToken : elementTokens) {
            if (!elementToken.empty()) {
                // Create a temporary interpreter to parse each element
                PSInterpreter tempInterpreter;
                tempInterpreter.executeToken(elementToken);
                if (!tempInterpreter.getStack().empty()) {
                    array.push_back(tempInterpreter.getStack().pop());
                }
            }
        }
        
        stack_.push(std::make_shared<PSObject>(array));
        return;
    }
    
    // Check if it's a dictionary literal (enclosed in << >>)
    if (token.length() >= 4 && token.substr(0, 2) == "<<" && token.substr(token.length()-2) == ">>") {
        // Parse dictionary contents
        std::string dictContent = token.substr(2, token.length() - 4);
        std::vector<std::string> elementTokens = parser_->parse(dictContent);
        
        PSDictionary dict;
        for (size_t i = 0; i < elementTokens.size(); i += 2) {
            if (i + 1 < elementTokens.size()) {
                std::string key = elementTokens[i];
                std::string valueToken = elementTokens[i + 1];
                
                // Remove leading slash from key
                if (key.length() > 0 && key[0] == '/') {
                    key = key.substr(1);
                }
                
                // Parse value
                PSInterpreter tempInterpreter;
                tempInterpreter.executeToken(valueToken);
                if (!tempInterpreter.getStack().empty()) {
                    dict[key] = tempInterpreter.getStack().pop();
                }
            }
        }
        
        stack_.push(std::make_shared<PSObject>(dict));
        return;
    }
    
    // Check if it's a procedure literal (enclosed in braces)
    if (token.length() >= 2 && token[0] == '{' && token[token.length()-1] == '}') {
        // Parse procedure contents
        std::string procContent = token.substr(1, token.length() - 2);
        std::vector<std::string> procTokens = parser_->parse(procContent);
        
        PSProcedure procedure;
        for (const auto& procToken : procTokens) {
            if (!procToken.empty()) {
                procedure.push_back(procToken);
            }
        }
        
        stack_.push(std::make_shared<PSObject>(procedure));
        return;
    }
    
    // Check if it's a boolean literal
    if (token == "true") {
        stack_.push(std::make_shared<PSObject>(true));
        return;
    }
    
    if (token == "false") {
        stack_.push(std::make_shared<PSObject>(false));
        return;
    }
    
    // If we get here, it's an unknown token
    std::cerr << "Warning: Unknown token '" << token << "' - treating as literal string" << std::endl;
    stack_.push(std::make_shared<PSObject>(token));
}

void PSInterpreter::printState() const {
    std::cout << "=== PostScript Interpreter State ===" << std::endl;
    stack_.print();
    std::cout << "Dictionary stack size: " << dictionaryStack_.size() << std::endl;
    std::cout << "===================================" << std::endl;
}

PSObjectPtr PSInterpreter::getCurrentDictionary() const {
    if (dictionaryStack_.empty()) {
        return nullptr;
    }
    return dictionaryStack_.back();
}

void PSInterpreter::pushDictionary(const PSObjectPtr& dict) {
    dictionaryStack_.push_back(dict);
}

PSObjectPtr PSInterpreter::popDictionary() {
    if (dictionaryStack_.empty()) {
        throw std::runtime_error("Dictionary stack underflow");
    }
    PSObjectPtr dict = dictionaryStack_.back();
    dictionaryStack_.pop_back();
    return dict;
}

} // namespace PostScript 