add_executable(app quine_mccluskey.cpp quine_mccluskey_test.cpp)

target_link_libraries(app catch2)

list(APPEND INCLUDES ${CMAKE_SOURCE_DIR}/src)
list(APPEND INCLUDES ${CMAKE_SOURCE_DIR}/src/third_party)

target_include_directories(app PUBLIC ${INCLUDES})

set_property(TARGET app PROPERTY CXX_STANDARD 20)
