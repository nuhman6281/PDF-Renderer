# PDF Parser Documentation

This document provides comprehensive documentation for the PDF parser implementation, covering all features, data structures, and usage patterns.

## Table of Contents

1. [Overview](#overview)
2. [PDF File Structure](#pdf-file-structure)
3. [Object Types](#object-types)
4. [Parser Architecture](#parser-architecture)
5. [Xref Stream Support](#xref-stream-support)
6. [Content Stream Processing](#content-stream-processing)
7. [API Reference](#api-reference)
8. [Usage Examples](#usage-examples)
9. [Error Handling](#error-handling)
10. [Performance Considerations](#performance-considerations)

## Overview

The PDF parser is a complete implementation that can parse PDF files according to the PDF specification (ISO 32000). It supports both traditional cross-reference tables and modern xref streams, handles compressed content streams, and provides access to all PDF objects.

### Key Features

- **Complete PDF Support**: Parses PDF files from version 1.0 to 1.7
- **Xref Stream Support**: Handles modern PDF 1.5+ xref streams
- **Compression**: Supports FlateDecode and other compression filters
- **Object Resolution**: Automatic resolution of indirect references
- **Page Tree Traversal**: Extracts page objects and content streams
- **Memory Efficient**: Uses smart pointers for automatic memory management

## PDF File Structure

### File Components

A PDF file consists of four main parts:

1. **Header**: PDF version identification
2. **Body**: Indirect objects containing document content
3. **Cross-Reference Table**: Object location index
4. **Trailer**: Document metadata and root object reference

### Header Format

```
%PDF-1.4
```

The header identifies the PDF version and must be the first line of the file.

### Body Objects

Each object in the body has the format:

```
object_number generation_number obj
object_content
endobj
```

### Cross-Reference Table

Traditional xref tables have the format:

```
xref
0 6
0000000003 65535 f
0000000017 00000 n
0000000081 00000 n
0000000000 00000 f
0000000331 00000 n
0000000403 00000 n
trailer
<<
/Size 6
/Root 1 0 R
>>
startxref
553
%%EOF
```

### Xref Streams

Modern PDFs use xref streams instead of traditional tables:

```
stream
<<
/Type /XRef
/Size 6
/W [1 2 1]
/Index [0 6]
/Length 67
/Filter /FlateDecode
>>
[compressed binary data]
endstream
```

## Object Types

### Basic Object Types

#### Null Object

```cpp
PDFObject::ObjectType::NULL_OBJECT
```

Represents the absence of a value.

#### Boolean

```cpp
PDFObject::ObjectType::BOOLEAN
```

True or false values.

#### Integer

```cpp
PDFObject::ObjectType::INTEGER
```

32-bit integer values.

#### Real

```cpp
PDFObject::ObjectType::REAL
```

Floating-point numbers.

#### String

```cpp
PDFObject::ObjectType::STRING
```

Text strings, either literal `(text)` or hexadecimal `<hex>`.

#### Name

```cpp
PDFObject::ObjectType::NAME
```

Identifiers starting with `/`.

### Composite Object Types

#### Array

```cpp
PDFObject::ObjectType::ARRAY
```

Ordered collections of objects: `[obj1 obj2 obj3]`

#### Dictionary

```cpp
PDFObject::ObjectType::DICTIONARY
```

Key-value mappings: `<< /key1 value1 /key2 value2 >>`

#### Stream

```cpp
PDFObject::ObjectType::STREAM
```

Dictionary + binary data with optional compression.

#### Reference

```cpp
PDFObject::ObjectType::REFERENCE
```

Indirect object references: `obj_number gen_number R`

## Parser Architecture

### Core Components

#### PDFParser

The main parser class that orchestrates the parsing process:

```cpp
class PDFParser {
public:
    PDFDocument parseFile(const std::string& filename);
    PDFObjectPtr parseObject(const std::string& input);
    std::vector<std::string> parseContentStream(const std::string& streamData);
private:
    std::string readFile(const std::string& filename);
    PDFObjectPtr parseIndirectObject(const std::string& content, int offset);
    void parseXrefTable(const std::string& content, PDFDocument& doc);
    void parseXrefStream(const std::string& content, PDFDocument& doc);
    std::string decompressStream(const std::string& compressedData);
};
```

#### PDFDocument

Represents a complete parsed PDF document:

```cpp
struct PDFDocument {
    PDFDictionary trailer;
    std::map<int, PDFObjectPtr> objects;
    int rootObjectNumber;
    int infoObjectNumber;
    int catalogObjectNumber;
    std::string version;
};
```

#### PDFObject

Base class for all PDF objects:

```cpp
class PDFObject {
public:
    enum class ObjectType {
        NULL_OBJECT, BOOLEAN, INTEGER, REAL,
        STRING, NAME, ARRAY, DICTIONARY, STREAM, REFERENCE
    };

    virtual ObjectType getType() const = 0;
    virtual std::string toString() const = 0;
    virtual ~PDFObject() = default;
};
```

### Parsing Process

1. **File Reading**: Read the entire PDF file into memory
2. **Header Detection**: Identify PDF version from header
3. **Trailer Location**: Find the trailer dictionary
4. **Xref Processing**: Parse cross-reference table or stream
5. **Object Extraction**: Extract all indirect objects
6. **Reference Resolution**: Resolve all object references
7. **Page Tree Traversal**: Extract page objects and content

## Xref Stream Support

### Xref Stream Format

Xref streams contain binary data that encodes object locations more efficiently than traditional xref tables.

#### Stream Dictionary

```cpp
<<
/Type /XRef
/Size 6          // Total number of objects
/W [1 2 1]       // Field widths: [type, offset, generation]
/Index [0 6]     // Object ranges: [first, count]
/Length 67       // Stream data length
/Filter /FlateDecode  // Compression filter
>>
```

#### Binary Data Format

Each entry in the xref stream is encoded as:

- **Type**: 1 byte (0=free, 1=in-use, 2=compressed)
- **Offset**: 2 bytes (object offset in file)
- **Generation**: 1 byte (generation number)

### Parsing Implementation

```cpp
void PDFParser::parseXrefStream(const std::string& content, PDFDocument& doc) {
    // Find xref stream object
    size_t xrefPos = content.find("/Type /XRef");
    if (xrefPos == std::string::npos) {
        throw std::runtime_error("Xref stream not found");
    }

    // Extract stream dictionary
    PDFDictionary dict = parseStreamDictionary(content, xrefPos);

    // Get field widths and index arrays
    std::vector<int> widths = dict.getArray("/W");
    std::vector<int> index = dict.getArray("/Index");

    // Extract and decompress stream data
    std::string streamData = extractStreamData(content, xrefPos);
    std::string decompressed = decompressStream(streamData);

    // Parse binary data according to field widths
    parseXrefEntries(decompressed, widths, index, doc);
}
```

### Compression Support

The parser supports FlateDecode compression using zlib:

```cpp
std::string PDFParser::decompressStream(const std::string& compressedData) {
    z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;

    if (inflateInit(&strm) != Z_OK) {
        throw std::runtime_error("Failed to initialize zlib");
    }

    strm.avail_in = compressedData.size();
    strm.next_in = (Bytef*)compressedData.data();

    std::string decompressed;
    char buffer[4096];

    do {
        strm.avail_out = sizeof(buffer);
        strm.next_out = (Bytef*)buffer;

        int ret = inflate(&strm, Z_NO_FLUSH);
        if (ret == Z_STREAM_ERROR) {
            inflateEnd(&strm);
            throw std::runtime_error("Zlib decompression error");
        }

        decompressed.append(buffer, sizeof(buffer) - strm.avail_out);
    } while (strm.avail_out == 0);

    inflateEnd(&strm);
    return decompressed;
}
```

## Content Stream Processing

### Content Stream Extraction

Content streams contain the actual drawing instructions for each page:

```cpp
std::vector<PDFObjectPtr> extractPages(const PDFDocument& doc) {
    std::vector<PDFObjectPtr> pages;

    // Get root object
    PDFObjectPtr root = doc.objects[doc.rootObjectNumber];
    if (!root->isDictionary()) {
        throw std::runtime_error("Root object is not a dictionary");
    }

    // Get catalog
    PDFObjectPtr catalog = resolveReference(root->asDictionary().getValue("/Type"));
    if (!catalog->isDictionary()) {
        throw std::runtime_error("Catalog is not a dictionary");
    }

    // Get pages tree
    PDFObjectPtr pages = resolveReference(catalog->asDictionary().getValue("/Pages"));
    extractPagesRecursive(pages, pages);

    return pages;
}
```

### Content Stream Parsing

Content streams are parsed into individual PostScript operators:

```cpp
std::vector<std::string> PDFParser::parseContentStream(const std::string& streamData) {
    std::vector<std::string> tokens;
    std::string current;

    for (char c : streamData) {
        if (std::isspace(c)) {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
        } else {
            current += c;
        }
    }

    if (!current.empty()) {
        tokens.push_back(current);
    }

    return tokens;
}
```

### Graphics State Management

The parser tracks graphics state changes in content streams:

```cpp
struct GraphicsState {
    double ctm[6];        // Current transformation matrix
    double lineWidth;     // Current line width
    double rgbColor[3];   // Current RGB color
    std::string font;     // Current font
    double fontSize;      // Current font size
};
```

## API Reference

### PDFParser Class

#### Constructor

```cpp
PDFParser();
```

#### Main Methods

##### parseFile

```cpp
PDFDocument parseFile(const std::string& filename);
```

Parses a complete PDF file and returns a PDFDocument object.

**Parameters:**

- `filename`: Path to the PDF file

**Returns:**

- `PDFDocument`: Complete parsed document

**Throws:**

- `std::runtime_error`: If file cannot be read or parsed

##### parseObject

```cpp
PDFObjectPtr parseObject(const std::string& input);
```

Parses a single PDF object from string input.

**Parameters:**

- `input`: String containing PDF object

**Returns:**

- `PDFObjectPtr`: Parsed object

##### parseContentStream

```cpp
std::vector<std::string> parseContentStream(const std::string& streamData);
```

Parses a content stream into individual tokens.

**Parameters:**

- `streamData`: Raw content stream data

**Returns:**

- `std::vector<std::string>`: Tokenized content stream

### PDFDocument Structure

```cpp
struct PDFDocument {
    PDFDictionary trailer;           // Trailer dictionary
    std::map<int, PDFObjectPtr> objects;  // All objects by number
    int rootObjectNumber;            // Root object number
    int infoObjectNumber;            // Info object number
    int catalogObjectNumber;         // Catalog object number
    std::string version;             // PDF version
};
```

### PDFObject Classes

#### PDFNull

```cpp
class PDFNull : public PDFObject {
public:
    ObjectType getType() const override { return ObjectType::NULL_OBJECT; }
    std::string toString() const override { return "null"; }
};
```

#### PDFBoolean

```cpp
class PDFBoolean : public PDFObject {
public:
    PDFBoolean(bool value) : value_(value) {}
    ObjectType getType() const override { return ObjectType::BOOLEAN; }
    bool getValue() const { return value_; }
    std::string toString() const override { return value_ ? "true" : "false"; }
private:
    bool value_;
};
```

#### PDFNumber

```cpp
class PDFNumber : public PDFObject {
public:
    PDFNumber(int value) : intValue_(value), isInteger_(true) {}
    PDFNumber(double value) : realValue_(value), isInteger_(false) {}

    ObjectType getType() const override {
        return isInteger_ ? ObjectType::INTEGER : ObjectType::REAL;
    }

    int asInteger() const { return intValue_; }
    double asReal() const { return realValue_; }

    std::string toString() const override {
        return isInteger_ ? std::to_string(intValue_) : std::to_string(realValue_);
    }
private:
    int intValue_;
    double realValue_;
    bool isInteger_;
};
```

#### PDFString

```cpp
class PDFString : public PDFObject {
public:
    PDFString(const std::string& value) : value_(value) {}
    ObjectType getType() const override { return ObjectType::STRING; }
    std::string getValue() const { return value_; }
    std::string toString() const override { return "(" + value_ + ")"; }
private:
    std::string value_;
};
```

#### PDFName

```cpp
class PDFName : public PDFObject {
public:
    PDFName(const std::string& value) : value_(value) {}
    ObjectType getType() const override { return ObjectType::NAME; }
    std::string getValue() const { return value_; }
    std::string toString() const override { return "/" + value_; }
private:
    std::string value_;
};
```

#### PDFArray

```cpp
class PDFArray : public PDFObject {
public:
    ObjectType getType() const override { return ObjectType::ARRAY; }
    void add(PDFObjectPtr obj) { elements_.push_back(obj); }
    PDFObjectPtr get(size_t index) const { return elements_[index]; }
    size_t size() const { return elements_.size(); }

    std::string toString() const override {
        std::string result = "[";
        for (size_t i = 0; i < elements_.size(); ++i) {
            if (i > 0) result += " ";
            result += elements_[i]->toString();
        }
        result += "]";
        return result;
    }
private:
    std::vector<PDFObjectPtr> elements_;
};
```

#### PDFDictionary

```cpp
class PDFDictionary : public PDFObject {
public:
    ObjectType getType() const override { return ObjectType::DICTIONARY; }
    void put(const std::string& key, PDFObjectPtr value) { entries_[key] = value; }
    PDFObjectPtr get(const std::string& key) const {
        auto it = entries_.find(key);
        return it != entries_.end() ? it->second : nullptr;
    }
    bool has(const std::string& key) const { return entries_.find(key) != entries_.end(); }

    std::string toString() const override {
        std::string result = "<<";
        for (const auto& entry : entries_) {
            result += " " + entry.first + " " + entry.second->toString();
        }
        result += " >>";
        return result;
    }
private:
    std::map<std::string, PDFObjectPtr> entries_;
};
```

#### PDFStream

```cpp
class PDFStream : public PDFObject {
public:
    PDFStream(PDFDictionary dict, const std::string& data)
        : dictionary_(dict), data_(data) {}

    ObjectType getType() const override { return ObjectType::STREAM; }
    PDFDictionary getDictionary() const { return dictionary_; }
    std::string getData() const { return data_; }

    std::string toString() const override {
        return dictionary_.toString() + "\nstream\n" +
               std::to_string(data_.length()) + " bytes\nendstream";
    }
private:
    PDFDictionary dictionary_;
    std::string data_;
};
```

#### PDFReference

```cpp
class PDFReference : public PDFObject {
public:
    PDFReference(int objNum, int genNum) : objNum_(objNum), genNum_(genNum) {}
    ObjectType getType() const override { return ObjectType::REFERENCE; }
    int getObjectNumber() const { return objNum_; }
    int getGenerationNumber() const { return genNum_; }

    std::string toString() const override {
        return std::to_string(objNum_) + " " + std::to_string(genNum_) + " R";
    }
private:
    int objNum_;
    int genNum_;
};
```

## Usage Examples

### Basic PDF Parsing

```cpp
#include "pdf_parser.h"

int main() {
    PDFParser parser;

    try {
        // Parse a PDF file
        PDFDocument doc = parser.parseFile("document.pdf");

        // Access the root object
        PDFObjectPtr root = doc.objects[doc.rootObjectNumber];
        std::cout << "Root object: " << root->toString() << std::endl;

        // Get document info
        if (doc.infoObjectNumber > 0) {
            PDFObjectPtr info = doc.objects[doc.infoObjectNumber];
            std::cout << "Document info: " << info->toString() << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error parsing PDF: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
```

### Page Extraction

```cpp
#include "pdf_parser.h"

void extractPages(const PDFDocument& doc) {
    std::vector<PDFObjectPtr> pages = extractPages(doc);

    std::cout << "Found " << pages.size() << " pages" << std::endl;

    for (size_t i = 0; i < pages.size(); ++i) {
        PDFObjectPtr page = pages[i];

        if (page->isDictionary()) {
            PDFDictionary dict = page->asDictionary();

            // Get page dimensions
            PDFObjectPtr mediaBox = dict.get("/MediaBox");
            if (mediaBox && mediaBox->isArray()) {
                PDFArray array = mediaBox->asArray();
                if (array.size() >= 4) {
                    double width = array.get(2)->asNumber();
                    double height = array.get(3)->asNumber();
                    std::cout << "Page " << (i+1) << ": "
                              << width << " x " << height << std::endl;
                }
            }

            // Get content streams
            PDFObjectPtr contents = dict.get("/Contents");
            if (contents) {
                if (contents->isStream()) {
                    PDFStream stream = contents->asStream();
                    std::string contentData = stream.getData();
                    std::cout << "Page " << (i+1) << " content: "
                              << contentData.length() << " bytes" << std::endl;
                } else if (contents->isArray()) {
                    PDFArray array = contents->asArray();
                    std::cout << "Page " << (i+1) << " has "
                              << array.size() << " content streams" << std::endl;
                }
            }
        }
    }
}
```

### Content Stream Processing

```cpp
#include "pdf_parser.h"
#include "ps_interpreter.h"

void processContentStream(const std::string& streamData) {
    PDFParser parser;
    PSInterpreter interpreter;

    // Parse content stream into tokens
    std::vector<std::string> tokens = parser.parseContentStream(streamData);

    // Execute with PostScript interpreter
    for (const std::string& token : tokens) {
        try {
            interpreter.execute(token);
        } catch (const std::exception& e) {
            std::cerr << "Error executing token '" << token << "': "
                      << e.what() << std::endl;
        }
    }
}
```

### Xref Stream Analysis

```cpp
void analyzeXrefStream(const PDFDocument& doc) {
    // Find xref stream object
    for (const auto& entry : doc.objects) {
        PDFObjectPtr obj = entry.second;
        if (obj->isStream()) {
            PDFStream stream = obj->asStream();
            PDFDictionary dict = stream.getDictionary();

            if (dict.has("/Type") && dict.get("/Type")->toString() == "/XRef") {
                std::cout << "Found xref stream:" << std::endl;
                std::cout << "  Size: " << dict.get("/Size")->toString() << std::endl;
                std::cout << "  W: " << dict.get("/W")->toString() << std::endl;
                std::cout << "  Index: " << dict.get("/Index")->toString() << std::endl;
                std::cout << "  Data length: " << stream.getData().length() << " bytes" << std::endl;
                break;
            }
        }
    }
}
```

## Error Handling

### Exception Types

The parser throws several types of exceptions:

#### std::runtime_error

Thrown for general parsing errors:

- File not found
- Invalid PDF format
- Corrupted data

#### std::invalid_argument

Thrown for invalid parameters:

- Null pointers
- Invalid object types

#### std::out_of_range

Thrown for index errors:

- Array access out of bounds
- Invalid object numbers

### Error Recovery

The parser includes error recovery mechanisms:

```cpp
try {
    PDFDocument doc = parser.parseFile("document.pdf");
} catch (const std::runtime_error& e) {
    std::cerr << "PDF parsing failed: " << e.what() << std::endl;
    // Try alternative parsing methods
} catch (const std::exception& e) {
    std::cerr << "Unexpected error: " << e.what() << std::endl;
}
```

### Debug Information

Enable debug output for detailed parsing information:

```cpp
// Set debug level
parser.setDebugLevel(2);

// Parse with debug output
PDFDocument doc = parser.parseFile("document.pdf");
```

## Performance Considerations

### Memory Management

- **Smart Pointers**: All objects use `std::shared_ptr` for automatic memory management
- **Object Caching**: Parsed objects are cached to avoid re-parsing
- **Stream Processing**: Large streams are processed incrementally

### Optimization Strategies

#### Lazy Loading

```cpp
class PDFDocument {
public:
    PDFObjectPtr getObject(int objNum) {
        auto it = objects_.find(objNum);
        if (it == objects_.end()) {
            // Load object on demand
            objects_[objNum] = loadObject(objNum);
        }
        return it->second;
    }
private:
    mutable std::map<int, PDFObjectPtr> objects_;
};
```

#### Efficient String Handling

```cpp
// Use string_view for parsing to avoid copies
std::string_view parseString(std::string_view input) {
    // Parse without copying
    return input.substr(start, length);
}
```

#### Binary Data Processing

```cpp
// Process binary data efficiently
void processBinaryData(const std::string& data) {
    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(data.data());
    size_t length = data.length();

    for (size_t i = 0; i < length; i += 4) {
        uint32_t value = bytes[i] | (bytes[i+1] << 8) |
                        (bytes[i+2] << 16) | (bytes[i+3] << 24);
        // Process value
    }
}
```

### Benchmarking

Measure parsing performance:

```cpp
#include <chrono>

auto start = std::chrono::high_resolution_clock::now();
PDFDocument doc = parser.parseFile("large_document.pdf");
auto end = std::chrono::high_resolution_clock::now();

auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
std::cout << "Parsing took " << duration.count() << " ms" << std::endl;
```

---

This documentation provides a complete reference for the PDF parser implementation. For more detailed information about specific components, see the source code in the `src/pdf_parser.cpp` file.
