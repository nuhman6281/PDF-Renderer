#ifndef PDF_TYPES_H
#define PDF_TYPES_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <variant>

namespace PDF {

/**
 * @brief PDF object types
 */
enum class ObjectType {
    NULL_OBJECT,
    BOOLEAN,
    INTEGER,
    REAL,
    STRING,
    NAME,
    ARRAY,
    DICTIONARY,
    STREAM,
    REFERENCE
};

/**
 * @brief Forward declaration for PDF objects
 */
class PDFObject;

/**
 * @brief Shared pointer to PDF objects
 */
using PDFObjectPtr = std::shared_ptr<PDFObject>;

/**
 * @brief PDF string (can be literal or hexadecimal)
 */
struct PDFString {
    std::string value;
    bool isHex;
    PDFString(const std::string& val, bool hex = false) : value(val), isHex(hex) {}
};

/**
 * @brief PDF name (starts with /)
 */
struct PDFName {
    std::string value;
    PDFName(const std::string& val) : value(val) {}
};

/**
 * @brief PDF array
 */
using PDFArray = std::vector<PDFObjectPtr>;

/**
 * @brief PDF dictionary
 */
using PDFDictionary = std::map<std::string, PDFObjectPtr>;

/**
 * @brief PDF reference (object number and generation)
 */
struct PDFReference {
    int objectNumber;
    int generation;
    PDFReference(int objNum, int gen) : objectNumber(objNum), generation(gen) {}
};

/**
 * @brief PDF stream (dictionary + data)
 */
struct PDFStream {
    PDFDictionary dict;
    std::string data;
    PDFStream(const PDFDictionary& d, const std::string& streamData) 
        : dict(d), data(streamData) {}
};

/**
 * @brief Main PDF object class
 */
class PDFObject {
public:
    using ValueType = std::variant<
        std::monostate,           // null
        bool,                     // boolean
        int,                      // integer
        double,                   // real
        PDFString,                // string
        PDFName,                  // name
        PDFArray,                 // array
        PDFDictionary,            // dictionary
        PDFStream,                // stream
        PDFReference              // reference
    >;

    PDFObject() : type_(ObjectType::NULL_OBJECT), value_(std::monostate{}) {}
    
    // Constructors for different types
    PDFObject(bool b) : type_(ObjectType::BOOLEAN), value_(b) {}
    PDFObject(int i) : type_(ObjectType::INTEGER), value_(i) {}
    PDFObject(double r) : type_(ObjectType::REAL), value_(r) {}
    PDFObject(const PDFString& s) : type_(ObjectType::STRING), value_(s) {}
    PDFObject(const PDFName& n) : type_(ObjectType::NAME), value_(n) {}
    PDFObject(const PDFArray& a) : type_(ObjectType::ARRAY), value_(a) {}
    PDFObject(const PDFDictionary& d) : type_(ObjectType::DICTIONARY), value_(d) {}
    PDFObject(const PDFStream& s) : type_(ObjectType::STREAM), value_(s) {}
    PDFObject(const PDFReference& r) : type_(ObjectType::REFERENCE), value_(r) {}

    // Type checking
    ObjectType getType() const { return type_; }
    bool isNull() const { return type_ == ObjectType::NULL_OBJECT; }
    bool isBoolean() const { return type_ == ObjectType::BOOLEAN; }
    bool isNumber() const { return type_ == ObjectType::INTEGER || type_ == ObjectType::REAL; }
    bool isInteger() const { return type_ == ObjectType::INTEGER; }
    bool isReal() const { return type_ == ObjectType::REAL; }
    bool isString() const { return type_ == ObjectType::STRING; }
    bool isName() const { return type_ == ObjectType::NAME; }
    bool isArray() const { return type_ == ObjectType::ARRAY; }
    bool isDictionary() const { return type_ == ObjectType::DICTIONARY; }
    bool isStream() const { return type_ == ObjectType::STREAM; }
    bool isReference() const { return type_ == ObjectType::REFERENCE; }

    // Value accessors
    bool asBoolean() const;
    int asInteger() const;
    double asReal() const;
    PDFString asString() const;
    PDFName asName() const;
    PDFArray asArray() const;
    PDFDictionary asDictionary() const;
    PDFStream asStream() const;
    PDFReference asReference() const;

    // Dictionary operations
    bool hasKey(const std::string& key) const;
    PDFObjectPtr getValue(const std::string& key) const;
    void setValue(const std::string& key, const PDFObjectPtr& value);

    // Array operations
    size_t arraySize() const;
    PDFObjectPtr arrayGet(size_t index) const;
    void arraySet(size_t index, const PDFObjectPtr& value);

    // String representation
    std::string toString() const;

private:
    ObjectType type_;
    ValueType value_;
};

/**
 * @brief PDF document structure
 */
struct PDFDocument {
    PDFDictionary trailer;
    std::map<int, PDFObjectPtr> objects;
    int rootObjectNumber;
    int infoObjectNumber;
    int catalogObjectNumber;
    
    PDFDocument() : rootObjectNumber(0), infoObjectNumber(0), catalogObjectNumber(0) {}
};

} // namespace PDF

#endif // PDF_TYPES_H 