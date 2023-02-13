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


#endif //SPARROW_MACROUTILS_H
