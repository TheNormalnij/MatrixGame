cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CONFIGURATION_TYPES Release Debug)

project(MatrixGame)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

option(MATRIXGAME_BUILD_LAUNCHER "Build standalone launcher for game" TRUE)
option(MATRIXGAME_CHEATS "Enable cheats" TRUE)

find_package(DIRECTX9 REQUIRED)
if(MSVC)
    if(NOT DIRECTX9_FOUND)
        message(FATAL_ERROR "Could not find DirectX SDK")
    endif()
    include_directories(AFTER ${DIRECTX9_INCLUDE_DIRS})
endif()

include(ThirdParty/CMakeLists.txt)

list(APPEND COMPILE_DEFINITIONS MATRIXGAME_EXPORTS USE_PNGLIB $<$<CONFIG:Release>:ASSERT_OFF>)

if(MSVC)
    set(COMPILE_OPTIONS
        $<$<CONFIG:Release>:/Zi /W3 /O2 /EHsc /GS- /Gy- /MP>
        $<$<CONFIG:Debug>:/Zi /W3 /Od /GF /Gm- /RTCc /RTC1 /MP>
    )
    set(LINK_OPTIONS
        /SUBSYSTEM:WINDOWS /DYNAMICBASE:NO /MACHINE:X86
        $<$<CONFIG:Release>:/INCREMENTAL:NO /OPT:REF /OPT:ICF>
        $<$<CONFIG:Debug>:/INCREMENTAL /DEBUG>
    )

    list(APPEND COMPILE_DEFINITIONS $<$<CONFIG:Release>:_CRT_SECURE_NO_WARNINGS> _AFX_NO_DEBUG_CRT NOMINMAX)

    # TODO: <codecvt> is deprecated since C++17. in future we should
    # use std::string everywhere and not cast string <-> wstring.
    # till then - here is a hotfix to suppress warnings
    list(APPEND COMPILE_DEFINITIONS _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)
else()
    list(APPEND COMPILE_DEFINITIONS $<$<CONFIG:Debug>:_DEBUG>)

    # All the warnings below are potential bugs.
    # -Wno-... options should be fixed and removed.
    list(APPEND COMPILE_OPTIONS
        -Wall
        -Wextra
        -Wpedantic
        -Werror

        -Wno-unknown-pragmas
        -Wno-reorder
        -Wno-sign-compare
        -Wno-unused-function
        -Wno-unused-variable
        -Wno-unused-but-set-variable
        -Wno-unused-value
        -Wno-format
        -Wno-class-memaccess
        -Wno-parentheses
        -Wno-nonnull-compare
        -Wno-switch
        -Wno-address
        -Wno-strict-aliasing
        -Wno-maybe-uninitialized
        -Wno-array-bounds
        -Wno-use-after-free
        -Wno-empty-body
        -Wno-unused-parameter
        -Wno-deprecated-copy
        -Wno-type-limits
        -Wno-missing-field-initializers
        -Wno-pedantic
    )
endif()

add_subdirectory(MatrixLib)
add_subdirectory(MatrixGame)

if(MATRIXGAME_BUILD_LAUNCHER)
    add_subdirectory(MatrixLauncher)

    install(
        TARGETS MatrixGameLauncher
        RUNTIME DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin
        LIBRARY DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin
    )

    if(MSVC)
        install(
            FILES $<TARGET_PDB_FILE:MatrixGameLauncher>
            CONFIGURATIONS Debug
            DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin
        )
    endif()

endif()

install(
    TARGETS MatrixGame
    RUNTIME DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin
    LIBRARY DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin
)
if(MSVC)
    install(
        FILES $<TARGET_PDB_FILE:MatrixGame>
        CONFIGURATIONS Debug
        DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin
    )
endif()
