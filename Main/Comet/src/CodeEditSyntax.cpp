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
#include "CodeEdit.h"

#include <wx/stc/stc.h>

#define DEFAULT_COLOR_WHITE 0xFCFCFC
#define DEFAULT_COLOR_BLACK 0x202020

#ifdef WIN32
#define COMET_DEFAULT_FONTNAME uT("Consolas")
#define COMET_DEFAULT_FONTSIZE 9
#else
#define COMET_DEFAULT_FONTNAME uT("Monospace")
#define COMET_DEFAULT_FONTSIZE 9
#endif

const int CodeEdit::KNOWN_LEXER[] = {
    wxSTC_LEX_LUA,
    wxSTC_LEX_CPP,
    wxSTC_LEX_BASH,
    wxSTC_LEX_PYTHON,
    wxSTC_LEX_OCTAVE,
    wxSTC_LEX_FORTRAN,
    wxSTC_LEX_LATEX,
    wxSTC_LEX_MAKEFILE,
    wxSTC_LEX_COMET,
    wxSTC_LEX_BATCH,
    wxSTC_LEX_SOLIS,
    wxSTC_LEX_NULL,
    -1
};
const char_t *CodeEdit::KNOWN_LEXER_DESCRIPTION[] = {
    uT("Lua"),
    uT("C/C++"),
    uT("Bash"),
    uT("Python"),
    uT("Octave"),
    uT("Fortran"),
    uT("LaTeX"),
    uT("Makefile"),
    uT("Comet"),
    uT("Batch"),
    uT("Solis"),
    uT("None"),
    uT("-1")
};
bool CodeEdit::isLexerKnown(int iLexer)
{
    for (int ii = 0;; ii++) {
        if (-1 == CodeEdit::KNOWN_LEXER[ii]) {
            return false;
        }
        if (iLexer == CodeEdit::KNOWN_LEXER[ii]) {
            return true;
        }
    }
    return false;
}
int CodeEdit::getLexerByIndex(int ll)
{
    for (int ii = 0;; ii++) {
        if (-1 == CodeEdit::KNOWN_LEXER[ii]) {
            return -1;
        }
        if (ll == ii) {
            return CodeEdit::KNOWN_LEXER[ii];
        }
    }
    return -1;
}

const StyleInfoDefault CodeEdit::STYLE_LIGHT[] = {

    // Attention : 0xBBGGRR (0xFF0000 is blue and 0x0000FF is red)
    {
        wxSTC_LUA_DEFAULT, wxSTC_LUA_DEFAULT,
        DEFAULT_COLOR_BLACK, DEFAULT_COLOR_WHITE,
        COMET_DEFAULT_FONTNAME, COMET_DEFAULT_FONTSIZE, 0, 0
    },
    { 
        wxSTC_LUA_COMMENT, wxSTC_LUA_COMMENT,
        0x328C3C, DEFAULT_COLOR_WHITE,
        COMET_DEFAULT_FONTNAME, COMET_DEFAULT_FONTSIZE, 0, 0
    },
    { 
        wxSTC_LUA_COMMENTLINE, wxSTC_LUA_COMMENTLINE,
        0x328C3C, DEFAULT_COLOR_WHITE,
        COMET_DEFAULT_FONTNAME, COMET_DEFAULT_FONTSIZE, 0, 0
    },
    { 
        wxSTC_LUA_COMMENTDOC, wxSTC_LUA_COMMENTDOC,
        0x328C3C, DEFAULT_COLOR_WHITE,
        COMET_DEFAULT_FONTNAME, COMET_DEFAULT_FONTSIZE, 0, 0
    },
    { 
        wxSTC_LUA_NUMBER, wxSTC_LUA_NUMBER,
        0x808000, DEFAULT_COLOR_WHITE,
        COMET_DEFAULT_FONTNAME, COMET_DEFAULT_FONTSIZE, 0, 0
    },
    { 
        wxSTC_LUA_WORD, wxSTC_LUA_WORD,
        0xC00000, DEFAULT_COLOR_WHITE,
        COMET_DEFAULT_FONTNAME, COMET_DEFAULT_FONTSIZE, SCRIPT_STYLE_BOLD, 0
    },
    { 
        wxSTC_LUA_STRING, wxSTC_LUA_STRING,
        0xC80CDC, DEFAULT_COLOR_WHITE,
        COMET_DEFAULT_FONTNAME, COMET_DEFAULT_FONTSIZE, 0, 0
    },
    { 
        wxSTC_LUA_CHARACTER, wxSTC_LUA_CHARACTER,
        0xC80CDC, DEFAULT_COLOR_WHITE,
        COMET_DEFAULT_FONTNAME, COMET_DEFAULT_FONTSIZE, 0, 0
    },
    { 
        wxSTC_LUA_LITERALSTRING, wxSTC_LUA_LITERALSTRING,
        0xC80CDC, DEFAULT_COLOR_WHITE,
        COMET_DEFAULT_FONTNAME, COMET_DEFAULT_FONTSIZE, 0, 0
    },
    { 
        wxSTC_LUA_PREPROCESSOR, wxSTC_LUA_PREPROCESSOR,
        0x9243D8, DEFAULT_COLOR_WHITE,
        COMET_DEFAULT_FONTNAME, COMET_DEFAULT_FONTSIZE, 0, 0
    },
    { 
        wxSTC_LUA_OPERATOR, wxSTC_LUA_OPERATOR,
        DEFAULT_COLOR_BLACK, DEFAULT_COLOR_WHITE,
        COMET_DEFAULT_FONTNAME, COMET_DEFAULT_FONTSIZE, 0, 0
    },
    { 
        wxSTC_LUA_IDENTIFIER, wxSTC_LUA_IDENTIFIER,
        DEFAULT_COLOR_BLACK, DEFAULT_COLOR_WHITE,
        COMET_DEFAULT_FONTNAME, COMET_DEFAULT_FONTSIZE, 0, 0
    },
    { 
        wxSTC_LUA_STRINGEOL, wxSTC_LUA_STRINGEOL,
        0xC80CDC, DEFAULT_COLOR_WHITE,
        COMET_DEFAULT_FONTNAME, COMET_DEFAULT_FONTSIZE, 0, 0
    },
    { 
        wxSTC_LUA_WORD2, wxSTC_LUA_WORD2,
        0xDC0064, DEFAULT_COLOR_WHITE,
        COMET_DEFAULT_FONTNAME, COMET_DEFAULT_FONTSIZE, 0, 0
    },
    { 
        wxSTC_LUA_WORD3, wxSTC_LUA_WORD3,
        0xA00050, DEFAULT_COLOR_WHITE,
        COMET_DEFAULT_FONTNAME, COMET_DEFAULT_FONTSIZE, 0, 0
    },
    { 
        wxSTC_LUA_WORD4, wxSTC_LUA_WORD4,
        0xA00050, DEFAULT_COLOR_WHITE,
        COMET_DEFAULT_FONTNAME, COMET_DEFAULT_FONTSIZE, 0, 0
    },
    { 
        wxSTC_LUA_WORD5, wxSTC_LUA_WORD5,
        0xA00050, DEFAULT_COLOR_WHITE,
        COMET_DEFAULT_FONTNAME, COMET_DEFAULT_FONTSIZE, 0, 0
    },
    { 
        wxSTC_LUA_WORD6, wxSTC_LUA_WORD6,
        0xA00050, DEFAULT_COLOR_WHITE,
        COMET_DEFAULT_FONTNAME, COMET_DEFAULT_FONTSIZE, 0, 0
    },
    { 
        wxSTC_LUA_WORD7, wxSTC_LUA_WORD7,
        0xA00050, DEFAULT_COLOR_WHITE,
        COMET_DEFAULT_FONTNAME, COMET_DEFAULT_FONTSIZE, 0, 0
    },
    { 
        wxSTC_LUA_WORD8, wxSTC_LUA_WORD8,
        0xA00050, DEFAULT_COLOR_WHITE,
        COMET_DEFAULT_FONTNAME, COMET_DEFAULT_FONTSIZE, 0, 0
    },
    // LINE NUMBER
    {
        STYLEINFO_INDEX_LINENUM, STYLEINFO_INDEX_LINENUM,
        0x827E3E, 0xECECEC,
        COMET_DEFAULT_FONTNAME, COMET_DEFAULT_FONTSIZE, 0, 0
    },
    // CARET
    {
        STYLEINFO_INDEX_CARET, STYLEINFO_INDEX_CARET,
        DEFAULT_COLOR_BLACK, 0xF8F4F0,
        COMET_DEFAULT_FONTNAME, COMET_DEFAULT_FONTSIZE, 0, 0
    },
    // SELECTION
    {
        STYLEINFO_INDEX_SELECTION, STYLEINFO_INDEX_SELECTION,
        DEFAULT_COLOR_BLACK, 0xF6D0AB,
        COMET_DEFAULT_FONTNAME, COMET_DEFAULT_FONTSIZE, 0, 0
    },
    // WHITESPACE
    {
        STYLEINFO_INDEX_WHITESPACE, STYLEINFO_INDEX_WHITESPACE,
        0x817357, DEFAULT_COLOR_WHITE,
        COMET_DEFAULT_FONTNAME, COMET_DEFAULT_FONTSIZE, 0, 0
    }
};

