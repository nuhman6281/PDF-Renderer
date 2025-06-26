#include "pdf_parser.h"
#include <iostream>
#include <sstream>
#include <cctype>
#include <stdexcept>
#include <regex>
#include <zlib.h>
#include <cstring>

namespace PDF {

// Forward declaration
std::string flate_decompress(const std::string& data);

// Helper function to parse xref stream dictionary
PDFDictionary parseXRefStreamDictionary(const std::string& dictStr) {
    PDFDictionary dict;
    
    // Parse simple key-value pairs (numeric values)
    std::regex entry_regex(R"(/(\w+)\s+(\d+))");
    std::smatch match;
    std::string::const_iterator searchStart(dictStr.cbegin());
    
    while (std::regex_search(searchStart, dictStr.cend(), match, entry_regex)) {
        std::string key = match[1].str();
        int value = std::stoi(match[2].str());
        dict[key] = std::make_shared<PDFObject>(value);
        searchStart = match.suffix().first;
    }
    
    // Parse name values (e.g., /Type /Pages or /Type/Page)
    std::regex name_regex(R"(/(\w+)\s*/(\w+))");
    searchStart = dictStr.cbegin();
    while (std::regex_search(searchStart, dictStr.cend(), match, name_regex)) {
        std::string key = match[1].str();
        std::string nameValue = match[2].str();
        dict[key] = std::make_shared<PDFObject>(PDFName(nameValue));
        searchStart = match.suffix().first;
    }
    
    // Parse references (e.g., "1 0 R")
    std::regex ref_regex(R"(/(\w+)\s+(\d+)\s+(\d+)\s+R)");
    searchStart = dictStr.cbegin();
    while (std::regex_search(searchStart, dictStr.cend(), match, ref_regex)) {
        std::string key = match[1].str();
        int objNum = std::stoi(match[2].str());
        int gen = std::stoi(match[3].str());
        dict[key] = std::make_shared<PDFObject>(PDFReference(objNum, gen));
        searchStart = match.suffix().first;
    }
    
    // Parse arrays with references (e.g., "/Kids [4 0 R]")
    std::regex array_ref_regex(R"(/(\w+)\s+\[([^\]]+)\])");
    searchStart = dictStr.cbegin();
    while (std::regex_search(searchStart, dictStr.cend(), match, array_ref_regex)) {
        std::string key = match[1].str();
        std::string arrayContent = match[2].str();
        PDFArray array;
        
        // Parse array content for references
        std::regex ref_in_array_regex(R"((\d+)\s+(\d+)\s+R)");
        std::string::const_iterator arraySearchStart(arrayContent.cbegin());
        while (std::regex_search(arraySearchStart, arrayContent.cend(), match, ref_in_array_regex)) {
            int objNum = std::stoi(match[1].str());
            int gen = std::stoi(match[2].str());
            array.push_back(std::make_shared<PDFObject>(PDFReference(objNum, gen)));
            arraySearchStart = match.suffix().first;
        }
        
        if (!array.empty()) {
            dict[key] = std::make_shared<PDFObject>(array);
        }
        searchStart = match.suffix().first;
        
        // Prevent infinite loop by checking if we've moved forward
        if (searchStart == match.suffix().first) {
            break;
        }
    }
    
    // Parse W array specifically
    std::regex w_regex(R"(/W\s+\[([^\]]+)\])");
    if (std::regex_search(dictStr, match, w_regex)) {
        std::string wStr = match[1].str();
        PDFArray wArray;
        std::regex num_regex(R"((\d+))");
        std::string::const_iterator searchStart(wStr.cbegin());
        
        while (std::regex_search(searchStart, wStr.cend(), match, num_regex)) {
            wArray.push_back(std::make_shared<PDFObject>(std::stoi(match[1].str())));
            searchStart = match.suffix().first;
        }
        dict["W"] = std::make_shared<PDFObject>(wArray);
    }
    
    // Parse Index array
    std::regex index_regex(R"(/Index\s+\[([^\]]+)\])");
    if (std::regex_search(dictStr, match, index_regex)) {
        std::string indexStr = match[1].str();
        PDFArray indexArray;
        std::regex pair_regex(R"((\d+)\s+(\d+))");
        std::string::const_iterator searchStart(indexStr.cbegin());
        
        while (std::regex_search(searchStart, indexStr.cend(), match, pair_regex)) {
            indexArray.push_back(std::make_shared<PDFObject>(std::stoi(match[1].str())));
            indexArray.push_back(std::make_shared<PDFObject>(std::stoi(match[2].str())));
            searchStart = match.suffix().first;
        }
        dict["Index"] = std::make_shared<PDFObject>(indexArray);
    }
    
    return dict;
}

// Helper function to parse Index array from xref stream dictionary
std::vector<std::pair<int, int>> parseIndexArray(const std::string& dictStr) {
    std::vector<std::pair<int, int>> index;
    std::regex index_regex(R"(/Index\s+\[([^\]]+)\])");
    std::smatch match;
    
    if (std::regex_search(dictStr, match, index_regex)) {
        std::string indexStr = match[1].str();
        std::regex pair_regex(R"((\d+)\s+(\d+))");
        std::string::const_iterator searchStart(indexStr.cbegin());
        
        while (std::regex_search(searchStart, indexStr.cend(), match, pair_regex)) {
            int first = std::stoi(match[1].str());
            int count = std::stoi(match[2].str());
            index.emplace_back(first, count);
            searchStart = match.suffix().first;
        }
    }
    
    return index;
}

// Helper function to parse W array from xref stream dictionary
std::vector<int> parseWArray(const std::string& dictStr) {
    std::vector<int> w;
    std::regex w_regex(R"(/W\s+\[([^\]]+)\])");
    std::smatch match;
    
    if (std::regex_search(dictStr, match, w_regex)) {
        std::string wStr = match[1].str();
        std::regex num_regex(R"((\d+))");
        std::string::const_iterator searchStart(wStr.cbegin());
        
        while (std::regex_search(searchStart, wStr.cend(), match, num_regex)) {
            w.push_back(std::stoi(match[1].str()));
            searchStart = match.suffix().first;
        }
    }
    
    return w;
}

// Helper function to read big-endian integers from binary data
uint32_t readBigEndian32(const std::string& data, size_t offset) {
    if (offset + 3 >= data.size()) return 0;
    return (static_cast<uint8_t>(data[offset]) << 24) |
           (static_cast<uint8_t>(data[offset + 1]) << 16) |
           (static_cast<uint8_t>(data[offset + 2]) << 8) |
           static_cast<uint8_t>(data[offset + 3]);
}

uint16_t readBigEndian16(const std::string& data, size_t offset) {
    if (offset + 1 >= data.size()) return 0;
    return (static_cast<uint8_t>(data[offset]) << 8) |
           static_cast<uint8_t>(data[offset + 1]);
}

uint8_t readBigEndian8(const std::string& data, size_t offset) {
    if (offset >= data.size()) return 0;
    return static_cast<uint8_t>(data[offset]);
}

// Parse binary xref stream data
std::map<int, size_t> parseXRefStreamData(const std::string& decompressedData, 
                                         const PDFDictionary& dict,
                                         const std::vector<std::pair<int, int>>& index) {
    std::map<int, size_t> objectOffsets;
    
    // Get W array (field widths)
    auto wIt = dict.find("W");
    if (wIt == dict.end()) {
        std::cout << "[DEBUG] No W array found in xref stream dictionary" << std::endl;
        return objectOffsets;
    }
    
    if (!wIt->second->isArray()) {
        std::cout << "[DEBUG] W is not an array" << std::endl;
        return objectOffsets;
    }
    
    PDFArray wArray = wIt->second->asArray();
    if (wArray.size() < 3) {
        std::cout << "[DEBUG] W array too small: " << wArray.size() << " elements" << std::endl;
        return objectOffsets;
    }
    
    int typeWidth = wArray[0]->asInteger();
    int field1Width = wArray[1]->asInteger();
    int field2Width = wArray[2]->asInteger();
    
    std::cout << "[DEBUG] W array: type=" << typeWidth << ", field1=" << field1Width << ", field2=" << field2Width << std::endl;
    
    size_t dataOffset = 0;
    
    for (const auto& [firstObj, count] : index) {
        std::cout << "[DEBUG] Processing index entry: firstObj=" << firstObj << ", count=" << count << std::endl;
        
        for (int i = 0; i < count; ++i) {
            int objNum = firstObj + i;
            
            if (dataOffset >= decompressedData.size()) {
                std::cout << "[DEBUG] Reached end of xref stream data" << std::endl;
                break;
            }
            
            // Read type field
            int type = 0;
            if (typeWidth == 1) {
                type = readBigEndian8(decompressedData, dataOffset);
            } else if (typeWidth == 2) {
                type = readBigEndian16(decompressedData, dataOffset);
            } else if (typeWidth == 4) {
                type = readBigEndian32(decompressedData, dataOffset);
            }
            dataOffset += typeWidth;
            
            // Read field1 (offset for type 1, object number for type 2)
            uint32_t field1 = 0;
            if (field1Width == 1) {
                field1 = readBigEndian8(decompressedData, dataOffset);
            } else if (field1Width == 2) {
                field1 = readBigEndian16(decompressedData, dataOffset);
            } else if (field1Width == 3) {
                // 3-byte big-endian integer
                field1 = (static_cast<uint8_t>(decompressedData[dataOffset]) << 16) |
                         (static_cast<uint8_t>(decompressedData[dataOffset + 1]) << 8) |
                         static_cast<uint8_t>(decompressedData[dataOffset + 2]);
            } else if (field1Width == 4) {
                field1 = readBigEndian32(decompressedData, dataOffset);
            }
            dataOffset += field1Width;
            
            // Read field2 (generation for type 1, offset for type 2)
            uint32_t field2 = 0;
            if (field2Width == 1) {
                field2 = readBigEndian8(decompressedData, dataOffset);
            } else if (field2Width == 2) {
                field2 = readBigEndian16(decompressedData, dataOffset);
            } else if (field2Width == 3) {
                field2 = (static_cast<uint8_t>(decompressedData[dataOffset]) << 16) |
                         (static_cast<uint8_t>(decompressedData[dataOffset + 1]) << 8) |
                         static_cast<uint8_t>(decompressedData[dataOffset + 2]);
            } else if (field2Width == 4) {
                field2 = readBigEndian32(decompressedData, dataOffset);
            }
            dataOffset += field2Width;
            
            std::cout << "[DEBUG] Object " << objNum << ": type=" << type << ", field1=" << field1 << ", field2=" << field2 << std::endl;
            
            // Type 1: free object, Type 2: in-use object
            if (type == 1) {
                // Free object, skip
                std::cout << "[DEBUG] Free object " << objNum << std::endl;
            } else if (type == 2) {
                // In-use object, field1 is offset
                objectOffsets[objNum] = field1;
                std::cout << "[DEBUG] In-use object " << objNum << " at offset " << field1 << std::endl;
            }
        }
    }
    
    return objectOffsets;
}

// Parse indirect object at given offset
PDFObjectPtr parseIndirectObjectAtOffset(const std::string& content, size_t offset) {
    if (offset >= content.size()) {
        return std::make_shared<PDFObject>();
    }
    
    // Find object boundaries
    size_t obj_start = content.find("obj", offset);
    if (obj_start == std::string::npos) {
        std::cout << "[DEBUG] No 'obj' found at offset " << offset << std::endl;
        return std::make_shared<PDFObject>();
    }
    
    // Find the start of the object (before "obj")
    size_t obj_header_start = offset;
    while (obj_header_start < obj_start && isspace(content[obj_header_start])) {
        obj_header_start++;
    }
    
    // Find object end
    size_t obj_end = content.find("endobj", obj_start);
    if (obj_end == std::string::npos) {
        std::cout << "[DEBUG] No 'endobj' found for object at offset " << offset << std::endl;
        return std::make_shared<PDFObject>();
    }
    
    // Extract object content
    std::string obj_content = content.substr(obj_start + 3, obj_end - obj_start - 3);
    
    // Skip whitespace after "obj"
    size_t content_start = 0;
    while (content_start < obj_content.size() && isspace(obj_content[content_start])) {
        content_start++;
    }
    
    // Check if it's a stream object
    size_t stream_pos = obj_content.find("stream", content_start);
    if (stream_pos != std::string::npos) {
        // Parse stream object
        std::string dict_str = obj_content.substr(content_start, stream_pos - content_start);
        size_t stream_data_start = stream_pos + 6;
        
        // Skip newlines after "stream"
        while (stream_data_start < obj_content.size() && 
               (obj_content[stream_data_start] == '\r' || obj_content[stream_data_start] == '\n')) {
            stream_data_start++;
        }
        
        size_t stream_data_end = obj_content.find("endstream", stream_data_start);
        if (stream_data_end == std::string::npos) {
            std::cout << "[DEBUG] No 'endstream' found in stream object" << std::endl;
            return std::make_shared<PDFObject>();
        }
        
        std::string stream_data = obj_content.substr(stream_data_start, stream_data_end - stream_data_start);
        
        // Parse dictionary (simplified)
        PDFDictionary dict = parseXRefStreamDictionary(dict_str);
        std::cout << "[DEBUG] Stream object with " << stream_data.length() << " bytes of data" << std::endl;
        
        PDFStream stream(dict, stream_data);
        return std::make_shared<PDFObject>(stream);
    } else {
        // Parse regular object content
        std::string obj_str = obj_content.substr(content_start);
        std::cout << "[DEBUG] Regular object: " << obj_str.substr(0, 100) << "..." << std::endl;
        
        // Try to parse as different object types
        std::string trimmed = obj_str;
        // Remove leading/trailing whitespace
        trimmed.erase(0, trimmed.find_first_not_of(" \t\r\n"));
        trimmed.erase(trimmed.find_last_not_of(" \t\r\n") + 1);
        
        if (trimmed.empty()) {
            return std::make_shared<PDFObject>();
        }
        
        // Check if it's a dictionary (starts with <<)
        if (trimmed.substr(0, 2) == "<<") {
            std::cout << "[DEBUG] Parsing as dictionary: " << trimmed.substr(0, 100) << "..." << std::endl;
            PDFDictionary dict = parseXRefStreamDictionary(trimmed);
            std::cout << "[DEBUG] Dictionary parsed with " << dict.size() << " entries" << std::endl;
            return std::make_shared<PDFObject>(dict);
        }
        
        // Check if it's an array (starts with [)
        if (trimmed[0] == '[') {
            // Simple array parsing - just return as string for now
            return std::make_shared<PDFObject>(PDFString(trimmed));
        }
        
        // Check if it's a number
        if (std::isdigit(trimmed[0]) || trimmed[0] == '-' || trimmed[0] == '+') {
            try {
                if (trimmed.find('.') != std::string::npos) {
                    double value = std::stod(trimmed);
                    return std::make_shared<PDFObject>(value);
                } else {
                    int value = std::stoi(trimmed);
                    return std::make_shared<PDFObject>(value);
                }
            } catch (...) {
                // Not a number, continue
            }
        }
        
        // Check if it's a boolean
        if (trimmed == "true") {
            return std::make_shared<PDFObject>(true);
        }
        if (trimmed == "false") {
            return std::make_shared<PDFObject>(false);
        }
        
        // Check if it's null
        if (trimmed == "null") {
            return std::make_shared<PDFObject>();
        }
        
        // Check if it's a name (starts with /)
        if (trimmed[0] == '/') {
            return std::make_shared<PDFObject>(PDFName(trimmed.substr(1)));
        }
        
        // Check if it's a reference (ends with R)
        if (trimmed.back() == 'R') {
            std::regex ref_regex(R"((\d+)\s+(\d+)\s+R)");
            std::smatch match;
            if (std::regex_match(trimmed, match, ref_regex)) {
                int objNum = std::stoi(match[1].str());
                int gen = std::stoi(match[2].str());
                return std::make_shared<PDFObject>(PDFReference(objNum, gen));
            }
        }
        
        // Default: treat as string
        return std::make_shared<PDFObject>(PDFString(trimmed));
    }
}

PDFParser::PDFParser() {
    // Constructor
}

PDFDocument PDFParser::parseFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open PDF file: " + filename);
    }

    PDFDocument document;
    try {
        std::cout << "[DEBUG] Parsing PDF file: " << filename << std::endl;
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        std::cout << "[DEBUG] File size: " << content.length() << " bytes" << std::endl;
        if (content.substr(0, 4) != "%PDF") {
            throw std::runtime_error("Invalid PDF file: missing PDF header");
        }
        std::cout << "[DEBUG] PDF header found" << std::endl;

        // --- Locate startxref ---
        size_t startxref_pos = content.rfind("startxref");
        if (startxref_pos == std::string::npos) {
            throw std::runtime_error("No startxref found in PDF");
        }
        size_t xref_offset_start = startxref_pos + 9;
        while (xref_offset_start < content.size() && isspace(content[xref_offset_start])) ++xref_offset_start;
        size_t xref_offset_end = xref_offset_start;
        while (xref_offset_end < content.size() && isdigit(content[xref_offset_end])) ++xref_offset_end;
        std::string xref_offset_str = content.substr(xref_offset_start, xref_offset_end - xref_offset_start);
        size_t xref_offset = std::stoul(xref_offset_str);
        std::cout << "[DEBUG] startxref at offset: " << xref_offset << std::endl;

        // --- Parse xref table or xref stream ---
        size_t xref_table_pos = content.find("xref", xref_offset);
        bool is_xref_stream = false;
        std::map<int, size_t> objectOffsets;
        
        // Check if the offset points to an indirect object (xref stream)
        std::regex obj_regex(R"((\d+) (\d+) obj[\s\S]*?stream[\s\S]*?endstream[\s\S]*?endobj)");
        std::smatch match;
        std::string xref_area = content.substr(xref_offset, std::min(size_t(1000), content.size() - xref_offset));
        if (std::regex_search(xref_area, match, obj_regex)) {
            is_xref_stream = true;
            std::cout << "[DEBUG] Detected xref stream at offset: " << xref_offset << std::endl;
            std::string xref_obj_str = match.str(0);
            std::cout << "[DEBUG] xref stream object (truncated):\n" << xref_obj_str.substr(0, 200) << "..." << std::endl;
            
            // Extract stream dictionary
            size_t dict_start = xref_obj_str.find("<<");
            size_t dict_end = xref_obj_str.find(">>", dict_start);
            std::string dict_str = xref_obj_str.substr(dict_start, dict_end + 2 - dict_start);
            std::cout << "[DEBUG] xref stream dictionary: " << dict_str << std::endl;
            
            // Parse xref stream dictionary
            PDFDictionary xrefDict = parseXRefStreamDictionary(dict_str);
            std::vector<std::pair<int, int>> index;
            
            // Parse Index array from dictionary
            auto indexIt = xrefDict.find("Index");
            if (indexIt != xrefDict.end() && indexIt->second->isArray()) {
                PDFArray indexArray = indexIt->second->asArray();
                for (size_t i = 0; i < indexArray.size(); i += 2) {
                    if (i + 1 < indexArray.size()) {
                        int first = indexArray[i]->asInteger();
                        int count = indexArray[i + 1]->asInteger();
                        index.emplace_back(first, count);
                    }
                }
            } else {
                // If no Index, default to [0 Size]
                auto sizeIt = xrefDict.find("Size");
                if (sizeIt != xrefDict.end() && sizeIt->second->isInteger()) {
                    int size = sizeIt->second->asInteger();
                    index.emplace_back(0, size);
                }
            }
            
            std::vector<int> wArray = parseWArray(dict_str);
            
            std::cout << "[DEBUG] Parsed " << index.size() << " index entries" << std::endl;
            std::cout << "[DEBUG] W array has " << wArray.size() << " elements" << std::endl;
            
            // Extract stream data
            size_t stream_start = xref_obj_str.find("stream", dict_end) + 6;
            size_t stream_end = xref_obj_str.find("endstream", stream_start);
            std::string stream_data = xref_obj_str.substr(stream_start, stream_end - stream_start);
            // Remove leading newlines/spaces
            stream_data.erase(0, stream_data.find_first_not_of("\r\n "));
            std::cout << "[DEBUG] xref stream data length: " << stream_data.length() << std::endl;
            
            // Check for /Filter /FlateDecode
            if (dict_str.find("/Filter /FlateDecode") != std::string::npos) {
                std::cout << "[DEBUG] Decompressing xref stream with FlateDecode..." << std::endl;
                try {
                    std::string decompressed = flate_decompress(stream_data);
                    std::cout << "[DEBUG] Decompressed xref stream length: " << decompressed.length() << std::endl;
                    std::cout << "[DEBUG] First 32 bytes: ";
                    for (size_t i = 0; i < std::min<size_t>(32, decompressed.size()); ++i) {
                        printf("%02X ", (unsigned char)decompressed[i]);
                    }
                    std::cout << std::endl;
                    
                    // Parse binary xref data with enhanced debug
                    objectOffsets.clear();
                    // Get W array (field widths)
                    auto wIt = xrefDict.find("W");
                    if (wIt == xrefDict.end() || !wIt->second->isArray()) {
                        std::cout << "[DEBUG] No W array found in xref stream dictionary" << std::endl;
                    } else {
                        PDFArray wArrayObj = wIt->second->asArray();
                        int typeWidth = wArrayObj[0]->asInteger();
                        int field1Width = wArrayObj[1]->asInteger();
                        int field2Width = wArrayObj[2]->asInteger();
                        size_t dataOffset = 0;
                        for (const auto& [firstObj, count] : index) {
                            for (int i = 0; i < count; ++i) {
                                int objNum = firstObj + i;
                                // Print raw bytes for this entry
                                std::cout << "[DEBUG] Entry bytes for object " << objNum << ": ";
                                for (int b = 0; b < typeWidth + field1Width + field2Width; ++b) {
                                    if (dataOffset + b < decompressed.size())
                                        printf("%02X ", (unsigned char)decompressed[dataOffset + b]);
                                }
                                std::cout << std::endl;
                                // Read type
                                int type = 0;
                                if (typeWidth == 1) type = readBigEndian8(decompressed, dataOffset);
                                else if (typeWidth == 2) type = readBigEndian16(decompressed, dataOffset);
                                else if (typeWidth == 4) type = readBigEndian32(decompressed, dataOffset);
                                dataOffset += typeWidth;
                                // Read field1 (offset for type 1, object stream obj num for type 2)
                                uint32_t field1 = 0;
                                if (field1Width == 1) field1 = readBigEndian8(decompressed, dataOffset);
                                else if (field1Width == 2) field1 = readBigEndian16(decompressed, dataOffset);
                                else if (field1Width == 3) {
                                    // 3-byte big-endian integer
                                    field1 = (static_cast<uint8_t>(decompressed[dataOffset]) << 16) |
                                             (static_cast<uint8_t>(decompressed[dataOffset + 1]) << 8) |
                                             static_cast<uint8_t>(decompressed[dataOffset + 2]);
                                } else if (field1Width == 4) field1 = readBigEndian32(decompressed, dataOffset);
                                dataOffset += field1Width;
                                // Read field2 (generation for type 1, index in object stream for type 2)
                                uint32_t field2 = 0;
                                if (field2Width == 1) field2 = readBigEndian8(decompressed, dataOffset);
                                else if (field2Width == 2) field2 = readBigEndian16(decompressed, dataOffset);
                                else if (field2Width == 3) {
                                    field2 = (static_cast<uint8_t>(decompressed[dataOffset]) << 16) |
                                             (static_cast<uint8_t>(decompressed[dataOffset + 1]) << 8) |
                                             static_cast<uint8_t>(decompressed[dataOffset + 2]);
                                } else if (field2Width == 4) field2 = readBigEndian32(decompressed, dataOffset);
                                dataOffset += field2Width;
                                // PDF spec: type 0 = free, type 1 = in-use (uncompressed), type 2 = compressed
                                std::cout << "[DEBUG] Object " << objNum << ": type=" << type << ", field1=" << field1 << ", field2=" << field2 << std::endl;
                                if (type == 0) {
                                    // Free object
                                    std::cout << "[DEBUG] Free object " << objNum << std::endl;
                                } else if (type == 1) {
                                    // In-use, field1 = offset, field2 = generation
                                    objectOffsets[objNum] = field1;
                                    std::cout << "[DEBUG] In-use object " << objNum << " at offset " << field1 << ", generation " << field2 << std::endl;
                                } else if (type == 2) {
                                    // Compressed object, skip for now
                                    std::cout << "[DEBUG] Compressed object " << objNum << " in object stream " << field1 << ", index " << field2 << std::endl;
                                } else {
                                    std::cout << "[DEBUG] Unknown xref entry type: " << type << std::endl;
                                }
                            }
                        }
                    }
                    std::cout << "[DEBUG] Extracted " << objectOffsets.size() << " object offsets from xref stream" << std::endl;
                    
                    // Use the xref stream dictionary as the trailer
                    document.trailer = xrefDict;
                    // Extract root object number
                    auto rootIt = document.trailer.find("Root");
                    if (rootIt != document.trailer.end() && rootIt->second->isReference()) {
                        document.rootObjectNumber = rootIt->second->asReference().objectNumber;
                        std::cout << "[DEBUG] Root object number: " << document.rootObjectNumber << std::endl;
                    }
                } catch (const std::exception& e) {
                    std::cout << "[DEBUG] FlateDecode failed: " << e.what() << std::endl;
                }
            }
        }
        
        if (!is_xref_stream) {
            // Fallback to classic xref parsing
            if (xref_table_pos == std::string::npos) {
                throw std::runtime_error("xref table not found at expected offset");
            }
            std::cout << "[DEBUG] xref table found at: " << xref_table_pos << std::endl;
            
            // Debug: Show content around xref table
            size_t debug_start = std::max(xref_table_pos - 20, size_t(0));
            size_t debug_end = std::min(xref_table_pos + 200, content.size());
            std::string debug_content = content.substr(debug_start, debug_end - debug_start);
            std::cout << "[DEBUG] Content around xref table:" << std::endl;
            std::cout << debug_content << std::endl;
            std::cout << "[DEBUG] End of debug content" << std::endl;

            // Parse xref entries
            size_t line_start = content.find("\n", xref_table_pos) + 1;
            if (line_start == std::string::npos) line_start = content.find("\r\n", xref_table_pos) + 2;
            std::cout << "[DEBUG] Starting xref parsing at position: " << line_start << std::endl;
            
            while (line_start < content.size()) {
                // Read section header (e.g., "0 6")
                size_t line_end = content.find("\n", line_start);
                if (line_end == std::string::npos) line_end = content.find("\r\n", line_start);
                if (line_end == std::string::npos) break;
                
                std::string line = content.substr(line_start, line_end - line_start);
                std::cout << "[DEBUG] xref line: '" << line << "'" << std::endl;
                
                if (line.find("trailer") != std::string::npos) {
                    std::cout << "[DEBUG] Found trailer, stopping xref parsing" << std::endl;
                    break;
                }
                
                std::istringstream iss(line);
                int firstObj = 0, count = 0;
                if (!(iss >> firstObj >> count)) {
                    std::cout << "[DEBUG] Skipping non-section-header line" << std::endl;
                    line_start = line_end + 1;
                    continue;
                }
                
                std::cout << "[DEBUG] xref section: firstObj=" << firstObj << ", count=" << count << std::endl;
                
                // Read count lines of xref entries
                for (int i = 0; i < count; ++i) {
                    line_start = line_end + 1;
                    line_end = content.find("\n", line_start);
                    if (line_end == std::string::npos) line_end = content.find("\r\n", line_start);
                    if (line_end == std::string::npos) break;
                    
                    std::string entry = content.substr(line_start, line_end - line_start);
                    std::cout << "[DEBUG] xref entry " << (firstObj + i) << ": '" << entry << "'" << std::endl;
                    
                    if (entry.length() < 18) {
                        std::cout << "[DEBUG] Entry too short, skipping" << std::endl;
                        continue;
                    }
                    
                    try {
                        size_t offset = std::stoul(entry.substr(0, 10));
                        char inuse = entry[17];
                        std::cout << "[DEBUG] Entry " << (firstObj + i) << ": offset=" << offset << ", inuse='" << inuse << "'" << std::endl;
                        if (inuse == 'n') {
                            objectOffsets[firstObj + i] = offset;
                            std::cout << "[DEBUG] Added object " << (firstObj + i) << " at offset " << offset << std::endl;
                        }
                    } catch (const std::exception& e) {
                        std::cout << "[DEBUG] Failed to parse entry: " << e.what() << std::endl;
                    }
                }
                
                line_start = line_end + 1;
            }
            std::cout << "[DEBUG] Parsed " << objectOffsets.size() << " xref entries" << std::endl;

            // --- Parse trailer ---
            size_t trailer_pos = content.find("trailer", xref_table_pos);
            if (trailer_pos == std::string::npos) {
                // Try alternative trailer location
                trailer_pos = content.find("trailer", line_start);
                if (trailer_pos == std::string::npos) {
                    throw std::runtime_error("trailer not found after xref");
                }
            }
            std::cout << "[DEBUG] trailer found at: " << trailer_pos << std::endl;
            
            size_t trailer_dict_start = content.find("<<", trailer_pos);
            if (trailer_dict_start == std::string::npos) {
                throw std::runtime_error("trailer dictionary not found");
            }
            size_t trailer_dict_end = content.find(">>", trailer_dict_start) + 2;
            std::string trailer_dict_str = content.substr(trailer_dict_start, trailer_dict_end - trailer_dict_start);
            std::cout << "[DEBUG] Trailer dictionary: " << trailer_dict_str << std::endl;
            // (Parsing trailer dictionary into PDFDictionary is left as a future step)

            // --- Parse indirect objects ---
            for (const auto& [objNum, offset] : objectOffsets) {
                // Find the object header at the offset
                size_t obj_header_pos = content.find("obj", offset);
                if (obj_header_pos == std::string::npos) continue;
                size_t obj_start = content.rfind("\n", obj_header_pos);
                if (obj_start == std::string::npos) obj_start = offset;
                size_t obj_end = content.find("endobj", obj_header_pos);
                if (obj_end == std::string::npos) continue;
                std::string obj_str = content.substr(obj_start, obj_end + 6 - obj_start);
                std::cout << "[DEBUG] Object " << objNum << ": " << obj_str.substr(0, 60) << "..." << std::endl;
                // (Parsing object content into PDFObject is left as a future step)
            }
        }

        // --- Parse indirect objects using extracted offsets ---
        std::cout << "[DEBUG] Parsing " << objectOffsets.size() << " indirect objects..." << std::endl;
        for (const auto& [objNum, offset] : objectOffsets) {
            std::cout << "[DEBUG] Parsing object " << objNum << " at offset " << offset << std::endl;
            PDFObjectPtr obj = parseIndirectObjectAtOffset(content, offset);
            document.objects[objNum] = obj;
        }

        // --- Page tree and content stream extraction ---
        std::cout << "[NEXT] Extracting page tree and content streams from parsed objects." << std::endl;

        // Helper lambda to resolve references
        auto resolve = [&](const PDFObjectPtr& obj) -> PDFObjectPtr {
            if (!obj) return nullptr;
            if (obj->isReference()) {
                int refNum = obj->asReference().objectNumber;
                auto it = document.objects.find(refNum);
                if (it != document.objects.end()) return it->second;
                return nullptr;
            }
            return obj;
        };

        // 1. Find the root object (Catalog)
        PDFObjectPtr rootObj = nullptr;
        if (document.rootObjectNumber > 0) {
            auto it = document.objects.find(document.rootObjectNumber);
            if (it != document.objects.end()) rootObj = it->second;
        }
        if (!rootObj) {
            std::cout << "[ERROR] Could not find root (Catalog) object." << std::endl;
            return document;
        }
        std::cout << "[PAGE] Found Catalog object (" << document.rootObjectNumber << ")" << std::endl;

        // 2. Find the Pages tree
        PDFObjectPtr pagesObj = nullptr;
        if (rootObj->isString()) {
            std::cout << "[ERROR] Root object is a string, not a dictionary." << std::endl;
            return document;
        }
        if (rootObj->isDictionary()) {
            auto dict = rootObj->asDictionary();
            auto it = dict.find("Pages");
            if (it != dict.end()) {
                pagesObj = resolve(it->second);
            }
        }
        if (!pagesObj) {
            std::cout << "[ERROR] Could not find Pages tree from Catalog." << std::endl;
            return document;
        }
        std::cout << "[PAGE] Found Pages tree object." << std::endl;

        // 3. Traverse the page tree to find all page objects
        std::vector<PDFObjectPtr> pageObjects;
        std::function<void(PDFObjectPtr)> traversePages;
        traversePages = [&](PDFObjectPtr node) {
            if (!node) {
                std::cout << "[PAGE] Node is null" << std::endl;
                return;
            }
            std::cout << "[PAGE] Traversing node of type: " << static_cast<int>(node->getType()) << std::endl;
            if (node->isDictionary()) {
                auto dict = node->asDictionary();
                std::cout << "[PAGE] Dictionary has " << dict.size() << " keys: ";
                for (const auto& pair : dict) {
                    std::cout << pair.first << " ";
                }
                std::cout << std::endl;
                auto typeIt = dict.find("Type");
                if (typeIt != dict.end()) {
                    PDFObjectPtr typeObj = resolve(typeIt->second);
                    if (typeObj && typeObj->isName()) {
                        std::string typeName = typeObj->asName().value;
                        std::cout << "[PAGE] Node type: " << typeName << std::endl;
                        if (typeName == "Page") {
                            // This is a page
                            pageObjects.push_back(node);
                            std::cout << "[PAGE] Found page object" << std::endl;
                            return;
                        } else if (typeName == "Pages") {
                            // If it's a Pages node, traverse Kids
                            auto kidsIt = dict.find("Kids");
                            if (kidsIt != dict.end()) {
                                PDFObjectPtr kidsObj = resolve(kidsIt->second);
                                if (kidsObj && kidsObj->isArray()) {
                                    PDFArray kids = kidsObj->asArray();
                                    std::cout << "[PAGE] Pages node has " << kids.size() << " kids" << std::endl;
                                    for (size_t i = 0; i < kids.size(); ++i) {
                                        std::cout << "[PAGE] Traversing kid " << i << std::endl;
                                        traversePages(resolve(kids[i]));
                                    }
                                } else {
                                    std::cout << "[PAGE] Kids is not an array" << std::endl;
                                }
                            } else {
                                std::cout << "[PAGE] Pages node has no Kids" << std::endl;
                            }
                        }
                    } else {
                        std::cout << "[PAGE] Type is not a name" << std::endl;
                    }
                } else {
                    std::cout << "[PAGE] Node has no Type" << std::endl;
                }
            } else {
                std::cout << "[PAGE] Node is not a dictionary" << std::endl;
            }
        };
        traversePages(pagesObj);
        std::cout << "[PAGE] Found " << pageObjects.size() << " page(s) in the document." << std::endl;

        // 4. Extract content streams for each page
        for (size_t i = 0; i < pageObjects.size(); ++i) {
            auto page = pageObjects[i];
            if (!page || !page->isDictionary()) continue;
            auto dict = page->asDictionary();
            auto contentIt = dict.find("Contents");
            if (contentIt == dict.end()) {
                std::cout << "[PAGE] Page " << i << " has no Contents." << std::endl;
                continue;
            }
            PDFObjectPtr contentsObj = resolve(contentIt->second);
            if (!contentsObj) {
                std::cout << "[PAGE] Page " << i << " Contents reference could not be resolved." << std::endl;
                continue;
            }
            if (contentsObj->isStream()) {
                auto stream = contentsObj->asStream();
                std::cout << "[PAGE] Page " << i << " has a content stream of length " << stream.data.size() << "." << std::endl;
            } else if (contentsObj->isArray()) {
                PDFArray arr = contentsObj->asArray();
                std::cout << "[PAGE] Page " << i << " has an array of " << arr.size() << " content streams." << std::endl;
                for (size_t j = 0; j < arr.size(); ++j) {
                    PDFObjectPtr streamObj = resolve(arr[j]);
                    if (streamObj && streamObj->isStream()) {
                        auto stream = streamObj->asStream();
                        std::cout << "[PAGE]   Content stream " << j << " length: " << stream.data.size() << std::endl;
                    } else {
                        std::cout << "[PAGE]   Content stream " << j << " is not a stream object." << std::endl;
                    }
                }
            } else {
                std::cout << "[PAGE] Page " << i << " Contents is not a stream or array." << std::endl;
            }
        }

    } catch (const std::exception& e) {
        throw std::runtime_error("Error parsing PDF file: " + std::string(e.what()));
    }
    return document;
}

