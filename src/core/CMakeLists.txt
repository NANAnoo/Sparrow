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

set(CoreSourceFiles "${SparrowCoreDir}/SparrowCore.h")

# --------------------------- PLEASE ADD SUBMODULES HERE -------------------
add_subdirectory(Utils)
add_subdirectory(Event)
add_subdirectory(Control)
add_subdirectory(ApplicationFramework)
add_subdirectory(EcsFramework)
add_subdirectory(Platforms)
add_subdirectory(Render)
add_subdirectory(Model)
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
        PUBLIC sol2
        PUBLIC lua_static
        PUBLIC FMOD::Core FMOD::Fsbank FMOD::Studio
        PUBLIC assimp
        PUBLIC ${OPENGL_LIBRARIES}
        )