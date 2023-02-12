add_executable(app
    bitset_algebra.cpp
    bitset_algebra_test.cpp
    quine_mccluskey.cpp
    quine_mccluskey_test.cpp
    petrick.cpp
    petrick_test.cpp
    expression.cpp
    expression_rewrite.cpp
    expression_rewrite_test.cpp
    expression_dnf.cpp
    expression_dnf_test.cpp
)

target_link_libraries(app catch2)

list(APPEND INCLUDES ${CMAKE_SOURCE_DIR}/src)
list(APPEND INCLUDES ${CMAKE_SOURCE_DIR}/src/third_party)

target_include_directories(app PUBLIC ${INCLUDES})

set_property(TARGET app PROPERTY CXX_STANDARD 20)
