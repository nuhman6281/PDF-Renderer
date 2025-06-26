#include "pdf_interpreter.h"
#include <iostream>

int main() {
    try {
        std::cout << "=== PDF Infrastructure Test ===" << std::endl;
        
        // Create a PDF interpreter
        PDF::PDFInterpreter interpreter;
        
        // Test parsing a real PDF file
        std::cout << "\n--- Testing Real PDF File Parsing ---" << std::endl;
        std::string pdfPath = "sample_pdf/41483.pdf";
        interpreter.processFile(pdfPath);
        
        std::cout << "\n=== PDF Infrastructure Test Completed ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
} 