PDFObjectPtr PDFParser::parseObject(const std::string& input) {
    std::istringstream stream(input);
    return parseObjectFromStream(stream);
}

std::vector<std::string> PDFParser::parseContentStream(const std::string& streamData) {
    std::vector<std::string> tokens;
    std::istringstream stream(streamData);
    std::string token;
    
    while (stream >> token) {
        // Skip comments
        if (token[0] == '%') {
            std::string line;
            std::getline(stream, line);
            continue;
        }
        
        tokens.push_back(token);
    }
    
    return tokens;
}

void PDFParser::skipWhitespace(std::ifstream& file) {
    char c;
    while (file.get(c)) {
        if (!isWhitespace(c)) {
            file.unget();
            break;
        }
    }
}

std::string PDFParser::readToken(std::ifstream& file) {
    std::string token;
    char c;
    
    skipWhitespace(file);
    
    while (file.get(c)) {
        if (isWhitespace(c) || isDelimiter(c)) {
            if (!token.empty()) {
                file.unget();
                break;
            }
            if (isDelimiter(c)) {
                token = c;
                break;
            }
        } else {
            token += c;
        }
    }
    
    return token;
}

bool PDFParser::isWhitespace(char c) const {
    return std::isspace(static_cast<unsigned char>(c));
}

