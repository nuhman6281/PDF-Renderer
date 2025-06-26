#include "pdf_types.h"
#include <stdexcept>
#include <sstream>
#include <iomanip>

namespace PDF {

bool PDFObject::asBoolean() const {
    if (type_ != ObjectType::BOOLEAN) {
        throw std::runtime_error("Object is not a boolean");
    }
    return std::get<bool>(value_);
}

int PDFObject::asInteger() const {
    if (type_ != ObjectType::INTEGER) {
        throw std::runtime_error("Object is not an integer");
    }
    return std::get<int>(value_);
}

double PDFObject::asReal() const {
    if (type_ != ObjectType::REAL) {
        throw std::runtime_error("Object is not a real number");
    }
    return std::get<double>(value_);
}

PDFString PDFObject::asString() const {
    if (type_ != ObjectType::STRING) {
        throw std::runtime_error("Object is not a string");
    }
    return std::get<PDFString>(value_);
}

PDFName PDFObject::asName() const {
    if (type_ != ObjectType::NAME) {
        throw std::runtime_error("Object is not a name");
    }
    return std::get<PDFName>(value_);
}

PDFArray PDFObject::asArray() const {
    if (type_ != ObjectType::ARRAY) {
        throw std::runtime_error("Object is not an array");
    }
    return std::get<PDFArray>(value_);
}

PDFDictionary PDFObject::asDictionary() const {
    if (type_ != ObjectType::DICTIONARY) {
        throw std::runtime_error("Object is not a dictionary");
    }
    return std::get<PDFDictionary>(value_);
}

PDFStream PDFObject::asStream() const {
    if (type_ != ObjectType::STREAM) {
        throw std::runtime_error("Object is not a stream");
    }
    return std::get<PDFStream>(value_);
}

PDFReference PDFObject::asReference() const {
    if (type_ != ObjectType::REFERENCE) {
        throw std::runtime_error("Object is not a reference");
    }
    return std::get<PDFReference>(value_);
}

bool PDFObject::hasKey(const std::string& key) const {
    if (type_ != ObjectType::DICTIONARY) {
        return false;
    }
    const auto& dict = std::get<PDFDictionary>(value_);
    return dict.find(key) != dict.end();
}

PDFObjectPtr PDFObject::getValue(const std::string& key) const {
    if (type_ != ObjectType::DICTIONARY) {
        throw std::runtime_error("Object is not a dictionary");
    }
    const auto& dict = std::get<PDFDictionary>(value_);
    auto it = dict.find(key);
    if (it == dict.end()) {
        throw std::runtime_error("Key not found in dictionary: " + key);
    }
    return it->second;
}

void PDFObject::setValue(const std::string& key, const PDFObjectPtr& value) {
    if (type_ != ObjectType::DICTIONARY) {
        throw std::runtime_error("Object is not a dictionary");
    }
    auto& dict = std::get<PDFDictionary>(value_);
    dict[key] = value;
}

size_t PDFObject::arraySize() const {
    if (type_ != ObjectType::ARRAY) {
        throw std::runtime_error("Object is not an array");
    }
    return std::get<PDFArray>(value_).size();
}

PDFObjectPtr PDFObject::arrayGet(size_t index) const {
    if (type_ != ObjectType::ARRAY) {
        throw std::runtime_error("Object is not an array");
    }
    const auto& array = std::get<PDFArray>(value_);
    if (index >= array.size()) {
        throw std::runtime_error("Array index out of bounds");
    }
    return array[index];
}

void PDFObject::arraySet(size_t index, const PDFObjectPtr& value) {
    if (type_ != ObjectType::ARRAY) {
        throw std::runtime_error("Object is not an array");
    }
    auto& array = std::get<PDFArray>(value_);
    if (index >= array.size()) {
        throw std::runtime_error("Array index out of bounds");
    }
    array[index] = value;
}

std::string PDFObject::toString() const {
    switch (type_) {
        case ObjectType::NULL_OBJECT:
            return "null";
        case ObjectType::BOOLEAN:
            return std::get<bool>(value_) ? "true" : "false";
        case ObjectType::INTEGER:
            return std::to_string(std::get<int>(value_));
        case ObjectType::REAL: {
            std::ostringstream oss;
            oss << std::get<double>(value_);
            return oss.str();
        }
        case ObjectType::STRING: {
            const auto& str = std::get<PDFString>(value_);
            if (str.isHex) {
                return "<" + str.value + ">";
            } else {
                return "(" + str.value + ")";
            }
        }
        case ObjectType::NAME:
            return "/" + std::get<PDFName>(value_).value;
        case ObjectType::ARRAY: {
            const auto& array = std::get<PDFArray>(value_);
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
            const auto& dict = std::get<PDFDictionary>(value_);
            std::ostringstream oss;
            oss << "<<";
            for (const auto& pair : dict) {
                oss << " /" << pair.first << " " << pair.second->toString();
            }
            oss << " >>";
            return oss.str();
        }
        case ObjectType::STREAM: {
            const auto& stream = std::get<PDFStream>(value_);
            std::ostringstream oss;
            oss << "<<";
            for (const auto& pair : stream.dict) {
                oss << " /" << pair.first << " " << pair.second->toString();
            }
            oss << " >>\nstream\n" << stream.data << "\nendstream";
            return oss.str();
        }
        case ObjectType::REFERENCE: {
            const auto& ref = std::get<PDFReference>(value_);
            std::ostringstream oss;
            oss << ref.objectNumber << " " << ref.generation << " R";
            return oss.str();
        }
        default:
            return "unknown";
    }
}

} // namespace PDF 