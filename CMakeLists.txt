cmake_minimum_required(VERSION 3.16)
project(DungeonRaylib LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Platform detection
if(EMSCRIPTEN)
    set(PLATFORM_WEB 1)
    message(STATUS "Building for Web (Emscripten)")
else()
    set(PLATFORM_DESKTOP 1)
    message(STATUS "Building for Desktop")
endif()

# Raylib configuration
if(PLATFORM_WEB)
    # Automatyczne pobieranie i kompilacja Raylib dla wersji Web za pomocą FetchContent
    include(FetchContent)
    
    set(PLATFORM "Web" CACHE STRING "Target platform for Raylib" FORCE)
    set(GRAPHICS "GRAPHICS_API_OPENGL_ES2" CACHE STRING "Graphics API for Raylib" FORCE)
    set(BUILD_EXAMPLES OFF CACHE BOOL "Disable Raylib examples" FORCE)
    
    # Naprawa ostrzeżenia CMP0135 (zalecane dla CMake >= 3.24)
    if(POLICY CMP0135)
        cmake_policy(SET CMP0135 NEW)
    endif()
    
    FetchContent_Declare(
      raylib
      GIT_REPOSITORY https://github.com
      GIT_TAG        5.5  # Pobiera dokładnie wersję 5.5 przez stabilny tag Gita
      GIT_SHALLOW    ON   # Szybkie pobieranie (tylko najnowsza rewizja, bez historii commitów)
    )
    FetchContent_MakeAvailable(raylib)
    
    set(RAYLIB_FOUND TRUE)

else()
    # Desktop: find locally installed raylib
    find_path(RAYLIB_INCLUDE_DIR raylib.h
        PATHS
        $ENV{RAYLIB_PATH}/include
        /usr/local/include
        /usr/include
        "C:/raylib/include"
    )
    find_library(RAYLIB_LIBRARY NAMES raylib
        PATHS
        $ENV{RAYLIB_PATH}/lib
        /usr/local/lib
        /usr/lib
        "C:/raylib/lib"
    )
    
    if(RAYLIB_INCLUDE_DIR AND RAYLIB_LIBRARY)
        set(RAYLIB_FOUND TRUE)
        set(RAYLIB_INCLUDE_DIRS ${RAYLIB_INCLUDE_DIR})
        set(RAYLIB_LIBRARIES ${RAYLIB_LIBRARY})
    endif()
endif()

if(NOT RAYLIB_FOUND)
    message(FATAL_ERROR "Raylib not found. Please install raylib or set RAYLIB_PATH environment variable.")
endif()

# Dołączamy nagłówki Twojego projektu (nagłówki pobranego raylib zostaną dołączone automatycznie)
include_directories(${RAYLIB_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c")

# Executable name
set(EXECUTABLE_NAME "DungeonRaylib")
if(PLATFORM_WEB)
    set(EXECUTABLE_NAME "index.html")
endif()

add_executable(${EXECUTABLE_NAME} ${SOURCES})

# Link libraries
if(PLATFORM_WEB)
    # Linkujemy do celu 'raylib' utworzonego przez FetchContent
    target_link_libraries(${EXECUTABLE_NAME} PUBLIC raylib)
    
    # Emscripten-specific linker flags
    target_link_options(${EXECUTABLE_NAME} PRIVATE
        "-sUSE_GLFW=3"
        "-sASYNCIFY"
        "-sTOTAL_MEMORY=67108864"
        "-sFORCE_FILESYSTEM=1"
        "-sALLOW_MEMORY_GROWTH=1"
        "-sEXPORTED_RUNTIME_METHODS=['ccall','cwrap']"
        "-sEXPORTED_FUNCTIONS=['_main','_malloc','_free']"
        "SHELL:--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/assets@/assets"
        "SHELL:--shell-file ${CMAKE_CURRENT_SOURCE_DIR}/web/shell.html"
    )

    # Set output name
    set_target_properties(${EXECUTABLE_NAME} PROPERTIES OUTPUT_NAME "index")
else()
    target_link_libraries(${EXECUTABLE_NAME} ${RAYLIB_LIBRARIES})
    if(WIN32)
        target_link_libraries(${EXECUTABLE_NAME} opengl32 gdi32 winmm)
    elseif(UNIX AND NOT APPLE)
        target_link_libraries(${EXECUTABLE_NAME} GL m pthread dl rt X11)
    elseif(APPLE)
        target_link_libraries(${EXECUTABLE_NAME} "-framework OpenGL" "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
    endif()
endif()

# Compiler options
target_compile_options(${EXECUTABLE_NAME} PRIVATE
    -Wall -Wextra -Wpedantic
    -DRAYLIB_VERSION=550
)

if(PLATFORM_WEB)
    target_compile_options(${EXECUTABLE_NAME} PRIVATE
        -DWEB_BUILD
        -DPLATFORM_WEB
    )
endif()

# Copy assets for desktop builds
if(NOT PLATFORM_WEB)
    add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/assets
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/assets
        COMMENT "Copying assets to build directory"
    )
endif()

# Install target
install(TARGETS ${EXECUTABLE_NAME} DESTINATION bin)
