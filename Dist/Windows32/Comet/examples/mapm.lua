-- Library for Arbitrary Precision Math (MAPM) by Michael C. Ring
-- Lua interface by Luiz Henrique de Figueiredo

cls()

lmapm = require ("lmapm")

-- lmapm library functions:
--  __add(x,y)      cbrt(x)          mod(x,y)
--  __div(x,y)      ceil(x)          mul(x,y)
--  __eq(x,y)       compare(x,y)     neg(x)
--  __lt(x,y)       cos(x)           number(x)
--  __mod(x,y)      cosh(x)          pow(x,y)
--  __mul(x,y)      digits([n])      round(x)
--  __pow(x,y)      digitsin(x)      sign(x)
--  __sub(x,y)      div(x,y)         sin(x)
--  __tostring(x)   exp(x)           sincos(x)
--  __unm(x)        exponent(x)      sinh(x)
--  abs(x)          factorial(x)     sqrt(x)
--  acos(x)         floor(x)         sub(x,y)
--  acosh(x)        idiv(x,y)        tan(x)
--  add(x,y)        inv(x)           tanh(x)
--  asin(x)         iseven(x)        tonumber(x)
--  asinh(x)        isint(x)         tostring(x,[n,exp])
--  atan(x)         isodd(x)         version
--  atan2(y,x)      log(x)
--  atanh(x)        log10(x)

print("\nSquare root of 2")

lmapm.digits(65)
print("math.sqrt(2)   ", math.sqrt(2))
print("lmapm.sqrt(2)  ", lmapm.sqrt(2))
print(lmapm.version)
