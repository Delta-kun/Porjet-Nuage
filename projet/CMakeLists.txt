cmake_minimum_required (VERSION 2.8)
project (nuage)

include(CheckCXXCompilerFlag)

include_directories(${CMAKE_SOURCE_DIR}/lib/)

function(enable_cxx_compiler_flag_if_supported flag)
    string(FIND "${CMAKE_CXX_FLAGS}" "${flag}" flag_already_set)
    if(flag_already_set EQUAL -1)
        check_cxx_compiler_flag("${flag}" flag_supported)
        if(flag_supported)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}" PARENT_SCOPE)
        endif()
        unset(flag_supported CACHE)
    endif()
endfunction()

enable_cxx_compiler_flag_if_supported("-Wall")
enable_cxx_compiler_flag_if_supported("-Wextra")
enable_cxx_compiler_flag_if_supported("-pedantic")
enable_cxx_compiler_flag_if_supported("-std=c++14")
enable_cxx_compiler_flag_if_supported("-O3")
enable_cxx_compiler_flag_if_supported("-fopenmp")

file(GLOB SOURCES *.h *.cpp)
file(GLOB_RECURSE source_files "lib/*.cpp")
file(GLOB_RECURSE header_files "lib/*.h" "lib/*.hpp")
file(GLOB_RECURSE source_common "common/*.cpp")
file(GLOB_RECURSE header_common "common/*.h" "lib/*.hpp") 

add_executable(${PROJECT_NAME} ${SOURCES} ${source_files} ${header_files} ${source_common} ${header_common})