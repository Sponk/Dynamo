
local n = 1
while n < 10 do
	n = n + 1
end

local m = 1
repeat
	m = m + 1
until m >= 10

dassert(n == 10, "The number of iterations should be 10!")
dassert(m == 10, "The number of iterations should be 10!")
