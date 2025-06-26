#include "pdf_interpreter.h"
#include <iostream>
#include <map>

namespace PDF {

PDFInterpreter::PDFInterpreter() {
    // Constructor
}

void PDFInterpreter::processFile(const std::string& filename) {
    try {
        std::cout << "=== PDF Interpreter ===" << std::endl;
        std::cout << "Processing PDF file: " << filename << std::endl;
        
        // Parse the PDF file
        PDFDocument document = parser_.parseFile(filename);
        
        std::cout << "PDF file parsed successfully" << std::endl;
        std::cout << "Document structure:" << std::endl;
        std::cout << "  Root object: " << document.rootObjectNumber << std::endl;
        std::cout << "  Info object: " << document.infoObjectNumber << std::endl;
        std::cout << "  Catalog object: " << document.catalogObjectNumber << std::endl;
        std::cout << "  Total objects: " << document.objects.size() << std::endl;
        
        // For now, we'll just demonstrate the basic structure
        // In a full implementation, we'd process pages and content streams
        
    } catch (const std::exception& e) {
        std::cerr << "Error processing PDF file: " << e.what() << std::endl;
        throw;
    }
}

void PDFInterpreter::executeContentStream(const std::string& streamData) {
    try {
        std::cout << "=== Executing PDF Content Stream ===" << std::endl;
        std::cout << "Stream data length: " << streamData.length() << " bytes" << std::endl;
        
        // Parse the content stream into tokens
        std::vector<std::string> tokens = parser_.parseContentStream(streamData);
        
        std::cout << "Parsed " << tokens.size() << " tokens from content stream" << std::endl;
        
        // Execute each token through the PostScript interpreter
        for (const auto& token : tokens) {
            std::string psOperator = mapPDFOperatorToPS(token);
            if (!psOperator.empty()) {
                std::cout << "Mapping PDF operator '" << token << "' to PS operator '" << psOperator << "'" << std::endl;
                psInterpreter_.executeToken(psOperator);
            } else {
                // Pass through as-is (numbers, strings, etc.)
                psInterpreter_.executeToken(token);
            }
        }
        
        std::cout << "Content stream execution completed" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error executing content stream: " << e.what() << std::endl;
        throw;
    }
}

std::string PDFInterpreter::mapPDFOperatorToPS(const std::string& pdfOperator) {
    // Map of PDF operators to PostScript equivalents
    static const std::map<std::string, std::string> operatorMap = {
        // Graphics operators (most PDF operators map directly to PostScript)
        {"m", "moveto"},
        {"l", "lineto"},
        {"c", "curveto"},
        {"h", "closepath"},
        {"S", "stroke"},
        {"f", "fill"},
        {"F", "fill"},
        {"f*", "eofill"},
        {"F*", "eofill"},
        {"W", "clip"},
        {"W*", "eoclip"},
        {"n", "newpath"},
        {"q", "gsave"},
        {"Q", "grestore"},
        {"cm", "concat"},
        {"w", "setlinewidth"},
        {"J", "setlinecap"},
        {"j", "setlinejoin"},
        {"M", "setmiterlimit"},
        {"d", "setdash"},
        {"ri", "setflat"},
        {"i", "setflat"},
        {"gs", "setgstate"},
        
        // Color operators
        {"g", "setgray"},
        {"G", "setgray"},
        {"rg", "setrgbcolor"},
        {"RG", "setrgbcolor"},
        {"k", "setcmykcolor"},
        {"K", "setcmykcolor"},
        
        // Text operators
        {"BT", "begintemplate"},
        {"ET", "endtemplate"},
        {"Td", "translate"},
        {"TD", "translate"},
        {"Tm", "setmatrix"},
        {"T*", "newline"},
        {"Tc", "setcharspacing"},
        {"Tw", "setwordspacing"},
        {"Tz", "setscale"},
        {"TL", "setleading"},
        {"Tf", "setfont"},
        {"Tr", "settextrendering"},
        {"Ts", "settextrise"},
        {"Tj", "show"},
        {"TJ", "show"},
        {"'", "show"},
        {"\"", "show"},
        
        // Path construction
        {"re", "rectfill"},
        
        // Special operators that need custom handling
        {"Do", "exec"},  // Execute named object
        {"MP", "markpoint"},
        {"DP", "definepoint"},
        {"BMC", "beginmarkedcontent"},
        {"EMC", "endmarkedcontent"},
        {"BDC", "begindefaultcontent"},
    };
    
    auto it = operatorMap.find(pdfOperator);
    if (it != operatorMap.end()) {
        return it->second;
    }
    
    // If no mapping found, return empty string (pass through as-is)
    return "";
}

void PDFInterpreter::processPage(const PDFObjectPtr& pageObject) {
    if (!pageObject || !pageObject->isDictionary()) {
        std::cerr << "Invalid page object" << std::endl;
        return;
    }
    
    try {
        const auto& dict = pageObject->asDictionary();
        
        // Look for content stream
        if (dict.find("Contents") != dict.end()) {
            auto contents = dict.at("Contents");
            if (contents->isStream()) {
                processStream(contents);
            } else if (contents->isArray()) {
                // Multiple content streams
                const auto& array = contents->asArray();
                for (const auto& stream : array) {
                    if (stream->isStream()) {
                        processStream(stream);
                    }
                }
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error processing page: " << e.what() << std::endl;
    }
}

void PDFInterpreter::processStream(const PDFObjectPtr& streamObject) {
    if (!streamObject || !streamObject->isStream()) {
        std::cerr << "Invalid stream object" << std::endl;
        return;
    }
    
    try {
        const auto& stream = streamObject->asStream();
        std::cout << "Processing stream with " << stream.data.length() << " bytes of data" << std::endl;
        
        // Execute the content stream
        executeContentStream(stream.data);
        
    } catch (const std::exception& e) {
        std::cerr << "Error processing stream: " << e.what() << std::endl;
    }
}

} // namespace PDF 