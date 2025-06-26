#include "ps_parser.h"
#include <fstream>
#include <sstream>
#include <cctype>
#include <iostream>

namespace PostScript {

std::vector<std::string> PSParser::parse(const std::string& program) {
    return tokenize(program);
}

std::vector<std::string> PSParser::parseFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return tokenize(buffer.str());
}

std::vector<std::string> PSParser::tokenize(const std::string& input) {
    std::cout << "[DEBUG] tokenize input length: " << input.length() << std::endl;
    std::cout << "[DEBUG] tokenize input snippet: '" << input.substr(0, 100) << "'" << std::endl;
    std::vector<std::string> tokens;
    std::string currentToken;
    bool inString = false;
    bool escapeNext = false;
    bool inComment = false;
    bool inArray = false;
    bool inDict = false;
    bool inProc = false;
    int braceLevel = 0;
    int bracketLevel = 0;
    int dictLevel = 0;
    
    for (size_t i = 0; i < input.length(); ++i) {
        char c = input[i];
        
        // Handle comments
        if (c == '%' && !inString && !escapeNext && !inArray && !inDict && !inProc) {
            inComment = true;
            // Skip the rest of the line
            while (i < input.length() && input[i] != '\n') {
                i++;
            }
            if (i < input.length()) {
                i--; // Adjust for the loop increment
            }
            continue;
        }
        
        if (inComment) {
            if (c == '\n') {
                inComment = false;
            }
            continue;
        }
        
        if (escapeNext) {
            currentToken += c;
            escapeNext = false;
            continue;
        }
        
        if (c == '\\' && inString) {
            escapeNext = true;
            continue;
        }
        
        // Handle array brackets
        if (c == '[' && !inString && !escapeNext && !inDict && !inProc) {
            if (!inArray) {
                inArray = true;
                bracketLevel = 1;
            } else {
                bracketLevel++;
            }
            currentToken += c;
            continue;
        }
        
        if (c == ']' && inArray && !inString && !escapeNext) {
            bracketLevel--;
            currentToken += c;
            if (bracketLevel == 0) {
                inArray = false;
            }
            continue;
        }
        
        // Handle dictionary brackets
        if (c == '<' && i + 1 < input.length() && input[i + 1] == '<' && 
            !inString && !escapeNext && !inArray && !inProc) {
            if (!inDict) {
                inDict = true;
                dictLevel = 1;
            } else {
                dictLevel++;
            }
            currentToken += c;
            currentToken += input[++i]; // Skip the second '<'
            continue;
        }
        
        if (c == '>' && i + 1 < input.length() && input[i + 1] == '>' && 
            inDict && !inString && !escapeNext) {
            dictLevel--;
            currentToken += c;
            currentToken += input[++i]; // Skip the second '>'
            if (dictLevel == 0) {
                inDict = false;
            }
            continue;
        }
        
        // Handle procedure braces
        if (c == '{' && !inString && !escapeNext && !inArray && !inDict) {
            if (!inProc) {
                inProc = true;
                braceLevel = 1;
            } else {
                braceLevel++;
            }
            currentToken += c;
            continue;
        }
        
        if (c == '}' && inProc && !inString && !escapeNext) {
            braceLevel--;
            currentToken += c;
            if (braceLevel == 0) {
                inProc = false;
            }
            continue;
        }
        
        if (c == '(' && !inString && !inArray && !inDict && !inProc) {
            // Start of string literal
            inString = true;
            currentToken += c;
            continue;
        }
        
        if (c == ')' && inString) {
            // End of string literal
            inString = false;
            currentToken += c;
            continue;
        }
        
        if (inString || inArray || inDict || inProc) {
            // Inside a composite literal, add all characters
            currentToken += c;
            continue;
        }
        
        if (isWhitespace(c)) {
            // End of current token
            if (!currentToken.empty()) {
                tokens.push_back(currentToken);
                currentToken.clear();
            }
            continue;
        }
        
        if (isDelimiter(c)) {
            // End of current token, then add delimiter as separate token
            if (!currentToken.empty()) {
                tokens.push_back(currentToken);
                currentToken.clear();
            }
            tokens.push_back(std::string(1, c));
            continue;
        }
        
        // Regular character, add to current token
        currentToken += c;
    }
    
    // Don't forget the last token
    if (!currentToken.empty()) {
        tokens.push_back(currentToken);
    }
    
    return tokens;
}

bool PSParser::isWhitespace(char c) const {
    return std::isspace(static_cast<unsigned char>(c));
}

bool PSParser::isDelimiter(char c) const {
    // PostScript delimiters: whitespace, parentheses, brackets, braces
    return isWhitespace(c) || c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}';
}

} // namespace PostScript 