const StyleInfoDefault CodeEdit::STYLE_DARK[] = {

    // Attention : 0xBBGGRR (0xFF0000 is blue and 0x0000FF is red)
    {
        wxSTC_LUA_DEFAULT, wxSTC_LUA_DEFAULT,
        DEFAULT_COLOR_WHITE, DEFAULT_COLOR_BLACK,
        COMET_DEFAULT_FONTNAME, COMET_DEFAULT_FONTSIZE, 0, 0
    },
    { 
        wxSTC_LUA_COMMENT, wxSTC_LUA_COMMENT,
        0x96C8A0, DEFAULT_COLOR_BLACK,
        COMET_DEFAULT_FONTNAME, COMET_DEFAULT_FONTSIZE, 0, 0
    },
    { 
        wxSTC_LUA_COMMENTLINE, wxSTC_LUA_COMMENTLINE,
        0x96C8A0, DEFAULT_COLOR_BLACK,
        COMET_DEFAULT_FONTNAME, COMET_DEFAULT_FONTSIZE, 0, 0
    },
    { 
        wxSTC_LUA_COMMENTDOC, wxSTC_LUA_COMMENTDOC,
        0x96C8A0, DEFAULT_COLOR_BLACK,
        COMET_DEFAULT_FONTNAME, COMET_DEFAULT_FONTSIZE, 0, 0
    },
    { 
        wxSTC_LUA_NUMBER, wxSTC_LUA_NUMBER,
        0xE6E6A0, DEFAULT_COLOR_BLACK,
        COMET_DEFAULT_FONTNAME, COMET_DEFAULT_FONTSIZE, 0, 0
    },
    { 
        wxSTC_LUA_WORD, wxSTC_LUA_WORD,
        0xFFC9A9, DEFAULT_COLOR_BLACK,
        COMET_DEFAULT_FONTNAME, COMET_DEFAULT_FONTSIZE, SCRIPT_STYLE_BOLD, 0
    },

    { 
        wxSTC_LUA_STRING, wxSTC_LUA_STRING,
        0xDCB4E6, DEFAULT_COLOR_BLACK,
        COMET_DEFAULT_FONTNAME, COMET_DEFAULT_FONTSIZE, 0, 0
    },
    { 
        wxSTC_LUA_CHARACTER, wxSTC_LUA_CHARACTER,
        0xDCB4E6, DEFAULT_COLOR_BLACK,
        COMET_DEFAULT_FONTNAME, COMET_DEFAULT_FONTSIZE, 0, 0
    },
    { 
        wxSTC_LUA_LITERALSTRING, wxSTC_LUA_LITERALSTRING,
        0xDCB4E6, DEFAULT_COLOR_BLACK,
        COMET_DEFAULT_FONTNAME, COMET_DEFAULT_FONTSIZE, 0, 0
    },
    { 
        wxSTC_LUA_PREPROCESSOR, wxSTC_LUA_PREPROCESSOR,
        0xE2B6D7, DEFAULT_COLOR_BLACK,
        COMET_DEFAULT_FONTNAME, COMET_DEFAULT_FONTSIZE, 0, 0
    },
    { 
        wxSTC_LUA_OPERATOR, wxSTC_LUA_OPERATOR,
        DEFAULT_COLOR_WHITE, DEFAULT_COLOR_BLACK,
        COMET_DEFAULT_FONTNAME, COMET_DEFAULT_FONTSIZE, 0, 0
    },
    { 
        wxSTC_LUA_IDENTIFIER, wxSTC_LUA_IDENTIFIER,
        DEFAULT_COLOR_WHITE, DEFAULT_COLOR_BLACK,
        COMET_DEFAULT_FONTNAME, COMET_DEFAULT_FONTSIZE, 0, 0
    },
    { 
        wxSTC_LUA_STRINGEOL, wxSTC_LUA_STRINGEOL,
        0xDCB4E6, DEFAULT_COLOR_BLACK,
        COMET_DEFAULT_FONTNAME, COMET_DEFAULT_FONTSIZE, 0, 0
    },
    { 
        wxSTC_LUA_WORD2, wxSTC_LUA_WORD2,
        0xFFA9C7, DEFAULT_COLOR_BLACK,
        COMET_DEFAULT_FONTNAME, COMET_DEFAULT_FONTSIZE, 0, 0
    },
    { 
        wxSTC_LUA_WORD3, wxSTC_LUA_WORD3,
        0xFFA9C7, DEFAULT_COLOR_BLACK,
        COMET_DEFAULT_FONTNAME, COMET_DEFAULT_FONTSIZE, 0, 0
    },
    { 
        wxSTC_LUA_WORD4, wxSTC_LUA_WORD4,
        0xFFA9C7, DEFAULT_COLOR_BLACK,
        COMET_DEFAULT_FONTNAME, COMET_DEFAULT_FONTSIZE, 0, 0
    },
    { 
        wxSTC_LUA_WORD5, wxSTC_LUA_WORD5,
        0xFFA9C7, DEFAULT_COLOR_BLACK,
        COMET_DEFAULT_FONTNAME, COMET_DEFAULT_FONTSIZE, 0, 0
    },
    { 
        wxSTC_LUA_WORD6, wxSTC_LUA_WORD6,
        0xFFA9C7, DEFAULT_COLOR_BLACK,
        COMET_DEFAULT_FONTNAME, COMET_DEFAULT_FONTSIZE, 0, 0
    },
    { 
        wxSTC_LUA_WORD7, wxSTC_LUA_WORD7,
        0xFFA9C7, DEFAULT_COLOR_BLACK,
        COMET_DEFAULT_FONTNAME, COMET_DEFAULT_FONTSIZE, 0, 0
    },
    { 
        wxSTC_LUA_WORD8, wxSTC_LUA_WORD8,
        DEFAULT_COLOR_WHITE, 0x82641E,
        COMET_DEFAULT_FONTNAME, COMET_DEFAULT_FONTSIZE, 0, 0
    },
    // LINE NUMBER
    {
        STYLEINFO_INDEX_LINENUM, STYLEINFO_INDEX_LINENUM,
        0xEBEBDF, 0x464646,
        COMET_DEFAULT_FONTNAME, COMET_DEFAULT_FONTSIZE, 0, 0
    },
    // CARET
    {
        STYLEINFO_INDEX_CARET, STYLEINFO_INDEX_CARET,
        DEFAULT_COLOR_WHITE, 0x82641E,
        COMET_DEFAULT_FONTNAME, COMET_DEFAULT_FONTSIZE, 0, 0
    },

    // SELECTION
    {
        STYLEINFO_INDEX_SELECTION, STYLEINFO_INDEX_SELECTION,
        DEFAULT_COLOR_WHITE, 0x6F1F0F,
        COMET_DEFAULT_FONTNAME, COMET_DEFAULT_FONTSIZE, 0, 0
    },

    // WHITESPACE
    {
        STYLEINFO_INDEX_WHITESPACE, STYLEINFO_INDEX_WHITESPACE,
        0xDBCFC3, DEFAULT_COLOR_BLACK,
        COMET_DEFAULT_FONTNAME, COMET_DEFAULT_FONTSIZE, 0, 0
    }
};

const int CodeEdit::AUTOC_API_LUA_LEN = 3;

const AutoCList CodeEdit::AUTOC_API_LUA[] = {

    { 2, uT("no"), 7, 4, uT("break|function|return|while") },
    { 2, uT("no"), 14, 23, uT("assert|collectgarbage|coroutine|dofile|error|getmetatable|ipairs|loadfile|loadstring|next|package|pairs|print|rawget|rawset|require|setmetatable|string|table|time|tonumber|tostring|type") },
    { 2, uT("no"), 6, 7, uT("gui|math|socket") },

    { 3, uT("io."), 7, 14, uT("close|flush|input|lines|open|output|popen|read|stderr|stdin|stdout|tmpfile|type|write") },
    { 3, uT("os."), 9, 11, uT("clock|date|difftime|execute|exit|getenv|remove|rename|setlocale|time|tmpname") },
    { 6, uT("table."), 6, 6, uT("concat|insert|pack|remove|sort|unpack") },
    { 7, uT("string."), 7, 14, uT("byte|char|dump|find|format|gmatch|gsub|len|lower|match|rep|reverse|sub|upper") },
    { 10, uT("coroutine."), 7, 6, uT("create|resume|running|status|wrap|yield") },
    { 8, uT("package."), 7, 7, uT("cpath|loaded|loaders|loadlib|path|preload|seeall") },

    { 5, uT("math."), 10, 56, uT("abs|acos|acosh|asin|asinh|atan|atan2|atanh|c|cbrt|ceil|cos|cosh|deg|erf|erfc|exp|exp2|floor|fmod|frexp|gauss|h|huge|hypot|isinf|isnan|isnormal|ldexp|lgamma|log|log10|logb|max|min|modf|na|pi|pow|q|rad|random|randomseed|rint|round|sin|sinh|sqrt|tan|tanh|tgamma|trunc") },

    // FFI C/LUA
    { 4, uT("ffi."), 8, 17, uT("arch|C|cast|cdef|copy|errno|fill|istype|load|metatype|new|os|sizeof|string|typeof|tonumber|tostring") },
    { 6, uT("ffi.C."), 7, 30, uT("acos|asin|atan|atan2|ceil|clock|cos|cosh|exp|fabs|floor|fmod|frexp|getchar|isdigit|isalpha|isalnum|ldexp|log|log10|printf|sqrt|putchar|rand|scanf|srand|strcat|strcmp|strcpy|time") },

    // TIME
    { 5, uT("time."), 6, 4, uT("format|sleep|tic|toc") },

    // GUI
    { 4, uT("gui."), 5, 2, uT("alert|ask") },

    // SOCKET
    { 7, uT("socket."), 16, 51, uT("accept|bind|close|connect|delete|geterr|gethostbyaddr|gethostbyname|gethostname|getpeername|getsockname|iserr|listen|new|recv|recvfrom|send|sendto|setsockopt|shutdown|timeout|AF_ECMA|AF_INET|AF_INET6|AF_INET6|AF_IRDA|AF_UNIX|AF_UNSPEC|INADDR_ANY|INADDR_BROADCAST|INADDR_LOOPBACK|INADDR_NONE|IPPROTO_TCP|IPPROTO_UDP|MSG_CTRUNC|MSG_DONTROUTE|MSG_OOB|MSG_PEEK|MSG_TRUNC|MSG_WAITALL|SO_ACCEPTCONN|SO_DONTROUTE|SO_KEEPALIVE|SO_LINGER|SO_RCVTIMEO|SO_REUSEADDR|SO_SNDTIMEO|SOCK_DGRAM|SOCK_RAW|SOCK_SEQPACKET|SOCK_STREAM") },

    { 0, NULL, 0, 0, NULL }
};

