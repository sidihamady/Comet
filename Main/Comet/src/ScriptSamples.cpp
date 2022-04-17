// -----------------------------------------------------------------------------------
// Comet <Programming Environment for Lua>
//      Copyright(C) 2010-2022 Pr. Sidi HAMADY
//      http://www.hamady.org
//      sidi@hamady.org
//
//      :STABLE:VERSION180:BUILD2104:
//
//      Released under the MIT licence (https://opensource.org/licenses/MIT)
//      See Copyright Notice in COPYRIGHT
// -----------------------------------------------------------------------------------

#include "Identifiers.h"

#include "CometApp.h"
#include "CometFrame.h"

#define CODESAMPLE_EXAMPLE_COUNT    8
#define CODESAMPLE_SNIPPET_COUNT    12

bool CodeSample::createLua(int iType)
{
    release();

    m_iType = iType;

    try {
        m_iCount = (m_iType == CODESAMPLE_EXAMPLE) ? CODESAMPLE_EXAMPLE_COUNT : CODESAMPLE_SNIPPET_COUNT;
        m_pCode = new CodeItem[m_iCount];
    }
    catch (...) {
        m_iCount = 0;
        m_pCode = NULL;
    }
    if (m_pCode == NULL) {
        return false;
    }

    m_iLexer = wxSTC_LEX_LUA;

    if (m_iType == CODESAMPLE_EXAMPLE) {
        m_pCode[0].set(
            uT("Hello world"),

            5,
            3,

            uT("-- Lua\n\n")

            uT("cls()\n")
            uT("io.write(\"Hello world!\\n\")\n"));


        m_pCode[1].set(
            uT("File IO"),

            11,
            4,

            uT("-- File IO\n\n")

            uT("cls()\n")
            uT("fn = \"test.txt\"\n")
            uT("fp = io.open(fn, \"w+\")\n")
            uT("fp:write(\"Hello world!\")\n")
            uT("fp:close()\n")
            uT("fp = io.open(fn, \"r\")\n")
            uT("tt = fp:read(\"*all\")\n")
            uT("fp:close()\n")
            uT("io.write(tt)\n"));

        m_pCode[2].set(
            uT("io.read"),

            6,
            3,

            uT("-- io.read\n\n")

            uT("cls()\n")
            uT("io.write(\"\\nEnter your name: \")\n")
            uT("name = io.read(\"*a\")\n")
            uT("io.write(\"\\nName: \", name)\n"));

        m_pCode[3].set(
            uT("Timer"),

            7,
            5,

            uT("-- Timer\n\n")

            uT("tic()\n")
            uT("sleep(200)\n")
            uT("dt = toc()\n")
            uT("cls()\n")
            uT("print(time.format(dt))\n"));

        m_pCode[4].set(
            uT("Create module"),

            10,
            4,

            uT("-- Create module\n")
            uT("-- To save as mdl.lua\n\n")

            uT("local mdl = {}\n\n")

            uT("function mdl.func()\n")
            uT("   print(\"Test\")\n")
            uT("end\n\n")
            uT("return mdl\n"));

        m_pCode[5].set(
            uT("Use module"),

            5,
            1,

            uT("-- Use module\n")
            uT("-- To save in the same directory than mdl.lua\n\n")

            uT("mdl = require \"mdl\"\n")
            uT("mdl.func()\n"));

        m_pCode[6].set(
            uT("Hex Viewer"),

            17,
            13,

            uT("-- Hex viewer\n\n")

            uT("cls()\n\n")

            uT("filename = \"hello.lua\"\n")
            uT("local filep = assert(io.open(filename, \"rb\"))\n")
            uT("local size = 10\n")
            uT("while true do\n")
            uT("    local bread = filep:read(size)\n")
            uT("    if not bread then\n")
            uT("        break\n")
            uT("    end\n")
            uT("    for tbyte in string.gmatch(bread, \".\") do\n")
            uT("        io.write(string.format(\"%02X \", string.byte(tbyte)))\n")
            uT("    end\n")
            uT("    io.write(string.rep(\"   \", size - string.len(bread) + 1))\n")
            uT("    io.write(string.gsub(bread, \"%c\", \".\"), \"\\n\")\n")
            uT("end\n")
            uT("io.close(filep)\n"));


        m_pCode[7].set(
            uT("MAPM"),

            33,
            0,

            uT("-- Library for Arbitrary Precision Math (MAPM) by Michael C. Ring\n")
            uT("-- Lua interface by Luiz Henrique de Figueiredo\n\n")

            uT("lmapm = require (\"lmapm\")\n\n")

            uT("-- lmapm library functions:\n")
            uT("--  __add(x,y)      cbrt(x)          mod(x,y)\n")
            uT("--  __div(x,y)      ceil(x)          mul(x,y)\n")
            uT("--  __eq(x,y)       compare(x,y)     neg(x)\n")
            uT("--  __lt(x,y)       cos(x)           number(x)\n")
            uT("--  __mod(x,y)      cosh(x)          pow(x,y)\n")
            uT("--  __mul(x,y)      digits([n])      round(x)\n")
            uT("--  __pow(x,y)      digitsin(x)      sign(x)\n")
            uT("--  __sub(x,y)      div(x,y)         sin(x)\n")
            uT("--  __tostring(x)   exp(x)           sincos(x)\n")
            uT("--  __unm(x)        exponent(x)      sinh(x)\n")
            uT("--  abs(x)          factorial(x)     sqrt(x)\n")
            uT("--  acos(x)         floor(x)         sub(x,y)\n")
            uT("--  acosh(x)        idiv(x,y)        tan(x)\n")
            uT("--  add(x,y)        inv(x)           tanh(x)\n")
            uT("--  asin(x)         iseven(x)        tonumber(x)\n")
            uT("--  asinh(x)        isint(x)         tostring(x,[n,exp])\n")
            uT("--  atan(x)         isodd(x)         version\n")
            uT("--  atan2(y,x)      log(x)\n")
            uT("--  atanh(x)        log10(x)\n\n")

            uT("print(\"\\nSquare root of 2\")\n\n")

            uT("lmapm.digits(65)\n")
            uT("print(\"math.sqrt(2)   \", math.sqrt(2))\n")
            uT("print(\"lmapm.sqrt(2)  \", lmapm.sqrt(2))\n")
            uT("print(lmapm.version)\n"));
    }

    // code snippets
    else {
        m_pCode[0].set(
            uT("cls"),

            1,
            5,

            uT("\ncls()\n"));

        m_pCode[1].set(
            uT("print"),

            1,
            7,

            uT("\nprint()\n"));

        m_pCode[2].set(
            uT("io.write"),

            1,
            10,

            uT("\nio.write()\n"));

        m_pCode[3].set(
            uT("io.read"),

            1,
            10,

            uT("\nio.read(\"*a\")\n"));

        m_pCode[4].set(
            uT("while"),

            1,
            8,

            uT("\nwhile () do\n    \nend\n"));

        m_pCode[5].set(
            uT("repeat"),

            1,
            11,

            uT("\nrepeat\n    \nuntil ()\n"));

        m_pCode[6].set(
            uT("if"),

            1,
            5,

            uT("\nif () then\n    \nend\n"));

        m_pCode[7].set(
            uT("if else"),

            1,
            5,

            uT("\nif () then\n    \nelse\n    \nend\n"));

        m_pCode[8].set(
            uT("for"),

            1,
            9,

            uT("\nfor i = , ,  do\n    \nend\n"));

        m_pCode[9].set(
            uT("function"),

            1,
            15,

            uT("\nfunction func()\n    \nend\n"));

        m_pCode[10].set(
            uT("return"),

            1,
            8,

            uT("\nreturn()\n"));

        m_pCode[11].set(
            uT("coroutine"),

            1,
            33,

            uT("\nco = coroutine.create(function ()\n    print(\"hello, world!\")\nend)"));
    }

    return true;
}

