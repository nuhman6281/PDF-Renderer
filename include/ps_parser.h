#ifndef PS_PARSER_H
#define PS_PARSER_H

#include <string>
#include <vector>
#include <sstream>

namespace PostScript {

/**
 * @brief Simple PostScript parser/tokenizer
 * 
 * This class handles the basic tokenization of PostScript input.
 * It breaks input into tokens that can be executed by the interpreter.
 */
class PSParser {
public:
    PSParser() = default;
    ~PSParser() = default;

    /**
     * @brief Parse a PostScript program string into tokens
     * @param program The PostScript program as a string
     * @return Vector of tokens
     */
    std::vector<std::string> parse(const std::string& program);

    /**
     * @brief Parse a PostScript file into tokens
     * @param filename The path to the PostScript file
     * @return Vector of tokens
     */
    std::vector<std::string> parseFile(const std::string& filename);

private:
    /**
     * @brief Tokenize a string into individual tokens
     * @param input The input string to tokenize
     * @return Vector of tokens
     */
    std::vector<std::string> tokenize(const std::string& input);

    /**
     * @brief Check if a character is whitespace
     * @param c The character to check
     * @return true if the character is whitespace
     */
    bool isWhitespace(char c) const;

    /**
     * @brief Check if a character is a delimiter
     * @param c The character to check
     * @return true if the character is a delimiter
     */
    bool isDelimiter(char c) const;
};

} // namespace PostScript

#endif // PS_PARSER_H 