-- class like

-- class A
function printf(...) io.write(string.format(unpack(arg))) end

-- class lua_B
lua_B = 
{
--property
	userdata=nil,

--method
	cool=function (self)
		print('lua_B cool()', self)
	end
}
setmetatable(lua_B, {__index=lua_A})
function lua_B:new()
	parent = lua_A:new()
	getmetatable(parent).__index = getmetatable(parent)
	setmetatable(self, getmetatable(parent))
	
	o = {}
	self.userdata = parent
	setmetatable(o, self)
	self.__index = self
	return o
end


-- class lua_C
lua_C = 
{
--method
	cool1 = function (self)
		print('lua_C cool1()', self)
	end
}
setmetatable(lua_C, {__index=lua_B})
function lua_C:new()
	parent = lua_B:new()
	parent.__index = parent
	setmetatable(self, parent)
	
	o = {}
	setmetatable(o, self)
	self.__index = self
	return o
end


global_hello()
lua_A:hellostatic()
lua_B:hellostatic()
lua_C:hellostatic()

a = lua_A(100)
a:hellostatic()
b = lua_A:new(30)

a0 = lua_A:createlua_A()
a0:hello("createlua_A()")
a0:deletelua_A(a0)

print('c = lua_B:new()')
c = lua_B:new()
--c:cool()
c:hello('lua_A lua_B')

print('c = lua_C:new()')
d = lua_C:new()
--d:cool()
d:cool1()
d:hello('lua_A lua_B lua_C')
d0 = lua_C:new()
d0:cool1()

print('a =', a)
print('b =', b)
print('c =', c)

print('metatable =', getmetatable(a))
print('lua_A =', lua_A)

a:hello("AAA")
b:hello("BBB")

a:cool(5, "hello")
b:cool(6, "hello world")

a:hellovoid()

a:hello("AAA", "AAB")
b:hello("BBB", "BBA")

a:hello("AAB", 6)
b:hello("BBA", 5)

-- end
