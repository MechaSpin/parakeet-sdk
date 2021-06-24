cmake_minimum_required (VERSION 3.10)
project(SimpleExample VERSION 1.0.0)

find_package(Parakeet REQUIRED)

add_executable (${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC Parakeet::Parakeet ${PARAKEET_SOURCE_ROOT})
target_link_libraries (${PROJECT_NAME} LINK_PUBLIC Parakeet::Parakeet)
