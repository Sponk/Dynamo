
function f1()
	return 5
end

function f2(x)
	return x + f1()
end

local y = f1

dassert(f2(5) == 10, "Result should be 5 + 1 = 6!")
dassert(y() == 5, "Result should be 5!");

