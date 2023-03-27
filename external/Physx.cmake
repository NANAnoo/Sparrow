set(PhysxSourceDir ${ExternalDir}/physx)

set(PhysxIncludeDir
    "${PhysxSourceDir}/physx/include"
    "${PhysxSourceDir}/pxshared/include"
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(PhysxLibDir "${PhysxSourceDir}/physx/bin/win.x86_64.vc143.mt/debug/")
else()
    set(PhysxLibDir "${PhysxSourceDir}/physx/bin/win.x86_64.vc143.mt/release/")
endif()

set(PhysxDllFile
            "${PhysxLibDir}/PhysXFoundation_64.dll"
            "${PhysxLibDir}/PhysXDevice64.dll"
            "${PhysxLibDir}/PhysXCooking_64.dll"
            "${PhysxLibDir}/PhysXCommon_64.dll"
            "${PhysxLibDir}/PhysX_64.dll")

set(PhysX_lib ${PhysxLibDir}/PhysX_64.lib)
set(PhysXFoundation_lib ${PhysxLibDir}/PhysXFoundation_64.lib)
set(PhysXExtensions_lib ${PhysxLibDir}/PhysXExtensions_static_64.lib)
set(PhysXPvdSDK_lib ${PhysxLibDir}/PhysXPvdSDK_static_64.lib)

add_library(Physx::base SHARED IMPORTED GLOBAL)
set_target_properties(Physx::base PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${PhysxSourceDir}/physx/include)

add_library(Physx::foundation SHARED IMPORTED GLOBAL)
set_target_properties(Physx::foundation PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${PhysxSourceDir}/physx/include)

add_library(Physx::extension SHARED IMPORTED GLOBAL)
set_target_properties(Physx::extension PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${PhysxSourceDir}/physx/include)

add_library(Physx::pvd SHARED IMPORTED GLOBAL)
set_target_properties(Physx::pvd PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${PhysxSourceDir}/physx/include)

set_target_properties(Physx::base PROPERTIES IMPORTED_IMPLIB ${PhysX_lib})
set_target_properties(Physx::foundation PROPERTIES IMPORTED_IMPLIB ${PhysXFoundation_lib})
set_target_properties(Physx::extension PROPERTIES IMPORTED_IMPLIB ${PhysXExtensions_lib})
set_target_properties(Physx::pvd PROPERTIES IMPORTED_IMPLIB ${PhysXPvdSDK_lib})

file(COPY ${PhysxDllFile} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_target_properties(Physx::base PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
    set_target_properties(Physx::foundation PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
    set_target_properties(Physx::extension PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
    set_target_properties(Physx::pvd PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
else()
    set_target_properties(Physx::base PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded")
    set_target_properties(Physx::foundation PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded")
    set_target_properties(Physx::extension PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded")
    set_target_properties(Physx::pvd PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif()
