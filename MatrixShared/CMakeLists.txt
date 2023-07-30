add_library(MatrixShared INTERFACE)

file(GLOB MATRIX_SHARED_HEADERS shared/**.h)
file(GLOB MATRIX_SHARED_SOURCES shared/**.cpp)

source_group ("MatrixShared\\Header Files" FILES ${MATRIX_SHARED_HEADERS})
source_group ("MatrixShared\\Header Files" FILES ${MATRIX_SHARED_SOURCES})

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
