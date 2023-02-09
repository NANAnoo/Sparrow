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

add_subdirectory(ApplicationFramework)

source_group(TREE "${SparrowSRCDir}" FILES ${CoreSourceFiles})

add_library(SparrowCore STATIC ${CoreSourceFiles})

target_include_directories(SparrowCore
        PRIVATE "${ExternalFolder}"
        PRIVATE "${SparrowCoreDir}"
        )

target_link_libraries(SparrowCore PUBLIC glfw
        PUBLIC Glad
        PUBLIC glm
        PUBLIC imgui
        PUBLIC ${OPENGL_LIBRARIES}
        )