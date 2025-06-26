#ifndef PDF_INTERPRETER_H
#define PDF_INTERPRETER_H

#include "pdf_parser.h"
#include "ps_interpreter.h"
#include <memory>

namespace PDF {

/**
 * @brief PDF interpreter that bridges PDF parsing with PostScript execution
 */
class PDFInterpreter {
public:
    PDFInterpreter();
    ~PDFInterpreter() = default;

    /**
     * @brief Process a PDF file and execute its content streams
     * @param filename Path to the PDF file
     */
    void processFile(const std::string& filename);

    /**
     * @brief Execute a PDF content stream using the PostScript interpreter
     * @param streamData The content stream data
     */
    void executeContentStream(const std::string& streamData);

    /**
     * @brief Get access to the underlying PostScript interpreter
     * @return Reference to the PostScript interpreter
     */
    PostScript::PSInterpreter& getPSInterpreter() { return psInterpreter_; }

    /**
     * @brief Get access to the PDF parser
     * @return Reference to the PDF parser
     */
    PDFParser& getParser() { return parser_; }

private:
    PDFParser parser_;
    PostScript::PSInterpreter psInterpreter_;

    /**
     * @brief Map PDF operators to PostScript equivalents
     * @param pdfOperator The PDF operator name
     * @return The corresponding PostScript operator name, or empty if no mapping
     */
    std::string mapPDFOperatorToPS(const std::string& pdfOperator);

    /**
     * @brief Process a PDF page object
     * @param pageObject The page object to process
     */
    void processPage(const PDFObjectPtr& pageObject);

    /**
     * @brief Process a PDF content stream object
     * @param streamObject The stream object to process
     */
    void processStream(const PDFObjectPtr& streamObject);
};

} // namespace PDF

#endif // PDF_INTERPRETER_H 