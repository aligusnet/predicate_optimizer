list(APPEND SOURCES
    bitset_algebra.cpp
    petrick.cpp expression.cpp
    expression_rewrite.cpp
    expression_dnf.cpp
    quine_mccluskey.cpp
    intervals_simplifier.cpp)

list(APPEND TEST_SOURCES
    bitset_algebra_test.cpp
    quine_mccluskey_test.cpp
    petrick_test.cpp
    expression_rewrite_test.cpp
    expression_dnf_test.cpp
    intervals_simplifier_test.cpp)

add_library(proptlib STATIC ${SOURCES})
add_executable(app ${TEST_SOURCES})

target_link_libraries(app catch2 proptlib)

list(APPEND INCLUDES ${CMAKE_SOURCE_DIR}/src)
list(APPEND INCLUDES ${CMAKE_SOURCE_DIR}/src/third_party)

target_include_directories(app PUBLIC ${INCLUDES})
target_include_directories(proptlib PUBLIC ${INCLUDES})

set_property(TARGET app PROPERTY CXX_STANDARD 20)
set_property(TARGET proptlib PROPERTY CXX_STANDARD 20)