const CallTipList CodeEdit::CALLTIP_API_LUA[] = {

    // STANDARD
    { 7, uT("assert("), uT("assert(arg [, message])\nIssues an error when arg is nil or false\n the default message is \"assertion failed!\"") },
    { 15, uT("collectgarbage("), uT("collectgarbage(opt [, arg])\nGarbage collector (GC) generic function, where opt is:\n \"stop\": stops GC\n \"restart\": restarts GC\n \"collect\": performs a full GC cycle\n \"count\": returns the memory in use (Kb)\n \"setstepmul\": sets arg as the GC step multiplier\n \"step\": runs one GC step") },
    { 7, uT("dofile("), uT("dofile(filename)\nOpens file and executes it") },
    { 6, uT("error("), uT("error(message [, level])\nRaises an error with the given error message\n level sets the error position:\n  1: where the error function was called\n  2: where the function that called error was called\n  etc...") },
    { 7, uT("ipairs("), uT("ipairs(t)\nReturns an iterator of type integer to traverse a table") },
    { 9, uT("loadfile("), uT("loadfile(filename)\nLoads file and returns the compiled chunk as a function") },
    { 11, uT("loadstring("), uT("loadstring(s)\nLoads string and returns the compiled chunk as a function") },
    { 5, uT("next("), uT("next(table [, index])\nTraverses table and returns the next index and its value\n if index is nil, returns the first index and its value") },
    { 6, uT("pairs("), uT("pairs(t)\nReturns an iterator to traverse a table") },
    { 6, uT("print("), uT("print(args)\nPrints args to the standard output") },
    { 8, uT("require("), uT("require(modname)\nLoads the given named module") },
    { 5, uT("type("), uT("type(arg)\nReturns the type of arg") },

    { 9, uT("io.close("), uT("io.close(handle)\nClose file handle") },
    { 8, uT("io.open("), uT("io.open(filename, mode)\nOpen filename where mode is:\n \"r\" (read), \"w\" (write), \"a\" (append) and \"b\" (binary)\n Returns handle for the file") },
    { 8, uT("io.read("), uT("io.read(arg)\nRead strings from the current input file, where arg is:\n \"*all\" (whole file) \n \"*line\" (next line) \n \"*line\" (next line) \n \"*number\" (number)") },
    { 9, uT("io.write("), uT("io.write(args)\nPrint args to the current output file") },

    { 9, uT("os.clock("), uT("os.clock()\nReturns CPU time in seconds") },
    { 8, uT("os.date("), uT("os.date([format [, time]])\nReturns formatted date and time, where:\n format: date/time format\n if the time is not given, the current time will be used") },
    { 8, uT("os.time("), uT("os.time([table])\nReturns the current time\n If table [y,m,d,h,m,s] is given, returns the corresponding time") },
    { 12, uT("os.difftime("), uT("os.difftime(t2, t1)\nReturns the time difference between t2 and t1 in seconds") },
    { 11, uT("os.execute("), uT("os.execute([command])\nExecute shell command\n if no command given, returns nonzero if a shell is available") },

    { 13, uT("table.concat("), uT("table.concat(table [, sep [, i [, j]]])\nReturns table[i] .. sep .. table[i+1] ... sep .. table[j]\n Default values: sep = empty string, i = 1, j = table length") },
    { 13, uT("table.insert("), uT("table.insert(table, [pos,] value)\nInserts value at position pos in table\n if pos is not given, value will be appended to the table") },
    { 13, uT("table.remove("), uT("table.remove(table [, pos])\nRemoves from table the element at position pos\n Returns the value of the removed element\n if pos is not given, the last element will be removed") },
    { 11, uT("table.sort("), uT("table.sort(table [, comp])\nSorts table elements\n if comp function is given, it will be used for < comparison\n comp prototype: isless = comp(elem1,elem2)\n otherwise the standard < operator will be used") },

    { 12, uT("string.find("), uT("string.find(s, pattern [, init [, plain]])\nLooks for the first match of pattern in the string s\n Returns the indices where pattern starts and ends in s, or nil") },
    { 14, uT("string.format("), uT("string.format(formatstring, ...)\nFormats the specified arguments into string\n Comparable to the sprintf C standard function\n Example: \n  i = 10\n  x = 1.5\n  s = string.format(\"%d %f\", i, x)") },
    { 11, uT("string.len("), uT("string.len(s)\nReturns the string length") },
    { 13, uT("string.lower("), uT("string.lower(s)\nReturns string with the uppercase letters changed to lowercase") },
    { 13, uT("string.upper("), uT("string.upper(s)\nReturns string with the lowercase letters changed to uppercase") },
    { 15, uT("string.reverse("), uT("string.reverse(s)\nReturns the string s reversed") },
    { 11, uT("string.sub("), uT("string.sub(s, i [, j])\nReturns the substring of s from position i to j (or to s length if j is not given)") },

    { 17, uT("coroutine.create("), uT("coroutine.create(f)\nCreates and returns a new coroutine with the function f") },
    { 17, uT("coroutine.resume("), uT("coroutine.resume(co [, val1, ...])\nStarts or continues the execution of coroutine co") },
    { 17, uT("coroutine.status("), uT("coroutine.status(co)\nReturns the status of coroutine co: \"running\" or \"suspended\"") },
    { 16, uT("coroutine.yield("), uT("coroutine.yield(...)\nSuspends the execution of the calling coroutine") },

    // FFI C/LUA
    { 9, uT("ffi.cast("), uT("ffi.cast(ct, init)\nInitializes ct with init using the cast") },
    { 9, uT("ffi.cdef("), uT("ffi.abi(...)\nC declaration,e.g.\nffi.cdef[[\n  double calc(double x);\n]]") },
    { 9, uT("ffi.copy("), uT("ffi.copy(dst, src, len)\nCopies the data pointed to by src to dst") },
    { 10, uT("ffi.errno("), uT("err = ffi.errno([newerr])\nReturns the error number set C function call") },
    { 9, uT("ffi.fill("), uT("ffi.fill(dst, len [,c])\nFills dst with len bytes by c (or 0 if c omitted)") },
    { 11, uT("ffi.istype("), uT("status = ffi.istype(ct, obj)\nReturns true if obj has the C type given by ct") },
    { 9, uT("ffi.load("), uT("clib = ffi.load(name [,global])\nLoads dynamic library, e.g.:\nclapack = ffi.load(\"lapack\")") },
    { 13, uT("ffi.metatype("), uT("ctype = ffi.metatype(ct, metatable)\nCreates a ctype object and associates it with a metatable") },
    { 8, uT("ffi.new("), uT("cdata = ffi.new(ct [,nelem] [,init...])\nCreates C data, e.g.:\nlocal sz = ffi.new(\"char[?]\", 64)") },
    { 11, uT("ffi.sizeof("), uT("size = ffi.sizeof(ct [,nelem])\nReturns the size of ct in bytes") },
    { 11, uT("ffi.string("), uT("str = ffi.string(ptr [,len])\nCreates Lua string from ptr") },
    { 13, uT("ffi.tonumber("), uT("n = tonumber(cdata)\nConverts cdata to a Lua number") },
    { 13, uT("ffi.tostring("), uT("s = tostring(cdata)\nConverts cdata to a Lua string") },
    { 11, uT("ffi.typeof("), uT("ctype = ffi.typeof(ct)\nCreates a C type object") },

    // OUTPUT
    { 4, uT("cls("), uT("cls()\nClear the output window") },

    // TIME
    { 9, uT("time.tic("), uT("time.tic()\nStart the timer") },
    { 9, uT("time.toc("), uT("ems = time.toc()\nStop the timer\nReturns the number of milliseconds elapsed since the last time.tic() call") },
    { 11, uT("time.sleep("), uT("time.sleep(dt)\nSleep during dt milliseconds") },
    { 12, uT("time.format("), uT("s = time.format(t)\nFormat time to appropriate string, where:\nt: time in milliseconds\nReturns formatted string") },

    // WIN
    { 10, uT("gui.alert("), uT("gui.alert(msg)\nDisplay message box") },
    { 8, uT("gui.ask("), uT("answ = gui.ask(msg)\nDisplay dialog box with YES,NO buttons\nReturns 1 if YES button pressed or 0 otherwise") },

    // SOCKET
    { 11, uT("socket.new("), uT("s = socket.new(af,type,proto)\nCreate socket where:\n af: network family (socket.AF_INET by default)\n type: socket type (socket.SOCK_STREAM by default)\n proto: Protocol (socket.IPPROTO_TCP by default)\nReturnss the socket identifier") },
    { 12, uT("socket.bind("), uT("ok = socket.bind(s, addr, port)\nBind socket s, where:\n s: socket identifier\n addr: address (usually addr = socket.INADDR_ANY)\n port: port to bind to\nReturns true on success") },
    { 14, uT("socket.listen("), uT("ok = socket.listen(s, backlog)\nlisten on socket s, where:\n s: socket identifier\n backlog: maximum queue length\nReturns true on success") },
    { 15, uT("socket.connect("), uT("ok = socket.connect(s, addr, port)\nConnect s, where:\n s: socket identifier\n addr: address\n port: port to connect to\nReturns true on success") },
    { 14, uT("socket.accept("), uT("sa = socket.accept(s)\nAccept connection and create new socket, where:\n s: socket identifier\nReturns the new socket identifier") },
    { 15, uT("socket.timeout("), uT("ok = socket.timeout(s, to)\nsSet the recv and send timeout, where:\n s: socket identifier\n to: timeout in milliseconds\nReturns true on success") },
    { 18, uT("socket.setsockopt("), uT("ok = socket.setsockopt(s, opt, val)\nSet socket option, where:\n s: socket identifier\n opt: option to set (ex: socket.SO_SNDTIMEO)\n val: option value\nReturns true on success") },
    { 19, uT("socket.getpeername("), uT("pn = socket.getpeername(s)\nGet the socket s peer ip address, where:\n s: socket identifier\nReturns peer ip address") },
    { 19, uT("socket.gethostname("), uT("hn = socket.gethostname(s)\nGet the local host name, where:\n s: socket identifier\nReturns host name") },
    { 21, uT("socket.gethostbyaddr("), uT("hn = socket.gethostbyaddr(s, addr)\nGet the host name corresponding to ip address, where:\n s: socket identifier\n addr: ip address\nReturns host name") },
    { 21, uT("socket.gethostbyname("), uT("ha = socket.gethostbyname(s, name)\nGet the host ip address corresponding to host name, where:\n s: socket identifier\n name: host name\nReturns ip address") },
    { 19, uT("socket.getsockname("), uT("sn = socket.getsockname(s)\nGet the socket name, where:\n s: socket identifier\nReturns socket name") },
    { 12, uT("socket.send("), uT("ok = socket.send(s, data, flags)\nSend data, where:\n s: socket identifier\n data: data to send\n flags: optional flags\nReturns true on success") },
    { 14, uT("socket.sendto("), uT("ok = socket.sendto(s, addr, port, data, flags)\nSend data to, where:\n s: socket identifier\n addr: remote address\n port: remote port\ndata: data to send\n flags: optional flags\nReturns true on success") },
    { 12, uT("socket.recv("), uT("data = socket.recv(s, datasize, flags)\nReceive data, where:\n s: socket identifier\n datasize: size of the data to be received\n flags: optional flags\nReturns received data") },
    { 16, uT("socket.recvfrom("), uT("data = socket.recvfrom(s, addr, port, datasize, flags)\nReceive data, where:\n s: socket identifier\n addr: remote address\n port: remote port\n datasize: size of the data to be received\nflags: optional flags\nReturns received data") },
    { 13, uT("socket.iserr("), uT("errf = socket.iserr(s)\nGet the error flag, where:\n s: socket identifier\nReturns true if error flag set") },
    { 14, uT("socket.geterr("), uT("errm = socket.geterr(s)\nGet the error message, where:\n s: socket identifier\nReturns the error message, if any") },
    { 16, uT("socket.shutdown("), uT("socket.shutdown(s)\nShutdown socket, where:\n s: socket identifier") },
    { 13, uT("socket.close("), uT("socket.close(s)\nClose socket, where:\n s: socket identifier") },
    { 14, uT("socket.delete("), uT("socket.delete(s)\nDelete socket and free resources, where:\n s: socket identifier") },

    { 0, NULL, NULL }
};

