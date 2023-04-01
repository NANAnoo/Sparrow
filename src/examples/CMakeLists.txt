
macro(LoadSparrow)
    target_link_libraries(${ARGN} PRIVATE SparrowCore)
    target_include_directories(${ARGN}
            PRIVATE "${SparrowExternalDir}"
            PRIVATE "${SparrowCoreDir}"
            )

endmacro(LoadSparrow)

add_executable(SPWTestApp "SPWTestApp.cpp")
add_executable(SPWSoundTest "SPWSoundTest.cpp")
add_executable(SPWNewRSTest "SPWNewRSTest.cpp")

# add_compile_definitions("ENGINE_ROOT_DIR=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

LoadSparrow(SPWTestApp)
LoadSparrow(SPWSoundTest)
LoadSparrow(SPWNewRSTest)