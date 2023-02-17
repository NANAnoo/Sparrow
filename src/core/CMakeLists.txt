macro(CORE_ADD_SOURCE)
    set(SOURCE_FILES "")
    foreach(SOURCE_FILE ${ARGN})
        if(SOURCE_FILE MATCHES "^/.*")
            list(APPEND SOURCE_FILES ${SOURCE_FILE})
        else()
            list(APPEND SOURCE_FILES
                    "${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_FILE}")
        endif()
    endforeach()
    set(CoreSourceFiles ${CoreSourceFiles} ${SOURCE_FILES} PARENT_SCOPE)
endmacro(CORE_ADD_SOURCE)

set(CoreSourceFiles "${SparrowCoreDir}/SparrowCore.h" Render/shader.cpp Render/shader.h Render/RenderAPII.h Render/Render.cpp Render/Render.h Render/GraphicsContext.cpp Render/GraphicsContext.h Render/VertexBuffer.cpp Render/VertexBuffer.h Render/IndexBuffer.cpp Render/IndexBuffer.h Platforms/OPENGL/OpenGLShader.cpp Platforms/OPENGL/OpenGLShader.h Platforms/OPENGL/OpenGLAPI.cpp Platforms/OPENGL/OpenGLAPI.h Platforms/OPENGL/OpenGLContext.cpp Platforms/OPENGL/OpenGLContext.h Platforms/OPENGL/OpenGLVertexBuffer.cpp Platforms/OPENGL/OpenGLVertexBuffer.h Platforms/OPENGL/OpenGLIndexBuffer.cpp Platforms/OPENGL/OpenGLIndexBuffer.h)

# --------------------------- PLEASE ADD SUBMODULES HERE -------------------
add_subdirectory(Utils)
add_subdirectory(Event)
add_subdirectory(Control)
add_subdirectory(ApplicationFramework)
add_subdirectory(EcsFramework)
add_subdirectory(Platforms)
add_subdirectory(Model)
add_subdirectory(Render)
# --------------------------- ALL SUBMODULES HERE --------------------------

message("Core Sources:" ${CoreSourceFiles})

source_group(TREE "${SparrowSRCDir}" FILES ${CoreSourceFiles})

add_library(SparrowCore STATIC ${CoreSourceFiles})

target_include_directories(SparrowCore
        PRIVATE "${SparrowExternalDir}"
        PRIVATE "${SparrowCoreDir}"
        )

target_link_libraries(SparrowCore PUBLIC glfw
        PUBLIC headers
        PUBLIC Glad
        PUBLIC glm
        PUBLIC imgui
        PUBLIC ${OPENGL_LIBRARIES}
        )