const DwellList CodeEdit::DWELL_API_LUA[] = {

    // STANDARD
    { 6, uT("assert"), uT("Issues an error") },
    { 14, uT("collectgarbage"), uT("Garbage collector generic function") },
    { 6, uT("dofile"), uT("Opens file and executes it") },
    { 5, uT("error"), uT("Raises an error with the given error message") },
    { 6, uT("ipairs"), uT("Returns an iterator of type integer to traverse a table") },
    { 8, uT("loadfile"), uT("Loads file and returns the compiled chunk as a function") },
    { 10, uT("loadstring"), uT("Loads string and returns the compiled chunk as a function") },
    { 4, uT("next"), uT("Traverses table and returns the next index and its value") },
    { 5, uT("pairs"), uT("Returns an iterator to traverse a table") },
    { 5, uT("print"), uT("Prints arguments to the standard output") },
    { 7, uT("require"), uT("Loads the named module") },
    { 4, uT("type"), uT("Returns the type of variable") },

    { 8, uT("io.close"), uT("Closes file handle") },
    { 7, uT("io.open"), uT("Opens file (returns handle)") },
    { 7, uT("io.read"), uT("Reads from the current input file") },
    { 8, uT("io.write"), uT("Prints to the current output file") },

    { 8, uT("os.clock"), uT("Returns CPU time in seconds") },
    { 7, uT("os.date"), uT("Returns formatted date and time") },
    { 7, uT("os.time"), uT("Returns the current time") },
    { 11, uT("os.difftime"), uT("Returns the time difference in seconds") },
    { 10, uT("os.execute"), uT("Execute shell command") },

    { 12, uT("table.concat"), uT("Returns table[i] .. sep .. table[i+1] ... sep .. table[j]") },
    { 12, uT("table.insert"), uT("Inserts value in table") },
    { 12, uT("table.remove"), uT("Removes element from table") },
    { 10, uT("table.sort"), uT("Sorts table elements") },

    { 11, uT("string.find"), uT("Looks for the first match of pattern in string") },
    { 13, uT("string.format"), uT("Formats arguments into string") },
    { 10, uT("string.len"), uT("Returns the string length") },
    { 12, uT("string.lower"), uT("Returns string with the uppercase letters changed to lowercase") },
    { 12, uT("string.upper"), uT("Returns string with the lowercase letters changed to uppercase") },
    { 14, uT("string.reverse"), uT("Returns the string reversed") },
    { 10, uT("string.sub"), uT("Returns substring") },

    { 16, uT("coroutine.create"), uT("Creates and returns a new coroutine") },
    { 16, uT("coroutine.resume"), uT("Starts or continues the execution of coroutine") },
    { 16, uT("coroutine.status"), uT("Returns the status of coroutine") },
    { 15, uT("coroutine.yield"), uT("Suspends the execution of the calling coroutine") },

    // FFI C/LUA
    { 8, uT("ffi.arch"), uT("architecture name") },
    { 5, uT("ffi.C"), uT("C library namespace") },
    { 8, uT("ffi.cast"), uT("Initializes using the cast") },
    { 8, uT("ffi.cdef"), uT("C declaration") },
    { 8, uT("ffi.copy"), uT("Copies data pointed to by src to dst") },
    { 9, uT("ffi.errno"), uT("Returns the error number") },
    { 8, uT("ffi.fill"), uT("Fills with bytes") },
    { 10, uT("ffi.istype"), uT("Checks the given C type") },
    { 8, uT("ffi.load"), uT("Loads dynamic library") },
    { 12, uT("ffi.metatype"), uT("Creates and associates object with metatable") },
    { 7, uT("ffi.new"), uT("Creates C data") },
    { 6, uT("ffi.os"), uT("OS name") },
    { 10, uT("ffi.sizeof"), uT("Returns the size in bytes") },
    { 10, uT("ffi.string"), uT("Creates Lua string from C data") },
    { 12, uT("ffi.tonumber"), uT("Converts C data to a Lua number") },
    { 12, uT("ffi.tostring"), uT("Converts C data to a Lua string") },
    { 10, uT("ffi.typeof"), uT("Creates a C type object") },

    // OUTPUT
    { 3, uT("cls"), uT("Clear the output window") },

    // TIME
    { 8, uT("time.tic"), uT("Start the timer") },
    { 8, uT("time.toc"), uT("Stop the timer") },
    { 10, uT("time.sleep"), uT("Sleep during dt milliseconds") },
    { 11, uT("time.format"), uT("Format time to appropriate string") },

    // WIN
    { 9, uT("gui.alert"), uT("Display message box") },
    { 7, uT("gui.ask"), uT("Display dialog box with YES,NO buttons") },

    // SOCKET
    { 10, uT("socket.new"), uT("New socket") },
    { 11, uT("socket.bind"), uT("Bind socket") },
    { 13, uT("socket.listen"), uT("Listen on socket") },
    { 14, uT("socket.connect"), uT("Connect socket") },
    { 13, uT("socket.accept"), uT("Accept connection") },
    { 14, uT("socket.timeout"), uT("Set timeout") },
    { 17, uT("socket.setsockopt"), uT("Set socket option") },
    { 18, uT("socket.getpeername"), uT("Get the socket peer ip address") },
    { 18, uT("socket.gethostname"), uT("Get the local host name") },
    { 20, uT("socket.gethostbyaddr"), uT("Get the host name corresponding to ip address") },
    { 20, uT("socket.gethostbyname"), uT("Get the host ip address corresponding to host name") },
    { 18, uT("socket.getsockname"), uT("Get the socket name") },
    { 11, uT("socket.send"), uT("Send data") },
    { 13, uT("socket.sendto"), uT("Send data") },
    { 11, uT("socket.recv"), uT("Receive data") },
    { 15, uT("socket.recvfrom"), uT("Receive data") },
    { 12, uT("socket.iserr"), uT("Get the error flag") },
    { 13, uT("socket.geterr"), uT("Get the error message") },
    { 15, uT("socket.shutdown"), uT("Shutdown socket") },
    { 12, uT("socket.close"), uT("Close socket") },
    { 13, uT("socket.delete"), uT("Delete socket and free resources") },

    { 0, NULL, NULL }
};

