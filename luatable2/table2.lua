-- class like

-- class A
function printf(...) io.write(string.format(unpack(arg))) end

function lua_A:show()
  printf("lua_A balance = $%0.02f\n", self:balance())
end

a = lua_A(100)
b = lua_A:new(30)

print('a =', a)
print('b =', b)
print('metatable =', getmetatable(a))
print('lua_A =', lua_A)

a:hello("AAA");
b:hello("BBB");

-- end
