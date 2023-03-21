add_executable(AppWrapper "AppWrapper.cpp")
target_link_libraries(AppWrapper PRIVATE SparrowCore)
target_include_directories(AppWrapper
        PRIVATE "${SparrowExternalDir}"
        PRIVATE "${SparrowCoreDir}"
        )