const int CodeEdit::AUTOC_API_CPP_LEN = 5;

const AutoCList CodeEdit::AUTOC_API_CPP[] = {

    { 2, uT("no"), 15, 33, uT("break|case|catch (...) { }|continue|delete |else|enum |extern |false|for ( ) { }|friend |goto|inline |namespace |new|nullptr|operator|private |protected |public |register |static |switch ( ) { }|this|throw|true|try { }|typedef |using |virtual |void |volatile |while ( ) { }") },
    { 2, uT("no"), 20, 19, uT("bool |class |const |const_cast<>() |char |double |dynamic_cast<>()|float |int |long |reinterpret_cast<>()|short |signed |sizeof( )|static_cast<>()|struct |union |unsigned |wchar_t ") },
    { 2, uT("no"), 12, 5, uT("#define |#else |#endif|#ifdef |#include <.h>") },
    { 2, uT("no"), 7, 16, uT("acos()|asin()|atan()|atan2()|ceil()|cos()|cosh()|exp()|fabs()|floor()|fmod()|frexp()|ldexp()|log()|log10()|sqrt()") },
    { 2, uT("no"), 9, 17, uT("clock()|free()|getchar()|isdigit()|isalpha()|isalnum()|malloc()|memset()|printf()|putchar()|rand()|scanf()|srand()|strcat()|strcmp()|strcpy()|time()") },
    { 0, NULL, 0, 0, NULL }
};

const int CodeEdit::AUTOC_API_C_LEN = 5;

const AutoCList CodeEdit::AUTOC_API_C[] = {

    { 2, uT("no"), 13, 18, uT("break|case|continue|else|enum |extern |False|for ( ) { }|goto|inline |return|static |switch ( ) { }|True|typedef |void |volatile |while ( ) { }") },
    { 2, uT("no"), 9, 14, uT("_Bool |const |char |double |float |int |long |short |signed |sizeof( )|struct |union |unsigned |wchar_t ") },
    { 2, uT("no"), 12, 5, uT("#define |#else |#endif|#ifdef |#include <.h>") },
    { 2, uT("no"), 7, 16, uT("acos()|asin()|atan()|atan2()|ceil()|cos()|cosh()|exp()|fabs()|floor()|fmod()|frexp()|ldexp()|log()|log10()|sqrt()") },
    { 2, uT("no"), 9, 17, uT("clock()|free()|getchar()|isdigit()|isalpha()|isalnum()|malloc()|memset()|printf()|putchar()|rand()|scanf()|srand()|strcat()|strcmp()|strcpy()|time()") },
    { 0, NULL, 0, 0, NULL }
};

const int CodeEdit::AUTOC_API_PYTHON_LEN = 1;

const AutoCList CodeEdit::AUTOC_API_PYTHON[] = {

    { 2, uT("no"), 9, 32, uT("and |del |from |not |while |self|elif |global |with |assert |else:|pass|yield |break|except |import |print( )|class |exec |raise |range |continue|finally|return|def |for |lambda |try |False|None|True|nonlocal ") },
    { 0, NULL, 0, 0, NULL }
};

const int CodeEdit::AUTOC_API_LATEX_LEN = 21;

const AutoCList CodeEdit::AUTOC_API_LATEX[] = {

    { 2, uT("no"), 12, 12, uT("\\abstractname{}|\\address{}|\\addto{}|\\Alert{}|\\AlertM{}|\\alph{}|\\Alph{}|\\appendix{}|\\appendixname{}|\\arabic{}|\\author{}|\\autoref{}") },
    { 2, uT("no"), 17, 8, uT("\\backmatter{}|\\begin{}|\\bibitem{}|\\bibliography{}|\\bibliographystyle{}|\\bibname{}|\\bigskip{}|\\bottomrule") },
    { 2, uT("no"), 11, 10, uT("\\caption{}|\\centering|\\chapter{}|\\chaptername{}|\\cite{}|\\cline{}|\\color{}|\\colorbox{}|\\columnwidth{}|\\cortext{}") },
    { 2, uT("no"), 13, 3, uT("\\date{}|\\definecolor{}|\\documentclass{}") },
    { 2, uT("no"), 12, 5, uT("\\emph{}|\\end{}|\\endgraf{}|\\endinput{}|\\eqref{}") },
    { 2, uT("no"), 12, 14, uT("\\fbox{}|\\fontfamily{}|\\fontsize{}|\\footnote{}|\\footnotesize|\\footcite{}|\\footfullcite{}|\\fbox{}|\\fontfamily{}|\\fontsize{}|\\framesubtitle{}|\\frametitle{}|\\footnote{}|\\frontmatter{}") },
    { 2, uT("no"), 12, 3, uT("\\gquote{}|\\graphicspath{}|\\Grayed{}") },
    { 2, uT("no"), 10, 2, uT("\\Highlight{}|\\HighlightM{}") },
    { 2, uT("no"), 11, 6, uT("\\hline{}|\\href{}|\\huge{}|\\Huge{}|\\hyperref{}|\\hyphenation{}") },
    { 2, uT("no"), 15, 8, uT("\\input{}|\\include{}|\\includeonly{}|\\includegraphics{}|\\includepdf{}|\\item{}|\\itemize{}|\\itshape{}") },
    { 2, uT("no"), 13, 8, uT("\\label{}|\\large{}|\\Large{}|\\LARGE{}|\\linewidth{}|\\lipsum{}|\\listoffigures{}|\\listoftables{}") },
    { 2, uT("no"), 13, 22, uT("\\mainmatter{}|\\makeatletter|\\makeatother|\\maketitle|\\MakeLowercase{}|\\MakeUppercase{}|\\mathbb{}|\\mathbf{}|\\mathcal{}|\\mathds{}|\\mathfrak{}|\\mathit{}|\\mathnormal{}|\\mathrm{}|\\mathscr{}|\\mathsf{}|\\mathtt{}|\\mathversion{}|\\medskip{}|\\midrule|\\mdseries{}|\\multicolumn{}") },
    { 2, uT("no"), 15, 12, uT("\\newcommand{}|\\newcounter{}|\\newenvironment{}|\\newline{}|\\newtheorem{}|\\newtheoremstyle{}|\\nocite{}|\\nolinkurl{}|\\normalcolor{}|\\normalfont{}|\\normalsize{}|\\notag{}") },
    { 2, uT("no"), 14, 9, uT("\\pageref{}|\\par{}|\\paragraph{}|\\parbox{}|\\parindent{}|\\parskip{}|\\part{}|\\partname{}|\\phantomsection{}") },
    { 2, uT("no"), 5, 2, uT("\\qquad{}|\\quad{}") },
    { 2, uT("no"), 16, 13, uT("\\raggedleft{}|\\raggedright{}|\\ref{}|\\refname{}|\\reflectbox{}|\\refstepcounter{}|\\renewcommand{}|\\renewenvironment{}|\\rmdefault{}|\\rmfamily{}|\\roman{}|\\Roman{}|\\rotatebox{}") },
    { 2, uT("no"), 17, 16, uT("\\section{}|\\selectfont{}|\\setbeamercolor{}|\\setbeamertemplate{}|\\setcounter{}|\\setlength{}|\\small{}|\\smaller|\\smallskip{}|\\stepcounter{}|\\subfloat{}|\\subparagraph{}|\\subsection{}|\\substack{}|\\subsubsection{}|\\SubTitle{}") },
    { 2, uT("no"), 15, 8, uT("\\tabcolsep{}|\\tableofcontents{}|\\tag{}|\\text{}|\\textbf{}|\\textsubscript{}|\\textsuperscript{}|\\toprule") },
    { 2, uT("no"), 10, 1, uT("\\usepackage{}") },
    { 2, uT("no"), 4, 1, uT("\\vsep") },
    { 0, NULL, 0, 0, NULL }
};

// Solis
const int CodeEdit::AUTOC_API_SOLIS_LEN = 29;

