cmake_minimum_required(VERSION 3.16)
project(ps1emulator)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "-O3")

set(GLEW_USE_STATIC_LIBS TRUE)
set(GLEW_VERBOSE TRUE)
set(GLEW_INCLUDE_DIR "C:/glew/include")
set(GLEW_STATIC_LIBRARY_DEBUG "C:/glew/lib/Release/Win32/glew32s.lib")


find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED STATIC)

include_directories("C:/GLFW/include")
link_directories("C:/GLFW/lib-mingw" )
find_library(glfw3 REQUIRED)


add_executable(ps1emulator main.cpp Bios.cpp Bios.h Cpu.cpp Cpu.h MemoryMap.cpp MemoryMap.h MemoryBus.cpp MemoryBus.h Coprocessor.cpp Coprocessor.h HardwareRegisters.cpp HardwareRegisters.h Ram.cpp Ram.h Interrupts.cpp Interrupts.h Timer.cpp Timer.h Dma.cpp Dma.h DmaChannel.cpp DmaChannel.h Gpu.cpp Gpu.h CdRom.cpp CdRom.h Renderer.cpp Renderer.h Controller.cpp Controller.h)

target_link_libraries(ps1emulator ${OPENGL_LIBRARIES} GLEW::glew_s glfw3 glu32 opengl32)