print('hello world!')
loadlib("CFuncDLL.dll", "luaopen_mylib")()
print(mylib.mysin(1.0))
