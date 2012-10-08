/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     ERROR = 258,
     DEFINE = 259,
     ZERO = 260,
     ONE = 261,
     OPEN_PAREN = 262,
     CLOSE_PAREN = 263,
     COMMA = 264,
     SEMI_COLON = 265,
     EXISTENTIAL_QUANTIFIER = 266,
     UNIVERSAL_QUANTIFIER = 267,
     ADJACENT = 268,
     RELATION = 269,
     IN = 270,
     NOT_IN = 271,
     SET = 272,
     VARIABLE = 273,
     NEQ = 274,
     EQ = 275,
     SUB = 276,
     RSUB = 277,
     IMPLIES = 278,
     EQUIV = 279,
     OR_TOKEN = 280,
     AND_TOKEN = 281,
     NEG = 282
   };
#endif
/* Tokens.  */
#define ERROR 258
#define DEFINE 259
#define ZERO 260
#define ONE 261
#define OPEN_PAREN 262
#define CLOSE_PAREN 263
#define COMMA 264
#define SEMI_COLON 265
#define EXISTENTIAL_QUANTIFIER 266
#define UNIVERSAL_QUANTIFIER 267
#define ADJACENT 268
#define RELATION 269
#define IN 270
#define NOT_IN 271
#define SET 272
#define VARIABLE 273
#define NEQ 274
#define EQ 275
#define SUB 276
#define RSUB 277
#define IMPLIES 278
#define EQUIV 279
#define OR_TOKEN 280
#define AND_TOKEN 281
#define NEG 282




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 117 "ms2grammar.ypp"
{
  char* value; //Used for the name of variables and sets.
  struct PVariable* variable;
  struct PSet* set;
  struct PFormula* formula;
  struct PQuant* quant;
  struct PFormulaOperation* negation;
  struct PFormulaPointer* formula_pointer;
}
/* Line 1529 of yacc.c.  */
#line 113 "ms2grammar.hpp"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif

extern YYLTYPE yylloc;
