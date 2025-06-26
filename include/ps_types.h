#ifndef PS_TYPES_H
#define PS_TYPES_H

#include <string>
#include <variant>
#include <memory>
#include <vector>
#include <unordered_map>
#include <map>
#include <optional>
#include <array>

namespace PostScript {

/**
 * @brief Represents the different types of PostScript objects
 */
enum class ObjectType {
    INTEGER,
    REAL,
    STRING,
    BOOLEAN,
    ARRAY,
    DICTIONARY,
    PROCEDURE,
    NULL_OBJECT
};

/**
 * @brief Forward declaration of the main PostScript object class
 */
class PSObject;

/**
 * @brief Type alias for shared pointer to PSObject
 */
using PSObjectPtr = std::shared_ptr<PSObject>;

/**
 * @brief Type alias for array of PostScript objects
 */
using PSArray = std::vector<PSObjectPtr>;

/**
 * @brief Type alias for dictionary of PostScript objects
 */
using PSDictionary = std::unordered_map<std::string, PSObjectPtr>;

/**
 * @brief Type alias for procedure (array of tokens/objects)
 */
using PSProcedure = std::vector<std::string>;

/**
 * @brief Union type for storing primitive values
 */
using PrimitiveValue = std::variant<int, double, std::string, bool, PSArray, PSDictionary, PSProcedure>;

struct Point {
    double x, y;
    Point(double x_ = 0, double y_ = 0) : x(x_), y(y_) {}
};

struct PathSegment {
    enum class Type { MoveTo, LineTo, CurveTo, ClosePath } type;
    std::vector<Point> points; // 1 for moveto/lineto, 3 for curveto, 0 for closepath
    PathSegment(Type t, std::vector<Point> pts) : type(t), points(std::move(pts)) {}
};

struct GraphicsState {
    Point currentPoint;
    std::vector<PathSegment> currentPath;
    std::array<double, 6> ctm; // Current transformation matrix (a, b, c, d, e, f)
    double lineWidth;
    double r, g, b; // Color (RGB)
    GraphicsState()
        : currentPoint(0, 0), ctm{1,0,0,1,0,0}, lineWidth(1.0), r(0), g(0), b(0) {}
};

/**
 * @brief Represents a PostScript object that can be pushed onto the stack
 */
class PSObject {
public:
    // Constructors for primitive types
    PSObject() : type_(ObjectType::NULL_OBJECT) {}
    explicit PSObject(int value) : type_(ObjectType::INTEGER), value_(value) {}
    explicit PSObject(double value) : type_(ObjectType::REAL), value_(value) {}
    explicit PSObject(const std::string& value) : type_(ObjectType::STRING), value_(value) {}
    explicit PSObject(bool value) : type_(ObjectType::BOOLEAN), value_(value) {}
    
    // Constructors for composite types
    explicit PSObject(const PSArray& array) : type_(ObjectType::ARRAY), value_(array) {}
    explicit PSObject(const PSDictionary& dict) : type_(ObjectType::DICTIONARY), value_(dict) {}
    explicit PSObject(const PSProcedure& proc) : type_(ObjectType::PROCEDURE), value_(proc) {}

    ObjectType getType() const { return type_; }
    const PrimitiveValue& getValue() const { return value_; }

    // Type-safe getters for primitive types
    int asInteger() const;
    double asReal() const;
    std::string asString() const;
    bool asBoolean() const;
    
    // Type-safe getters for composite types
    PSArray asArray() const;
    PSDictionary asDictionary() const;
    PSProcedure asProcedure() const;

    // String representation for debugging
    std::string toString() const;

    // Array operations
    void arrayAppend(const PSObjectPtr& obj);
    PSObjectPtr arrayGet(size_t index) const;
    void arraySet(size_t index, const PSObjectPtr& obj);
    size_t arrayLength() const;
    
    // Dictionary operations
    void dictPut(const std::string& key, const PSObjectPtr& value);
    PSObjectPtr dictGet(const std::string& key) const;
    bool dictHas(const std::string& key) const;
    void dictRemove(const std::string& key);
    std::vector<std::string> dictKeys() const;

private:
    ObjectType type_;
    PrimitiveValue value_;
};

} // namespace PostScript

#endif // PS_TYPES_H 