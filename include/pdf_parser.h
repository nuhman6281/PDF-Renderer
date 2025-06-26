#ifndef PDF_PARSER_H
#define PDF_PARSER_H

#include "pdf_types.h"
#include <string>
#include <fstream>
#include <memory>

namespace PDF {

/**
 * @brief PDF parser class for parsing PDF files and objects
 */
class PDFParser {
public:
    PDFParser();
    ~PDFParser() = default;

    /**
     * @brief Parse a PDF file and return the document structure
     * @param filename Path to the PDF file
     * @return PDFDocument containing the parsed structure
     */
    PDFDocument parseFile(const std::string& filename);

    /**
     * @brief Parse a PDF object from a string
     * @param input String containing PDF object syntax
     * @return Parsed PDF object
     */
    PDFObjectPtr parseObject(const std::string& input);

    /**
     * @brief Parse a PDF content stream
     * @param streamData The content stream data
     * @return Vector of tokens from the content stream
     */
    std::vector<std::string> parseContentStream(const std::string& streamData);

    /**
     * @brief Parse an object from a stream (helper method)
     * @param stream Input stream
     * @return Parsed PDF object
     */
    PDFObjectPtr parseObjectFromStream(std::istream& stream);

private:
    /**
     * @brief Parse the PDF header and version
     * @param file Input file stream
     */
    void parseHeader(std::ifstream& file);

    /**
     * @brief Parse the xref table
     * @param file Input file stream
     * @param document Document to populate
     */
    void parseXRef(std::ifstream& file, PDFDocument& document);

    /**
     * @brief Parse the trailer dictionary
     * @param file Input file stream
     * @param document Document to populate
     */
    void parseTrailer(std::ifstream& file, PDFDocument& document);

    /**
     * @brief Parse an indirect object
     * @param file Input file stream
     * @param objectNumber Object number
     * @param generation Generation number
     * @return Parsed PDF object
     */
    PDFObjectPtr parseIndirectObject(std::ifstream& file, int objectNumber, int generation);

    /**
     * @brief Parse a dictionary
     * @param file Input file stream
     * @return Parsed dictionary
     */
    PDFDictionary parseDictionary(std::ifstream& file);

    /**
     * @brief Parse an array
     * @param file Input file stream
     * @return Parsed array
     */
    PDFArray parseArray(std::ifstream& file);

    /**
     * @brief Parse a string (literal or hexadecimal)
     * @param file Input file stream
     * @return Parsed string
     */
    PDFString parseString(std::ifstream& file);

    /**
     * @brief Parse a name
     * @param file Input file stream
     * @return Parsed name
     */
    PDFName parseName(std::ifstream& file);

    /**
     * @brief Parse a number (integer or real)
     * @param file Input file stream
     * @return Parsed number object
     */
    PDFObjectPtr parseNumber(std::ifstream& file);

    /**
     * @brief Parse a reference
     * @param file Input file stream
     * @return Parsed reference
     */
    PDFReference parseReference(std::ifstream& file);

    /**
     * @brief Skip whitespace and comments
     * @param file Input file stream
     */
    void skipWhitespace(std::ifstream& file);

    /**
     * @brief Read a token (word or delimiter)
     * @param file Input file stream
     * @return Token string
     */
    std::string readToken(std::ifstream& file);

    /**
     * @brief Check if character is whitespace
     * @param c Character to check
     * @return true if whitespace
     */
    bool isWhitespace(char c) const;

    /**
     * @brief Check if character is a delimiter
     * @param c Character to check
     * @return true if delimiter
     */
    bool isDelimiter(char c) const;
};

} // namespace PDF

#endif // PDF_PARSER_H 