bool CodeSample::createCPP(int iType)
{
    release();

    m_iType = iType;

    // Examples only given for Lua
    if (m_iType != CODESAMPLE_SNIPPET) {
        return false;
    }

    try {
        m_iCount = (m_iType == CODESAMPLE_EXAMPLE) ? 14 : 12;
        m_pCode = new CodeItem[m_iCount];
    }
    catch (...) {
        m_iCount = 0;
        m_pCode = NULL;
    }
    if (m_pCode == NULL) {
        return false;
    }

    m_iLexer = wxSTC_LEX_CPP;

    if (m_iType == CODESAMPLE_EXAMPLE) {
        // Nothing
    }
    else {
        m_pCode[0].set(
            uT("define"),

            1,
            9,

            uT("\n#define \n"));

        m_pCode[1].set(
            uT("include"),

            1,
            11,

            uT("\n#include <.h>\n"));


        m_pCode[2].set(
            uT("printf"),

            1,
            9,

            uT("\nprintf(\" \", )\n"));

        m_pCode[3].set(
            uT("scanf"),

            1,
            8,

            uT("\nscanf(\" \", &)\n"));

        m_pCode[4].set(
            uT("while"),

            1,
            8,

            uT("\nwhile () {\n    ;\n}\n"));

        m_pCode[5].set(
            uT("do while"),

            1,
            9,

            uT("\ndo {\n    ;\n} while ();\n"));

        m_pCode[6].set(
            uT("if"),

            1,
            5,

            uT("\nif () {\n    ;\n}\n"));

        m_pCode[7].set(
            uT("if else"),

            1,
            5,

            uT("\nif () {\n    ;\n}\nelse {\n    ;\n}\n"));

        m_pCode[8].set(
            uT("for"),

            1,
            14,

            uT("\nfor (int i = ; i < ; i++) {\n    ;\n}\n"));

        m_pCode[9].set(
            uT("function"),

            1,
            15,

            uT("\ndouble gammax( )\n{\n    ;\n    ;\n    return y;\n}\n"));

        m_pCode[10].set(
            uT("return"),

            1,
            8,

            uT("\nreturn 0;\n"));

        m_pCode[11].set(
            uT("include Lua"),

            1,
            58,

            uT("\n#include \"lua.h\"\n#include \"lualib.h\"\n#include \"lauxlib.h\"\n"));
    }

    return true;
}

