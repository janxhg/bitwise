cmake_minimum_required(VERSION 3.20)
project(BitwiseCompiler VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configuration options
option(BITWISE_BUILD_TESTS "Build tests for the Bitwise compiler" ON)

# Versioning header configuration
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/bitwise/version.hpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/include/bitwise/version.hpp
)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)

# Source files for the core library
set(BITWISE_CORE_SOURCES
    src/frontend/lexer.cpp
    src/frontend/parser.cpp
    src/frontend/sema.cpp
    src/bir/bir_gen.cpp
    src/backend/codegen.cpp
    src/backend/arm_backend.cpp
)

# Core library
add_library(bitwise_core STATIC ${BITWISE_CORE_SOURCES})
target_include_directories(bitwise_core PUBLIC include)

# Main executable
add_executable(bwc src/main.cpp)
target_link_libraries(bwc PRIVATE bitwise_core)

# Testing
if(BITWISE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Installation
include(GNUInstallDirs)

install(TARGETS bwc
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(TARGETS bitwise_core
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY include/bitwise
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
