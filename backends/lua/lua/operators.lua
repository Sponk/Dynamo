
local v = 5 * 7 == 6
local p = -(5 + 7) + 3 >= 0

dassert(not v, "5 * 7 shouldn't be 6!")
dassert(not p, " -(5 + 7) + 3 should be negative!")

local str = "Hello " .. "World " .. v .. " " .. p
dassert(str == "Hello World false false", str)