bool CodeSample::createPython(int iType)
{
    release();

    m_iType = iType;

    // Examples only given for Lua
    if (m_iType != CODESAMPLE_SNIPPET) {
        return false;
    }

    try {
        m_iCount = (m_iType == CODESAMPLE_EXAMPLE) ? 14 : 12;
        m_pCode = new CodeItem[m_iCount];
    }
    catch (...) {
        m_iCount = 0;
        m_pCode = NULL;
    }
    if (m_pCode == NULL) {
        return false;
    }

    m_iLexer = wxSTC_LEX_PYTHON;

    if (m_iType == CODESAMPLE_EXAMPLE) {
        // Nothing
    }
    else {
        m_pCode[0].set(
            uT("import numpy"),

            1,
            8,

            uT("\nimport numpy as np\n"));

        m_pCode[1].set(
            uT("import scipy"),

            1,
            8,

            uT("\nimport scipy as sp\n"));

        m_pCode[2].set(
            uT("import matplotlib"),

            1,
            8,

            uT("\nimport matplotlib as mp\n"));

        m_pCode[3].set(
            uT("print"),

            1,
            7,

            uT("\nprint()\n"));

        m_pCode[4].set(
            uT("try except"),

            1,
            9,

            uT("\ntry:\n    \n    \nexcept Exception as excT:\n    \n"));

        m_pCode[5].set(
            uT("while"),

            1,
            8,

            uT("\nwhile ():\n    \n"));

        m_pCode[6].set(
            uT("if"),

            1,
            5,

            uT("\nif ():\n    \n"));

        m_pCode[7].set(
            uT("if else"),

            1,
            5,

            uT("\nif ():\n    \nelse:\n    \n"));

        m_pCode[8].set(
            uT("for"),

            1,
            6,

            uT("\nfor i in range(0, n):\n    \n"));

        m_pCode[9].set(
            uT("function"),

            1,
            10,

            uT("\ndef func():\n    \n    return()\n"));

        m_pCode[10].set(
            uT("return"),

            1,
            8,

            uT("\nreturn()\n"));

        m_pCode[11].set(
            uT("\n# end"),

            1,
            7,

            uT("\n# end "));
    }

    return true;
}

