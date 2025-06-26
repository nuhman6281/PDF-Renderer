# Quick Start Guide

This guide will help you get up and running with the PostScript/PDF interpreter quickly.

## Prerequisites

- C++17 compatible compiler (GCC 7+, Clang 5+, or MSVC 2017+)
- Make or CMake
- zlib development libraries (for PDF support)

## Building the Project

### Using Make (Recommended)

```bash
# Clone or download the project
cd PDF_Renderer

# Build the main interpreter
make

# Build PDF test program
make pdf-test
```

### Using CMake

```bash
mkdir build
cd build
cmake ..
make
```

## Running Examples

### Basic PostScript

```bash
# Run a simple PostScript file
./bin/ps_interpreter examples/test.ps
```

**Expected output:**

```
Hello, PostScript!
8
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
# Parse and analyze a PDF file
./bin/pdf_test sample_pdf/41483.pdf
```

## Creating Your First PostScript Program

Create a file called `hello.ps`:

```postscript
% My first PostScript program
(Hello, World!) show
5 3 add = show
```

Run it:

```bash
./bin/ps_interpreter hello.ps
```

## Key Features

### PostScript Interpreter

- Full PostScript language support
- Stack-based execution
- Graphics operations
- Type system (numbers, strings, arrays, dictionaries)
- Control flow (if, loops, procedures)

### PDF Parser

- Complete PDF file parsing
- Xref stream support
- Object extraction
- Content stream processing
- Compression support (FlateDecode)

### Graphics Engine

- Path construction
- Painting operations
- Transformations
- Graphics state management
- Text rendering

## Common Operations

### Stack Operations

```postscript
5 dup        % Duplicate: 5 5
pop          % Remove top: 5
3 exch       % Exchange: 3 5
```

### Arithmetic

```postscript
5 3 add      % Addition: 8
5 3 sub      % Subtraction: 2
5 3 mul      % Multiplication: 15
5 3 div      % Division: 1.66667
```

### Graphics

```postscript
100 100 moveto    % Move to point
200 200 lineto    % Line to point
stroke            % Draw the line
showpage          % Output page
```

### Procedures

```postscript
/square { dup mul } def
5 square          % Result: 25
```

## Troubleshooting

### Build Issues

- Ensure you have a C++17 compiler
- Install zlib development libraries
- Check that all dependencies are installed

### Runtime Issues

- Verify PostScript syntax
- Check file paths
- Ensure PDF files are valid

### Common Errors

- **Stack underflow**: Not enough values on stack
- **Type error**: Wrong object type for operation
- **Undefined operator**: Operator not implemented

## Next Steps

1. Read the full documentation in `README.md`
2. Explore the examples in the `examples/` directory
3. Check the operator reference in `docs/OPERATORS.md`
4. Learn about the architecture in `docs/ARCHITECTURE.md`

## Getting Help

- Check the documentation files
- Review the source code
- Run the test examples
- Examine error messages carefully

The interpreter provides detailed error messages to help debug issues.
