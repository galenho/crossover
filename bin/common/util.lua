
----------------------------------------------------------------------------------
--                util辅助函数
----------------------------------------------------------------------------------
require "common"

local M = {}

--获取文件名
function M.getFileName(str)
    local idx = str:match(".+()%.%w+$")
    if(idx) then
        return str:sub(1, idx-1)
    else
        return str
    end
end

--获取扩展名
function M.getExtension(str)
    return str:match(".+%.(%w+)$")
end

--递归遍历目录
function M.searchDir(rootpath)
    for entry in lfs.dir(rootpath) do
        if entry ~= '.' and entry ~= '..' then
            local path = rootpath .. '/' .. entry
            local attr = lfs.attributes(path)
			local filename = M.getFileName(entry)

			if attr.mode ~= 'directory' then
				local postfix = M.getExtension(entry)
				print(filename .. '\t' .. attr.mode .. '\t' .. postfix)
			else
				print(filename .. '\t' .. attr.mode)
				M.searchDir(path)
			end
        end
    end
end
   
return M