macro(LoadSparrow)
    target_link_libraries(${ARGN} PRIVATE SparrowCore)
    target_link_libraries(${ARGN} PRIVATE SparrowEditor)
    target_include_directories(${ARGN}
            PRIVATE "${SparrowExternalDir}"
            PRIVATE "${SparrowCoreDir}"
            PRIVATE "${SparrowEditorDir}"
            )

endmacro(LoadSparrow)

add_executable(SPWTestApp "SPWTestApp.cpp")
add_executable(SPWSoundTest "SPWSoundTest.cpp")
add_executable(SPWPhysicsTest "SPWPhysicsTest.cpp")
add_executable(SPWNewRSTest "SPWNewRSTest.cpp")
add_executable(SPWSceneTest "SPWSceneTest.cpp")
add_executable(SPWEditorTest "SPWEditorTest.cpp")
add_executable(AssetLoader "SPWEOfflineToolTest.cpp")

# add_compile_definitions("ENGINE_ROOT_DIR=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
LoadSparrow(AssetLoader)
LoadSparrow(SPWTestApp)
LoadSparrow(SPWSoundTest)
LoadSparrow(SPWPhysicsTest)
LoadSparrow(SPWNewRSTest)
LoadSparrow(SPWEditorTest)
LoadSparrow(SPWSceneTest)