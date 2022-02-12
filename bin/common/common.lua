RETRY_CONNECT_INTERVAL	= 3000
INVALID_INDEX = 4294967295

ServerType = 
{
	SERVERTYPE_INVALID  = 0, 
	SERVERTYPE_WORLD    = 1, 
	SERVERTYPE_CONNECT  = 2, 
	SERVERTYPE_MAP      = 3, 
    SERVERTYPE_LOGIN    = 4, 
	SERVERTYPE_CSM      = 5, 
	SERVERTYPE_DB       = 6, 
	SERVERTYPE_LOG      = 7, 
	SERVERTYPE_PORTAL   = 8, 
	SERVERTYPE_PM       = 9
}

LogLevel = 
{
	LOG_LEVEL_DEBUG = 0,
	LOG_LEVEL_INFO  = 1,
	LOG_LEVEL_WARN  = 2,
	LOG_LEVEL_ERROR = 3
}

PeerStatusType = 
{
	E_PEER_CREATED			= 1,
	E_PEER_CONNECTING		= 2,	
	E_PEER_ALREADY_LOGIN	= 3,
	E_PEER_CLOSE			= 4
}

-- 登录方式
PlatformType = 
{
	E_PT_INTERNAL			= 0,	-- md5加密密码 (内网)
	E_PT_DIYIDAN_ANDROID	= 1,	-- 第一弹ANDROID
}

-- 登录结果
LoginResult = 
{
	E_LR_FAILED_UNKNOWNERROR		= 0,
	E_LR_SUCCESS					= 1,	-- success
	E_LR_FAILED_ALREADYLOGIN		= 2,	-- 同帐号已经登录
	E_LR_FAILED_SERVERINTERNALERROR	= 3,	--	e.g. no available
	E_LR_FAILED_INVALIDACCOUNT		= 4,
	E_LR_FAILED_INVALIDPASSWORD		= 5,
	E_LR_FAILED_VERSION_ERROR		= 6,
	E_LR_FAILED_TOKEN_TIMEOUT		= 7,
	E_LR_FAILED_NOT_ACTIVATION		= 8,	-- 账号没激活
	E_LR_FAILED_ACCOUNT_INFO_ERROR	= 9,	-- 账号相关的验证信息错误
	E_LR_FAILED_REGISTER_EXCESS		= 10,	-- 本服注册人数超额---NetForServer::on_login_verify（小服注册人数跟激活绑定）
	E_LR_FAILED_MAX_REGISTER_REALM	= 11	-- 个人已经达到小服最大注册数量，不能再注册新服
}

-- 创角结果
CreateCharacterResult = 
{
	E_CCR_SUCCESS							= 1,	-- 成功					
	E_CCR_FAILED_CHARCOUNTLIMIT				= 2,	-- 创建人数已经达到上限
	E_CCR_FAILED_INVALIDPARAM_REPEATED_NAME	= 3,	-- 名字重复
	E_CCR_FAILED_INVALIDPARAM_INVALID_NAME	= 4		-- 名字非法
}

----------------------------------------------------------------------------------
--                全局函数 (注: 不可以热更新，一般不需要改变)
----------------------------------------------------------------------------------

function LOG_DEBUG(str)
	crossover.write_log(str, 0)
end

function LOG_INFO(str)
	crossover.write_log(str, 1)
end

function LOG_WARN(str)
	crossover.write_log(str, 2)
end

function LOG_ERROR(str)
	crossover.write_log(str, 3)
end

function FormatNum(num)
	if num <= 0 then
		return 0
	else
		local t1, t2 = math.modf(num)
		---小数如果为0，则去掉
		if t2 > 0 then
			return num
		else
			return t1
		end
	end
end

