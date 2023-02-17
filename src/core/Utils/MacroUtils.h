//
// Created by NANAnoo on 2/11/2023.
//

#ifndef SPARROW_MACROUTILS_H
#define SPARROW_MACROUTILS_H


#ifdef SPW_DEBUG
#define DEBUG_EXPRESSION(expr) do{expr;}while(0);
#define DEBUG_PROPERTY(expr) expr;
#else
#define DEBUG_EXPRESSION(expr)
#define DEBUG_PROPERTY(expr)
#endif

/*
 *  Define Visual Studio Debugging Marcos
 */
#ifdef _MSC_VER // Debug-Breakpoint / Platform-specific
	#include <intrin.h>
	#define BP() __debugbreak()
#elif defined (__unix__)  || (defined (__APPLE__) && defined (__MACH__))
	#include <signal.h>
	#define BP() raise(SIGTRAP)
#else
	#define BP() ;
#endif


/*
 *  Define OpenGL State Handle Wrapper for Debugging
 */
#include <glad/glad.h>
#include <iostream>
#include <iomanip>
#define SPW_GLCALL(bar) \
	while (glGetError() != GL_NO_ERROR) /* pop */;\
	bar;\
	if (glGetError() != GL_NO_ERROR) {\
		std::cerr << __FILE__ << ':' << __LINE__ << ": " << #bar << "\n\n";\
		BREAKPOINT(); } // <-- scope_end}


/*
 *  Define Windows Index/Counting Aliasing
 */
#ifdef _WIN32
    using Count = unsigned int ;
	using Count = uint32_t;
#else
    #include <stdint.h>
#endif


/*
 *  Define STL Smart Pointers Aliasing
 */
template<typename T> using UniPtr = std::unique_ptr<T>;

template<typename T> using SharedPtr = std::shared_ptr<T>;

template<typename T> using WeakPtr = std::weak_ptr<T>;

#include <filesystem>
using FilePath = std::filesystem::path;

#include "unordered_map"
template<typename T, typename Q> using UnorderMap = std::unordered_map<T, Q>;

#endif //SPARROW_MACROUTILS_H
