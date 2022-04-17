-- Hex viewer

cls()

filename = "hello.lua"
local filep = assert(io.open(filename, "rb"))
local size = 10
while true do
	local bread = filep:read(size)
	if not bread then
		break
	end
	for tbyte in string.gmatch(bread, ".") do
		io.write(string.format("%02X ", string.byte(tbyte)))
	end
	io.write(string.rep("   ", size - string.len(bread) + 1))
	io.write(string.gsub(bread, "%c", "."), "\n")
end
io.close(filep)