bool PDFParser::isDelimiter(char c) const {
    return c == '(' || c == ')' || c == '<' || c == '>' || 
           c == '[' || c == ']' || c == '{' || c == '}' || 
           c == '/' || c == '%';
}

// Helper method for parsing objects from streams
PDFObjectPtr PDFParser::parseObjectFromStream(std::istream& stream) {
    // This is a simplified implementation
    // In a full implementation, we'd handle all PDF object types
    
    std::string token;
    stream >> token;
    
    if (token.empty()) {
        return std::make_shared<PDFObject>();
    }
    
    // Try to parse as number
    try {
        if (token.find('.') != std::string::npos) {
            double value = std::stod(token);
            return std::make_shared<PDFObject>(value);
        } else {
            int value = std::stoi(token);
            return std::make_shared<PDFObject>(value);
        }
    } catch (...) {
        // Not a number, continue to other types
    }
    
    // Try to parse as boolean
    if (token == "true") {
        return std::make_shared<PDFObject>(true);
    }
    if (token == "false") {
        return std::make_shared<PDFObject>(false);
    }
    
    // Try to parse as null
    if (token == "null") {
        return std::make_shared<PDFObject>();
    }
    
    // Try to parse as name
    if (token[0] == '/') {
        return std::make_shared<PDFObject>(PDFName(token.substr(1)));
    }
    
    // For now, treat as string
    return std::make_shared<PDFObject>(PDFString(token));
}

// Utility: FlateDecode decompression
std::string flate_decompress(const std::string& data) {
    if (data.empty()) return "";
    // Estimate output size (arbitrary, can be improved)
    size_t out_size = data.size() * 4 + 1024;
    std::string out(out_size, '\0');
    z_stream zs{};
    zs.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(data.data()));
    zs.avail_in = data.size();
    zs.next_out = reinterpret_cast<Bytef*>(&out[0]);
    zs.avail_out = out_size;
    if (inflateInit(&zs) != Z_OK) throw std::runtime_error("zlib inflateInit failed");
    int ret = inflate(&zs, Z_FINISH);
    if (ret != Z_STREAM_END && ret != Z_OK) {
        inflateEnd(&zs);
        throw std::runtime_error("zlib inflate failed");
    }
    out.resize(zs.total_out);
    inflateEnd(&zs);
    return out;
}

} // namespace PDF 