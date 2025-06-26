# PostScript/PDF Interpreter

A clean-room implementation of a PostScript interpreter with PDF support, written in C++ without any external dependencies beyond standard libraries and zlib for PDF compression.

## Table of Contents

1. [Project Overview](#project-overview)
2. [Architecture](#architecture)
3. [Directory Structure](#directory-structure)
4. [Build System](#build-system)
5. [PostScript Interpreter](#postscript-interpreter)
6. [PDF Parser](#pdf-parser)
7. [Usage Examples](#usage-examples)
8. [API Reference](#api-reference)
9. [Development Guide](#development-guide)

## Project Overview

This project implements a complete PostScript interpreter with PDF parsing capabilities. It follows a clean-room approach, meaning no existing PostScript/PDF libraries (like Ghostscript) were referenced during development.

### Key Features

- **PostScript Interpreter**: Full PostScript language support with stack-based execution
- **PDF Parser**: Complete PDF file parsing including xref streams and object extraction
- **Graphics Operations**: Path construction, painting, transformations, and graphics state management
- **Type System**: Comprehensive object types (numbers, strings, arrays, dictionaries, procedures)
- **Control Flow**: Conditional statements, loops, and procedure definitions
- **Memory Management**: Automatic garbage collection and reference counting

## Architecture

### High-Level Architecture

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   PDF Parser    │    │ PostScript      │    │   Graphics      │
│                 │    │ Interpreter     │    │   Engine        │
│ • Xref Stream   │───▶│ • Stack         │───▶│ • Paths         │
│ • Object        │    │ • Operators     │    │ • Painting      │
│   Extraction    │    │ • Type System   │    │ • Transform     │
└─────────────────┘    └─────────────────┘    └─────────────────┘
```

### Core Components

1. **Parser**: Tokenizes and parses PostScript/PDF syntax
2. **Interpreter**: Executes PostScript operators and manages execution context
3. **Stack**: Manages the operand stack and execution stack
4. **Type System**: Handles all PostScript object types
5. **Graphics Engine**: Manages drawing operations and graphics state

## Directory Structure

```
PDF_Renderer/
├── bin/                    # Compiled executables
│   ├── ps_interpreter     # Main PostScript interpreter
│   └── pdf_test          # PDF parsing test program
├── include/               # Header files
│   ├── pdf_interpreter.h  # PDF interpreter interface
│   ├── pdf_parser.h      # PDF parser interface
│   ├── pdf_types.h       # PDF object types
│   ├── ps_interpreter.h  # PostScript interpreter interface
│   ├── ps_operators.h    # PostScript operator definitions
│   ├── ps_parser.h       # PostScript parser interface
│   ├── ps_stack.h        # Stack data structure
│   └── ps_types.h        # PostScript object types
├── src/                   # Source files
│   ├── data_structures/   # Core data structures
│   │   └── ps_stack.cpp  # Stack implementation
│   ├── interpreter/       # Interpreter components
│   │   ├── ps_interpreter.cpp  # Main interpreter
│   │   └── ps_operators.cpp    # Operator implementations
│   ├── parser/           # Parser components
│   │   └── ps_parser.cpp # PostScript parser
│   ├── main.cpp          # Main entry point
│   ├── pdf_interpreter.cpp # PDF interpreter
│   ├── pdf_parser.cpp    # PDF parser implementation
│   ├── pdf_types.cpp     # PDF type implementations
│   └── ps_types.cpp      # PostScript type implementations
├── examples/             # Example files
│   ├── test.ps          # Basic test file
│   ├── graphics_test.ps # Graphics operations test
│   ├── advanced_features.ps # Advanced features test
│   └── pdf_test.cpp     # PDF test program
├── sample_pdf/          # Sample PDF files for testing
├── CMakeLists.txt       # CMake build configuration
├── Makefile            # Make build system
└── README.md           # This documentation
```

## Build System

### Prerequisites

- C++17 compatible compiler (GCC 7+, Clang 5+, or MSVC 2017+)
- Make or CMake
- zlib development libraries (for PDF compression support)

### Building with Make

```bash
# Build the main interpreter
make

# Build and run tests
make test
make graphics-test
make advanced-test

# Build PDF test program
make pdf-test

# Clean build files
make clean
```

### Building with CMake

```bash
mkdir build
cd build
cmake ..
make
```

## PostScript Interpreter

### Stack-Based Architecture

The interpreter uses a stack-based architecture where:

- **Operand Stack**: Stores values for operations
- **Execution Stack**: Manages procedure calls and control flow
- **Dictionary Stack**: Manages variable and procedure definitions

### Object Types

The interpreter supports all standard PostScript object types:

#### Basic Types

- **Numbers**: Integer and real numbers
- **Strings**: Literal strings `(text)` and hexadecimal strings `<hex>`
- **Booleans**: `true` and `false`
- **Names**: `/name` - identifiers for variables and procedures
- **Null**: `null` - represents absence of value

#### Composite Types

- **Arrays**: `[1 2 3]` - ordered collections
- **Dictionaries**: `<< /key value >>` - key-value mappings
- **Procedures**: `{ code }` - executable code blocks
- **References**: `obj gen R` - indirect object references

### Operator Categories

#### Stack Operations

```postscript
% Basic stack manipulation
5 dup        % Duplicate top element: 5 5
5 3 pop      % Remove top element: 5
5 3 exch     % Exchange top two elements: 3 5
5 3 2 roll   % Roll stack elements
5 3 2 index  % Copy nth element to top
```

#### Arithmetic Operations

```postscript
% Basic arithmetic
5 3 add      % Addition: 8
5 3 sub      % Subtraction: 2
5 3 mul      % Multiplication: 15
5 3 div      % Division: 1.66667
5 3 mod      % Modulo: 2
5 neg        % Negation: -5
5 abs        % Absolute value: 5
```

#### Comparison Operations

```postscript
% Comparison operators
5 3 eq       % Equal: false
5 3 ne       % Not equal: true
5 3 gt       % Greater than: true
5 3 lt       % Less than: false
5 3 ge       % Greater or equal: true
5 3 le       % Less or equal: false
```

#### Control Flow

```postscript
% Conditional execution
true { (Hello) } if
false { (Hello) } { (World) } ifelse

% Loops
0 1 5 { dup . } for
{ dup 5 lt } { dup 1 add } while
```

#### Array and Dictionary Operations

```postscript
% Array operations
[1 2 3] length    % Get array length: 3
[1 2 3] 1 get     % Get element at index: 2
[1 2 3] 4 2 put   % Put value at index: [1 2 4]

% Dictionary operations
<< /a 1 /b 2 >> /a get    % Get value: 1
<< /a 1 >> /c 3 put       % Put key-value pair
```

#### Graphics Operations

##### Path Construction

```postscript
% Basic path operations
100 100 moveto    % Move to point
200 200 lineto    % Line to point
closepath         % Close current path
```

##### Painting

```postscript
% Painting operations
stroke            % Stroke current path
fill              % Fill current path
```

##### Transformations

```postscript
% Coordinate transformations
100 100 translate % Translate coordinate system
1.5 1.5 scale     % Scale coordinate system
45 rotate         % Rotate coordinate system
```

##### Graphics State

```postscript
% Graphics state management
gsave             % Save current graphics state
grestore          % Restore graphics state
0.5 setlinewidth  % Set line width
1 0 0 setrgbcolor % Set RGB color (red)
```

##### Text Operations

```postscript
% Text operations
/Times-Roman findfont 12 scalefont setfont
100 100 moveto (Hello, World!) show
```

### Advanced Features

#### Procedure Definitions

```postscript
/square {
    dup dup mul
} def

5 square    % Result: 25
```

#### Recursion

```postscript
/factorial {
    dup 1 le { pop 1 } { dup 1 sub factorial mul } ifelse
} def

5 factorial  % Result: 120
```

#### Error Handling

```postscript
% Error handling with error dictionaries
errordict /undefined begin
    (Undefined operator: ) print
    /errorname get print
    newline
end
```

## PDF Parser

### PDF Structure Support

The PDF parser supports the complete PDF file format:

#### File Structure

- **Header**: PDF version identification
- **Body**: Indirect objects containing document content
- **Cross-Reference Table**: Object location index
- **Trailer**: Document metadata and root object reference

#### Object Types

- **Indirect Objects**: Numbered objects with generation numbers
- **Streams**: Dictionary + binary data (with compression support)
- **References**: Cross-references to other objects
- **Arrays and Dictionaries**: Same as PostScript

#### Xref Stream Support

- **Binary Xref Data**: Efficient object location storage
- **Compression**: FlateDecode compression support
- **Index Arrays**: Support for multiple object ranges
- **Field Widths**: Configurable field sizes for different PDF versions

### PDF Parsing Features

#### Object Extraction

```cpp
// Parse PDF file
PDFDocument doc = parser.parseFile("document.pdf");

// Access root object
PDFObjectPtr root = doc.objects[doc.rootObjectNumber];

// Extract page tree
std::vector<PDFObjectPtr> pages = extractPages(doc);
```

#### Content Stream Processing

```cpp
// Extract content streams from pages
for (auto& page : pages) {
    PDFObjectPtr contents = page->getValue("Contents");
    if (contents->isStream()) {
        std::string streamData = contents->asStream().data;
        // Process with PostScript interpreter
        interpreter.execute(streamData);
    }
}
```

#### Compression Support

- **FlateDecode**: Standard PDF compression
- **Stream Decompression**: Automatic decompression of compressed streams
- **Xref Streams**: Support for PDF 1.5+ xref streams

## Usage Examples

### Basic PostScript Execution

```bash
# Run a simple PostScript file
./bin/ps_interpreter examples/test.ps

# Execute PostScript code directly
echo "(Hello, World!) show" | ./bin/ps_interpreter
```

### Graphics Operations

```bash
# Run graphics test
./bin/ps_interpreter examples/graphics_test.ps
```

### Advanced Features

```bash
# Test advanced PostScript features
./bin/ps_interpreter examples/advanced_features.ps
```

### PDF Processing

```bash
# Parse and analyze PDF file
./bin/pdf_test sample_pdf/document.pdf
```

### Example PostScript Programs

#### Hello World

```postscript
(Hello, World!) show
```

#### Simple Graphics

```postscript
% Draw a red square
1 0 0 setrgbcolor
100 100 moveto
100 0 rlineto
0 100 rlineto
-100 0 rlineto
closepath
fill
showpage
```

#### Mathematical Operations

```postscript
% Calculate factorial
/factorial {
    dup 1 le { pop 1 } { dup 1 sub factorial mul } ifelse
} def

5 factorial = show
```

#### Array Processing

```postscript
% Process array elements
[1 2 3 4 5] {
    dup mul = show
} forall
```

## API Reference

### PostScript Interpreter API

#### Core Classes

##### PSInterpreter

```cpp
class PSInterpreter {
public:
    PSInterpreter();
    void execute(const std::string& code);
    void executeFile(const std::string& filename);
    PSStack& getStack();
    PSDictionary& getDictionary();
};
```

##### PSStack

```cpp
class PSStack {
public:
    void push(const PSObject& obj);
    PSObject pop();
    PSObject& top();
    size_t size() const;
    bool empty() const;
    void clear();
};
```

##### PSObject

```cpp
class PSObject {
public:
    enum class Type { NUMBER, STRING, BOOLEAN, NAME, ARRAY, DICTIONARY, PROCEDURE, NULL_OBJ };

    // Type checking
    Type getType() const;
    bool isNumber() const;
    bool isString() const;
    // ... other type checks

    // Value access
    double asNumber() const;
    std::string asString() const;
    bool asBoolean() const;
    // ... other accessors
};
```

### PDF Parser API

#### Core Classes

##### PDFParser

```cpp
class PDFParser {
public:
    PDFDocument parseFile(const std::string& filename);
    PDFObjectPtr parseObject(const std::string& input);
    std::vector<std::string> parseContentStream(const std::string& streamData);
};
```

##### PDFDocument

```cpp
struct PDFDocument {
    PDFDictionary trailer;
    std::map<int, PDFObjectPtr> objects;
    int rootObjectNumber;
    int infoObjectNumber;
    int catalogObjectNumber;
};
```

##### PDFObject

```cpp
class PDFObject {
public:
    enum class ObjectType { NULL_OBJECT, BOOLEAN, INTEGER, REAL, STRING, NAME, ARRAY, DICTIONARY, STREAM, REFERENCE };

    // Type checking and value access similar to PSObject
    ObjectType getType() const;
    bool isDictionary() const;
    PDFDictionary asDictionary() const;
    // ... other methods
};
```

### Operator Registration

#### Custom Operator Definition

```cpp
// Register a custom operator
interpreter.registerOperator("myop", [](PSInterpreter& interp) {
    // Operator implementation
    PSObject obj = interp.getStack().pop();
    // Process object
    interp.getStack().push(result);
});
```

#### Built-in Operators

All standard PostScript operators are built-in and automatically available:

- Stack: `dup`, `pop`, `exch`, `roll`, `index`
- Arithmetic: `add`, `sub`, `mul`, `div`, `mod`, `neg`, `abs`
- Comparison: `eq`, `ne`, `gt`, `lt`, `ge`, `le`
- Control: `if`, `ifelse`, `for`, `repeat`, `while`
- Graphics: `moveto`, `lineto`, `stroke`, `fill`, `showpage`
- And many more...

## Development Guide

### Adding New Operators

1. **Define the operator function**:

```cpp
void myOperator(PSInterpreter& interpreter) {
    // Implementation
}
```

2. **Register in operator map**:

```cpp
operators["myop"] = myOperator;
```

3. **Add to header file**:

```cpp
// In ps_operators.h
void myOperator(PSInterpreter& interpreter);
```

### Extending the Type System

1. **Add new type to enum**:

```cpp
enum class Type {
    // ... existing types
    NEW_TYPE
};
```

2. **Extend PSObject class**:

```cpp
class PSObject {
    // Add constructor
    PSObject(NewType value);

    // Add type checking
    bool isNewType() const;

    // Add value access
    NewType asNewType() const;
};
```

3. **Update toString() method**:

```cpp
std::string PSObject::toString() const {
    switch (type_) {
        // ... existing cases
        case Type::NEW_TYPE:
            return "new_type_value";
    }
}
```

### Testing

#### Unit Tests

```bash
# Run specific test
make test TEST=stack_operations

# Run all tests
make test-all
```

#### Integration Tests

```bash
# Test PostScript interpreter
make integration-test

# Test PDF parser
make pdf-integration-test
```

### Performance Considerations

#### Memory Management

- Objects use shared pointers for automatic memory management
- Stack operations are optimized for common patterns
- Dictionary lookups use hash maps for O(1) access

#### Optimization Tips

- Use `const` references where possible
- Minimize object copying in hot paths
- Consider using move semantics for large objects
- Profile with real PostScript/PDF files

### Debugging

#### Debug Output

```bash
# Enable debug output
./bin/ps_interpreter --debug examples/test.ps

# Verbose stack tracing
./bin/ps_interpreter --trace examples/test.ps
```

#### Common Issues

1. **Stack underflow**: Check operator implementations for proper stack management
2. **Type errors**: Verify object types before operations
3. **Memory leaks**: Use smart pointers consistently
4. **PDF parsing errors**: Check xref table and object references

## License

This project is developed as a clean-room implementation and is provided under the MIT License. See LICENSE file for details.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Implement your changes
4. Add tests for new functionality
5. Submit a pull request

## Roadmap

### Planned Features

- [ ] Additional PDF compression formats
- [ ] Font rendering and text layout
- [ ] Image support (JPEG, PNG)
- [ ] Advanced graphics operations
- [ ] Performance optimizations
- [ ] Web interface
- [ ] Mobile platform support

### Known Limitations

- Limited font support (basic text rendering only)
- No image format support yet
- Basic graphics operations only
- No advanced PDF features (forms, annotations, etc.)

---

For more information, see the individual header files and source code for detailed implementation documentation.
