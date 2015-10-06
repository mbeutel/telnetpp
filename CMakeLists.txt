project(telnetpp)
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
cmake_policy(VERSION 3.2)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")
include(cotire OPTIONAL)
find_package(CppUnit)
find_package(Boost)

add_library(telnetpp
    src/client_option.cpp
    src/command.cpp
    src/detail/generate_helper.cpp
    src/detail/parse_helper.cpp
    src/negotiation.cpp
    src/options/echo/client.cpp
    src/options/echo/server.cpp
    src/options/naws/server.cpp
    src/server_option.cpp
    src/subnegotiation.cpp
)

target_include_directories(telnetpp
    PUBLIC "${CMAKE_SOURCE_DIR}/include"
)

target_compile_features(telnetpp
    PUBLIC
        cxx_strong_enums
        cxx_relaxed_constexpr
)

if (COMMAND cotire)
    cotire(telnetpp)
endif()

enable_testing()

add_executable(telnetpp_tester 
    test/client_option_test.cpp
    test/echo_client_test.cpp
    test/echo_server_test.cpp
    test/expect_tokens.cpp
    test/generator_test.cpp
    test/main.cpp
    test/naws_server_test.cpp    
    test/parser_test.cpp
    test/server_option_test.cpp
)

target_compile_features(telnetpp_tester
    PUBLIC
        cxx_strong_enums
        cxx_relaxed_constexpr)

target_link_libraries(telnetpp_tester telnetpp cppunit)

# NOTE: Do not cotire() telnetpp_tester for now.  It causes Boost.Signals2
# to have various errors, and it appears to be difficult not to get it to
# be ignored in the pch.

add_test(telnetpp_test telnetpp_tester)

install(
    TARGETS
        telnetpp
    ARCHIVE DESTINATION
        lib
)

install(
    DIRECTORY
        include/
    DESTINATION
        include
)