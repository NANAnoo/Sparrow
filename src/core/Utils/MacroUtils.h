//
// Created by NANAnoo on 2/11/2023.
//

#ifndef SPARROW_MACROUTILS_H
#define SPARROW_MACROUTILS_H

#include <cstddef>
#include <tuple>
#include <functional>

#ifdef SPW_DEBUG
#define DEBUG_EXPRESSION(expr) do{expr;}while(0);
#define DEBUG_PROPERTY(expr) expr;
#else
#define DEBUG_EXPRESSION(expr)
#define DEBUG_PROPERTY(expr)
#endif


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
