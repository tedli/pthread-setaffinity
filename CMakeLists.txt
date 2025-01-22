cmake_minimum_required(VERSION 3.13)

set(PROJECT_NAME pthread-setaffinity)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

project(${PROJECT_NAME})
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/include)

add_library(cpuset STATIC src/cpuset.cc)
target_include_directories(cpuset PUBLIC ${INCLUDE_DIRS})
set_target_properties(cpuset PROPERTIES POSITION_INDEPENDENT_CODE ON)
set_property(TARGET cpuset PROPERTY CXX_STANDARD 17)

set(LIB_SRC_FILES src/${PROJECT_NAME}.cc)
add_library(${PROJECT_NAME} SHARED ${LIB_SRC_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES IMPORTED_SONAME "lib${PROJECT_NAME}.so" POSITION_INDEPENDENT_CODE ON)
target_compile_definitions(${PROJECT_NAME} PRIVATE __USE_GNU _GNU_SOURCE)
target_link_libraries(${PROJECT_NAME} dl cpuset)
target_link_options(${PROJECT_NAME} PRIVATE -static-libgcc -stdlib=libc++ -fuse-ld=lld)
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIRS})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

set(SOURCE_FILES src/main.cc)
add_executable(main ${SOURCE_FILES})
target_include_directories(main PUBLIC ${INCLUDE_DIRS})
target_link_libraries(main cpuset)
set_property(TARGET main PROPERTY CXX_STANDARD 17)
