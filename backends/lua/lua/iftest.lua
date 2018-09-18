
a = 5
b = a + 2
c = 12

function fib(n)
	if n == 0 then
		return 0
	elseif n <= 2 then
		return 1
	end

	return fib(n - 2) + fib(n - 1)
end

fib(5)

local x = 0
while x < 15 do
	x = x + 1
end

for i = 0, i < 1, 4 do
	fib(i)
end

local table = {
	a,
	b,
	c
}
