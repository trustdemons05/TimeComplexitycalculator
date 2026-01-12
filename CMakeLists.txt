cmake_minimum_required(VERSION 3.10)
project(ComplexityAnalyzer VERSION 6.0 LANGUAGES CXX)

# Set C++11 standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Check for C++11 support
if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|MSVC")
    message(FATAL_ERROR "Compiler ${CMAKE_CXX_COMPILER_ID} is not supported. Please use GCC, Clang, or MSVC.")
endif()

# Compiler warnings
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Generate compile_commands.json for IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Source files - explicitly defined (no globbing)
set(UTIL_SOURCES
    src/CodeUtils.cpp
    src/ComplexityUtils.cpp
)

set(ANALYZER_SOURCES
    src/Parser.cpp
    src/LoopAnalyzer.cpp
    src/RecursionAnalyzer.cpp
    src/PatternDetector.cpp
    src/ComplexityEstimator.cpp
    src/ResultComparator.cpp
)

set(PROFILER_SOURCES
    src/TestGenerator.cpp
    src/CompilerWrapper.cpp
    src/ProcessManager.cpp
    src/EmpiricalAnalyzer.cpp
    src/RuntimeProfiler.cpp
)

set(ALL_SOURCES
    ${UTIL_SOURCES}
    ${ANALYZER_SOURCES}
    ${PROFILER_SOURCES}
)

# Main executable
add_executable(complexity_analyzer
    main.cpp
    ${ALL_SOURCES}
)

# Set output directory
set_target_properties(complexity_analyzer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
)

# Platform-specific settings
if(WIN32)
    # Windows-specific settings
    target_compile_definitions(complexity_analyzer PRIVATE _WIN32)
elseif(UNIX AND NOT APPLE)
    # Linux-specific settings
    target_compile_definitions(complexity_analyzer PRIVATE __linux__)
elseif(APPLE)
    # macOS-specific settings
    target_compile_definitions(complexity_analyzer PRIVATE __APPLE__)
endif()

# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()

# Debug build settings
if(CMAKE_BUILD_TYPE MATCHES Debug)
    message(STATUS "Building in Debug mode")
    if(MSVC)
        add_compile_options(/Od /Zi)
    else()
        add_compile_options(-O0 -g)
    endif()
    target_compile_definitions(complexity_analyzer PRIVATE DEBUG)
endif()

# Release build settings
if(CMAKE_BUILD_TYPE MATCHES Release)
    message(STATUS "Building in Release mode")
    if(MSVC)
        add_compile_options(/O2)
    else()
        add_compile_options(-O2)
    endif()
endif()

# Print build information
message(STATUS "")
message(STATUS "=== C++ Complexity Analyzer v${PROJECT_VERSION} ===")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "Source directory: ${PROJECT_SOURCE_DIR}")
message(STATUS "Binary directory: ${PROJECT_BINARY_DIR}")
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "")

# Installation rules (optional)
install(TARGETS complexity_analyzer
    RUNTIME DESTINATION bin
)

# Test targets (optional - can be expanded later)
enable_testing()

# Custom targets
add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${PROJECT_SOURCE_DIR}/build
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${PROJECT_SOURCE_DIR}/bin
    COMMENT "Cleaning all build artifacts"
)

# Print success message
add_custom_command(TARGET complexity_analyzer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "========================================"
    COMMAND ${CMAKE_COMMAND} -E echo "  BUILD SUCCESSFUL!"
    COMMAND ${CMAKE_COMMAND} -E echo "  Executable: bin/complexity_analyzer"
    COMMAND ${CMAKE_COMMAND} -E echo "========================================"
    COMMAND ${CMAKE_COMMAND} -E echo ""
)
