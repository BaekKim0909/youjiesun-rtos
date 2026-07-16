cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME YouJieSun)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
file(GLOB_RECURSE LVGL_SOURCE "Middlewares/lvgl/*.c")
file(GLOB_RECURSE IMG_SOURCE "App/UI/Source/Img/*.c")
file(GLOB_RECURSE FONT_SOURCE "App/UI/Source/Font/*.c")
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
        ${LVGL_SOURCE}
        ${IMG_SOURCE}
        ${FONT_SOURCE}
        App/UI/Source/Font/chinese_character_16.c
        App/UI/Source/Font/chinese_character_20.c
        App/UI/Source/Font/chinese_character_24.c
        App/UI/Src/mainUI.c
        App/UI/Src/header.c
        App/UI/Src/footer.c
        App/UI/Style/Src/style_g.c
        App/Task/Src/UITask.c
        BSP/Src/lcd_screen.c
        BSP/Src/sdram.c
        BSP/Src/keypad.c
        BSP/Src/uart_device.c
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
        Middlewares/lvgl
        Middlewares/lvgl/src
        Middlewares/lvgl/porting
        App/UI/Inc
        App/UI/Style/Inc
        BSP/Inc
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
        LV_LVGL_H_INCLUDE_SIMPLE
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)
