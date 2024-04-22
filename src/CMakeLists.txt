# Filename: CMakeLists.txt / v2024.03.26-001, part of cc-CMake

add_subdirectory(cpu)
add_subdirectory(entradasalida)
add_subdirectory(kernel)
add_subdirectory(memoria)
add_subdirectory(utils)

set(EXECUTABLES_LIST ${EXECUTABLES_LIST} PARENT_SCOPE)
set(STATIC_LIBRARIES_LIST ${STATIC_LIBRARIES_LIST} PARENT_SCOPE)
set(SHARED_LIBRARIES_LIST ${SHARED_LIBRARIES_LIST} PARENT_SCOPE)