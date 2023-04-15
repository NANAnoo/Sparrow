macro(EDITOR_ADD_SOURCE)
    set(SOURCE_FILES "")
    foreach(SOURCE_FILE ${ARGN})
        if(SOURCE_FILE MATCHES "^/.*")
            list(APPEND SOURCE_FILES ${SOURCE_FILE})
        else()
            list(APPEND SOURCE_FILES
                    "${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_FILE}")
        endif()
    endforeach()
    set(EditorSourceFiles ${EditorSourceFiles} ${SOURCE_FILES} PARENT_SCOPE)
endmacro(EDITOR_ADD_SOURCE)

set(EditorSourceFiles
        "${SparrowEditorDir}/SparrowEditor.h"
        ImGui/UIComponent/ImGuiFileDialogPanel.h ImGui/UIComponent/ImGuiEntityPanel.cpp)

# --------------------------- PLEASE ADD SUBMODULES HERE -------------------
add_subdirectory(ImGui)
# --------------------------- ALL SUBMODULES HERE --------------------------

message("Editor Sources:" ${EditorSourceFiles})

source_group(TREE "${SparrowSRCDir}" FILES ${EditorSourceFiles})

add_library(SparrowEditor  STATIC ${EditorSourceFiles})

target_include_directories(SparrowEditor
        PRIVATE "${SparrowExternalDir}"
        PRIVATE "${SparrowCoreDir}"
        PRIVATE "${SparrowEditorDir}"
        )

target_link_libraries(SparrowEditor
        PUBLIC glfw
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