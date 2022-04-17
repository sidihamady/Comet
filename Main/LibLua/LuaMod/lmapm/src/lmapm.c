/*
* lmapm.c
* big-number library for Lua 5.1 based on the MAPM library
* Luiz Henrique de Figueiredo <lhf@tecgraf.puc-rio.br>
* 01 May 2012 11:55:29
* This code is hereby placed in the public domain.
*/

#define LMAPM_LIB
#define IGNORE_MAPM_WARNINGS

#include <stdlib.h>

#include "m_apm.h"

#include "lmapm.h"

#define lua_boxpointer(L, u) (*(void **)(lua_newuserdata(L, sizeof(void *))) = (u))

#define LUAEXT_VERSION_STRING "\nLua Library for MAPM " MAPM_LIB_SHORT_VERSION

#define MYNAME "lmapm"
#define MYTYPE MYNAME " bignumber"

static int g_iDigits = 48;
static lua_State *g_pLua = NULL;

#ifdef USE_LUAJIT
#define luaL_len lua_objlen
#endif

API_DLLIMPORT int luaL_errorlevel(lua_State *pLua, const char *fmt, ...);

void M_apm_log_error_msg(int fatal, char *message)
{
#ifdef IGNORE_MAPM_WARNINGS
    if (fatal)
#endif
        luaL_errorlevel(g_pLua, (const char *)message);
}

static M_APM Bnew(lua_State *L)
{
    M_APM x = m_apm_init();
    lua_boxpointer(L, x);
    luaL_getmetatable(L, MYTYPE);
    lua_setmetatable(L, -2);
    return x;
}

static M_APM Bget(lua_State *L, int i)
{
    g_pLua = L;
    switch (lua_type(L, i)) {
        case LUA_TNUMBER:
        {
            M_APM x = Bnew(L);
            m_apm_set_double(x, lua_tonumber(L, i));
            lua_replace(L, i);
            return x;
        }
        case LUA_TSTRING:
        {
            M_APM x = Bnew(L);
            m_apm_set_string(x, (char *)lua_tostring(L, i));
            lua_replace(L, i);
            return x;
        }
        default:
            return *((void **)luaL_checkudata(L, i, MYTYPE));
    }
    return NULL;
}

static int Bdo0(lua_State *L, void (*f)(M_APM y, M_APM x))
{
    M_APM a = Bget(L, 1);
    M_APM c = Bnew(L);
    f(c, a);
    return 1;
}

static int Bdo1(lua_State *L, void (*f)(M_APM y, int n, M_APM x))
{
    int n = luaL_optint(L, 2, g_iDigits);
    M_APM a = Bget(L, 1);
    M_APM c = Bnew(L);
    f(c, n, a);
    return 1;
}

static int Bdo2(lua_State *L, void (*f)(M_APM z, M_APM x, M_APM y))
{
    M_APM a = Bget(L, 1);
    M_APM b = Bget(L, 2);
    M_APM c = Bnew(L);
    f(c, a, b);
    return 1;
}

static int Bdo3(lua_State *L, void (*f)(M_APM z, int n, M_APM x, M_APM y))
{
    int n = luaL_optint(L, 3, g_iDigits);
    M_APM a = Bget(L, 1);
    M_APM b = Bget(L, 2);
    M_APM c = Bnew(L);
    f(c, n, a, b);
    return 1;
}

static int Bdigits(lua_State *L) /** digits([n]) */
{
    lua_pushinteger(L, g_iDigits);
    g_iDigits = luaL_optint(L, 1, g_iDigits);
    return 1;
}

static int Btostring(lua_State *L) /** tostring(x,[n,exp]) */
{
    char *s;
    M_APM a = Bget(L, 1);
    int n = luaL_optint(L, 2, g_iDigits);
    if (lua_toboolean(L, 3)) {
        int m = (n < 0) ? m_apm_significant_digits(a) : n;
        s = malloc(m + 16);
        if (s != NULL) {
            m_apm_to_string(s, n, a);
        }
    }
    else {
        s = m_apm_to_fixpt_stringexp(n, a, '.', 0, 0);
    }
    lua_pushstring(L, s);
    if (s != NULL) {
        free(s);
    }
    return 1;
}

static int Btonumber(lua_State *L) /** tonumber(x) */
{
    lua_settop(L, 1);
    lua_pushinteger(L, 20); /* enough for IEEE doubles */
    lua_pushboolean(L, 1);
    Btostring(L);
    lua_pushnumber(L, lua_tonumber(L, -1));
    return 1;
}

static int Bnumber(lua_State *L) /** number(x) */
{
    Bget(L, 1);
    lua_settop(L, 1);
    return 1;
}

static int Bround(lua_State *L) /** round(x) */
{
    return Bdo1(L, m_apm_round);
}