const AutoCList CodeEdit::AUTOC_API_SOLIS[] = {

    // words list should be alphabetically ordered

    { 2, uT("no"), 9, 16, uT("[Anode]|[Cathode]|[Command]|[Device]|[Layer1]|[Layer2]|[Layer3]|[Layer4]|[Layer5]|[Layer6]|[Layer7]|[Layer8]|[Layer9]|[Light]|[Numeric]|[Voltage]") },
    { 2, uT("no"), 12, 23, uT("Affinity|Bandgap|Beta|Cn|Cp|Description|Ebr|Eso|Label|Lattice|Material|Me|Mesh|Mhh|Mlh|Msh|Name|NC|NV|Permittivity|Pz|Ps|Taun|Taup|Temperature|Thickness") },
    { 2, uT("no"), 7, 22, uT("Anode|Bgn|Cathode|Command|Dopant|Index|Light|Mesh|Mobility|Numeric|Polarization|Recombination|Signal|Therm|Trap1|Trap2|Trap3|Trap4|Trap5|Tunnel|Urbach|Voltage") },
    { 13, uT("no"), 18, 1, uT("Polarization.Scale") },
    { 13, uT("no"), 18, 1, uT("Permittivity.Model") },
    { 14, uT("no"), 18, 1, uT("Recombination.Model") },
    { 8, uT("no"), 11, 7, uT("Lattice.a|Lattice.c13|Lattice.c33|Lattice.e31|Lattice.e33|Lattice.Model|Lattice.Psp") },

    { 8, uT("no"), 13, 2, uT("Bandgap.Bowing|Bandgap.Model") },
    { 7, uT("no"), 12, 1, uT("Doping.Model") },
    { 9, uT("no"), 12, 1, uT("Affinity.Bowing") },
    { 11, uT("no"), 13, 2, uT("Composition.x|Composition.y") },
    { 7, uT("no"), 9, 9, uT("Urbach.EA|Urbach.ED|Urbach.Enable|Urbach.GA|Urbach.GD|Urbach.TaunA|Urbach.TaunD|Urbach.TaupA|Urbach.TaupD") },
    { 6, uT("no"), 11, 4, uT("Index.File|Index.k|Index.n|Index.Model") },
    { 6, uT("no"), 10, 2, uT("Therm.C|Therm.Heat") },
    { 7, uT("no"), 11, 1, uT("Tunnel.Model") },
    { 4, uT("no"), 9, 4, uT("Bgn.C|Bgn.E|Bgn.Model|Bgn.N") },
    { 9, uT("no"), 15, 11, uT("Mobility.AlphaN|Mobility.AlphaP|Mobility.CrN|Mobility.CrP|Mobility.Model|Mobility.Mu1N|Mobility.Mu1P|Mobility.Mu2N|Mobility.Mu2P|Mobility.MuN|Mobility.MuP") },
    { 7, uT("no"), 11, 7, uT("Dopant.E|Dopant.N|Dopant.Nr|Dopant.Taun|Dopant.Taup|Dopant.Type|Dopant.W") },
    { 8, uT("no"), 9, 7, uT("Trap1.E|Trap1.N|Trap1.Nr|Trap1.Taun|Trap1.Taup|Trap1.Type|Trap1.W") },
    { 8, uT("no"), 9, 7, uT("Trap2.E|Trap2.N|Trap2.Nr|Trap2.Taun|Trap2.Taup|Trap2.Type|Trap2.W") },
    { 8, uT("no"), 9, 7, uT("Trap3.E|Trap3.N|Trap3.Nr|Trap3.Taun|Trap3.Taup|Trap3.Type|Trap3.W") },
    { 8, uT("no"), 9, 7, uT("Trap4.E|Trap4.N|Trap4.Nr|Trap4.Taun|Trap4.Taup|Trap4.Type|Trap4.W") },
    { 8, uT("no"), 9, 7, uT("Trap5.E|Trap5.N|Trap5.Nr|Trap5.Taun|Trap5.Taup|Trap5.Type|Trap5.W") },
    { 6, uT("no"), 15, 11, uT("Anode.Boundary|Anode.Index.Model|Anode.Index.k|Anode.Index.n|Anode.Material|Anode.Phi|Anode.Rn|Anode.Rp|Anode.Thickness|Anode.Tunnel|Anode.Type") },
    { 8, uT("no"), 17, 11, uT("Cathode.Boundary|Cathode.Index.Model|Cathode.Index.k|Cathode.Index.n|Cathode.Material|Cathode.Phi|Cathode.Rn|Cathode.Rp|Cathode.Thickness|Cathode.Tunnel|Cathode.Type") },
    { 8, uT("no"), 13, 8, uT("Voltage.End|Voltage.File|Voltage.Jinf|Voltage.Jsup|Voltage.Points|Voltage.Source|Voltage.Start|Voltage.Step") },
    { 6, uT("no"), 16, 5, uT("Signal.Amplitude|Signal.Frequency.End|Signal.Frequency.Points|Signal.Source|Signal.Frequency.Start") },
    { 6, uT("no"), 12, 7, uT("Light.EQE|Light.End|Light.Flux|Light.Points|Light.Source|Light.Start|Light.Step") },
    { 8, uT("no"), 16, 9, uT("Numeric.Damp|Numeric.Guess|Numeric.InnermostSolver|Numeric.Mesh|Numeric.Method|Numeric.Points|Numeric.SaveFull|Numeric.Tol|Numeric.Verbose") },
    { 8, uT("no"), 14, 3, uT("Command.OutDir|Command.OutDirDate|Command.Plot") },
    { 5, uT("no"), 11, 1, uT("Mesh.Points") },

    { 8, uT("no"), 14, 7, uT("Voltage.End|Voltage.Jinf|Voltage.Jsup|Voltage.Points|Voltage.Source|Voltage.Start|Voltage.Step") },

    /* shortcuts for record name */
    { 2, uT("no"), 11, 11, uT("Index.Model|Index.k|Index.n|Material|Name|Phi|Rn|Rp|Thickness|Tunnel|Type") },
    { 2, uT("no"), 6, 7, uT("End|Jinf|Jsup|Source|Start|Step") },
    { 2, uT("no"), 4, 2, uT("EQE|Flux") },
    { 2, uT("no"), 14, 7, uT("Damp|InnermostSolver|Mesh|Method|Points|SaveFull|Tol|Verbose") },
    { 2, uT("no"), 6, 2, uT("OutDir|Plot") },
    /* */

    { 0, NULL, 0, 0, NULL }
};

// LUA
const char_t *CodeEdit::WORDLIST_LUA =
    uT("and break do else elseif end for ")
    uT("function if local nil not or repeat return then ")
    uT("until while");

const char_t *CodeEdit::WORDLIST_API_LUA =
    uT("_G _VERSION cls clear ")
    uT("assert collectgarbage dofile error getfenv getmetatable ipairs ")
    uT("load loadfile loadstring module next pairs pcall print rawequal ")
    uT("rawget rawset require select setfenv setmetatable tonumber tostring ")
    uT("type unpack xpcall ")

    uT("coroutine.create coroutine.resume coroutine.running coroutine.status coroutine.wrap coroutine.yield ")

    uT("debug.getfenv debug.gethook debug.getinfo debug.getlocal debug.getmetatable debug.getregistry debug.getupvalue ")
    uT("debug.setfenv debug.sethook debug.setlocal debug.setmetatable debug.setupvalue debug.traceback ")

    uT("file:close file:flush file:lines file:read file:seek file:setvbuf file:write ")

    uT("io.close io.flush io.input io.lines io.open io.output io.popen io.read io.stderr ")
    uT("io.stdin io.stdout io.tmpfile io.type io.write ")

    uT("gui.alert gui.ask ")

    uT("math.abs math.acos math.asin math.atan math.atan2 math.ceil math.cos math.cosh math.deg math.exp ")
    uT("math.floor math.fmod math.frexp math.huge math.ldexp math.log math.log10 math.max math.min ")
    uT("math.modf math.pi math.pow math.rad math.random math.randomseed ")
    uT("math.sin math.sinh math.sqrt math.tan math.tanh ")
    uT("math.kb math.q math.h math.c math.eps0 ")
    uT("math.na math.me math.kbt math.erf math.erfc ")
    uT("math.exp2 math.logb math.cbrt math.hypot math.lgamma ")
    uT("math.tgamma math.trunc math.round math.rint math.isnan ")
    uT("math.isinf math.isnormal math.asinh math.acosh math.atanh ")
    uT("math.gauss math.lorentz ")

    uT("linalg.array linalg.get linalg.cat linalg.swap linalg.copy linalg.add ")
    uT("linalg.format linalg.mult linalg.norm linalg.sum linalg.imin linalg.imax linalg.dot linalg.rot ")
    uT("linalg.zeros linalg.ones linalg.trans linalg.rand linalg.inv linalg.det linalg.trace ")
    uT("linalg.solve linalg.tridiag linalg.sparse ")
    uT("linalg.leastsq linalg.eig ")

    uT("ode.solve ")

    uT("optim.minimize optim.root ")

    uT("data.min data.max data.sum data.mean data.median ")
    uT("data.var data.dev data.coeff data.rms data.skew data.kurt ")
    uT("data.fit data.sort data.acorr data.filter data.fft data.load data.save ")

    uT("time.tic time.toc time.format time.sleep ")

    uT("plot.new plot.set plot.add plot.rem plot.save plot.update plot.show plot.close ")

    uT("parser.new parser.set parser.get parser.solve parser.eval parser.evalf parser.delete ")

    uT("socket.accept socket.bind socket.close socket.connect socket.delete socket.geterr socket.gethostbyaddr socket.gethostbyname socket.gethostname socket.getpeername socket.getsockname socket.iserr socket.listen socket.new socket.recv socket.recvfrom socket.send socket.sendto socket.setsockopt socket.shutdown socket.timeout socket.AF_ECMA socket.AF_INET socket.AF_INET6 socket.AF_INET6 socket.AF_IRDA socket.AF_UNIX socket.AF_UNSPEC socket.INADDR_ANY socket.INADDR_BROADCAST socket.INADDR_LOOPBACK socket.INADDR_NONE socket.IPPROTO_TCP socket.IPPROTO_UDP socket.MSG_CTRUNC socket.MSG_DONTROUTE socket.MSG_OOB socket.MSG_PEEK socket.MSG_TRUNC socket.MSG_WAITALL socket.SO_ACCEPTCONN socket.SO_DONTROUTE socket.SO_KEEPALIVE socket.SO_LINGER socket.SO_RCVTIMEO socket.SO_REUSEADDR socket.SO_SNDTIMEO socket.SOCK_DGRAM socket.SOCK_RAW socket.SOCK_SEQPACKET socket.SOCK_STREAM ")

    uT("os.clock os.date os.difftime os.execute os.exit os.getenv ")
    uT("os.remove os.rename os.setlocale os.time os.tmpname ")

    uT("package.cpath package.loaded package.loaders package.loadlib ")
    uT("package.path package.preload package.seeall ")

    uT("string.byte string.char string.dump string.find string.format string.gmatch ")
    uT("string.gsub string.len string.lower string.match string.rep string.reverse string.sub string.upper ")

    uT("table.concat table.insert table.maxn table.remove table.sort");

