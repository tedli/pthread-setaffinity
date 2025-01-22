cmake_minimum_required(VERSION 3.5)

set(PROJECT_NAME pthread-setaffinity)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

project(${PROJECT_NAME})
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/include)
set(LIBC_OVERLAY_ARCHIVE_DIR "/opt/llvm/lib/x86_64-unknown-linux-gnu")

add_library(cpuset STATIC src/cpuset.c)
target_include_directories(cpuset PUBLIC ${INCLUDE_DIRS})
set_property(TARGET cpuset PROPERTY C_STANDARD 99)

find_package(Git REQUIRED)
execute_process(COMMAND "${GIT_EXECUTABLE}" rev-parse HEAD OUTPUT_VARIABLE COMMIT_ID OUTPUT_STRIP_TRAILING_WHITESPACE)

set(LIB_SRC_FILES src/${PROJECT_NAME}.c)
add_library(${PROJECT_NAME} SHARED ${LIB_SRC_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES IMPORTED_SONAME "lib${PROJECT_NAME}.so" POSITION_INDEPENDENT_CODE ON)
target_compile_definitions(${PROJECT_NAME} PRIVATE __USE_GNU _GNU_SOURCE COMMIT_ID="${COMMIT_ID}")
target_link_directories(${PROJECT_NAME} PRIVATE ${LIBC_OVERLAY_ARCHIVE_DIR})
target_link_options(${PROJECT_NAME} PRIVATE -l:libllvmlibc.a)
target_link_libraries(${PROJECT_NAME} dl cpuset)
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIRS})
set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 99)

set(SOURCE_FILES src/main.c)
add_executable(main ${SOURCE_FILES})
target_include_directories(main PUBLIC ${INCLUDE_DIRS})
target_link_directories(${PROJECT_NAME} PRIVATE ${LIBC_OVERLAY_ARCHIVE_DIR})
target_link_options(${PROJECT_NAME} PRIVATE -l:libllvmlibc.a)
target_link_libraries(main cpuset)
set_property(TARGET main PROPERTY C_STANDARD 99)
