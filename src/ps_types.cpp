#include "ps_types.h"
#include <stdexcept>
#include <sstream>

namespace PostScript {

int PSObject::asInteger() const {
    if (type_ != ObjectType::INTEGER) {
        throw std::runtime_error("Object is not an integer");
    }
    return std::get<int>(value_);
}

double PSObject::asReal() const {
    if (type_ != ObjectType::REAL) {
        throw std::runtime_error("Object is not a real number");
    }
    return std::get<double>(value_);
}

std::string PSObject::asString() const {
    if (type_ != ObjectType::STRING) {
        throw std::runtime_error("Object is not a string");
    }
    return std::get<std::string>(value_);
}

bool PSObject::asBoolean() const {
    if (type_ != ObjectType::BOOLEAN) {
        throw std::runtime_error("Object is not a boolean");
    }
    return std::get<bool>(value_);
}

PSArray PSObject::asArray() const {
    if (type_ != ObjectType::ARRAY) {
        throw std::runtime_error("Object is not an array");
    }
    return std::get<PSArray>(value_);
}

PSDictionary PSObject::asDictionary() const {
    if (type_ != ObjectType::DICTIONARY) {
        throw std::runtime_error("Object is not a dictionary");
    }
    return std::get<PSDictionary>(value_);
}

PSProcedure PSObject::asProcedure() const {
    if (type_ != ObjectType::PROCEDURE) {
        throw std::runtime_error("Object is not a procedure");
    }
    return std::get<PSProcedure>(value_);
}

std::string PSObject::toString() const {
    switch (type_) {
        case ObjectType::INTEGER:
            return std::to_string(std::get<int>(value_));
        case ObjectType::REAL: {
            std::ostringstream oss;
            oss << std::get<double>(value_);
            return oss.str();
        }
        case ObjectType::STRING:
            return std::get<std::string>(value_);
        case ObjectType::BOOLEAN:
            return std::get<bool>(value_) ? "true" : "false";
        case ObjectType::ARRAY: {
            const auto& array = std::get<PSArray>(value_);
            std::ostringstream oss;
            oss << "[";
            for (size_t i = 0; i < array.size(); ++i) {
                if (i > 0) oss << " ";
                oss << array[i]->toString();
            }
            oss << "]";
            return oss.str();
        }
        case ObjectType::DICTIONARY: {
            const auto& dict = std::get<PSDictionary>(value_);
            std::ostringstream oss;
            oss << "<<";
            bool first = true;
            for (const auto& pair : dict) {
                if (!first) oss << " ";
                oss << "/" << pair.first << " " << pair.second->toString();
                first = false;
            }
            oss << ">>";
            return oss.str();
        }
        case ObjectType::PROCEDURE: {
            const auto& proc = std::get<PSProcedure>(value_);
            std::ostringstream oss;
            oss << "{";
            for (size_t i = 0; i < proc.size(); ++i) {
                if (i > 0) oss << " ";
                oss << proc[i];
            }
            oss << "}";
            return oss.str();
        }
        case ObjectType::NULL_OBJECT:
            return "null";
        default:
            return "unknown";
    }
}

// Array operations
void PSObject::arrayAppend(const PSObjectPtr& obj) {
    if (type_ != ObjectType::ARRAY) {
        throw std::runtime_error("Object is not an array");
    }
    std::get<PSArray>(value_).push_back(obj);
}

PSObjectPtr PSObject::arrayGet(size_t index) const {
    if (type_ != ObjectType::ARRAY) {
        throw std::runtime_error("Object is not an array");
    }
    const auto& array = std::get<PSArray>(value_);
    if (index >= array.size()) {
        throw std::runtime_error("Array index out of bounds");
    }
    return array[index];
}

void PSObject::arraySet(size_t index, const PSObjectPtr& obj) {
    if (type_ != ObjectType::ARRAY) {
        throw std::runtime_error("Object is not an array");
    }
    auto& array = std::get<PSArray>(value_);
    if (index >= array.size()) {
        throw std::runtime_error("Array index out of bounds");
    }
    array[index] = obj;
}

size_t PSObject::arrayLength() const {
    if (type_ != ObjectType::ARRAY) {
        throw std::runtime_error("Object is not an array");
    }
    return std::get<PSArray>(value_).size();
}

// Dictionary operations
void PSObject::dictPut(const std::string& key, const PSObjectPtr& value) {
    if (type_ != ObjectType::DICTIONARY) {
        throw std::runtime_error("Object is not a dictionary");
    }
    std::get<PSDictionary>(value_)[key] = value;
}

PSObjectPtr PSObject::dictGet(const std::string& key) const {
    if (type_ != ObjectType::DICTIONARY) {
        throw std::runtime_error("Object is not a dictionary");
    }
    const auto& dict = std::get<PSDictionary>(value_);
    auto it = dict.find(key);
    if (it == dict.end()) {
        throw std::runtime_error("Dictionary key not found: " + key);
    }
    return it->second;
}

bool PSObject::dictHas(const std::string& key) const {
    if (type_ != ObjectType::DICTIONARY) {
        return false;
    }
    const auto& dict = std::get<PSDictionary>(value_);
    return dict.find(key) != dict.end();
}

void PSObject::dictRemove(const std::string& key) {
    if (type_ != ObjectType::DICTIONARY) {
        throw std::runtime_error("Object is not a dictionary");
    }
    auto& dict = std::get<PSDictionary>(value_);
    dict.erase(key);
}

std::vector<std::string> PSObject::dictKeys() const {
    if (type_ != ObjectType::DICTIONARY) {
        throw std::runtime_error("Object is not a dictionary");
    }
    const auto& dict = std::get<PSDictionary>(value_);
    std::vector<std::string> keys;
    keys.reserve(dict.size());
    for (const auto& pair : dict) {
        keys.push_back(pair.first);
    }
    return keys;
}

} // namespace PostScript 