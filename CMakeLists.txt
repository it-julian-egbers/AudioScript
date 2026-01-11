cmake_minimum_required(VERSION 3.15)
project(SpracheZuText VERSION 1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Source files for console version
set(CONSOLE_SOURCES
    src/main.cpp
    src/AudioRecorder.cpp
    src/WhisperSTT.cpp
    src/ClipboardHelper.cpp
)

# Source files for GUI version
set(GUI_SOURCES
    src/main_gui.cpp
    src/ModernUI.cpp
    src/AudioRecorder.cpp
    src/WhisperSTT.cpp
    src/ClipboardHelper.cpp
)

set(HEADERS
    src/AudioRecorder.h
    src/WhisperSTT.h
    src/ClipboardHelper.h
    src/ModernUI.h
)

# Create console executable
add_executable(SpracheZuText ${CONSOLE_SOURCES} ${HEADERS})

# Create GUI executable (Windows subsystem)
add_executable(SpracheZuText_GUI WIN32 ${GUI_SOURCES} ${HEADERS})

# Include directories (for both targets)
target_include_directories(SpracheZuText PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/external/miniaudio
    ${CMAKE_SOURCE_DIR}/external/whisper.cpp/include
    ${CMAKE_SOURCE_DIR}/external/whisper.cpp/ggml/include
)

target_include_directories(SpracheZuText_GUI PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/external/miniaudio
    ${CMAKE_SOURCE_DIR}/external/whisper.cpp/include
    ${CMAKE_SOURCE_DIR}/external/whisper.cpp/ggml/include
)

# Windows libraries
target_link_libraries(SpracheZuText PRIVATE
    User32.lib
    winmm.lib
)

target_link_libraries(SpracheZuText_GUI PRIVATE
    User32.lib
    winmm.lib
    Gdi32.lib
    dwmapi.lib
)

# Link whisper.cpp library to both targets
set(WHISPER_LIB_PATH "${CMAKE_SOURCE_DIR}/external/whisper.cpp/build/src/Release/whisper.lib")
set(WHISPER_DLL_PATH "${CMAKE_SOURCE_DIR}/external/whisper.cpp/build/bin/Release")

if(EXISTS "${WHISPER_LIB_PATH}")
    target_link_libraries(SpracheZuText PRIVATE "${WHISPER_LIB_PATH}")
    target_link_libraries(SpracheZuText_GUI PRIVATE "${WHISPER_LIB_PATH}")
    
    # Copy all required DLLs to output directory (Console)
    add_custom_command(TARGET SpracheZuText POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${WHISPER_DLL_PATH}/whisper.dll"
        "$<TARGET_FILE_DIR:SpracheZuText>/whisper.dll"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${WHISPER_DLL_PATH}/ggml.dll"
        "$<TARGET_FILE_DIR:SpracheZuText>/ggml.dll"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${WHISPER_DLL_PATH}/ggml-base.dll"
        "$<TARGET_FILE_DIR:SpracheZuText>/ggml-base.dll"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${WHISPER_DLL_PATH}/ggml-cpu.dll"
        "$<TARGET_FILE_DIR:SpracheZuText>/ggml-cpu.dll"
    )
    
    # Copy all required DLLs to output directory (GUI)
    add_custom_command(TARGET SpracheZuText_GUI POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${WHISPER_DLL_PATH}/whisper.dll"
        "$<TARGET_FILE_DIR:SpracheZuText_GUI>/whisper.dll"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${WHISPER_DLL_PATH}/ggml.dll"
        "$<TARGET_FILE_DIR:SpracheZuText_GUI>/ggml.dll"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${WHISPER_DLL_PATH}/ggml-base.dll"
        "$<TARGET_FILE_DIR:SpracheZuText_GUI>/ggml-base.dll"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${WHISPER_DLL_PATH}/ggml-cpu.dll"
        "$<TARGET_FILE_DIR:SpracheZuText_GUI>/ggml-cpu.dll"
    )
else()
    message(WARNING "whisper.lib not found at: ${WHISPER_LIB_PATH}")
endif()

# Copy model file to output directory (if exists)
if(EXISTS "${CMAKE_SOURCE_DIR}/models/ggml-base.bin")
    add_custom_command(TARGET SpracheZuText POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/models/ggml-base.bin"
        "$<TARGET_FILE_DIR:SpracheZuText>/ggml-base.bin"
    )
    add_custom_command(TARGET SpracheZuText_GUI POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/models/ggml-base.bin"
        "$<TARGET_FILE_DIR:SpracheZuText_GUI>/ggml-base.bin"
    )
endif()

# Set subsystem to console for debugging
set_target_properties(SpracheZuText PROPERTIES
    WIN32_EXECUTABLE FALSE
)

# GUI exe should have no console window
set_target_properties(SpracheZuText_GUI PROPERTIES
    WIN32_EXECUTABLE TRUE
)