bool CodeSample::createLaTeX(int iType)
{
    release();

    m_iType = iType;

    // Examples only given for Lua
    if (m_iType != CODESAMPLE_SNIPPET) {
        return false;
    }

    try {
        m_iCount = (m_iType == CODESAMPLE_EXAMPLE) ? 14 : 21;
        m_pCode = new CodeItem[m_iCount];
    }
    catch (...) {
        m_iCount = 0;
        m_pCode = NULL;
    }
    if (m_pCode == NULL) {
        return false;
    }

    m_iLexer = wxSTC_LEX_LATEX;

    if (m_iType == CODESAMPLE_EXAMPLE) {
        // Nothing
    }
    else {
        m_pCode[0].set(
            uT("documentclass"),

            1,
            16,

            uT("\n\\documentclass[]{}\n"));

        m_pCode[1].set(
            uT("usepackage"),

            1,
            13,

            uT("\n\\usepackage{}\n"));

        m_pCode[2].set(
            uT("input"),

            1,
            8,

            uT("\n\\input{}\n"));

        m_pCode[3].set(
            uT("begin end"),

            1,
            8,

            uT("\n\\begin{}\n\n\n\\end{}\n"));

        m_pCode[4].set(
            uT("newcommand"),

            1,
            14,

            uT("\n\\newcommand{\\}{}\n"));

        m_pCode[5].set(
            uT("section"),

            1,
            10,

            uT("\n\\section{}\n\\label{}\n\n"));

        m_pCode[6].set(
            uT("subsection"),

            1,
            13,

            uT("\n\\subsection{}\n\\label{}\n\n"));

        m_pCode[7].set(
            uT("subsubsection"),

            1,
            16,

            uT("\n\\subsubsection{}\n\\label{}\n\n"));

        m_pCode[8].set(
            uT("figure"),

            1,
            52,

            uT("\n\\begin{figure}\n\t\\includegraphics[width=\\linewidth]{}\n\t\\caption{}\n\t\\label{}\n\\end{figure}\n"));

        m_pCode[9].set(
            uT("table"),

            1,
            53,

            uT("\n\\begin{table}\n\\begin{center}\n\\begin{tabular}{|l|c|c|}\n\t\\hline\n\t\\\\\n\t\\hline\n\t\\\\\n\t\\hline\n\\end{tabular}\n\\end{center}\n\\caption{Caption}\n\\label{tablelabel}\n\\end{table}\n"));

        m_pCode[10].set(
            uT("equation"),

            1,
            19,

            uT("\n\\begin{equation}\n\t\n\\end{equation}\n"));

        m_pCode[11].set(
            uT("itemize"),

            1,
            27,

            uT("\n\\begin{itemize}\n")
            uT("    \\item  \n")
            uT("    \\item  \n")
            uT("    \\item  \n")
            uT("\\end{itemize}\n"));

        m_pCode[12].set(
            uT("enumerate"),

            1,
            29,

            uT("\n\\begin{enumerate}\n")
            uT("    \\item  \n")
            uT("    \\item  \n")
            uT("    \\item  \n")
            uT("\\end{enumerate}\n"));

        m_pCode[13].set(
            uT("mathrm"),

            1,
            9,

            uT("\n\\mathrm{}\n"));

        m_pCode[14].set(
            uT("listings"),

            1,
            0,

            uT("\n% in the preambule\n")
            uT("\\usepackage{listings}\n")
            uT("\\usepackage{color}\n")
            uT("\n")
            uT("\\definecolor{darkgreen}{rgb}{0,0.5,0}\n")
            uT("\\definecolor{darkblue}{rgb}{0,0,0.5}\n")
            uT("\\definecolor{darkgray}{rgb}{0.5,0.5,0.5}\n")
            uT("\\definecolor{darkpink}{rgb}{0.6,0,0.5}\n")
            uT("\n")
            uT("\\lstset{\n")
            uT("    language=C,\n")
            uT("    basicstyle=\\footnotesize\\ttfamily,\n")
            uT("    breaklines=true,\n")
            uT("    commentstyle=\\color{darkgreen},\n")
            uT("    keepspaces=true,\n")
            uT("    keywordstyle=\\color{darkblue},\n")
            uT("    showspaces=false,\n")
            uT("    showstringspaces=false,\n")
            uT("    showtabs=false,\n")
            uT("    stringstyle=\\color{darkpink},\n")
            uT("    tabsize=4,\n")
            uT("}\n\n")
            uT("% inside the document\n")
            uT("\\begin{lstlisting}\n")
            uT("\n")
            uT("\\end{lstlisting}\n"));

        m_pCode[15].set(
            uT("bibliography (bibtex)"),

            1,
            0,

            uT("\n% in the preambule\n")
            uT("\\usepackage{natbib}\n\n")
            uT("% at the end of the document\n")
            uT("\\bibliographystyle{unsrt}\n")
            uT("\\bibliography{references}\n"));

        m_pCode[16].set(
            uT("bibliography (biblatex)"),

            1,
            0,

            uT("\n% in the preambule\n")
            uT("\\usepackage[style=numeric,defernumbers=true,sorting=none,maxnames=12,backend=biber]{biblatex}\n")
            uT("\\addbibresource{references.bib}\n\n")
            uT("% at the end of the document\n")
            uT("\\defbibheading{bibliographychapter}{%\n")
            uT("    \\chapter*{\\centering #1}%\n")
            uT("    \\markboth{Bibliography}{Bibliography}%\n")
            uT("    \\addstarredchapter{Bibliography}\n")
            uT("}\n")
            uT("\\begin{refcontext}[sorting=none]\n")
            uT("\\printbibliography[resetnumbers=true,heading=bibliographychapter,title={Bibliography}]\n")
            uT("\\end{refcontext}\n"));

        m_pCode[17].set(
            uT("biblatex: article"),

            1,
            10,

            uT("\n@article{,\n")
            uT("    author = {},\n")
            uT("    title = {},\n")
            uT("    journal = {},\n")
            uT("    volume = {},\n")
            uT("    pages = {},\n")
            uT("    year = {},\n")
            uT("    publisher = {},\n")
            uT("}\n"));

        m_pCode[18].set(
            uT("biblatex: book"),

            1,
            7,

            uT("\n@book{,\n")
            uT("    author = {},\n")
            uT("    title = {},\n")
            uT("    year = {},\n")
            uT("    publisher = {},\n")
            uT("}\n"));

        m_pCode[19].set(
            uT("biblatex: inproceedings"),

            1,
            16,

            uT("\n@inproceedings{,\n")
            uT("    author = {},\n")
            uT("    title = {},\n")
            uT("    booktitle = {},\n")
            uT("    pages = {},\n")
            uT("    year = {},\n")
            uT("    organization = {},\n")
            uT("}\n"));

        m_pCode[20].set(
            uT("biblatex: phdthesis"),

            1,
            12,

            uT("\n@phdthesis{,\n")
            uT("    author = {},\n")
            uT("    title = {},\n")
            uT("    year = {},\n")
            uT("    school = {},\n")
            uT("}\n"));
    }

    return true;
}

