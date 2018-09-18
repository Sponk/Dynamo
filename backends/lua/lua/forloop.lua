
local n = 1
for i = 1, 10, 1 do
	n = n + 1
end

dassert(n == 10, "The number of iterations should be 10!")

n = 1
local t = {1, 2, 3}
for i = 1, #t, 1 do
	n = n + 1
end

dassert(n == 3, "The number of iterations should be 3!")
