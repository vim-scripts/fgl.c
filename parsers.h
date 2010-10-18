/*
*   $Id: parsers.h,v 1.6 2002/10/16 02:19:03 darren Exp $
*
*   Copyright (c) 2000-2002, Darren Hiebert
*
*   This source code is released for free distribution under the terms of the
*   GNU General Public License.
*
*   External interface to all language parsing modules.
*
*   To add a new language parser, you need only modify this single source
*   file to add the name of the parser definition function.
*/
#ifndef _PARSERS_H
#define _PARSERS_H

/* Add the name of any new parser definition function here */
#define PARSER_LIST \
    FglParser, \
    AsmParser, \
    AspParser, \
    AwkParser, \
    BetaParser, \
    CParser, \
    CppParser, \
    CobolParser, \
    EiffelParser, \
    FortranParser, \
    JavaParser, \
    LispParser, \
    LuaParser, \
    MakefileParser, \
    PascalParser, \
    PerlParser, \
    PhpParser, \
    PythonParser, \
    RexxParser, \
    RubyParser, \
    SchemeParser, \
    ShParser, \
    SlangParser, \
    SqlParser, \
    TclParser, \
    VeraParser, \
    VerilogParser, \
    VimParser, \
    YaccParser

#endif	/* _PARSERS_H */

/* vi:set tabstop=8 shiftwidth=4: */
