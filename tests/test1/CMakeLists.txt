# Filename: CMakeLists.txt / v2024.03.26-001, part of cc-CMake

set(TEST1 test1)
add_executable(${TEST1})
file(GLOB_RECURSE TEST1_SOURCES "src/*.c")
target_sources(${TEST1} PRIVATE ${TEST1_SOURCES})
#add_dependencies(${TEST1} Unity)
target_compile_features(${TEST1} PUBLIC c_std_17)
# target_precompile_headers(${TEST1} PRIVATE)
target_include_directories(${TEST1} BEFORE PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
#target_link_directories(${TEST1} BEFORE PUBLIC "${PROJECT_SOURCE_DIR}/build/${CMAKE_HOST_SYSTEM_NAME}/thirdparty/unity/lib")
# target_compile_definitions(${TEST1} PUBLIC)
target_compile_options(${TEST1} PUBLIC "")
target_link_options(${TEST1} BEFORE PUBLIC "")
#target_link_libraries(${TEST1} PUBLIC "m" PUBLIC "unity" PRIVATE "")

add_test(
    NAME "Test1"
    COMMAND ${TEST1}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)