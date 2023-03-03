//
// Created by NANAnoo on 2/11/2023.
//

#ifndef SPARROW_MACROUTILS_H
#define SPARROW_MACROUTILS_H

#include <cstddef>
#include <tuple>
#include <functional>
#include <memory>

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

namespace SPW
{

    template< std::size_t... index >
    struct X_Index_tuple
    {

    };

    template<std::size_t num, typename Tuple = X_Index_tuple<>>
    struct X_Build_index_tuple;

    template< std::size_t num, size_t... index >
    struct X_Build_index_tuple<num, X_Index_tuple< index... > >
        : X_Build_index_tuple< num - 1, X_Index_tuple< index..., sizeof...(index) > > {

    };

    template< size_t... index >
    struct X_Build_index_tuple< 0, X_Index_tuple< index... >> {
        typedef X_Index_tuple< index... > type;
    };

    template<class TupleType, typename FuncType, size_t... index>
    constexpr void InvokeTupleFunc(TupleType && tuple_value, FuncType &&func, X_Index_tuple< index... >)
    {
        func(std::get<index>(tuple_value)...);
    }

    template<class TupleType, typename FuncType, size_t... index>
    constexpr void InvokeTupleFunc_Ref(TupleType && tuple_value, const FuncType &func, X_Index_tuple< index... >)
    {
        func(&std::get<index>(tuple_value)...);
    }

    template<class TupleType, typename FuncType, size_t... index>
    constexpr void InvokeTupleFunc_Address(TupleType && tuple_value, const FuncType &func, X_Index_tuple< index... >)
    {
        func(*std::get<index>(tuple_value)...);
    }

}; // namespace SPW

#endif //SPARROW_MACROUTILS_H
