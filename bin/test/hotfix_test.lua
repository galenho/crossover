package.path = package.path..";../common/hotfix/?.lua"
package.cpath = package.cpath..";../?.dll"

test = require("hotfix_module")
hotfix = require("hotfix")

test.func()
print(test.count2)

hotfix.hotfix_module("hotfix_module")
test.func()
print(test.count2)