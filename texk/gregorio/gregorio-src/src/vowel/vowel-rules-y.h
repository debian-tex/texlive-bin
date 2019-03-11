/* A Bison parser, made by GNU Bison 3.3.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2019 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_GREGORIO_VOWEL_RULEFILE_VOWEL_VOWEL_RULES_Y_H_INCLUDED
# define YY_GREGORIO_VOWEL_RULEFILE_VOWEL_VOWEL_RULES_Y_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int gregorio_vowel_rulefile_debug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    LANGUAGE = 258,
    VOWEL = 259,
    PREFIX = 260,
    SUFFIX = 261,
    SECONDARY = 262,
    ALIAS = 263,
    SEMICOLON = 264,
    TO = 265,
    NAME = 266,
    CHARACTERS = 267,
    INVALID = 268
  };
#endif
/* Tokens.  */
#define LANGUAGE 258
#define VOWEL 259
#define PREFIX 260
#define SUFFIX 261
#define SECONDARY 262
#define ALIAS 263
#define SEMICOLON 264
#define TO 265
#define NAME 266
#define CHARACTERS 267
#define INVALID 268

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE gregorio_vowel_rulefile_lval;

int gregorio_vowel_rulefile_parse (const char *const filename, char **language, rulefile_parse_status *const status);

#endif /* !YY_GREGORIO_VOWEL_RULEFILE_VOWEL_VOWEL_RULES_Y_H_INCLUDED  */
