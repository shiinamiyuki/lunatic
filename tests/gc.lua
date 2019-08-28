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

tree = build(16)

find(tree)

print(collectgarbage('count'))

tree = {}

print(collectgarbage('count'))

print('now force gc')

collectgarbage()

print(collectgarbage('count'))
