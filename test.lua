function test()
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
		return o
	end

	o = object()
	o.value= 3
	o.print()

	for i=0,10 do
		print(i)
	end

	function add(x) return function (y) return x+y end end 
	print(add(3)(4))

	function build(depth)
		if depth == 0 then
			return nil
		end
		local tree = {}
		tree.left = build(depth - 1)
		tree.right = build(depth - 1)
		--print(tree)
		return tree
	end

	function find(tree)
		if not tree then
			return
		end
		if tree.left then
			tree.left.parent = tree
			find(tree.left)
		end
		if tree.right then
			tree.right.parent = tree
			find(tree.right)
		end
	end

	N = 4
	print('----no cycles----')
	tree = build(N)

	print(collectgarbage('count'))

	tree = {}

	print('now force gc')

	collectgarbage()

	print(collectgarbage('count'))

	print('---- cycles----')
	tree = build(N)
	find(tree)

	print(collectgarbage('count'))

	tree = {}

	print('now force gc')

	collectgarbage()

	print(collectgarbage('count'))

	function t_error()
		function g()
			h()
		end
		function h()
			local m = {}
			m = m + 1
		end
		function f()
			g()
		end
		f()
	end
	local ret,msg =pcall(t_error)
	if  ret then
		print('failed to catch error',ret,msg)
	else
		print('successfully caugth error',msg)
	end

	function range(n)
		local i = 1
		return function()
			if i > n then
				return nil
			else
				local r = i
				i = i + 1
				return r
			end
		end
	end

	r = range(10)
	print(r())

	for i in range(10) do print(i) end

	function add(x)return function (y)return x+y end end
	print(add(3)(4))
end


ret, msg = pcall(test)
if not ret then
	print('test failed ', ret, msg)
else
	print('test completed')
end 


