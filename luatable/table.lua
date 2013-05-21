function hello(x, y)
	return x+y, ' hello world!'
end

function gettable()
	return
	{
		version = 1.0,
		title = "test",
		func = hello
	}
end

function settable(t)
	print(t.version)
	print(t.title)
	t.callback()
	return "set table result!"
end