wxString CodeSample::getTemplate(int iLexer, int iLexerVar)
{
    wxString strT = wxEmptyString;
    wxDateTime dt = wxDateTime::Now();
    wxString strDate = dt.Format(uT("%Y-%m-%d %H:%M"));

    switch (iLexer) {
        case wxSTC_LEX_LUA:
            strT = uT("#!/opt/Comet/bin/comet -run\n\n");
            strT += uT("-- File: \n-- Generated: ");
            strT += strDate;
            strT += uT("\n-- License: ");
            strT += uT("\n-- Description: \n\n");
            strT += uT("print(\" \")\n");
            break;

        case wxSTC_LEX_CPP:

            strT = uT("// File: \n// Generated: ");
            strT += strDate;
            strT += uT("\n// License: ");
            strT += uT("\n// Description: \n\n");

            if (iLexerVar == ID_TEMPLATE_C) {
                strT += uT("#include <stdio.h>\n");
                strT += uT("#include <string.h>\n");
                strT += uT("#include <math.h>\n\n");
                strT += uT("int main(void)\n");
                strT += uT("{\n");
                if (CometFrame::getToolOut(wxSTC_LEX_CPP)) {
                    strT += uT("\t\n\tsetbuf(stdout, NULL);   /* disable buffering (to redirect stdout) */\n");
                }
                strT += uT("\n\tprintf(\" \\n\");\n\t\n\t\n");
            }

            else if (iLexerVar == ID_TEMPLATE_CMOD) {
                strT += uT("#include <lua.h>\n");
                strT += uT("#include <lualib.h>\n");
                strT += uT("#include <lauxlib.h>\n\n");

                strT += uT("#ifdef WIN32\n");
                strT += uT("#define CMODULE_API __declspec(dllexport)\n");
                strT += uT("#else\n");
                strT += uT("#define CMODULE_API\n");
                strT += uT("#endif\n\n");

                strT += uT("#if (LUA_VERSION_NUM == 501)\n");
                strT += uT("#define luaL_newlib(L,f)        luaL_register(L,\"cmodule\",f)\n");
                strT += uT("#endif\n\n");

                strT += uT("static int cmodule_version(lua_State* pLua)\n");
                strT += uT("{\n");
                strT += uT("\tlua_pushstring(pLua, \"CModule v0.1\");\n");
                strT += uT("\treturn 1;\n");
                strT += uT("}\n\n");

                strT += uT("static const luaL_Reg cmodule[] =\n");
                strT += uT("{\n");
                strT += uT("\t{ \"version\", cmodule_version },\n");
                strT += uT("\t{ NULL, NULL }\n");
                strT += uT("};\n\n");

                strT += uT("CMODULE_API int luaopen_cmodule(lua_State* pLua)\n");
                strT += uT("{\n");
                strT += uT("\tluaL_newlib(pLua, cmodule);\n");
                strT += uT("\treturn 1;\n");
                strT += uT("}\n");

                break;
            }

            else if (iLexerVar == ID_TEMPLATE_CPP) {
                strT += uT("#include <iostream>\n\n");
                strT += uT("using namespace std;\n\n");
                strT += uT("int main(void)\n");
                strT += uT("{\n");
                strT += uT("\tcout << \" \" << endl;\n\t\n\t\n");
            }

            strT += uT("\treturn 0;\n");
            strT += uT("}");
            break;

        case wxSTC_LEX_PYTHON:
#if defined(__WXGTK__)
            strT = uT("#!/usr/local/bin/python -u\n\n");
#endif
            strT += uT("# File: \n# Generated: ");
            strT += strDate;
            strT += uT("\n# License: ");
            strT += uT("\n# Description: \n\n");
            strT += uT("import numpy as np\n");
            strT += uT("import scipy as sp\n");
            strT += uT("import matplotlib as mpl\n");
            strT += uT("import matplotlib.pyplot as plt\n\n");
            strT += uT("arrT = np.arange(0.0, 1.0, 0.1)\n");
            strT += uT("plt.plot(arrT, arrT**2, 'ro')\n");
            strT += uT("plt.show()\n");
            break;
        case wxSTC_LEX_FORTRAN:
            strT = uT("! File: \n! Generated: ");
            strT += strDate;
            strT += uT("\n! License: ");
            strT += uT("\n! Description: \n\n");
            strT += uT("PROGRAM myprog\n");
            strT += uT("IMPLICIT NONE\n\n\n");
            strT += uT("END PROGRAM myprog\n");
            break;

        case wxSTC_LEX_MAKEFILE:
            strT = uT("# File: \n# Generated: ");
            strT += strDate;
            strT += uT("\n# License: ");
            strT += uT("\n# Description: \n\n");
            strT += uT("CC=gcc\n");
            strT += uT("CFLAGS=-c -Wall\n");
            strT += uT("LDFLAGS=\n");
            strT += uT("SRC=main.c func.c\n");
            strT += uT("OBJ=$(SRC:.c=.o)\n");
            strT += uT("EXE=test\n\n");
            strT += uT("default: $(SRC) $(EXE)\n\n");
            strT += uT("all: default\n\n");
            strT += uT("$(EXE): $(OBJ)\n");
            strT += uT("\t$(CC) $(LDFLAGS) $(OBJ) -o $@\n\n");
            strT += uT(".c.o:\n");
            strT += uT("\t$(CC) $(CFLAGS) $< -o $@\n\n");
            strT += uT("clean:\n");
            strT += uT("\t-rm -f *.o\n");
            strT += uT("\t-rm -f $(EXE)\n");
            break;

        case wxSTC_LEX_LATEX:
            strT = uT("% File: \n% Generated: ");
            strT += strDate;
            strT += uT("\n% License: ");
            strT += uT("\n% Description: \n\n");

            strT += uT("\\documentclass[11pt]{article}\n");
            strT += uT("\\usepackage[utf8]{inputenc}\n\n");

            strT += uT("\\begin{document}\n\n");

            strT += uT("\\title{Title}\n\n");
            strT += uT("\\author{Authors}\n\n");
            strT += uT("\\date{\\today}\n\n");
            strT += uT("\\maketitle\n\n");

            strT += uT("\\section{Introduction}\n\\label{sec:introduction}\n\n\n");

            strT += uT("\\section{Results and Discussion}\n\\label{sec:main}\n\n");

            strT += uT("\\subsection{Results}\n\\label{subsec:results}\n\n");

            strT += uT("\\subsection{Discussion}\n\\label{subsec:discussion}\n\n");

            strT += uT("\\section{Conclusion}\n\\label{sec:conclusion}\n\n\n");

            strT += uT("\\section*{References}\n");
            strT += uT("\\label{sec:references}\n\n");

            strT += uT("\\end{document}\n");
            break;

        case wxSTC_LEX_BATCH:
            strT = uT("rem File: \nrem Generated: ");
            strT += strDate;
            strT += uT("\nrem License: ");
            strT += uT("\nrem Description: \n\n");
            strT += uT("@echo off\n\n");
            strT += uT("SETLOCAL\n\n");
            strT += uT("@set CC=scc.exe\n");
            strT += uT("@set CFLAGS=\n\n");
            strT += uT("%CC% mytool.c -o mytool.exe\n");
            strT += uT("mytool.exe\n\n");
            strT += uT("ENDLOCAL\n");
            break;

        default:
            break;
    }

    return strT;
}