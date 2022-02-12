
function fun_stop_callback(timer_id)
	-- 这里写相关的逻辑，直到判定可以退出
    
    
    
	crossover.remove_timer(timer_id)
	crossover.exit()
end

function fun_console_callback(cmd)
	crossover.write_log(cmd, 1)
	crossover.save_log()
	
	if cmd == "exit" then
		crossover.stop(fun_stop_callback)
	end
end

crossover.set_console_handler(fun_console_callback)