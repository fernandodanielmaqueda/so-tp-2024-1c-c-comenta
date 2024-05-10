# Filename: CMakeLists.txt / v2024.03.26-001, part of cc-CMake

#include_directories(BEFORE "${PROJECT_SOURCE_DIR}/build/${CMAKE_HOST_SYSTEM_NAME}/thirdparty/unity/include/unity")
add_compile_definitions("")
add_compile_options("")
add_link_options("")

# CTEST_BINARY_DIRECTORY
# CTEST_BUILD_COMMAND
# CTEST_BUILD_NAME

add_subdirectory(test1)

set(VALGRIND_FLAGS "")
set(VALGRIND_MEMCHECK_FLAGS "--leak-check=full")
set(VALGRIND_HELGRIND_FLAGS "")

find_program(VALGRIND valgrind)
  if(VALGRIND)
    message(STATUS "Valgrind found: ${VALGRIND}")

    function(add_valgrind_tool_for_target TOOL_NAME TARGET_NAME)
        string(TOUPPER ${TOOL_NAME} TOOL_NAME_UPPER)
        message(STATUS "Adding custom target: ${TOOL_NAME}-${TARGET_NAME}")
        add_custom_target("${TOOL_NAME}-${TARGET_NAME}"
            COMMAND "${VALGRIND}" --tool=${TOOL_NAME} ${VALGRIND_${TOOL_NAME_UPPER}_FLAGS} $<TARGET_FILE:${TARGET_NAME}>
            DEPENDS ${TARGET_NAME}
            COMMAND_EXPAND_LISTS
            VERBATIM
            USES_TERMINAL
            COMMENT "Using Valgrind with tool ${TOOL_NAME} on: ${TARGET_NAME}"
        )
        unset(TOOL_NAME_UPPER)
    endfunction()

    set(TARGET_NAMES ${EXECUTABLES_LIST})
    set(VALGRIND_TOOLS memcheck helgrind)

    foreach(TARGET_NAME ${TARGET_NAMES})
      foreach(TOOL_NAME ${VALGRIND_TOOLS})
        add_valgrind_tool_for_target(${TOOL_NAME} ${TARGET_NAME})
      endforeach()
    endforeach()

    unset(TARGET_NAMES)
    unset(VALGRIND_TOOLS)
    
  else()
    message(STATUS "Valgrind not found - Corresponding custom targets and tests won't be available")
  endif()

#include(ExternalProject)
#ExternalProject_Add(Unity
#  PREFIX ${CMAKE_BINARY_DIR}/thirdparty/unity
#  GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
#  GIT_TAG master
#  GIT_PROGRESS ON
#  CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=${PROJECT_SOURCE_DIR}/build/${CMAKE_HOST_SYSTEM_NAME}/thirdparty/unity"
#  CONFIGURE_HANDLED_BY_BUILD TRUE
#  BUILD_BYPRODUCTS ""
#  INSTALL_BYPRODUCTS ""
#  USES_TERMINAL_DOWNLOAD ON
#  EXCLUDE_FROM_ALL TRUE
#)