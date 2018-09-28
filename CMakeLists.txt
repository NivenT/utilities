cmake_minimum_required(VERSION 2.8.7)

# https://stackoverflow.com/questions/7787823/cmake-how-to-get-the-name-of-all-subdirectories-of-a-directory
MACRO(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
      LIST(APPEND dirlist ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()

# This is kinda trash
MACRO(COMPLEMENT_FILE_LIST result curdir glob1)
  file(GLOB_RECURSE all RELATIVE ${curdir} ${curdir}/*)
  set(complement "")
  set(globs ${glob1} ${ARGN})
  foreach(glob ${globs})
    file(GLOB_RECURSE temp RELATIVE ${curdir} ${glob})
    list(APPEND complement ${temp})
  endforeach(glob)
  list(REMOVE_ITEM all ${complement})
  set(${result} ${all})
ENDMACRO()

# Allow use of project folders for IDEs like Visual Studio, so we
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

project(utilities)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (UNIX)
  # I'm doing this instead of add_definitions so the parent directory can use these flags as well
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_UTILS_UNIX" CACHE INTERNAL "")
  if (APPLE)
    message(STATUS "Computer runs OS X")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_UTILS_APPLE" CACHE INTERNAL "")
  else()
    message(STATUS "Computer runs Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_UTILS_LINUX" CACHE INTERNAL "")
  endif()
else()
  message(STATUS "Computer runs Windows")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_UTILS_WINDOWS" CACHE INTERNAL "")
endif()

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)


add_library(common ${CMAKE_CURRENT_SOURCE_DIR}/common.cpp)

enable_testing()

subdirlist(DIRS ${CMAKE_CURRENT_SOURCE_DIR}/utils)
foreach(dir ${DIRS})
  message(STATUS "Setting up ${dir}")

  file(GLOB SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} utils/${dir}/*.cpp)
  if(SRCS)
    add_library(${dir} ${SRCS})
    target_include_directories(${dir} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
  endif()

  file(GLOB TEST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} utils/${dir}/tests/*.cpp)
  add_executable("${dir}_tests" ${TEST_SRCS})
  if (SRCS)
    target_link_libraries("${dir}_tests" ${dir} common)
  else()
    target_link_libraries("${dir}_tests" common)
  endif()
  target_include_directories("${dir}_tests" PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/utils/${dir})

  set_target_properties("${dir}_tests" PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tests_bin/${dir})
  add_test(NAME "${dir}_tests"
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tests_bin/${dir}
            COMMAND ${CMAKE_CURRENT_BINARY_DIR}/tests_bin/${dir}/${dir}_tests)

  complement_file_list(RSRCS ${CMAKE_CURRENT_SOURCE_DIR}/utils/${dir}/tests "*.cpp" "*.h")
  foreach(rsrc ${RSRCS})
    message(STATUS "Copying ${rsrc}")
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/utils/${dir}/tests/${rsrc}
      ${CMAKE_CURRENT_BINARY_DIR}/tests_bin/${dir}/${rsrc} COPYONLY)
  endforeach(rsrc)
endforeach(dir)
