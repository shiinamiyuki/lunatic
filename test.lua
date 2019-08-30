function hello()
	print('Hello world!')
end

hello()

function mult(a,b,c)
	print(a,b,c)
end

mult(1)
mult(1,2)
mult(1,2,3)

tab ={
	["compiler"] = 'gcc',
	["version"] = "8.1"

}

print(tab)


function rec(i, f)
	if i > 0 then
		f(i)
		rec(i-1,f)
	end
end

rec(100,print)

functor = {}
function functor:__call(x)
	print(self)
	return self.f(x)
end

func = {}
setmetatable(func, functor)
func.f = print
print(func)
print(functor)
print(getmetatable(func))
print(func.__call)
--func(3)

print('gc test')

m = {}
m.a = {}
m.b = {}
m.c = m
m.d = m
--m = {}

print(collectgarbage('count'))
--collectgarbage()
m = {}
print(collectgarbage('count'))


function object()
	local o = {}
	function o:print()
		print(o.value)
	end
	return o
end

o = object()
o.value= 3
o.print()

function object()
	local o = {}
	o.print = function()
		print(o.value)
	end
	print(o)
	o.set = function(x)
		o.value = x
	end
	print(o)
	return o
end

o = object()
o.value= 3
o.print()