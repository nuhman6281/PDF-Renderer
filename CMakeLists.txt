cmake_minimum_required(VERSION 3.16)
project(PostScriptInterpreter VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set compiler flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")
endif()

# Include directories
include_directories(include)

# Source files
set(SOURCES
    src/main.cpp
    src/interpreter/ps_interpreter.cpp
    src/interpreter/ps_operators.cpp
    src/data_structures/ps_stack.cpp
    src/parser/ps_parser.cpp
    src/lexer/ps_lexer.cpp
)

# Create executable
add_executable(ps_interpreter ${SOURCES})

# Set output directory
set_target_properties(ps_interpreter PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Add test target
enable_testing()
add_executable(ps_tests tests/test_main.cpp)
target_link_libraries(ps_tests ps_interpreter)

# Install target
install(TARGETS ps_interpreter
    RUNTIME DESTINATION bin
) 