static int Binv(lua_State *L) /** inv(x) */
{
    return Bdo1(L, m_apm_reciprocal);
}

static int Bsqrt(lua_State *L) /** sqrt(x) */
{
    return Bdo1(L, m_apm_sqrt);
}

static int Bcbrt(lua_State *L) /** cbrt(x) */
{
    return Bdo1(L, m_apm_cbrt);
}

static int Blog(lua_State *L) /** log(x) */
{
    return Bdo1(L, m_apm_log);
}

static int Blog10(lua_State *L) /** log10(x) */
{
    return Bdo1(L, m_apm_log10);
}

static int Bexp(lua_State *L) /** exp(x) */
{
    return Bdo1(L, m_apm_exp);
}

static int Bsin(lua_State *L) /** sin(x) */
{
    return Bdo1(L, m_apm_sin);
}

static int Bcos(lua_State *L) /** cos(x) */
{
    return Bdo1(L, m_apm_cos);
}

static int Btan(lua_State *L) /** tan(x) */
{
    return Bdo1(L, m_apm_tan);
}

static int Basin(lua_State *L) /** asin(x) */
{
    return Bdo1(L, m_apm_asin);
}

static int Bacos(lua_State *L) /** acos(x) */
{
    return Bdo1(L, m_apm_acos);
}

static int Batan(lua_State *L) /** atan(x) */
{
    return Bdo1(L, m_apm_atan);
}

static int Bsincos(lua_State *L) /** sincos(x) */
{
    int n = luaL_optint(L, 2, g_iDigits);
    M_APM a = Bget(L, 1);
    M_APM s = Bnew(L);
    M_APM c = Bnew(L);
    m_apm_sin_cos(s, c, n, a);
    return 2;
}

static int Batan2(lua_State *L) /** atan2(y,x) */
{
    return Bdo3(L, m_apm_atan2);
}

static int Bsinh(lua_State *L) /** sinh(x) */
{
    return Bdo1(L, m_apm_sinh);
}

static int Bcosh(lua_State *L) /** cosh(x) */
{
    return Bdo1(L, m_apm_cosh);
}

static int Btanh(lua_State *L) /** tanh(x) */
{
    return Bdo1(L, m_apm_tanh);
}

static int Basinh(lua_State *L) /** asinh(x) */
{
    return Bdo1(L, m_apm_asinh);
}

static int Bacosh(lua_State *L) /** acosh(x) */
{
    return Bdo1(L, m_apm_acosh);
}

static int Batanh(lua_State *L) /** atanh(x) */
{
    return Bdo1(L, m_apm_atanh);
}

static int Babs(lua_State *L) /** abs(x) */
{
    return Bdo0(L, m_apm_absolute_value);
}

static int Bneg(lua_State *L) /** neg(x) */
{
    return Bdo0(L, m_apm_negate);
}

static int Bfactorial(lua_State *L) /** factorial(x) */
{
    return Bdo0(L, m_apm_factorial);
}

static int Bfloor(lua_State *L) /** floor(x) */
{
    return Bdo0(L, m_apm_floor);
}

static int Bceil(lua_State *L) /** ceil(x) */
{
    return Bdo0(L, m_apm_ceil);
}

static int Badd(lua_State *L) /** add(x,y) */
{
    return Bdo2(L, m_apm_add);
}

static int Bsub(lua_State *L) /** sub(x,y) */
{
    return Bdo2(L, m_apm_subtract);
}

static int Bmul(lua_State *L) /** mul(x,y) */
{
    return Bdo2(L, m_apm_multiply);
}

static int Bdiv(lua_State *L) /** div(x,y) */
{
    return Bdo3(L, m_apm_divide);
}

static int Bidiv(lua_State *L) /** idiv(x,y) */
{
    M_APM a = Bget(L, 1);
    M_APM b = Bget(L, 2);
    M_APM q = Bnew(L);
    M_APM r = Bnew(L);
    m_apm_integer_div_rem(q, r, a, b);
    return 2;
}

static int Bmod(lua_State *L) /** mod(x,y) */
{
    Bidiv(L);
    return 1;
}

static int Bpow(lua_State *L) /** pow(x,y) */
{
    return Bdo3(L, m_apm_pow);
}

static int Bcompare(lua_State *L) /** compare(x,y) */
{
    M_APM a = Bget(L, 1);
    M_APM b = Bget(L, 2);
    lua_pushinteger(L, m_apm_compare(a, b));
    return 1;
}

static int Beq(lua_State *L)
{
    M_APM a = Bget(L, 1);
    M_APM b = Bget(L, 2);
    lua_pushboolean(L, m_apm_compare(a, b) == 0);
    return 1;
}