function dump2(obj)
    local getIndent, quoteStr, wrapKey, wrapVal, dumpObj
    getIndent = function(level)
        return string.rep("\t", level)
    end
    quoteStr = function(str)
        return '"' .. string.gsub(str, '"', '\\"') .. '"'
    end
    wrapKey = function(val)
        if type(val) == "number" then
            if math.type(val) == "integer" then
                return "[" .. FormatNum(val) .. "]"
            else
                return "[" .. val .. "]"
            end
        elseif type(val) == "string" then
            return "[" .. quoteStr(val) .. "]"
        else
            return "[" .. tostring(val) .. "]"
        end
    end
    wrapVal = function(val, level)
        if type(val) == "table" then
            return dumpObj(val, level)
        elseif type(val) == "number" then
            return tostring(val)
        elseif type(val) == "string" then
            return quoteStr(val)
        else
            return tostring(val)
        end
    end
    dumpObj = function(obj, level)
        if type(obj) ~= "table" then
            return wrapVal(obj)
        end
        level = level + 1
        local tokens = {}
        tokens[#tokens + 1] = "{"
        for k, v in pairs(obj) do
            tokens[#tokens + 1] = getIndent(level) .. wrapKey(k) .. " = " .. wrapVal(v, level) .. ","
        end
        tokens[#tokens + 1] = getIndent(level - 1) .. "}"
        return table.concat(tokens, "\n")
    end
    return dumpObj(obj, 0)
end

function dump(obj)
	LOG_INFO(dump2(obj))
end

function PrintTraceInfo()
    str = debug.traceback("", 2)
    LOG_ERROR(str)
    
    local startLevel = 2 --0表示getinfo本身, 1表示调用getinfo的函数(PrintTraceInfo), 2表示调用PrintLuaStack的函数,可以想象一个getinfo(0级)在顶的栈.
    local maxLevel = 10  --最大递归10层
    for level = startLevel, maxLevel do

        -- 打印堆栈每一层
        local info = debug.getinfo(level, "nSl")
        if info == nil then break end
        
        LOG_ERROR("--------------------------------------------------------------------")
        LOG_ERROR( string.format("[ line : %-4d] %-20s :: %s", info.currentline, info.name or "", info.source or "" ) )

        -- 打印该层的参数与局部变量
        local index = 1 --1表示第一个参数或局部变量, 依次类推

        while true do

            local name, value = debug.getlocal( level, index )

            if name == nil or name == "(*temporary)" or name == "self" then break end
         
            if value ~= nil then
                LOG_ERROR( string.format( "\t%s = %s", name, dump2(value) ) )
            end

            index = index + 1

        end
    end
    
    LOG_INFO("-----------------------------------------------------------------------------------------------\n")
end

function GenerateSrvUID( st, realm_idx, area_idx )
	-- 8位十进制: 服号(5位) + ServerType(2位)  + 区域号(1位)
	--                00000       00                0
	return FormatNum(realm_idx * 1000 + st * 10 + area_idx)
end

function GenerateClientUID( srv_uid, conn_idx )
	return FormatNum(srv_uid * 10000000000 + conn_idx)
end

function GenerateObjID( serial_idx, obj_idx)
	-- 19位十进制: serial_idx(12位) + obj_idx(7位)
    return FormatNum( serial_idx * 10000000 + obj_idx )
end

function GetCmd(bytes)
	return FormatNum(string.byte(bytes, 1) + string.byte(bytes, 2) * 256 + string.byte(bytes, 3) * 65535 + string.byte(bytes, 4) * 16777216)
end

function clone(object)
    local lookup_table = {}
    local function _copy(object)
        if type(object) ~= "table" then
            return object
        elseif lookup_table[object] then
            return lookup_table[object]
        end
        local newObject = {}
        lookup_table[object] = newObject
        for key, value in pairs(object) do
            newObject[_copy(key)] = _copy(value)
        end
        return setmetatable(newObject, getmetatable(object))
    end
    return _copy(object)
end

function get_appsrv_name(srv_type)
	if srv_type == ServerType.SERVERTYPE_WORLD then
		return "SERVERTYPE_WORLD"
	elseif srv_type == ServerType.SERVERTYPE_CONNECT then
		return "SERVERTYPE_CONNECT"
	elseif srv_type == ServerType.SERVERTYPE_MAP then
		return "SERVERTYPE_MAP"
     elseif srv_type == ServerType.SERVERTYPE_LOGIN then
		return "SERVERTYPE_LOGIN"
	elseif srv_type == ServerType.SERVERTYPE_CSM then
		return "SERVERTYPE_CSM"
	elseif srv_type == ServerType.SERVERTYPE_DB then
		return "SERVERTYPE_DB"
	elseif srv_type == ServerType.SERVERTYPE_LOG then
		return "SERVERTYPE_LOG"
	elseif srv_type == ServerType.SERVERTYPE_PORTAL then
		return "SERVERTYPE_PORTAL"
	elseif srv_type == ServerType.SERVERTYPE_PM then
		return "SERVERTYPE_PM"
	else
		return "SERVERTYPE_INVALID"
	end
end

function table_len(t)
    local len=0
    for k, v in pairs(t) do
        len=len+1
    end
    return len
end
