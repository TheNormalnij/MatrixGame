add_library(MatrixGameInternal STATIC)

macro(add_matrix_directory DIRECTORY)
    file(GLOB DIR_SOURCES ${DIRECTORY}/*.cpp ${DIRECTORY}/*.c)
    file(GLOB DIR_HEADERS ${DIRECTORY}/*.hpp ${DIRECTORY}/*.h)
    target_sources(MatrixGameInternal PRIVATE ${DIR_SOURCES} ${DIR_HEADERS})
    get_filename_component (GROUP ${DIRECTORY} NAME)
    source_group ("${GROUP}\\Source Files" FILES ${DIR_SOURCES})
    source_group ("${GROUP}\\Header Files" FILES ${DIR_HEADERS})
endmacro()

file(GLOB MATRIX_SOURCES src/*.cpp src/*.c)
file(GLOB MATRIX_HEADERS src/*.hpp src/*.h)

target_sources(
    MatrixGameInternal
    PRIVATE
        ${MATRIX_SOURCES}
        ${MATRIX_HEADERS}
)

target_include_directories(
    MatrixGameInternal
    PUBLIC
        .
        src
        "$<$<C_COMPILER_ID:MSVC>:${DIRECTX9_INCLUDE_DIRS}>")

target_compile_options(
    MatrixGameInternal
    PUBLIC
        ${COMPILE_OPTIONS})

target_compile_definitions(
    MatrixGameInternal
    PUBLIC
        MATRIXGAME_EXPORTS
        $<$<CONFIG:Release>:ASSERT_OFF>
        _ALLOW_RTCc_IN_STL
        WINDOWS_IGNORE_PACKING_MISMATCH
        ${COMPILE_DEFINITIONS})

target_link_options(
    MatrixGameInternal
    PUBLIC
        "$<$<C_COMPILER_ID:MSVC>:-SAFESEH:NO>"
        ${LINK_OPTIONS})

target_link_libraries(
    MatrixGameInternal
    PUBLIC
        winmm
        xaudio2_8
        ${DIRECTX9_LIBRARIES}
        MatrixLib
        "$<$<C_COMPILER_ID:MSVC>:legacy_stdio_definitions>"
)

if(MATRIXGAME_CHEATS)
    set_source_files_properties(
        src/MatrixFormGame.cpp
        PROPERTIES
            COMPILE_DEFINITIONS CHEATS_ON)
endif()

add_matrix_directory(src/Effects)
add_matrix_directory(src/Interface)
add_matrix_directory(src/Logic)
add_matrix_directory(src/RangersInterface)

add_library(MatrixGame SHARED)
# remove lib prefix from the DLL name (for GCC build)
set_target_properties(MatrixGame PROPERTIES PREFIX "")

target_sources(MatrixGame PRIVATE
    src/MatrixGame.cpp
)

target_link_libraries(MatrixGame MatrixGameInternal)
