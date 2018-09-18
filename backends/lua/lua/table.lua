
local t = {}

for i = 1, 15, 1 do
	t[i] = i*i
end

for i = 1, 15, 1 do
	dassert(t[i] == i*i, "Table was not filled correctly!")
end

t.k = 4
t.f = function()
	print("Hello Table!")
end

t.f()

function t.g()
	print("Hello other stuff!")
end

t.g()

function t:h()
	print("Hello self!")
end

t.h(t)
t:h()

local q = {123}
dassert(q[1] == 123, "Indexing is wrong!") 
