--[[
	Solis example
	C Module
]]

cls()

-- load the C module
local cmodule = require("Cmodule")

-- test
print(cmodule.version())
