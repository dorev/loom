project(loom)

file(GLOB_RECURSE
    SOURCES
    src/*.h
    src/*.cpp
)

add_library(
    ${PROJECT_NAME}
    ${SOURCES}
)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
    ${EXTERNAL_DIR}/external
    ${EXTERNAL_DIR}/libsndfile/include
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    sndfile
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Werror -pedantic)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(${PROJECT_NAME} PRIVATE /WX /W4)
endif()
