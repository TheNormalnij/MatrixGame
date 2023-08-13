add_library(MatrixShared STATIC)

file(GLOB_RECURSE MATRIX_SHARED_HEADERS shared/*.h)
file(GLOB_RECURSE MATRIX_SHARED_SOURCES shared/*.cpp)

source_group ("MatrixShared\\Header Files" FILES ${MATRIX_SHARED_HEADERS})
source_group ("MatrixShared\\Source Files" FILES ${MATRIX_SHARED_SOURCES})

target_compile_options(
    MatrixShared
    PUBLIC
        ${COMPILE_OPTIONS})

target_compile_definitions(
    MatrixShared
    PUBLIC
        $<$<CONFIG:Release>:ASSERT_OFF>
        _ALLOW_RTCc_IN_STL
        ${COMPILE_DEFINITIONS})

target_sources(
    MatrixShared
    PRIVATE
        ${MATRIX_SHARED_HEADERS}
        ${MATRIX_SHARED_SOURCES}
)

target_include_directories(
    MatrixShared
    INTERFACE
        .
)
