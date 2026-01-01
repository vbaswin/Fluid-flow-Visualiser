cmake_minimum_required(VERSION 3.16)
project(FluidFlowVisualizer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON) # Handle Qt macros automatically

# Dependencies
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets OpenGLWidgets)
find_package(VTK REQUIRED)


if(NOT VTK_FOUND)
    message(FATAL_ERROR "VTK not found. Please check CMAKE_PREFIX_PATH in.envrc")
endif()

# Source Files
set(SOURCES
    src/main.cpp
    src/vis/VtkAdapter.cpp
    src/ui/MainWindow.cpp
)

set(HEADERS
    src/common/Types.h
    src/physics/IVectorField.h
    src/physics/PipeFlow.h
    src/physics/ParticleSys.h
    src/vis/VtkAdapter.h
    src/ui/MainWindow.h
)

add_executable(FluidFlowVisualizer ${SOURCES} ${HEADERS})

# Linking
target_link_libraries(FluidFlowVisualizer PRIVATE
    Qt6::Core Qt6::Widgets Qt6::OpenGLWidgets
    VTK::CommonCore
    VTK::CommonDataModel
    VTK::RenderingCore
    VTK::RenderingOpenGL2
    VTK::GUISupportQt
    VTK::IOFFMPEG     # For recording
    VTK::InteractionStyle
)

target_include_directories(FluidFlowVisualizer PRIVATE
    ${CMAKE_SOURCE_DIR}/src/,
    ${CMAKE_CURRENT_SOURCE_DIR}/src/
)


# Required for VTK 9 auto-initialization
vtk_module_autoinit(
    TARGETS FluidFlowVisualizer
    MODULES VTK::RenderingOpenGL2 VTK::GUISupportQt
)
