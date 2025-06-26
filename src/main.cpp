#include "ps_interpreter.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    PostScript::PSInterpreter interpreter;
    
    std::cout << "PostScript Interpreter v1.0.0" << std::endl;
    std::cout << "=============================" << std::endl;
    
    if (argc > 1) {
        // Execute PostScript file
        std::string filename = argv[1];
        std::cout << "Executing file: " << filename << std::endl;
        try {
            interpreter.executeFile(filename);
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;
        }
    } else {
        // Execute example PostScript program
        std::cout << "Executing example PostScript program..." << std::endl;
        
        std::string exampleProgram = "(Hello, PostScript!) show\n5 3 add show";
        
        try {
            interpreter.execute(exampleProgram);
            std::cout << std::endl; // Add newline after show operations
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;
        }
        
        std::cout << "\nFinal stack state:" << std::endl;
        interpreter.printState();
    }
    
    return 0;
} 