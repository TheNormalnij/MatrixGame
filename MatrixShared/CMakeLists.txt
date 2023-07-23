add_library(MatrixShared INTERFACE)

file(GLOB MATRIX_SHARED_HEADERS shared/**.h)
#file(GLOB MATRIX_SHARED_HEADERS shared/game/commands/*.h)

source_group ("MatrixShared\\Header Files" FILES ${MATRIX_SHARED_HEADERS})

target_sources(
    MatrixShared
    PRIVATE
        ${MATRIX_SHARED_HEADERS}
)

target_include_directories(
    MatrixShared
    INTERFACE
        .
)
