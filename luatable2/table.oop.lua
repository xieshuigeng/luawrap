-- class like

-- class A
lua_TA = 
{
	version = 1.0,
	title = "lua_TA",
	showme = function (self) print("version : "..version.."\ntitle : "..title) end
}

function lua_TA:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end

-- class B
lua_TB = 
{
}

function lua_TB:new(o)
	o = self
	p = lua_TA:new({})
	setmetatable(o,p);
	return o
end

-- 
