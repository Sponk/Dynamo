
table = {
}

function fib(n)
	if n == 0 then
		return 0
	elseif n <= 2 then
		return 1
	end

	return fib(n - 2) + fib(n - 1)
end

for i = 1, 15, 1 do
	table[i] = fib(i)
end

return fib(42)
