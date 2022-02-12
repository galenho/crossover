luaxml = {}

function luaxml.find_val(val, key)
	if type(val) == "table" then
        return luaxml.find_node(val, key)
	else
		return false, {}
    end
end

function luaxml.find_node(obj, key)
	if obj[0] == key then
		return true, obj
	else
		for k, v in pairs(obj) do
			if type(k) == "number" then
				success, result = luaxml.find_val(v, key)
				if success then
					return true, result
				end
			end
		end
		
		return false, {}
	end
end

function luaxml.load(filename)
	return crossover.loadxml(filename)
end

return luaxml