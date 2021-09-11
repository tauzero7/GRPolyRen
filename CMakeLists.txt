cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

project(GRPolyRen)

find_package(OpenGL REQUIRED)

set(GLAD_DIR externals/glad CACHE FILEPATH "GLAD root path")
set(GLFW_DIR externals/glfw-3.3.4 CACHE FILEPATH "Root path to glfw")
set(GLM_DIR externals/glm-0.9.9.8 CACHE FILEPATH "Root path to glm")
set(IMGUI_DIR externals/imgui-1.83 CACHE FILEPATH "Root path to imgui")
set(PFD_DIR externals/portable-file-dialogs CACHE FILEPATH "Root path to portable-file-dialogs")

set(USE_FPS OFF CACHE BOOL "Use fps counter")

set(LUA_DIR externals/lua-5.4.3 CACHE FILEPATH "Root path to lua")
add_subdirectory(${LUA_DIR})

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(${GLFW_DIR})

file(GLOB imgui_files 
    ${IMGUI_DIR}/imconfig.h
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui.h
    ${IMGUI_DIR}/imgui_internal.h
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imstb_rectpack.h
    ${IMGUI_DIR}/imstb_textedit.h
    ${IMGUI_DIR}/imstb_truetype.h
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.h
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.h
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp)

file(GLOB source_files 
    ${GLAD_DIR}/src/glad.c 
    src/AnimOrbitCam.cpp
    src/AnimOrbitCam.h
    src/AnimParam.cpp
    src/AnimParam.h
    src/Camera.cpp
    src/Camera.h
    src/CoordSystem.cpp
    src/CoordSystem.h
    src/CrossHairs3D.cpp
    src/CrossHairs3D.h
    src/EulerRotation.cpp
    src/EulerRotation.h
    src/FileTokenizer.cpp
    src/FileTokenizer.h
    src/FPSCounter.cpp
    src/FPSCounter.h
    src/VertexArray.cpp
    src/VertexArray.h
    src/GLShader.cpp
    src/GLShader.h
    src/LightSource.cpp
    src/LightSource.h
    src/LUT.cpp
    src/LUT.h
    src/Mouse.cpp
    src/Mouse.h
    src/Object.cpp
    src/Object.h
    src/OBJLoader.cpp
    src/OBJLoader.h
    src/Quaternion.cpp
    src/Quaternion.h
    src/SDSphere.cpp
    src/SDSphere.h
    src/StringUtils.cpp
    src/StringUtils.h
    src/TransScale.cpp
    src/TransScale.h
    src/Utilities.cpp
    src/Utilities.h
    src/Renderer.cpp 
    src/Renderer.h
)


# ---------------------------------------------
# Interactive target.
add_executable(GRPolyRen 
    src/main.cpp 
    src/ImGUIHandle.cpp
    src/LuaHandle.cpp
    ${source_files}
    ${imgui_files}
)
    
set_target_properties(GRPolyRen PROPERTIES 
    DEBUG_POSTFIX "d"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}"
    CXX_STANDARD 11)
    
if(USE_FPS)
    target_compile_definitions(GRPolyRen PRIVATE USE_FPS)
endif()

target_compile_definitions(GRPolyRen PRIVATE 
    HAVE_IMGUI
    HAVE_LUA)
target_include_directories(GRPolyRen PRIVATE
    ${PFD_DIR}
    ${IMGUI_DIR} 
    ${IMGUI_DIR}/backends
    ${GLAD_DIR}/include
    ${GLM_DIR})
target_link_libraries(GRPolyRen PRIVATE lua glfw ${OPENGL_LIBRARIES})

if(UNIX)
    target_link_libraries(GRPolyRen PRIVATE dl pthread)
endif()


# ---------------------------------------------
# Offline target.
add_executable(OfflineRen
    src/offrender.cpp
    src/LuaHandle.cpp
    ${source_files}
)

set_target_properties(OfflineRen PROPERTIES 
    DEBUG_POSTFIX "d"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}"
    CXX_STANDARD 11)

target_compile_definitions(OfflineRen PRIVATE 
    HAVE_LUA)
target_include_directories(OfflineRen PRIVATE 
    ${IMGUI_DIR} 
    ${IMGUI_DIR}/backends
    ${GLAD_DIR}/include
    ${GLM_DIR})
target_link_libraries(OfflineRen PRIVATE lua glfw ${OPENGL_LIBRARIES})
    
if(UNIX)
    target_link_libraries(OfflineRen PRIVATE dl pthread)
endif()

# ---------------------------------------------
# GenLookupTable target.
find_package(OpenMP)

add_executable(GenLookupTable 
    genlookup/main.cpp
    genlookup/schwarzschild.cpp
    genlookup/nrRungeKutta.cpp
    genlookup/helper.cpp)

set_target_properties(GenLookupTable PROPERTIES
    DEBUG_POSTFIX "d"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}"
    CXX_STANDARD 11)

if (OpenMP_FOUND)
    target_compile_definitions(GenLookupTable PRIVATE HAVE_OPENMP_AVAIL)
    target_link_libraries(GenLookupTable PRIVATE OpenMP::OpenMP_CXX)
endif (OpenMP_FOUND)

# ---------------------------------------------
# Deployment target to copy dependencies.
if(WIN32)
    # Locate runtime libraries.
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE)
    include(InstallRequiredSystemLibraries)

    add_custom_target(WinDeploy
        COMMAND
            ${CMAKE_COMMAND}
            -E copy
            ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
            $<TARGET_FILE_DIR:GRPolyRen>
        COMMENT "Deploying libraries...")
    add_dependencies(WinDeploy GRPolyRen)
endif()

# ---------------------------------------------
# Packaging target (copy to flatten, then zip)
set(zip_files      
    ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
    $<TARGET_FILE:GRPolyRen>
    $<TARGET_FILE:OfflineRen>
    "${CMAKE_CURRENT_SOURCE_DIR}/readme.md")
set(lut_file "${CMAKE_CURRENT_SOURCE_DIR}/lut_r40_256x512.dat")
if(EXISTS ${lut_file})
    list(APPEND zip_files ${lut_file})
else()
    message(WARNING "Skipping ${lut_file} file (will be missing in the ZIP). \
        Please generate one using GenLookupTable, copy it, and re-run CMake.")
endif()

add_custom_target(ZipIt
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_CURRENT_BINARY_DIR}/Ziporello/"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/Ziporello/"
    COMMAND ${CMAKE_COMMAND} -E copy 
        ${zip_files}
        "${CMAKE_CURRENT_BINARY_DIR}/Ziporello/"
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
        "${CMAKE_CURRENT_SOURCE_DIR}/examples" "${CMAKE_CURRENT_BINARY_DIR}/Ziporello/examples/"
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
        "${CMAKE_CURRENT_SOURCE_DIR}/objects" "${CMAKE_CURRENT_BINARY_DIR}/Ziporello/objects"
    COMMENT "Packaging files...")
add_custom_command(TARGET ZipIt
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E tar cvf "${CMAKE_SOURCE_DIR}/GRPoly.zip" --format=zip -- .
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/Ziporello")
add_dependencies(ZipIt GRPolyRen OfflineRen)
