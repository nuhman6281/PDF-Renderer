# Architecture Documentation

This document provides a comprehensive overview of the PostScript/PDF interpreter architecture, including design patterns, data flow, and system components.

## Table of Contents

1. [System Overview](#system-overview)
2. [Architecture Patterns](#architecture-patterns)
3. [Core Components](#core-components)
4. [Data Flow](#data-flow)
5. [Memory Management](#memory-management)
6. [Error Handling](#error-handling)
7. [Performance Design](#performance-design)
8. [Extensibility](#extensibility)
9. [Testing Strategy](#testing-strategy)

## System Overview

### High-Level Architecture

The system follows a layered architecture with clear separation of concerns:

```
┌─────────────────────────────────────────────────────────────┐
│                    Application Layer                        │
├─────────────────────────────────────────────────────────────┤
│                    Interpreter Layer                        │
├─────────────────────────────────────────────────────────────┤
│                     Parser Layer                            │
├─────────────────────────────────────────────────────────────┤
│                   Data Structure Layer                      │
├─────────────────────────────────────────────────────────────┤
│                    Memory Management                        │
└─────────────────────────────────────────────────────────────┘
```

### Component Interaction

```
┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│   PDF       │    │ PostScript  │    │   Graphics  │
│  Parser     │───▶│Interpreter  │───▶│   Engine    │
└─────────────┘    └─────────────┘    └─────────────┘
       │                   │                   │
       ▼                   ▼                   ▼
┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│   Object    │    │   Stack     │    │   State     │
│  Manager    │    │  Manager    │    │  Manager    │
└─────────────┘    └─────────────┘    └─────────────┘
```

## Architecture Patterns

### 1. Interpreter Pattern

The core of the system uses the Interpreter pattern to process PostScript operators:

```cpp
class PSInterpreter {
private:
    std::map<std::string, OperatorFunction> operators_;
    PSStack operandStack_;
    PSStack executionStack_;
    PSDictionary dictionaryStack_;

public:
    void execute(const std::string& code) {
        auto tokens = parser_.tokenize(code);
        for (const auto& token : tokens) {
            if (isOperator(token)) {
                executeOperator(token);
            } else {
                operandStack_.push(createObject(token));
            }
        }
    }

private:
    void executeOperator(const std::string& op) {
        auto it = operators_.find(op);
        if (it != operators_.end()) {
            it->second(*this);
        } else {
            throw std::runtime_error("Unknown operator: " + op);
        }
    }
};
```

### 2. Factory Pattern

Object creation uses the Factory pattern for type safety:

```cpp
class PSObjectFactory {
public:
    static PSObjectPtr createNumber(const std::string& value) {
        if (value.find('.') != std::string::npos) {
            return std::make_shared<PSNumber>(std::stod(value));
        } else {
            return std::make_shared<PSNumber>(std::stoi(value));
        }
    }

    static PSObjectPtr createString(const std::string& value) {
        return std::make_shared<PSString>(value);
    }

    static PSObjectPtr createArray() {
        return std::make_shared<PSArray>();
    }

    static PSObjectPtr createDictionary() {
        return std::make_shared<PSDictionary>();
    }
};
```

### 3. Strategy Pattern

Different parsing strategies for different input types:

```cpp
class ParsingStrategy {
public:
    virtual std::vector<std::string> parse(const std::string& input) = 0;
    virtual ~ParsingStrategy() = default;
};

class PostScriptParsingStrategy : public ParsingStrategy {
public:
    std::vector<std::string> parse(const std::string& input) override {
        // PostScript-specific parsing logic
    }
};

class PDFParsingStrategy : public ParsingStrategy {
public:
    std::vector<std::string> parse(const std::string& input) override {
        // PDF-specific parsing logic
    }
};
```

### 4. Observer Pattern

For graphics state changes and debugging:

```cpp
class GraphicsStateObserver {
public:
    virtual void onStateChange(const GraphicsState& state) = 0;
    virtual ~GraphicsStateObserver() = default;
};

class GraphicsEngine {
private:
    std::vector<std::shared_ptr<GraphicsStateObserver>> observers_;

public:
    void addObserver(std::shared_ptr<GraphicsStateObserver> observer) {
        observers_.push_back(observer);
    }

    void notifyStateChange(const GraphicsState& state) {
        for (auto& observer : observers_) {
            observer->onStateChange(state);
        }
    }
};
```

## Core Components

### 1. Type System

The type system is designed for extensibility and type safety:

```cpp
class PSObject {
public:
    enum class Type {
        NUMBER, STRING, BOOLEAN, NAME,
        ARRAY, DICTIONARY, PROCEDURE, NULL_OBJ
    };

    virtual Type getType() const = 0;
    virtual std::string toString() const = 0;
    virtual PSObjectPtr clone() const = 0;
    virtual ~PSObject() = default;

    // Type checking methods
    bool isNumber() const { return getType() == Type::NUMBER; }
    bool isString() const { return getType() == Type::STRING; }
    bool isArray() const { return getType() == Type::ARRAY; }
    bool isDictionary() const { return getType() == Type::DICTIONARY; }

    // Safe casting methods
    template<typename T>
    std::shared_ptr<T> as() const {
        return std::dynamic_pointer_cast<T>(shared_from_this());
    }
};
```

### 2. Stack Management

The stack system supports both operand and execution stacks:

```cpp
template<typename T>
class Stack {
private:
    std::vector<T> elements_;

public:
    void push(const T& element) {
        elements_.push_back(element);
    }

    T pop() {
        if (elements_.empty()) {
            throw std::runtime_error("Stack underflow");
        }
        T element = elements_.back();
        elements_.pop_back();
        return element;
    }

    T& top() {
        if (elements_.empty()) {
            throw std::runtime_error("Stack underflow");
        }
        return elements_.back();
    }

    size_t size() const { return elements_.size(); }
    bool empty() const { return elements_.empty(); }
    void clear() { elements_.clear(); }

    // Stack manipulation operations
    void dup() {
        if (elements_.empty()) {
            throw std::runtime_error("Stack underflow");
        }
        elements_.push_back(elements_.back());
    }

    void exch() {
        if (elements_.size() < 2) {
            throw std::runtime_error("Stack underflow");
        }
        std::swap(elements_[elements_.size()-2], elements_[elements_.size()-1]);
    }
};
```

### 3. Operator Registry

The operator registry provides a flexible way to register and execute operators:

```cpp
class OperatorRegistry {
private:
    std::map<std::string, std::function<void(PSInterpreter&)>> operators_;

public:
    template<typename F>
    void registerOperator(const std::string& name, F&& func) {
        operators_[name] = std::forward<F>(func);
    }

    void executeOperator(const std::string& name, PSInterpreter& interpreter) {
        auto it = operators_.find(name);
        if (it != operators_.end()) {
            it->second(interpreter);
        } else {
            throw std::runtime_error("Unknown operator: " + name);
        }
    }

    bool hasOperator(const std::string& name) const {
        return operators_.find(name) != operators_.end();
    }
};
```

### 4. Parser System

The parser system supports multiple input formats:

```cpp
class Parser {
public:
    virtual std::vector<std::string> tokenize(const std::string& input) = 0;
    virtual PSObjectPtr parseObject(const std::string& token) = 0;
    virtual ~Parser() = default;
};

class PostScriptParser : public Parser {
private:
    std::regex tokenPattern_;

public:
    PostScriptParser() : tokenPattern_(R"((\S+))") {}

    std::vector<std::string> tokenize(const std::string& input) override {
        std::vector<std::string> tokens;
        std::sregex_iterator iter(input.begin(), input.end(), tokenPattern_);
        std::sregex_iterator end;

        for (; iter != end; ++iter) {
            std::string token = iter->str();
            if (!token.empty() && token[0] != '%') { // Skip comments
                tokens.push_back(token);
            }
        }

        return tokens;
    }

    PSObjectPtr parseObject(const std::string& token) override {
        // Parse individual tokens into PSObjects
        if (isNumber(token)) {
            return PSObjectFactory::createNumber(token);
        } else if (isString(token)) {
            return PSObjectFactory::createString(extractString(token));
        } else if (isName(token)) {
            return PSObjectFactory::createName(extractName(token));
        } else {
            return nullptr; // Will be handled as operator
        }
    }
};
```

## Data Flow

### 1. PostScript Execution Flow

```
Input File/Stream
       │
       ▼
   Tokenizer
       │
       ▼
   Parser
       │
       ▼
   Interpreter
       │
       ▼
   Operator Dispatch
       │
       ▼
   Stack Operations
       │
       ▼
   Output/Graphics
```

### 2. PDF Processing Flow

```
PDF File
   │
   ▼
File Reader
   │
   ▼
Header Parser
   │
   ▼
Xref Parser
   │
   ▼
Object Extractor
   │
   ▼
Reference Resolver
   │
   ▼
Page Tree Traversal
   │
   ▼
Content Stream Extraction
   │
   ▼
PostScript Interpreter
```

### 3. Graphics Pipeline

```
Content Stream
       │
       ▼
   Tokenizer
       │
       ▼
   Graphics State
       │
       ▼
   Path Construction
       │
       ▼
   Painting Operations
       │
       ▼
   Output Device
```

## Memory Management

### 1. Smart Pointer Strategy

The system uses smart pointers for automatic memory management:

```cpp
// Use shared_ptr for shared ownership
using PSObjectPtr = std::shared_ptr<PSObject>;
using PDFObjectPtr = std::shared_ptr<PDFObject>;

// Use unique_ptr for exclusive ownership
using ParserPtr = std::unique_ptr<Parser>;
using DevicePtr = std::unique_ptr<OutputDevice>;

// Use weak_ptr for non-owning references
using ObserverPtr = std::weak_ptr<GraphicsStateObserver>;
```

### 2. Object Lifecycle Management

```cpp
class ObjectManager {
private:
    std::map<int, PSObjectPtr> objects_;
    std::vector<PSObjectPtr> tempObjects_;

public:
    PSObjectPtr createObject(PSObject::Type type) {
        auto obj = PSObjectFactory::create(type);
        tempObjects_.push_back(obj);
        return obj;
    }

    void cleanup() {
        tempObjects_.clear();
    }

    void registerObject(int id, PSObjectPtr obj) {
        objects_[id] = obj;
    }

    PSObjectPtr getObject(int id) {
        auto it = objects_.find(id);
        return it != objects_.end() ? it->second : nullptr;
    }
};
```

### 3. Resource Management

```cpp
class ResourceManager {
private:
    std::map<std::string, std::shared_ptr<Resource>> resources_;

public:
    template<typename T>
    std::shared_ptr<T> getResource(const std::string& name) {
        auto it = resources_.find(name);
        if (it != resources_.end()) {
            return std::dynamic_pointer_cast<T>(it->second);
        }
        return nullptr;
    }

    template<typename T>
    void registerResource(const std::string& name, std::shared_ptr<T> resource) {
        resources_[name] = resource;
    }

    void cleanup() {
        resources_.clear();
    }
};
```

## Error Handling

### 1. Exception Hierarchy

```cpp
class PSException : public std::exception {
public:
    PSException(const std::string& message) : message_(message) {}
    const char* what() const noexcept override { return message_.c_str(); }
private:
    std::string message_;
};

class StackUnderflowException : public PSException {
public:
    StackUnderflowException() : PSException("Stack underflow") {}
};

class TypeMismatchException : public PSException {
public:
    TypeMismatchException(const std::string& expected, const std::string& got)
        : PSException("Type mismatch: expected " + expected + ", got " + got) {}
};

class UndefinedOperatorException : public PSException {
public:
    UndefinedOperatorException(const std::string& op)
        : PSException("Undefined operator: " + op) {}
};
```

### 2. Error Recovery

```cpp
class ErrorHandler {
public:
    virtual void handleError(const PSException& error) = 0;
    virtual bool canRecover(const PSException& error) = 0;
    virtual void recover() = 0;
    virtual ~ErrorHandler() = default;
};

class DefaultErrorHandler : public ErrorHandler {
public:
    void handleError(const PSException& error) override {
        std::cerr << "Error: " << error.what() << std::endl;
        if (canRecover(error)) {
            recover();
        }
    }

    bool canRecover(const PSException& error) override {
        // Determine if error is recoverable
        return true; // Simplified
    }

    void recover() override {
        // Implement recovery logic
        interpreter_.getStack().clear();
    }
private:
    PSInterpreter& interpreter_;
};
```

### 3. Debugging Support

```cpp
class Debugger {
private:
    bool enabled_;
    int logLevel_;
    std::ostream& logStream_;

public:
    Debugger(std::ostream& stream = std::cout)
        : enabled_(false), logLevel_(0), logStream_(stream) {}

    void enable(bool enabled) { enabled_ = enabled; }
    void setLogLevel(int level) { logLevel_ = level; }

    template<typename... Args>
    void log(int level, Args&&... args) {
        if (enabled_ && level <= logLevel_) {
            (logStream_ << ... << args) << std::endl;
        }
    }

    void traceStack(const PSStack& stack) {
        if (enabled_) {
            logStream_ << "Stack: ";
            for (size_t i = 0; i < stack.size(); ++i) {
                logStream_ << stack.get(i)->toString() << " ";
            }
            logStream_ << std::endl;
        }
    }
};
```

## Performance Design

### 1. Caching Strategy

```cpp
template<typename K, typename V>
class Cache {
private:
    std::map<K, V> cache_;
    size_t maxSize_;

public:
    Cache(size_t maxSize = 1000) : maxSize_(maxSize) {}

    V get(const K& key) {
        auto it = cache_.find(key);
        if (it != cache_.end()) {
            return it->second;
        }
        return V{};
    }

    void put(const K& key, const V& value) {
        if (cache_.size() >= maxSize_) {
            cache_.clear(); // Simple eviction strategy
        }
        cache_[key] = value;
    }

    void clear() { cache_.clear(); }
};
```

### 2. Object Pooling

```cpp
template<typename T>
class ObjectPool {
private:
    std::vector<std::unique_ptr<T>> pool_;
    std::vector<T*> available_;

public:
    ObjectPool(size_t initialSize = 100) {
        for (size_t i = 0; i < initialSize; ++i) {
            auto obj = std::make_unique<T>();
            available_.push_back(obj.get());
            pool_.push_back(std::move(obj));
        }
    }

    T* acquire() {
        if (available_.empty()) {
            auto obj = std::make_unique<T>();
            available_.push_back(obj.get());
            pool_.push_back(std::move(obj));
        }

        T* obj = available_.back();
        available_.pop_back();
        return obj;
    }

    void release(T* obj) {
        available_.push_back(obj);
    }
};
```

### 3. Lazy Evaluation

```cpp
class LazyObject {
private:
    std::function<PSObjectPtr()> evaluator_;
    mutable PSObjectPtr cachedValue_;
    mutable bool evaluated_;

public:
    LazyObject(std::function<PSObjectPtr()> evaluator)
        : evaluator_(evaluator), evaluated_(false) {}

    PSObjectPtr getValue() const {
        if (!evaluated_) {
            cachedValue_ = evaluator_();
            evaluated_ = true;
        }
        return cachedValue_;
    }

    void invalidate() {
        evaluated_ = false;
        cachedValue_.reset();
    }
};
```

## Extensibility

### 1. Plugin Architecture

```cpp
class Plugin {
public:
    virtual void initialize(PSInterpreter& interpreter) = 0;
    virtual void cleanup() = 0;
    virtual ~Plugin() = default;
};

class PluginManager {
private:
    std::vector<std::shared_ptr<Plugin>> plugins_;

public:
    void registerPlugin(std::shared_ptr<Plugin> plugin) {
        plugins_.push_back(plugin);
    }

    void initializePlugins(PSInterpreter& interpreter) {
        for (auto& plugin : plugins_) {
            plugin->initialize(interpreter);
        }
    }

    void cleanupPlugins() {
        for (auto& plugin : plugins_) {
            plugin->cleanup();
        }
    }
};
```

### 2. Custom Operators

```cpp
class CustomOperator {
public:
    virtual void execute(PSInterpreter& interpreter) = 0;
    virtual std::string getName() const = 0;
    virtual ~CustomOperator() = default;
};

class CustomOperatorRegistry {
private:
    std::map<std::string, std::shared_ptr<CustomOperator>> operators_;

public:
    void registerOperator(std::shared_ptr<CustomOperator> op) {
        operators_[op->getName()] = op;
    }

    void executeOperator(const std::string& name, PSInterpreter& interpreter) {
        auto it = operators_.find(name);
        if (it != operators_.end()) {
            it->second->execute(interpreter);
        }
    }
};
```

### 3. Output Device Abstraction

```cpp
class OutputDevice {
public:
    virtual void drawLine(double x1, double y1, double x2, double y2) = 0;
    virtual void drawRectangle(double x, double y, double width, double height) = 0;
    virtual void drawText(double x, double y, const std::string& text) = 0;
    virtual void setColor(double r, double g, double b) = 0;
    virtual void setLineWidth(double width) = 0;
    virtual ~OutputDevice() = default;
};

class ConsoleOutputDevice : public OutputDevice {
public:
    void drawLine(double x1, double y1, double x2, double y2) override {
        std::cout << "Line: (" << x1 << "," << y1 << ") to (" << x2 << "," << y2 << ")" << std::endl;
    }

    void drawRectangle(double x, double y, double width, double height) override {
        std::cout << "Rectangle: (" << x << "," << y << ") " << width << "x" << height << std::endl;
    }

    void drawText(double x, double y, const std::string& text) override {
        std::cout << "Text: \"" << text << "\" at (" << x << "," << y << ")" << std::endl;
    }

    void setColor(double r, double g, double b) override {
        std::cout << "Color: RGB(" << r << "," << g << "," << b << ")" << std::endl;
    }

    void setLineWidth(double width) override {
        std::cout << "Line width: " << width << std::endl;
    }
};
```

## Testing Strategy

### 1. Unit Testing Framework

```cpp
class TestFramework {
public:
    template<typename T>
    void assertEqual(const T& expected, const T& actual, const std::string& message = "") {
        if (expected != actual) {
            throw std::runtime_error("Assertion failed: " + message +
                                   " Expected: " + std::to_string(expected) +
                                   " Actual: " + std::to_string(actual));
        }
    }

    void assertTrue(bool condition, const std::string& message = "") {
        if (!condition) {
            throw std::runtime_error("Assertion failed: " + message);
        }
    }

    void assertFalse(bool condition, const std::string& message = "") {
        assertTrue(!condition, message);
    }

    void assertThrows(std::function<void()> func, const std::string& expectedError = "") {
        try {
            func();
            throw std::runtime_error("Expected exception but none was thrown");
        } catch (const std::exception& e) {
            if (!expectedError.empty() && e.what() != expectedError) {
                throw std::runtime_error("Unexpected exception: " + std::string(e.what()));
            }
        }
    }
};

#define TEST(name) void test_##name()
#define RUN_TEST(name) test_##name()
```

### 2. Integration Testing

```cpp
class IntegrationTest {
protected:
    PSInterpreter interpreter_;
    PDFParser parser_;

public:
    void setUp() {
        // Initialize test environment
    }

    void tearDown() {
        // Clean up test environment
    }

    void testPostScriptExecution() {
        setUp();

        std::string code = "5 3 add";
        interpreter_.execute(code);

        PSObjectPtr result = interpreter_.getStack().top();
        assertTrue(result->isNumber());
        assertEqual(8.0, result->asNumber());

        tearDown();
    }

    void testPDFParsing() {
        setUp();

        PDFDocument doc = parser_.parseFile("test.pdf");
        assertTrue(doc.objects.size() > 0);
        assertTrue(doc.rootObjectNumber > 0);

        tearDown();
    }
};
```

### 3. Performance Testing

```cpp
class PerformanceTest {
public:
    template<typename F>
    double measureTime(F&& func) {
        auto start = std::chrono::high_resolution_clock::now();
        func();
        auto end = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        return duration.count() / 1000.0; // Return milliseconds
    }

    void benchmarkParsing() {
        std::vector<std::string> testFiles = {"small.ps", "medium.ps", "large.ps"};

        for (const auto& file : testFiles) {
            double time = measureTime([&]() {
                PSInterpreter interpreter;
                interpreter.executeFile(file);
            });

            std::cout << "Parsing " << file << " took " << time << " ms" << std::endl;
        }
    }
};
```

---

This architecture documentation provides a comprehensive overview of the system design. The architecture is designed to be modular, extensible, and maintainable, with clear separation of concerns and well-defined interfaces between components.
