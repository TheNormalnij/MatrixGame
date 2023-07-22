add_library(MatrixShared INTERFACE)

file(GLOB MATRIX_SHARED_HEADERS src/net/*.h)

source_group ("MatrixShared\\Header Files" FILES ${MATRIX_SHARED_HEADERS})

target_sources(
    MatrixShared
    PRIVATE
        ${MATRIX_SHARED_HEADERS}
)

target_include_directories(
    MatrixShared
    INTERFACE
        src
)
