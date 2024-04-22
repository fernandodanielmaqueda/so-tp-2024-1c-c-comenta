# Filename: CMakeLists.txt / v2024.03.26-001, part of cc-CMake
# CMake files for building, executing, debugging, testing and packaging C language projects
# For more information and getting the most recent clean template, visit <https://github.com/fernandodanielmaqueda/cc-CMake>

# Copyright (C) 2024 Fernando Daniel Maqueda <https://github.com/fernandodanielmaqueda/>
# This makefile is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
# This makefile is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
# You should have received a copy of the GNU General Public License along with this. If not, see <https://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.25)

project(C_Comenta
  VERSION 0.1.1
  DESCRIPTION "TP Cuatrimestral (2024 - 1C) de la asignatura Sistemas Operativos (código 08-2027) en la UTN FRBA"
  LANGUAGES C
)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_EXTENSIONS ON) # OFF
set(CMAKE_C_STANDARD_REQUIRED TRUE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

# If set, runtime paths are not added when using shared libraries.
set(CMAKE_SKIP_RPATH OFF)
# If set, runtime paths are not added when installing shared libraries, but are added when building.
set(CMAKE_SKIP_INSTALL_RPATH OFF)

# include_directories(BEFORE ".")
add_compile_definitions("")
add_compile_options("")
add_link_options("")

set(EXECUTABLES_LIST "")
set(STATIC_LIBRARIES_LIST "")
set(SHARED_LIBRARIES_LIST "")
set(MODULE_LIBRARIES_LIST "")

add_subdirectory(src)

set(BUILD_TESTS ON)
if(BUILD_TESTS)

  enable_testing()
  include(CTest)

  add_subdirectory(tests) # EXCLUDE_FROM_ALL

endif()

add_custom_target(cleaning
  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target clean
  COMMENT "Cleaning build artifacts"
)

add_custom_target(cleaning-first
  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --clean-first
  COMMENT "Cleaning build artifacts first"
)