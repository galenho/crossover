# crossover

        一个跨平台的lua游戏服务器开发框架，该框架采用多线程并发来处理消息，开发者只需要调用相应的接口函数并绑定相应的回调
	函数即可，在逻辑层表现为单线程的开发模式，使开发者易用，易调试，易维护，易扩展，同时拥有快速的响应能力。
    
	    框架使用面向对象的开发模式，去除协程，RPC远程调用，更贴近传统开发方式，适合C++, Java等程序员在短时间内转为 lua  
	开发，开发者可以利用现成代码模板，快速搭建类似bigworld引擎的多进程的MMORPG集群架构，也可以轻松地搭建其他游戏类型的
	集群架构。

	    Giraffe在crossover框架的基础上开发的MMORPG分布式游戏服务器框架：https://github.com/galenho/Giraffe.git
		
	    QQ交流群: 365280857 (欢迎加入，互相学习)
	        作者: galen
	          QQ: 88104725
	
	
	一、构建
	
	1. 在windows 10上的构建: 
	
		VS.net 2019打开crossover.sln， 按F7编译即可
	
	2. 在centos7.6上的构建:
	
	  	2.1 centos7.6下libstdc++版本过低的解决办法：
		
		(1) 把crossover/bin/libstdc++.so.6.0.21复制到/usr/lib64目录
		(2) cd /usr/lib64
			rm libstdc++.so.6
			ln -s libstdc++.so.6.0.21 libstdc++.so.6

			strings /usr/lib64/libstdc++.so.6 | grep GLIBC
			
		2.2 编译程序
		
			cd sh
			chmod -R 750 *
			./automake
			./makeDebug.sh
			./makeRelease.sh
			
	二、安装mongoDB
	  
	  	(1) windows 10
		
		(2) centos 7.6
		
	三、执行第一个crossover程序

		(1) windows 10
			cd test
			..\crossover.exe hello_world.lua
			
		(2) centos 7.6
			cd test
			../crossover hello_world.lua
