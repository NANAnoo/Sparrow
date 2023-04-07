add_executable(SPWEngine "SPWEngine.cpp" "GameWrapper.hpp" "LuaAppContext.hpp")
target_link_libraries(SPWEngine PRIVATE SparrowCore)
link_libraries(ws2_32)
target_include_directories(SPWEngine
        PRIVATE "${SparrowExternalDir}"
        PRIVATE "${SparrowCoreDir}"
        )