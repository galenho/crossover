-----------------------------------------------------
		crossover(交叉步)简介
-----------------------------------------------------
    一个跨平台的lua游戏服务器开发框架，该框架采用多线程并发来处理消息，开发者只需要调用相应的接口函数并绑定相应的回调函数即可，在逻辑层表现为单线程的开发模式，使开发者易用，易调试，
易维护，易扩展，同时拥有快速的响应能力。

	开发者可以利用现成代码模板，快速搭建类似bigworld引擎的多进程的MMORPG集群架构，也可以轻松地搭建其他游戏类型的集群架构。



	稳定性，易用性，可维护性(容灾)，可扩展性，并发性能

	
-----------------------------------------------------
			构建和使用
-----------------------------------------------------
1. 在windows 10上的构建, VS.net 2019 按F7编译即可

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

3. 安装mongoDB
	(1) windows 10
		
	(2) centos 7.6
		
4. 执行第一个crossover程序

	(1) windows 10
		cd test
		..\crossover.exe hello_world.lua

	(2)	centos 7.6
		cd test
		../crossover hello_world.lua

-----------------------------------------------------
		CrossOver简易教程
-----------------------------------------------------