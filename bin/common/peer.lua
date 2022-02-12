local common = require "common"
local util = require "util"

Peer = {}

function Peer:New(o)
    o = o or {}	
	self.__index = self
	setmetatable(o, self)
	
	-- 下面写成员变量
	o.status_= PeerStatusType.E_PEER_CREATED
	o.srv_info_ = {}
	o.network_ = nil
	o.res_loaded_ = false
	o.handlers_ = {}
	o.client_handlers_ = {}
	
    return o
end

function Peer:InitMsgHandle()
    
end

function Peer:Send(cmd, data)
	data.cmd = cmd
	byte, len = seri.pack(data)
    self.network_:send_msg(self.srv_info_.conn_idx, byte, len)
end

function Peer:HandleMsg(conn_idx, msg)
	if self.handlers_[msg.cmd] then
		self.handlers_[msg.cmd](self, msg)
	else
		LOG_ERROR("recv invalid msg idx:" .. msg.cmd)
	end
end

return Peer