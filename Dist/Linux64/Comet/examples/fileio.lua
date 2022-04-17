-- File IO

cls()

fn = "test.txt"
fp = io.open(fn, "w+")
fp:write("Hello world!")
fp:close()
fp = io.open(fn, "r")
tt = fp:read("*all")
fp:close()
io.write(tt)
