cmake_minimum_required(VERSION 3.10)
project(Namepicker VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    src/main.cpp
    src/namesorter.cpp
    src/constraints.cpp
    src/io_utils.cpp
)

# Create executable
add_executable(Namepicker ${SOURCES})

# Include directories
target_include_directories(Namepicker PRIVATE src)

# Link threading if needed
find_package(Threads REQUIRED)
target_link_libraries(NamePicker Threads::Threads)
