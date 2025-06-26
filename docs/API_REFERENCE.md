# API Reference

This document provides a comprehensive reference for all classes, functions, and interfaces in the PostScript/PDF interpreter.

## Table of Contents

1. [PostScript Interpreter API](#postscript-interpreter-api)
2. [PDF Parser API](#pdf-parser-api)
3. [Data Structures](#data-structures)
4. [Type System](#type-system)
5. [Graphics Engine](#graphics-engine)
6. [Utility Functions](#utility-functions)

## PostScript Interpreter API

### PSInterpreter Class

The main PostScript interpreter class that manages execution context and operator dispatch.

#### Constructor

```cpp
PSInterpreter();
```

#### Public Methods

##### execute

```cpp
void execute(const std::string& code);
```

Executes PostScript code from a string.

**Parameters:**

- `code`: PostScript code to execute

**Throws:**

- `std::runtime_error`: If parsing or execution fails

##### executeFile

```cpp
void executeFile(const std::string& filename);
```

Executes PostScript code from a file.

**Parameters:**

- `filename`: Path to PostScript file

**Throws:**

- `std::runtime_error`: If file cannot be read or parsed

##### getStack

```cpp
PSStack& getStack();
```

Returns reference to the operand stack.

**Returns:**

- `PSStack&`: Reference to operand stack

##### getDictionary

```cpp
PSDictionary& getDictionary();
```

Returns reference to the current dictionary.

**Returns:**

- `PSDictionary&`: Reference to current dictionary

##### registerOperator

```cpp
template<typename F>
void registerOperator(const std::string& name, F&& func);
```

Registers a custom operator.

**Parameters:**

- `name`: Operator name
- `func`: Function to execute

#### Private Methods

##### executeOperator

```cpp
void executeOperator(const std::string& op);
```

Executes a single operator.

**Parameters:**

- `op`: Operator name to execute

**Throws:**

- `std::runtime_error`: If operator is undefined

## PDF Parser API

### PDFParser Class

The main PDF parser class that handles PDF file parsing and object extraction.

#### Constructor

```cpp
PDFParser();
```

#### Public Methods

##### parseFile

```cpp
PDFDocument parseFile(const std::string& filename);
```

Parses a complete PDF file.

**Parameters:**

- `filename`: Path to PDF file

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

#### Private Methods

##### readFile

```cpp
std::string readFile(const std::string& filename);
```

Reads entire file into memory.

**Parameters:**

- `filename`: Path to file

**Returns:**

- `std::string`: File contents

**Throws:**

- `std::runtime_error`: If file cannot be read

##### parseIndirectObject

```cpp
PDFObjectPtr parseIndirectObject(const std::string& content, int offset);
```

Parses an indirect object at the specified offset.

**Parameters:**

- `content`: File content
- `offset`: Object offset in file

**Returns:**

- `PDFObjectPtr`: Parsed object

##### parseXrefTable

```cpp
void parseXrefTable(const std::string& content, PDFDocument& doc);
```

Parses traditional xref table.

**Parameters:**

- `content`: File content
- `doc`: Document to populate

##### parseXrefStream

```cpp
void parseXrefStream(const std::string& content, PDFDocument& doc);
```

Parses xref stream.

**Parameters:**

- `content`: File content
- `doc`: Document to populate

##### decompressStream

```cpp
std::string decompressStream(const std::string& compressedData);
```

Decompresses FlateDecode stream data.

**Parameters:**

- `compressedData`: Compressed data

**Returns:**

- `std::string`: Decompressed data

**Throws:**

- `std::runtime_error`: If decompression fails

## Data Structures

### PSStack Class

Template-based stack implementation for PostScript objects.

#### Constructor

```cpp
PSStack();
```

#### Public Methods

##### push

```cpp
void push(const T& element);
```

Pushes element onto stack.

**Parameters:**

- `element`: Element to push

##### pop

```cpp
T pop();
```

Removes and returns top element.

**Returns:**

- `T`: Top element

**Throws:**

- `std::runtime_error`: If stack is empty

##### top

```cpp
T& top();
```

Returns reference to top element.

**Returns:**

- `T&`: Reference to top element

**Throws:**

- `std::runtime_error`: If stack is empty

##### size

```cpp
size_t size() const;
```

Returns number of elements in stack.

**Returns:**

- `size_t`: Stack size

##### empty

```cpp
bool empty() const;
```

Checks if stack is empty.

**Returns:**

- `bool`: True if empty

##### clear

```cpp
void clear();
```

Removes all elements from stack.

##### dup

```cpp
void dup();
```

Duplicates top element.

**Throws:**

- `std::runtime_error`: If stack is empty

##### exch

```cpp
void exch();
```

Exchanges top two elements.

**Throws:**

- `std::runtime_error`: If stack has fewer than 2 elements

##### roll

```cpp
void roll(int n, int j);
```

Rolls j elements up n positions.

**Parameters:**

- `n`: Number of positions
- `j`: Number of elements

**Throws:**

- `std::runtime_error`: If stack has insufficient elements

##### index

```cpp
void index(int n);
```

Copies nth element to top.

**Parameters:**

- `n`: Element index

**Throws:**

- `std::runtime_error`: If index is invalid

### PSDictionary Class

Dictionary implementation for key-value storage.

#### Constructor

```cpp
PSDictionary();
```

#### Public Methods

##### put

```cpp
void put(const std::string& key, PSObjectPtr value);
```

Associates key with value.

**Parameters:**

- `key`: Dictionary key
- `value`: Value to store

##### get

```cpp
PSObjectPtr get(const std::string& key) const;
```

Retrieves value for key.

**Parameters:**

- `key`: Dictionary key

**Returns:**

- `PSObjectPtr`: Associated value or nullptr

##### has

```cpp
bool has(const std::string& key) const;
```

Checks if key exists.

**Parameters:**

- `key`: Dictionary key

**Returns:**

- `bool`: True if key exists

##### remove

```cpp
void remove(const std::string& key);
```

Removes key-value pair.

**Parameters:**

- `key`: Key to remove

##### size

```cpp
size_t size() const;
```

Returns number of key-value pairs.

**Returns:**

- `size_t`: Dictionary size

##### clear

```cpp
void clear();
```

Removes all key-value pairs.

## Type System

### PSObject Base Class

Base class for all PostScript objects.

#### Public Methods

##### getType

```cpp
virtual Type getType() const = 0;
```

Returns object type.

**Returns:**

- `Type`: Object type

##### toString

```cpp
virtual std::string toString() const = 0;
```

Returns string representation.

**Returns:**

- `std::string`: String representation

##### clone

```cpp
virtual PSObjectPtr clone() const = 0;
```

Creates deep copy of object.

**Returns:**

- `PSObjectPtr`: Cloned object

##### isNumber

```cpp
bool isNumber() const;
```

Checks if object is a number.

**Returns:**

- `bool`: True if number

##### isString

```cpp
bool isString() const;
```

Checks if object is a string.

**Returns:**

- `bool`: True if string

##### isArray

```cpp
bool isArray() const;
```

Checks if object is an array.

**Returns:**

- `bool`: True if array

##### isDictionary

```cpp
bool isDictionary() const;
```

Checks if object is a dictionary.

**Returns:**

- `bool`: True if dictionary

##### asNumber

```cpp
double asNumber() const;
```

Converts to number.

**Returns:**

- `double`: Numeric value

**Throws:**

- `std::runtime_error`: If conversion not possible

##### asString

```cpp
std::string asString() const;
```

Converts to string.

**Returns:**

- `std::string`: String value

**Throws:**

- `std::runtime_error`: If conversion not possible

### PSNumber Class

Represents numeric values (integer and real).

#### Constructor

```cpp
PSNumber(int value);
PSNumber(double value);
```

#### Public Methods

##### getValue

```cpp
double getValue() const;
```

Returns numeric value.

**Returns:**

- `double`: Numeric value

##### isInteger

```cpp
bool isInteger() const;
```

Checks if value is integer.

**Returns:**

- `bool`: True if integer

### PSString Class

Represents string values.

#### Constructor

```cpp
PSString(const std::string& value);
```

#### Public Methods

##### getValue

```cpp
std::string getValue() const;
```

Returns string value.

**Returns:**

- `std::string`: String value

##### length

```cpp
size_t length() const;
```

Returns string length.

**Returns:**

- `size_t`: String length

### PSArray Class

Represents array values.

#### Constructor

```cpp
PSArray();
```

#### Public Methods

##### add

```cpp
void add(PSObjectPtr obj);
```

Adds element to array.

**Parameters:**

- `obj`: Object to add

##### get

```cpp
PSObjectPtr get(size_t index) const;
```

Gets element at index.

**Parameters:**

- `index`: Element index

**Returns:**

- `PSObjectPtr`: Element at index

**Throws:**

- `std::out_of_range`: If index invalid

##### put

```cpp
void put(size_t index, PSObjectPtr obj);
```

Sets element at index.

**Parameters:**

- `index`: Element index
- `obj`: Object to set

**Throws:**

- `std::out_of_range`: If index invalid

##### size

```cpp
size_t size() const;
```

Returns array size.

**Returns:**

- `size_t`: Array size

### PSBoolean Class

Represents boolean values.

#### Constructor

```cpp
PSBoolean(bool value);
```

#### Public Methods

##### getValue

```cpp
bool getValue() const;
```

Returns boolean value.

**Returns:**

- `bool`: Boolean value

### PSName Class

Represents name values.

#### Constructor

```cpp
PSName(const std::string& value);
```

#### Public Methods

##### getValue

```cpp
std::string getValue() const;
```

Returns name value.

**Returns:**

- `std::string`: Name value

### PSProcedure Class

Represents executable procedures.

#### Constructor

```cpp
PSProcedure(const std::vector<std::string>& tokens);
```

#### Public Methods

##### getTokens

```cpp
const std::vector<std::string>& getTokens() const;
```

Returns procedure tokens.

**Returns:**

- `const std::vector<std::string>&`: Procedure tokens

##### execute

```cpp
void execute(PSInterpreter& interpreter);
```

Executes the procedure.

**Parameters:**

- `interpreter`: Interpreter to execute with

## Graphics Engine

### GraphicsState Class

Manages graphics state information.

#### Public Members

```cpp
double ctm[6];        // Current transformation matrix
double lineWidth;     // Current line width
double rgbColor[3];   // Current RGB color
std::string font;     // Current font
double fontSize;      // Current font size
```

#### Public Methods

##### save

```cpp
void save();
```

Saves current state.

##### restore

```cpp
void restore();
```

Restores previous state.

##### setColor

```cpp
void setColor(double r, double g, double b);
```

Sets RGB color.

**Parameters:**

- `r`: Red component (0.0-1.0)
- `g`: Green component (0.0-1.0)
- `b`: Blue component (0.0-1.0)

##### setLineWidth

```cpp
void setLineWidth(double width);
```

Sets line width.

**Parameters:**

- `width`: Line width

##### transform

```cpp
void transform(double a, double b, double c, double d, double e, double f);
```

Applies transformation matrix.

**Parameters:**

- `a`, `b`, `c`, `d`, `e`, `f`: Transformation matrix elements

### GraphicsEngine Class

Handles graphics operations and rendering.

#### Constructor

```cpp
GraphicsEngine();
```

#### Public Methods

##### moveto

```cpp
void moveto(double x, double y);
```

Moves current point.

**Parameters:**

- `x`: X coordinate
- `y`: Y coordinate

##### lineto

```cpp
void lineto(double x, double y);
```

Adds line to current path.

**Parameters:**

- `x`: X coordinate
- `y`: Y coordinate

##### curveto

```cpp
void curveto(double x1, double y1, double x2, double y2, double x3, double y3);
```

Adds Bézier curve to current path.

**Parameters:**

- `x1`, `y1`: First control point
- `x2`, `y2`: Second control point
- `x3`, `y3`: End point

##### closepath

```cpp
void closepath();
```

Closes current path.

##### stroke

```cpp
void stroke();
```

Strokes current path.

##### fill

```cpp
void fill();
```

Fills current path.

##### showpage

```cpp
void showpage();
```

Outputs current page.

## Utility Functions

### String Utilities

##### isNumber

```cpp
bool isNumber(const std::string& str);
```

Checks if string represents a number.

**Parameters:**

- `str`: String to check

**Returns:**

- `bool`: True if numeric

##### isString

```cpp
bool isString(const std::string& str);
```

Checks if string represents a literal string.

**Parameters:**

- `str`: String to check

**Returns:**

- `bool`: True if literal string

##### isName

```cpp
bool isName(const std::string& str);
```

Checks if string represents a name.

**Parameters:**

- `str`: String to check

**Returns:**

- `bool`: True if name

##### extractString

```cpp
std::string extractString(const std::string& str);
```

Extracts content from literal string.

**Parameters:**

- `str`: Literal string

**Returns:**

- `std::string`: String content

##### extractName

```cpp
std::string extractName(const std::string& str);
```

Extracts content from name.

**Parameters:**

- `str`: Name string

**Returns:**

- `std::string`: Name content

### PDF Utilities

##### extractPages

```cpp
std::vector<PDFObjectPtr> extractPages(const PDFDocument& doc);
```

Extracts all pages from PDF document.

**Parameters:**

- `doc`: PDF document

**Returns:**

- `std::vector<PDFObjectPtr>`: Page objects

##### resolveReference

```cpp
PDFObjectPtr resolveReference(PDFObjectPtr ref, const PDFDocument& doc);
```

Resolves indirect reference.

**Parameters:**

- `ref`: Reference object
- `doc`: PDF document

**Returns:**

- `PDFObjectPtr`: Referenced object

##### parseStreamDictionary

```cpp
PDFDictionary parseStreamDictionary(const std::string& content, size_t pos);
```

Parses stream dictionary.

**Parameters:**

- `content`: File content
- `pos`: Position of stream

**Returns:**

- `PDFDictionary`: Stream dictionary

## Error Handling

### Exception Classes

#### PSException

Base class for PostScript exceptions.

```cpp
class PSException : public std::exception {
public:
    PSException(const std::string& message);
    const char* what() const noexcept override;
};
```

#### StackUnderflowException

Thrown when stack operations fail.

```cpp
class StackUnderflowException : public PSException {
public:
    StackUnderflowException();
};
```

#### TypeMismatchException

Thrown when type conversions fail.

```cpp
class TypeMismatchException : public PSException {
public:
    TypeMismatchException(const std::string& expected, const std::string& got);
};
```

#### UndefinedOperatorException

Thrown when undefined operator is encountered.

```cpp
class UndefinedOperatorException : public PSException {
public:
    UndefinedOperatorException(const std::string& op);
};
```

## Memory Management

### Smart Pointer Types

```cpp
using PSObjectPtr = std::shared_ptr<PSObject>;
using PDFObjectPtr = std::shared_ptr<PDFObject>;
using ParserPtr = std::unique_ptr<Parser>;
using DevicePtr = std::unique_ptr<OutputDevice>;
```

### Object Factory

```cpp
class PSObjectFactory {
public:
    static PSObjectPtr createNumber(const std::string& value);
    static PSObjectPtr createString(const std::string& value);
    static PSObjectPtr createArray();
    static PSObjectPtr createDictionary();
    static PSObjectPtr createBoolean(bool value);
    static PSObjectPtr createName(const std::string& value);
    static PSObjectPtr createProcedure(const std::vector<std::string>& tokens);
};
```

---

This API reference provides comprehensive documentation for all public interfaces in the PostScript/PDF interpreter. For implementation details, see the source code files.
