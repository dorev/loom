project(tests)

add_executable(
    ${PROJECT_NAME}
    main.cpp
    tests.cpp
    ${EXTERNAL_DIR}/rtaudio/RtAudio.cpp
    ${EXTERNAL_DIR}/rtaudio/RtAudio.h
)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
    loom
)

set(GTEST_ROOT ${EXTERNAL_DIR}/googletest)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
    ${GTEST_ROOT}/googletest
    ${GTEST_ROOT}/googletest/include
    ${GTEST_ROOT}/googlemock
    ${GTEST_ROOT}/googlemock/include
    ${CMAKE_SOURCE_DIR}/loom/src
    ${EXTERNAL_DIR}
)

if(WIN32)
    set(RTAUDIO_API "__WINDOWS_WASAPI__")
elseif(APPLE)
    set(RTAUDIO_API "__MACOSX_CORE__")
elseif(UNIX)
    set(RTAUDIO_API "__LINUX_PULSE__")
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE ${RTAUDIO_API})
