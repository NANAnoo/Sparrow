add_executable(AppWrapper "AppWrapper.cpp")
target_link_libraries(AppWrapper PRIVATE SparrowCore)
link_libraries(ws2_32)
target_include_directories(AppWrapper
        PRIVATE "${SparrowExternalDir}"
        PRIVATE "${SparrowCoreDir}"
        )