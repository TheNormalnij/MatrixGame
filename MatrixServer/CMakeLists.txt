add_executable(MatrixServer)

file(GLOB_RECURSE MATRIX_SERVER_SOURCES src/*.cpp)
file(GLOB_RECURSE MATRIX_SERVER_HEADERS src/*.h)

source_group ("MatrixServer\\Source Files" FILES ${MATRIX_SERVER_SOURCES})
source_group ("MatrixServer\\Header Files" FILES ${MATRIX_SERVER_HEADERS})

target_sources(
    MatrixServer
    PRIVATE
        ${MATRIX_SERVER_SOURCES}
        ${MATRIX_SERVER_HEADERS}
)

target_include_directories(
    MatrixServer
    PUBLIC
        src
)

target_compile_options(
    MatrixServer
    PUBLIC
        ${COMPILE_OPTIONS})

target_compile_definitions(
    MatrixServer
    PUBLIC
        $<$<CONFIG:Release>:ASSERT_OFF>
        _ALLOW_RTCc_IN_STL
        WINDOWS_IGNORE_PACKING_MISMATCH
        ${COMPILE_DEFINITIONS}
        -DUNICODE -D_UNICODE)

target_link_libraries(
    MatrixServer
    PUBLIC
        LIBUV
        MatrixShared
)

if(NOT MSVC)
    target_link_options(
        MatrixServer
        PUBLIC
            -municode
    )
endif()

target_sources(MatrixServer PRIVATE
    src/main.cpp
)