static int Blt(lua_State *L)
{
    M_APM a = Bget(L, 1);
    M_APM b = Bget(L, 2);
    lua_pushboolean(L, m_apm_compare(a, b) < 0);
    return 1;
}

static int Bsign(lua_State *L) /** sign(x) */
{
    M_APM a = Bget(L, 1);
    lua_pushinteger(L, m_apm_sign(a));
    return 1;
}

static int Bexponent(lua_State *L) /** exponent(x) */
{
    M_APM a = Bget(L, 1);
    lua_pushinteger(L, m_apm_exponent(a));
    return 1;
}

static int Bisint(lua_State *L) /** isint(x) */
{
    M_APM a = Bget(L, 1);
    lua_pushboolean(L, m_apm_is_integer(a));
    return 1;
}

static int Biseven(lua_State *L) /** iseven(x) */
{
    M_APM a = Bget(L, 1);
    lua_pushboolean(L, m_apm_is_integer(a) && m_apm_is_even(a));
    return 1;
}

static int Bisodd(lua_State *L) /** isodd(x) */
{
    M_APM a = Bget(L, 1);
    lua_pushboolean(L, m_apm_is_integer(a) && m_apm_is_odd(a));
    return 1;
}

static int Bdigitsin(lua_State *L) /** digitsin(x) */
{
    M_APM a = Bget(L, 1);
    lua_pushinteger(L, m_apm_significant_digits(a));
    return 1;
}

static int Bgc(lua_State *L)
{
    M_APM a = Bget(L, 1);
    m_apm_free(a);
    lua_pushnil(L);
    lua_setmetatable(L, 1);
    return 0;
}

static const struct luaL_Reg LMAPM_CFUNCTION[] = {
    { "__add", Badd }, /** __add(x,y) */
    { "__div", Bdiv }, /** __div(x,y) */
    { "__eq", Beq },   /** __eq(x,y) */
    { "__gc", Bgc },
    { "__lt", Blt },             /** __lt(x,y) */
    { "__mod", Bmod },           /** __mod(x,y) */
    { "__mul", Bmul },           /** __mul(x,y) */
    { "__pow", Bpow },           /** __pow(x,y) */
    { "__sub", Bsub },           /** __sub(x,y) */
    { "__tostring", Btostring }, /** __tostring(x) */
    { "__unm", Bneg },           /** __unm(x) */
    { "abs", Babs },
    { "acos", Bacos },
    { "acosh", Bacosh },
    { "add", Badd },
    { "asin", Basin },
    { "asinh", Basinh },
    { "atan", Batan },
    { "atan2", Batan2 },
    { "atanh", Batanh },
    { "cbrt", Bcbrt },
    { "ceil", Bceil },
    { "compare", Bcompare },
    { "cos", Bcos },
    { "cosh", Bcosh },
    { "digits", Bdigits },
    { "digitsin", Bdigitsin },
    { "div", Bdiv },
    { "exp", Bexp },
    { "exponent", Bexponent },
    { "factorial", Bfactorial },
    { "floor", Bfloor },
    { "idiv", Bidiv },
    { "inv", Binv },
    { "iseven", Biseven },
    { "isint", Bisint },
    { "isodd", Bisodd },
    { "log", Blog },
    { "log10", Blog10 },
    { "mod", Bmod },
    { "mul", Bmul },
    { "neg", Bneg },
    { "number", Bnumber },
    { "pow", Bpow },
    { "round", Bround },
    { "sign", Bsign },
    { "sin", Bsin },
    { "sincos", Bsincos },
    { "sinh", Bsinh },
    { "sqrt", Bsqrt },
    { "sub", Bsub },
    { "tan", Btan },
    { "tanh", Btanh },
    { "tonumber", Btonumber },
    { "tostring", Btostring },
    { NULL, NULL }
};

LMAPM_API int luaopen_lmapm(lua_State *pLua)
{
    luaL_newmetatable(pLua, MYTYPE);

#if (LUA_VERSION_NUM == 502)
    luaL_setfuncs(pLua, LMAPM_CFUNCTION, 0);
#else
    lua_setglobal(pLua, MYNAME);
    luaL_register(pLua, MYNAME, LMAPM_CFUNCTION);
#endif

    lua_pushliteral(pLua, "version"); /** version */
    lua_pushliteral(pLua, LUAEXT_VERSION_STRING);
    lua_settable(pLua, -3);
    lua_pushliteral(pLua, "__index");
    lua_pushvalue(pLua, -2);
    lua_settable(pLua, -3);

    return 1;
}

#ifdef __cplusplus
extern "C" {
#endif
void SharedLibEntry(void)
{
    printf(LUAEXT_VERSION_STRING);
    exit(0);
}
#ifdef __cplusplus
}
#endif