const LanguageInfo CodeEdit::LANGPREFS_LUA = {
    uT("Lua"),
    uT("*.lua"),
    wxSTC_LEX_LUA,
    {
        { 0, wxSTC_LUA_WORD, CodeEdit::WORDLIST_LUA },
        { 1, wxSTC_LUA_WORD2, CodeEdit::WORDLIST_API_LUA },
        { -1, -1, NULL }
    }
};
//

// C/C++
const char_t *CodeEdit::WORDLIST_CPP =
    uT("_Bool alignas alignof and and_eq asm auto bitand bitor bool break case catch char char16_t char32_t ")
    uT("class compl const constexpr const_cast continue decltype default delete do double dynamic_cast ")
    uT("else enum explicit export extern false float for friend goto if inline int long mutable namespace ")
    uT("new noexcept not not_eq nullptr operator or or_eq private protected public register reinterpret_cast ")
    uT("return short signed sizeof static static_assert static_cast struct switch template this thread_local ")
    uT("throw true try typedef typeid typename union unsigned using virtual void volatile wchar_t while xor xor_eq");

const LanguageInfo CodeEdit::LANGPREFS_CPP = {
    uT("cpp"),
    uT("*.cpp"),
    wxSTC_LEX_CPP,
    {
        { 0, wxSTC_C_WORD, CodeEdit::WORDLIST_CPP },
        { -1, -1, NULL }
    }
};
//

// C
const char_t *CodeEdit::WORDLIST_C =
    uT("_Bool break case char char16_t char32_t ")
    uT("const continue decltype default do double  ")
    uT("else enum extern float for goto if inline int long ")
    uT("register ")
    uT("return short signed sizeof static struct switch ")
    uT("typedef union unsigned void volatile wchar_t while ")
    uT("__attribute__ __asm__ __typeof__ __volatile__ __inline__ ");

const LanguageInfo CodeEdit::LANGPREFS_C = {
    uT("c"),
    uT("*.c"),
    wxSTC_LEX_CPP,
    {
        { 0, wxSTC_C_WORD, CodeEdit::WORDLIST_C },
        { -1, -1, NULL }
    }
};
//

// Bash
const char_t *CodeEdit::WORDLIST_BASH =
    uT("if then else elif fi case esac for select while until do done in function time coproc ");

const LanguageInfo CodeEdit::LANGPREFS_BASH = {
    uT("sh"),
    uT("*.sh"),
    wxSTC_LEX_BASH,
    {
        { 0, wxSTC_SH_WORD, CodeEdit::WORDLIST_BASH },
        { -1, -1, NULL }
    }
};
//

// PYTHON
const char_t *CodeEdit::WORDLIST_PYTHON =
    uT("and del from not while self ")
    uT("as elif global or with ")
    uT("assert else if pass yield ")
    uT("break except import print ")
    uT("class exec in range raise ")
    uT("continue finally is return ")
    uT("def for lambda try ")
    uT("False None True nonlocal ");

const LanguageInfo CodeEdit::LANGPREFS_PYTHON = {
    uT("Python"),
    uT("*.py"),
    wxSTC_LEX_PYTHON,
    {
        { 0, wxSTC_P_WORD, CodeEdit::WORDLIST_PYTHON },
        { -1, -1, NULL }
    }
};
//

// OCTAVE
const char_t *CodeEdit::WORDLIST_OCTAVE =
    uT("__FILE__ LINE__ break case catch classdef ")
    uT("continue do else elseif end end_try_catch ")
    uT("end_unwind_protect endclassdef endenumeration ")
    uT("endevents endfor endif endmethods endparfor ")
    uT("endproperties endswitch endwhile enumeration ")
    uT("events for function endfunction get global ")
    uT("if methods otherwise parfor persistent properties ")
    uT("return set static switch try until unwind_protect ")
    uT("unwind_protect_cleanup while");

const LanguageInfo CodeEdit::LANGPREFS_OCTAVE = {
    uT("m"),
    uT("*.m"),
    wxSTC_LEX_OCTAVE,
    {
        { 0, wxSTC_MATLAB_KEYWORD, CodeEdit::WORDLIST_OCTAVE },
        { -1, -1, NULL }
    }
};
//

// FORTRAN
const char_t *CodeEdit::WORDLIST_FORTRAN =
    uT("access action advance allocatable allocate ")
    uT("apostrophe assign assignment associate asynchronous backspace ")
    uT("bind blank blockdata call case character class close common ")
    uT("complex contains continue critical cycle data deallocate decimal delim ")
    uT("default dimension direct do dowhile double doubleprecision else ")
    uT("elseif elsewhere encoding end endassociate endblockdata enddo ")
    uT("endfile endforall endfunction endif endinterface endmodule endprogram ")
    uT("endselect endsubroutine endtype endwhere entry eor equivalence ")
    uT("err errmsg exist exit external file flush fmt forall form format ")
    uT("formatted function go goto id if implicit in include inout ")
    uT("integer inquire intent interface intrinsic iomsg iolength ")
    uT("iostat kind len logical module name named namelist nextrec nml ")
    uT("none nullify number only open opened operator optional out pad ")
    uT("parameter pass pause pending pointer pos position precision ")
    uT("print private procedure program protected public quote read readwrite ")
    uT("real rec recl recursive result return rewind save select ")
    uT("selectcase selecttype sequential sign size stat status stop stream ")
    uT("subroutine target then to type unformatted unit use value ")
    uT("volatile wait where while write");

const LanguageInfo CodeEdit::LANGPREFS_FORTRAN = {
    uT("for"),
    uT("*.for"),
    wxSTC_LEX_FORTRAN,
    {
        { 0, wxSTC_F_WORD, CodeEdit::WORDLIST_FORTRAN },
        { -1, -1, NULL }
    }
};
//

// LATEX
const char_t *CodeEdit::WORDLIST_LATEX =
    uT("abstractname addcontentsline address addto addtocounter alph Alph and appendix appendixname arabic article author ")
    uT("backmatter begin bfseries bibitem bibliography bibliographystyle bibname boldmath book booklet boldsymbol ")
    uT("caption captionof captionsfrench centering ")
    uT("chapter chaptername cite cline color colorbox colorlet columnwidth conference contentsname cortext ")
    uT("date DeclareMathOperator definecolor displaystyle documentclass ")
    uT("em emph end endgraf endinput eqref fbox fcolorbox fnsymbol fontfamily fontsize footnote footnotesize ")
    uT("frontmatter hline href huge Huge hyperref input include includeonly includegraphics includepdf ")
    uT("inbook incollection inproceedings item itemize itshape label large Large LARGE linewidth lipsum listoffigures listoftables ")
    uT("mainmatter maketitle MakeLowercase MakeUppercase mastersthesis mathbb mathbf mathcal mathds mathfrak mathit ")
    uT("manual mathnormal mathrm mathscr mathsf mathtt mathversion mdseries misc multicolumn newcommand newcounter ")
    uT("newenvironment newline newtheorem newtheoremstyle nocite nolinkurl normalcolor normalfont ")
    uT("normalsize notag pageref par paragraph parbox parindent parskip part partname phantomsection phdthesis proceedings ")
    uT("qquad quad raggedleft raggedright ref refname reflectbox refstepcounter renewcommand renewenvironment ")
    uT("rmdefault rmfamily roman Roman rotatebox SaveVerb scriptsize scriptscriptstyle scriptstyle scshape ")
    uT("section selectfont setcounter sfdefault sffamily shoveleft shoveright small stepcounter string subfloat subparagraph ")
    uT("subsection substack subsubsection tableofcontents tag techreport text thesis unpublished usepackage gquote url vsep ");

const LanguageInfo CodeEdit::LANGPREFS_LATEX = {
    uT("LaTeX"),
    uT("*.tex"),
    wxSTC_LEX_LATEX,
    {
        { 0, wxSTC_TEX_COMMAND, CodeEdit::WORDLIST_LATEX },
        { -1, -1, NULL }
    }
};
//

// MAKEFILE
const char_t *CodeEdit::WORDLIST_MAKEFILE =
    uT("sidi ");

const LanguageInfo CodeEdit::LANGPREFS_MAKEFILE = {
    uT("Makefile"),
    uT(""),
    wxSTC_LEX_MAKEFILE,
    {
        { 0, wxSTC_MAKE_IDENTIFIER, CodeEdit::WORDLIST_MAKEFILE },
        { -1, -1, NULL }
    }
};
//

// COMET
const int CodeEdit::TYPECOUNT_USER = 1;
UserLanguageInfo *CodeEdit::PLANGPREFS_USER = NULL;
//

// Batch
const char_t *CodeEdit::WORDLIST_BATCH =
    uT("call choice echo else for goto if pause prompt rem set shift setlocal endlocal ");
const char_t *CodeEdit::WORDLIST_BATCH_COMMAND =
    uT("solis solis.exe comet comet.exe scc scc.exe at attrib path prompt cd cls cmd copy del deltree erase errorlevel exit mkdir move popd print pushd ren replace rd rmdir run runas sort start title xcopy ");

const LanguageInfo CodeEdit::LANGPREFS_BATCH = {
    uT("bat"),
    uT("*.bat"),
    wxSTC_LEX_BATCH,
    {
        { 0, wxSTC_BAT_WORD, CodeEdit::WORDLIST_BATCH },
        { 1, wxSTC_BAT_COMMAND, CodeEdit::WORDLIST_BATCH_COMMAND },
        { -1, -1, NULL }
    }
};
//

