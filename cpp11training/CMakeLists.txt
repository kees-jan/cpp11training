project(cpp11training)
cmake_minimum_required(VERSION 2.8.12)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_definitions("-std=c++17")

include("${CMAKE_CURRENT_SOURCE_DIR}/CMake/SetupGoogletest.cmake")
setup_cpp_project_with_googletest("cpp11training")


# add_executable(timer timer.cpp)
# target_link_libraries(timer ${CONAN_LIBS})