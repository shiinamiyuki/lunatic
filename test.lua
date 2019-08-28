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
func(3)