// Solis
const char_t *CodeEdit::WORDLIST_SOLIS_KEYWORD =
    uT("Device Layer1 Layer2 Layer3 Layer4 Layer5 Layer6 Layer7 Layer8 Layer9 Anode Cathode Voltage Light Signal Numeric Command ");
const char_t *CodeEdit::WORDLIST_SOLIS_KEYWORD2 =
    uT("Material Name Label Description Temperature Thickness Bandgap Bandgap.Bowing Affinity Affinity.Bowing Composition.x Composition.y Permittivity Permittivity.Model Recombination.Model Lattice.Model Lattice.Psp Lattice.a Lattice.e31 Lattice.e33 Lattice.c13 Lattice.c33 Polarization.Scale ")
    uT("NC NV Eso Me Mhh Mlh Msh Ebr ")
    uT("Beta Cn Cp Taun Taup ")

    uT("Urbach.Model ")
    uT("Urbach.EA Urbach.ED Urbach.Enable Urbach.GA Urbach.GD Urbach.TaunA Urbach.TaunD Urbach.TaupA Urbach.TaupD ")

    uT("Index.Model ")
    uT("Index.n Index.k ")
    uT("Therm.C Therm.Heat ")

    uT("Tunnel.Model ")

    uT("Bandgap.Model ")

    uT("Mobility.Model Mobility.N Mobility.P ")

    uT("Dopant.Type Dopant.N Dopant.Nr Dopant.Taun Dopant.Taup Dopant.E Dopant.Model ")

    uT("Trap.Model Trap.Count ")

    uT("Trap1.N Trap1.Nr Trap1.E Trap1.W Trap1.Taun Trap1.Taup Trap1.Type ")
    uT("Trap2.N Trap2.Nr Trap2.E Trap2.W Trap2.Taun Trap2.Taup Trap2.Type ")
    uT("Trap3.N Trap3.Nr Trap3.E Trap3.W Trap3.Taun Trap3.Taup Trap3.Type ")
    uT("Trap4.N Trap4.Nr Trap4.E Trap4.W Trap4.Taun Trap4.Taup Trap4.Type ")
    uT("Trap5.N Trap5.Nr Trap5.E Trap5.W Trap5.Taun Trap5.Taup Trap5.Type ")
    uT("Trap.Count Trap.Model ")

    uT("Anode.Type Anode.Rn Anode.Rp Anode.Phi Anode.Tunnel Anode.Boundary ")
    uT("Anode.Thickness Anode.Index.n Anode.Index.k Anode.Index.Model Anode.Material Anode.Name ")

    uT("Cathode.Type Cathode.Rn Cathode.Rp Cathode.Phi Cathode.Tunnel Cathode.Boundary ")
    uT("Cathode.Thickness Cathode.Index.n Cathode.Index.k Cathode.Index.Model Cathode.Material Cathode.Name ")

    uT("Signal.Source Signal.Amplitude Signal.Frequency.Start Signal.Frequency.End Signal.Frequency.Points ")

    uT("Voltage.Source Voltage.Start Voltage.End Voltage.Step Voltage.Jinf Voltage.Jsup Voltage.Points ")

    uT("Light.Source Light.Start Light.Wavelength Light.End Light.Step Light.Points Light.Flux Light.EQE ")

    uT("Numeric.Verbose Numeric.SaveFull Numeric.Method Numeric.Points Numeric.Tol Numeric.Damp Numeric.Guess Numeric.Mesh Numeric.InnermostSolver ")

    uT("Command.OutDir Command.OutDirDate Command.Plot ")

    uT("Mesh.Points ")

    /* shortcuts for record name */
    uT("Index.Model Index.k Index.n Material Name Phi Rn Rp Thickness Tunnel Type ")
    uT("End Jinf Jsup Source Start Step ")
    uT("EQE Flux ")
    uT("Damp Mesh Method Points InnermostSolver SaveFull Tol Verbose ")
    uT("OutDir Plot ");
/* */

const LanguageInfo CodeEdit::LANGPREFS_SOLIS = {
    uT("solis"),
    uT("*.solis"),
    wxSTC_LEX_SOLIS,
    {
        { 0, wxSTC_SOLIS_KEYWORD, CodeEdit::WORDLIST_SOLIS_KEYWORD },
        { 1, wxSTC_SOLIS_KEYWORD2, CodeEdit::WORDLIST_SOLIS_KEYWORD2 },
        { -1, -1, NULL }
    }
};
//

// Solis Output
const char_t *CodeEdit::WORDLIST_SOLISOUTPUT_KEYWORD =
    uT("INFO WARNING ERROR DEVICE INPUT LIGHT VOLTAGE COMM SEND DEBUG PV SIMUL ");
const char_t *CodeEdit::WORDLIST_SOLISOUTPUT_KEYWORD2 =
    uT("VJ WQ VY FY Start End Step Jsc Voc FF Eff Gummel Newton Poisson Mono AM1.5 Mesh Points Mobility Index Doping Trap Bandgap Permittivity ");

const LanguageInfo CodeEdit::LANGPREFS_SOLISOUTPUT = {
    uT("solis"),
    uT("*.solis"),
    wxSTC_LEX_SOLIS,
    {
        { 0, wxSTC_SOLIS_KEYWORD, CodeEdit::WORDLIST_SOLISOUTPUT_KEYWORD },
        { 1, wxSTC_SOLIS_KEYWORD2, CodeEdit::WORDLIST_SOLISOUTPUT_KEYWORD2 },
        { -1, -1, NULL }
    }
};
//

// Comet Settings
const char_t *CodeEdit::WORDLIST_SETTINGS_KEYWORD =
    uT("Common Find Recent GUI Tools Default Keyword Number String Comment Preprocessor Identifier Lib LineNumber Caret Selection Whitespace ");
const char_t *CodeEdit::WORDLIST_SETTINGS_KEYWORD2 =
    uT("LastDir DetectChangeOutside ")
    uT("FindMarker FindCyclic ")
    uT("Find01 Find02 Find03 Find04 Find05 Find06 Find07 FindF ")
    uT("Replace01 Replace02 Replace03 Replace04 Replace05 Replace06 Replace07 ReplaceF ")
    uT("Dir01 Dir02 Dir03 Dir04 Dir05 Dir06 Dir07 DirF ")
    uT("FileRecent01 FileRecent02 FileRecent03 FileRecent04 FileRecent05 FileRecent06 FileRecent07 FileRecent08 FileRecent09 FileRecent10 FileRecent11 FileRecent12 FileRecentIndex FileOpenStatus FileRecentSel ")
    uT("DirRecent01 DirRecent02 DirRecent03 DirRecent04 DirRecent05 DirRecent06 DirRecent07 DirRecent08 DirRecent09 DirRecent10 DirRecent11 DirRecent12 DirRecentIndex FileOpenStatus DirRecentSel ")
    uT("FrameMaximized FrameWidth FrameHeight FindFilePosX FindFilePosY FindDirPosX FindDirPosY FindDirWidth FindDirHeight ViewExplorer ViewConsole ViewOutput ViewBookmark ViewToolbar ViewStatusbar ReopenRecent SaveNavigation ViewInfobar ")
    uT("ToolLog ")
    uT("Tool01lex Tool01cmd Tool01out Tool01cls Tool02lex Tool02cmd Tool02out Tool02cls Tool03lex Tool03cmd Tool03out Tool03cls Tool04lex Tool04cmd Tool04out Tool04cls ")
    uT("Tool05lex Tool05cmd Tool05out Tool05cls Tool06lex Tool06cmd Tool06out Tool06cls Tool07lex Tool07cmd Tool07out Tool07cls ")
    uT("ToolSilent ")
    uT("EnableSyntax EnableFold WrapMode EnableIndent DisplayEOL ShowLinenum ShowMarker ShowStatus ShowWhitespace EnableCompletion EnableCalltip LineSpacing UseTab IndentGuide TabSize ReloadDelay EnableLongLine LongLine MarkerColorModified MarkerColorSaved MarkerColorFind ")
    uT("ColorFore ColorBack FontName FontSize FontStyle LetterCase ");

const LanguageInfo CodeEdit::LANGPREFS_SETTINGS = {
    uT("solis"),
    uT("*.solis"),
    wxSTC_LEX_SOLIS,
    {
        { 0, wxSTC_SOLIS_KEYWORD, CodeEdit::WORDLIST_SETTINGS_KEYWORD },
        { 1, wxSTC_SOLIS_KEYWORD2, CodeEdit::WORDLIST_SETTINGS_KEYWORD2 },
        { -1, -1, NULL }
    }
};
//

// User-defined syntax
const char_t *CodeEdit::WORDLIST_USERSYNTAX_KEYWORD =
    uT("Lexer ");
const char_t *CodeEdit::WORDLIST_USERSYNTAX_KEYWORD2 =
    uT("Name FileExt Keywords ");

const LanguageInfo CodeEdit::LANGPREFS_USERSYNTAX = {
    uT("solis"),
    uT("*.solis"),
    wxSTC_LEX_SOLIS,
    {
        { 0, wxSTC_SOLIS_KEYWORD, CodeEdit::WORDLIST_USERSYNTAX_KEYWORD },
        { 1, wxSTC_SOLIS_KEYWORD2, CodeEdit::WORDLIST_USERSYNTAX_KEYWORD2 },
        { -1, -1, NULL }
    }
};
//
