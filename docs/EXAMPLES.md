# Examples Guide

This document provides comprehensive examples for using the PostScript/PDF interpreter, covering basic operations, graphics, advanced features, and PDF processing.

## Table of Contents

1. [Basic PostScript Examples](#basic-postscript-examples)
2. [Graphics Examples](#graphics-examples)
3. [Advanced PostScript Examples](#advanced-postscript-examples)
4. [PDF Processing Examples](#pdf-processing-examples)
5. [Integration Examples](#integration-examples)
6. [Debugging Examples](#debugging-examples)

## Basic PostScript Examples

### Hello World

```postscript
% Basic "Hello, World!" example
(Hello, World!) show
```

**Output:**

```
Hello, World!
```

### Stack Operations

```postscript
% Demonstrate basic stack operations
5 dup        % Duplicate: stack becomes 5 5
add          % Add: stack becomes 10
3 exch       % Exchange: stack becomes 3 10
sub          % Subtract: stack becomes 7
=            % Print result
```

**Output:**

```
7
```

### Arithmetic Operations

```postscript
% Basic arithmetic
10 3 add =   % Addition: 13
10 3 sub =   % Subtraction: 7
10 3 mul =   % Multiplication: 30
10 3 div =   % Division: 3.33333
10 3 mod =   % Modulo: 1
10 neg =     % Negation: -10
-10 abs =    % Absolute value: 10
```

**Output:**

```
13
7
30
3.33333
1
-10
10
```

### Comparison Operations

```postscript
% Comparison operators
5 3 eq =     % Equal: false
5 5 eq =     % Equal: true
5 3 ne =     % Not equal: true
5 3 gt =     % Greater than: true
5 3 lt =     % Less than: false
5 5 ge =     % Greater or equal: true
3 5 le =     % Less or equal: true
```

**Output:**

```
false
true
true
true
false
true
true
```

### String Operations

```postscript
% String manipulation
(Hello) dup =        % Duplicate string
(Hello) length =     % String length: 5
(Hello) 1 get =      % Get character at index 1: 101 (ASCII 'e')
(Hello) 1 120 put =  % Put 'x' at index 1
=                    % Print modified string: (Hxllo)
```

**Output:**

```
(Hello)
(Hello)
5
101
(Hxllo)
```

## Graphics Examples

### Basic Shapes

#### Rectangle

```postscript
% Draw a simple rectangle
100 100 moveto      % Move to starting point
200 0 rlineto       % Line right 200 units
0 100 rlineto       % Line up 100 units
-200 0 rlineto      % Line left 200 units
closepath           % Close the path
stroke              % Stroke the rectangle
showpage            % Output the page
```

#### Circle (approximated with curves)

```postscript
% Draw a circle using Bézier curves
100 200 moveto      % Move to center-top
50 0 100 0 100 50 curveto    % Top-right curve
0 50 0 100 -50 100 curveto   % Bottom-right curve
-50 0 -100 0 -100 -50 curveto % Bottom-left curve
0 -50 0 -100 50 -100 curveto % Top-left curve
closepath
stroke
showpage
```

#### Triangle

```postscript
% Draw a triangle
100 100 moveto      % Bottom-left
200 100 lineto      % Bottom-right
150 200 lineto      % Top
closepath           % Close back to start
fill                % Fill the triangle
showpage
```

### Color and Style

#### Colored Shapes

```postscript
% Draw shapes with different colors
1 0 0 setrgbcolor   % Set color to red
100 100 50 50 rectfill

0 1 0 setrgbcolor   % Set color to green
200 100 50 50 rectfill

0 0 1 setrgbcolor   % Set color to blue
150 200 50 50 rectfill

showpage
```

#### Line Styles

```postscript
% Demonstrate different line styles
1 setlinewidth      % Thin line
100 100 200 100 lineto stroke

5 setlinewidth      % Thick line
100 150 200 150 lineto stroke

0.5 setlinewidth    % Medium line
100 200 200 200 lineto stroke

showpage
```

### Transformations

#### Translation

```postscript
% Draw the same shape in different positions
100 100 moveto 50 50 lineto stroke  % Original

100 100 translate   % Move coordinate system
100 100 moveto 50 50 lineto stroke  % Translated

100 100 translate   % Move again
100 100 moveto 50 50 lineto stroke  % Further translated

showpage
```

#### Scaling

```postscript
% Draw shapes at different scales
100 100 moveto 50 50 lineto stroke  % Original size

1.5 1.5 scale      % Scale up by 1.5
100 100 moveto 50 50 lineto stroke

0.5 0.5 scale      % Scale down by 0.5
100 100 moveto 50 50 lineto stroke

showpage
```

#### Rotation

```postscript
% Draw shapes at different rotations
100 100 moveto 150 100 lineto stroke  % Horizontal line

45 rotate          % Rotate 45 degrees
100 100 moveto 150 100 lineto stroke

90 rotate          % Rotate another 90 degrees
100 100 moveto 150 100 lineto stroke

showpage
```

### Text Rendering

#### Basic Text

```postscript
% Basic text rendering
/Times-Roman findfont 12 scalefont setfont
100 100 moveto
(Hello, PostScript!) show
showpage
```

#### Text with Different Fonts and Sizes

```postscript
% Multiple text styles
/Times-Roman findfont 12 scalefont setfont
100 100 moveto (Small text) show

/Times-Bold findfont 18 scalefont setfont
100 150 moveto (Large bold text) show

/Helvetica findfont 14 scalefont setfont
100 200 moveto (Helvetica text) show

showpage
```

#### Colored Text

```postscript
% Text with colors
/Times-Roman findfont 16 scalefont setfont

1 0 0 setrgbcolor   % Red
100 100 moveto (Red text) show

0 1 0 setrgbcolor   % Green
100 150 moveto (Green text) show

0 0 1 setrgbcolor   % Blue
100 200 moveto (Blue text) show

showpage
```

## Advanced PostScript Examples

### Procedure Definitions

#### Simple Procedures

```postscript
% Define a procedure to draw a square
/square {
    dup dup mul     % Square the number
} def

5 square =          % Result: 25
10 square =         % Result: 100
```

#### Graphics Procedures

```postscript
% Define a procedure to draw a rectangle
/rect {
    moveto
    dup 0 rlineto
    0 exch rlineto
    neg 0 rlineto
    closepath
    stroke
} def

100 100 50 30 rect  % Draw rectangle at (100,100) with size 50x30
showpage
```

#### Recursive Procedures

```postscript
% Factorial function
/factorial {
    dup 1 le { pop 1 } { dup 1 sub factorial mul } ifelse
} def

5 factorial =       % Result: 120
6 factorial =       % Result: 720
```

### Control Flow

#### Conditional Execution

```postscript
% Simple if statement
true { (This will print) } if
false { (This won't print) } if

% If-else statement
5 3 gt { (5 is greater than 3) } { (5 is not greater than 3) } ifelse
3 5 gt { (3 is greater than 5) } { (3 is not greater than 5) } ifelse
```

#### Loops

```postscript
% For loop
0 1 10 { dup . } for    % Print numbers 0 to 10

% While loop
0 { dup 10 lt } { dup 1 add } while  % Count to 10

% Repeat loop
3 { (Hello) } repeat    % Print "Hello" 3 times
```

### Array Operations

#### Array Creation and Manipulation

```postscript
% Create and manipulate arrays
[1 2 3 4 5] dup =      % Create array
length =               % Length: 5
1 get =                % Get element at index 1: 2
10 2 put =             % Put 10 at index 2
=                      % Print modified array: [1 2 10 4 5]
```

#### Array Processing

```postscript
% Process array elements
[1 2 3 4 5] {
    dup mul =          % Square each element
} forall
```

**Output:**

```
1
4
9
16
25
```

### Dictionary Operations

#### Dictionary Creation and Access

```postscript
% Create and use dictionaries
<< /a 1 /b 2 /c 3 >> dup =  % Create dictionary
/a get =                     % Get value for key 'a': 1
/b get =                     % Get value for key 'b': 2
/c get =                     % Get value for key 'c': 3
```

#### Variable Storage

```postscript
% Use dictionary for variable storage
/x 5 def                   % Define variable x
/y 10 def                  % Define variable y
x y add =                  % Use variables: 15
x y mul =                  % Multiply variables: 50
```

### Error Handling

#### Basic Error Handling

```postscript
% Handle undefined operators
errordict /undefined begin
    (Undefined operator: ) print
    /errorname get print
    newline
end

unknown_operator  % This will trigger the error handler
```

#### Stack Error Handling

```postscript
% Handle stack underflow
{
    pop pop pop pop pop pop  % Try to pop more than available
} stopped {
    (Stack underflow handled) =
} if
```

## PDF Processing Examples

### Basic PDF Parsing

```cpp
#include "pdf_parser.h"
#include <iostream>

int main() {
    PDFParser parser;

    try {
        // Parse a PDF file
        PDFDocument doc = parser.parseFile("document.pdf");

        std::cout << "PDF Version: " << doc.version << std::endl;
        std::cout << "Number of objects: " << doc.objects.size() << std::endl;
        std::cout << "Root object: " << doc.rootObjectNumber << std::endl;

        // Access the root object
        if (doc.objects.find(doc.rootObjectNumber) != doc.objects.end()) {
            PDFObjectPtr root = doc.objects[doc.rootObjectNumber];
            std::cout << "Root object type: " << root->toString() << std::endl;
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
#include "pdf_interpreter.h"
#include <iostream>

void extractPages(const std::string& filename) {
    PDFParser parser;
    PDFDocument doc = parser.parseFile(filename);

    // Extract all pages
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
                    std::cout << "Page " << (i+1) << " content: "
                              << stream.getData().length() << " bytes" << std::endl;
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
#include <iostream>

void processContentStream(const std::string& filename) {
    PDFParser parser;
    PSInterpreter interpreter;

    PDFDocument doc = parser.parseFile(filename);
    std::vector<PDFObjectPtr> pages = extractPages(doc);

    for (size_t i = 0; i < pages.size(); ++i) {
        PDFObjectPtr page = pages[i];

        if (page->isDictionary()) {
            PDFDictionary dict = page->asDictionary();
            PDFObjectPtr contents = dict.get("/Contents");

            if (contents && contents->isStream()) {
                PDFStream stream = contents->asStream();
                std::string contentData = stream.getData();

                std::cout << "Processing content stream for page " << (i+1) << std::endl;

                // Parse content stream into tokens
                std::vector<std::string> tokens = parser.parseContentStream(contentData);

                // Execute with PostScript interpreter
                for (const std::string& token : tokens) {
                    try {
                        interpreter.execute(token);
                    } catch (const std::exception& e) {
                        std::cerr << "Error executing token '" << token << "': "
                                  << e.what() << std::endl;
                    }
                }

                std::cout << "Finished processing page " << (i+1) << std::endl;
            }
        }
    }
}
```

### Xref Stream Analysis

```cpp
#include "pdf_parser.h"
#include <iostream>

void analyzeXrefStream(const std::string& filename) {
    PDFParser parser;
    PDFDocument doc = parser.parseFile(filename);

    // Find xref stream object
    for (const auto& entry : doc.objects) {
        PDFObjectPtr obj = entry.second;
        if (obj->isStream()) {
            PDFStream stream = obj->asStream();
            PDFDictionary dict = stream.getDictionary();

            if (dict.has("/Type") && dict.get("/Type")->toString() == "/XRef") {
                std::cout << "Found xref stream:" << std::endl;
                std::cout << "  Object number: " << entry.first << std::endl;
                std::cout << "  Size: " << dict.get("/Size")->toString() << std::endl;
                std::cout << "  W: " << dict.get("/W")->toString() << std::endl;
                std::cout << "  Index: " << dict.get("/Index")->toString() << std::endl;
                std::cout << "  Data length: " << stream.getData().length() << " bytes" << std::endl;

                // Analyze binary data
                std::string data = stream.getData();
                std::cout << "  First 32 bytes (hex): ";
                for (size_t i = 0; i < std::min(data.length(), size_t(32)); ++i) {
                    printf("%02x ", (unsigned char)data[i]);
                }
                std::cout << std::endl;
                break;
            }
        }
    }
}
```

## Integration Examples

### PostScript to PDF Bridge

```cpp
#include "ps_interpreter.h"
#include "pdf_parser.h"
#include <iostream>

class PostScriptToPDFBridge {
private:
    PSInterpreter psInterpreter_;
    PDFParser pdfParser_;

public:
    void processPDFContent(const std::string& pdfFile) {
        // Parse PDF
        PDFDocument doc = pdfParser_.parseFile(pdfFile);
        std::vector<PDFObjectPtr> pages = extractPages(doc);

        for (size_t i = 0; i < pages.size(); ++i) {
            std::cout << "Processing page " << (i+1) << std::endl;

            // Extract content stream
            PDFObjectPtr page = pages[i];
            if (page->isDictionary()) {
                PDFDictionary dict = page->asDictionary();
                PDFObjectPtr contents = dict.get("/Contents");

                if (contents && contents->isStream()) {
                    PDFStream stream = contents->asStream();

                    // Convert PDF content to PostScript
                    std::string psCode = convertPDFToPostScript(stream);

                    // Execute with PostScript interpreter
                    psInterpreter_.execute(psCode);
                }
            }
        }
    }

private:
    std::string convertPDFToPostScript(const PDFStream& stream) {
        // This is a simplified conversion
        // In practice, you'd need to handle PDF-specific operators
        std::string content = stream.getData();

        // Basic conversion of common operators
        // PDF 'm' -> PostScript 'moveto'
        // PDF 'l' -> PostScript 'lineto'
        // PDF 'S' -> PostScript 'stroke'
        // etc.

        return content; // Simplified
    }
};
```

### Graphics State Tracking

```cpp
#include "ps_interpreter.h"
#include <iostream>

class GraphicsStateTracker {
private:
    struct GraphicsState {
        double ctm[6];        // Current transformation matrix
        double lineWidth;     // Current line width
        double rgbColor[3];   // Current RGB color
        std::string font;     // Current font
        double fontSize;      // Current font size
    };

    GraphicsState currentState_;
    std::vector<GraphicsState> stateStack_;

public:
    GraphicsStateTracker() {
        // Initialize default state
        currentState_.ctm[0] = 1; currentState_.ctm[1] = 0;
        currentState_.ctm[2] = 0; currentState_.ctm[3] = 1;
        currentState_.ctm[4] = 0; currentState_.ctm[5] = 0;
        currentState_.lineWidth = 1.0;
        currentState_.rgbColor[0] = 0; currentState_.rgbColor[1] = 0; currentState_.rgbColor[2] = 0;
        currentState_.font = "Times-Roman";
        currentState_.fontSize = 12.0;
    }

    void onOperator(const std::string& op, PSInterpreter& interpreter) {
        if (op == "gsave") {
            stateStack_.push_back(currentState_);
        } else if (op == "grestore") {
            if (!stateStack_.empty()) {
                currentState_ = stateStack_.back();
                stateStack_.pop_back();
            }
        } else if (op == "setlinewidth") {
            currentState_.lineWidth = interpreter.getStack().top().asNumber();
        } else if (op == "setrgbcolor") {
            PSStack& stack = interpreter.getStack();
            currentState_.rgbColor[2] = stack.pop().asNumber(); // Blue
            currentState_.rgbColor[1] = stack.pop().asNumber(); // Green
            currentState_.rgbColor[0] = stack.pop().asNumber(); // Red
        }

        printState();
    }

private:
    void printState() {
        std::cout << "Graphics State:" << std::endl;
        std::cout << "  CTM: [" << currentState_.ctm[0] << " " << currentState_.ctm[1]
                  << " " << currentState_.ctm[2] << " " << currentState_.ctm[3]
                  << " " << currentState_.ctm[4] << " " << currentState_.ctm[5] << "]" << std::endl;
        std::cout << "  Line width: " << currentState_.lineWidth << std::endl;
        std::cout << "  Color: RGB(" << currentState_.rgbColor[0] << ", "
                  << currentState_.rgbColor[1] << ", " << currentState_.rgbColor[2] << ")" << std::endl;
        std::cout << "  Font: " << currentState_.font << " " << currentState_.fontSize << std::endl;
        std::cout << "  State stack depth: " << stateStack_.size() << std::endl;
    }
};
```

## Debugging Examples

### Stack Tracing

```cpp
#include "ps_interpreter.h"
#include <iostream>

class DebugInterpreter : public PSInterpreter {
private:
    bool debugMode_;
    int traceLevel_;

public:
    DebugInterpreter() : debugMode_(false), traceLevel_(0) {}

    void setDebugMode(bool enabled) { debugMode_ = enabled; }
    void setTraceLevel(int level) { traceLevel_ = level; }

    void execute(const std::string& code) override {
        if (debugMode_) {
            std::cout << "Executing: " << code << std::endl;
        }

        PSInterpreter::execute(code);

        if (debugMode_ && traceLevel_ > 0) {
            printStack();
        }
    }

    void executeOperator(const std::string& op) override {
        if (debugMode_) {
            std::cout << "Executing operator: " << op << std::endl;
        }

        PSInterpreter::executeOperator(op);

        if (debugMode_ && traceLevel_ > 1) {
            printStack();
        }
    }

private:
    void printStack() {
        PSStack& stack = getStack();
        std::cout << "Stack (" << stack.size() << " elements): ";
        for (size_t i = 0; i < stack.size(); ++i) {
            std::cout << stack.get(i)->toString() << " ";
        }
        std::cout << std::endl;
    }
};
```

### Performance Profiling

```cpp
#include "ps_interpreter.h"
#include <chrono>
#include <map>
#include <iostream>

class ProfilingInterpreter : public PSInterpreter {
private:
    std::map<std::string, std::chrono::microseconds> operatorTimes_;
    std::map<std::string, int> operatorCounts_;
    bool profilingEnabled_;

public:
    ProfilingInterpreter() : profilingEnabled_(false) {}

    void enableProfiling(bool enabled) { profilingEnabled_ = enabled; }

    void executeOperator(const std::string& op) override {
        if (!profilingEnabled_) {
            PSInterpreter::executeOperator(op);
            return;
        }

        auto start = std::chrono::high_resolution_clock::now();
        PSInterpreter::executeOperator(op);
        auto end = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        operatorTimes_[op] += duration;
        operatorCounts_[op]++;
    }

    void printProfile() {
        if (!profilingEnabled_) return;

        std::cout << "Operator Profile:" << std::endl;
        std::cout << "=================" << std::endl;

        for (const auto& entry : operatorTimes_) {
            const std::string& op = entry.first;
            auto totalTime = entry.second;
            int count = operatorCounts_[op];
            auto avgTime = totalTime.count() / count;

            std::cout << op << ":" << std::endl;
            std::cout << "  Count: " << count << std::endl;
            std::cout << "  Total time: " << totalTime.count() << " μs" << std::endl;
            std::cout << "  Average time: " << avgTime << " μs" << std::endl;
            std::cout << std::endl;
        }
    }
};
```

### Memory Usage Tracking

```cpp
#include "ps_interpreter.h"
#include <iostream>

class MemoryTrackingInterpreter : public PSInterpreter {
private:
    size_t initialMemory_;
    size_t peakMemory_;

public:
    MemoryTrackingInterpreter() {
        initialMemory_ = getCurrentMemoryUsage();
        peakMemory_ = initialMemory_;
    }

    void execute(const std::string& code) override {
        PSInterpreter::execute(code);
        updateMemoryUsage();
    }

    void printMemoryUsage() {
        size_t current = getCurrentMemoryUsage();
        size_t total = current - initialMemory_;

        std::cout << "Memory Usage:" << std::endl;
        std::cout << "  Current: " << formatBytes(current) << std::endl;
        std::cout << "  Peak: " << formatBytes(peakMemory_) << std::endl;
        std::cout << "  Total allocated: " << formatBytes(total) << std::endl;
    }

private:
    size_t getCurrentMemoryUsage() {
        // This is a simplified implementation
        // In practice, you'd use platform-specific APIs
        return 0; // Placeholder
    }

    void updateMemoryUsage() {
        size_t current = getCurrentMemoryUsage();
        if (current > peakMemory_) {
            peakMemory_ = current;
        }
    }

    std::string formatBytes(size_t bytes) {
        const char* units[] = {"B", "KB", "MB", "GB"};
        int unit = 0;
        double size = bytes;

        while (size >= 1024 && unit < 3) {
            size /= 1024;
            unit++;
        }

        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%.2f %s", size, units[unit]);
        return std::string(buffer);
    }
};
```

---

This examples guide provides comprehensive coverage of the PostScript/PDF interpreter capabilities. Each example demonstrates specific features and can be used as a starting point for more complex applications. The examples are designed to be educational and practical, showing real-world usage patterns.
