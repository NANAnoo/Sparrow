set(FmodSourceDir ${ExternalDir}/FMOD)

if (${IS_ARM_MAC}) 
    set(FmodCoreLibFile "${FmodSourceDir}/core/lib/arm_macos/libfmod.dylib")
    set(FmodFsBankLibFile "${FmodSourceDir}/fsbank/lib/arm_macos/libfsbank.dylib")
    set(FmodStudioLibFile "${FmodSourceDir}/studio/lib/arm_macos/libfmodstudio.dylib")
elseif(WIN32)
    if(${IS_WIN_64})
        set(FmodCoreLibFile "${FmodSourceDir}/core/lib/win_64/fmod_vc.lib")
        set(FmodFsBankLibFile "${FmodSourceDir}/fsbank/lib/win_64/fsbank_vc.lib")
        set(FmodStudioLibFile "${FmodSourceDir}/studio/lib/win_64/fmodstudio_vc.lib")
        set(FmodCoreDllFile
            "${FmodSourceDir}/core/lib/win_64/fmod.dll"
            "${FmodSourceDir}/fsbank/lib/win_64/fsbank.dll"
            "${FmodSourceDir}/studio/lib/win_64/fmodstudio.dll")
    else()
        set(FmodCoreLibFile "${FmodSourceDir}/core/lib/win_32/fmod_vc.lib")
        set(FmodFsBankLibFile "${FmodSourceDir}/fsbank/lib/win_32/fsbank_vc.lib")
        set(FmodStudioLibFile "${FmodSourceDir}/studio/lib/win_32/fmodstudio_vc.lib")
        set(FmodCoreDllFile
            "${FmodSourceDir}/core/lib/win_32/fmod.dll"
            "${FmodSourceDir}/fsbank/lib/win_32/fsbank.dll"
            "${FmodSourceDir}/studio/lib/win_32/fmodstudio.dll")
    endif(${IS_WIN_64})
elseif(UNIX)
    if ("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
        set(FmodCoreLibFile "${FmodSourceDir}/core/lib/linux/x86_64/libfmod.so")
        set(FmodFsBankLibFile "${FmodSourceDir}/fsbank/lib/linux/x86_64/libfsbank.so")
        set(FmodStudioLibFile "${FmodSourceDir}/studio/lib/linux/x86_64/libfmodstudio.so")
    else()
        set(FmodCoreLibFile "${FmodSourceDir}/core/lib/linux/x86/libfmod.so")
        set(FmodFsBankLibFile "${FmodSourceDir}/fsbank/lib/linux/x86/libfsbank.so")
        set(FmodStudioLibFile "${FmodSourceDir}/studio/lib/linux/x86/libfmodstudio.so")
    endif()
else()
    message(FATAL_ERROR "unsupport platforms of FMOD")
endif()


set(FmodIncludeDir
    "${FmodSourceDir}/core/inc"
    "${FmodSourceDir}/fsbank/inc"
    "${FmodSourceDir}/studio/inc"
)

add_library(FMOD_CORE SHARED IMPORTED GLOBAL)
add_library(FMOD::Core ALIAS FMOD_CORE)
set_target_properties(FMOD_CORE PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${FmodSourceDir}/core/inc")


add_library(FMOD_FSBANK SHARED IMPORTED GLOBAL)
add_library(FMOD::Fsbank ALIAS FMOD_FSBANK)
set_target_properties(FMOD_FSBANK PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${FmodSourceDir}/fsbank/inc")

add_library(FMOD_STUDIO SHARED IMPORTED GLOBAL)
add_library(FMOD::Studio ALIAS FMOD_STUDIO)
set_target_properties(FMOD_STUDIO PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${FmodSourceDir}/studio/inc")


if(NOT UNIX)
    set_target_properties(FMOD_CORE PROPERTIES IMPORTED_IMPLIB ${FmodCoreLibFile})
    set_target_properties(FMOD_FSBANK PROPERTIES IMPORTED_IMPLIB ${FmodFsBankLibFile})
    set_target_properties(FMOD_STUDIO PROPERTIES IMPORTED_IMPLIB ${FmodStudioLibFile})
endif()


if(WIN32)
    file(COPY ${FmodCoreDllFile} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
else()
    set_target_properties(FMOD_STUDIO PROPERTIES IMPORTED_LOCATION ${FmodStudioLibFile})
    set_target_properties(FMOD_FSBANK PROPERTIES IMPORTED_LOCATION ${FmodFsBankLibFile})
    set_target_properties(FMOD_CORE PROPERTIES IMPORTED_LOCATION ${FmodCoreLibFile})
endif()

# file(COPY ${FmodLibFile} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})