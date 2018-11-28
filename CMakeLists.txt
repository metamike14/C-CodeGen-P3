cmake_minimum_required(VERSION 3.8)
project(CodeGenP3)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES Project2.cpp LexicalAnalyzer.cpp SyntacticalAnalyzer.cpp)
add_executable(CodeGenP3 ${SOURCE_FILES})