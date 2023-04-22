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
add_executable(SPWNewRSTest "SPWNewRSTest.cpp")
add_executable(SPWEditorTest "SPWEditorTest.cpp")

# add_compile_definitions("ENGINE_ROOT_DIR=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

LoadSparrow(SPWTestApp)
LoadSparrow(SPWSoundTest)
LoadSparrow(SPWNewRSTest)
LoadSparrow(SPWEditorTest)