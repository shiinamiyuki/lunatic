function foo()
	local m = {}
	local i = 0
	while i < 10000 do
		m[i] = {}
		i = i + 1
	end
	return m	
end

a = {}
b = {}
a.a = b
b.b = {}
a = {}
b = {}
collectgarbage()

