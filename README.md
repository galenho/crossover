# crossover

һ����ƽ̨��lua��Ϸ�������������


-------------------------------------------------------------
		crossover(���沽)���
-------------------------------------------------------------
    һ����ƽ̨��lua��Ϸ������������ܣ��ÿ�ܲ��ö��̲߳�����������Ϣ��������ֻ��Ҫ������Ӧ�Ľӿں���������Ӧ�Ļص��������ɣ����߼������Ϊ���̵߳Ŀ���ģʽ��ʹ���������ã��׵��ԣ�
��ά��������չ��ͬʱӵ�п��ٵ���Ӧ������

	���ʹ���������Ŀ���ģʽ��ȥ��Э�̣�RPCԶ�̵��ã���������ͳ������ʽ���ʺ�C++, Java�ȳ���Ա�ڶ�ʱ����תΪlua����

	�����߿��������ֳɴ���ģ�壬���ٴ����bigworld����Ķ���̵�MMORPG��Ⱥ�ܹ���Ҳ�������ɵش������Ϸ���͵ļ�Ⱥ�ܹ���

	Giraffe��crossover��ܵĻ����Ͽ�����MMORPG�ֲ�ʽ��Ϸ��������ܣ�https://github.com/galenho/Giraffe.git

	QQ����Ⱥ: 365280857 (��ӭ���룬����ѧϰ)
	    ����: galen  
		  QQ: 88104725

-----------------------------------------------------
			������ʹ��
-----------------------------------------------------
1. ��windows 10�ϵĹ���, VS.net 2019 ��F7���뼴��

2. ��centos7.6�ϵĹ���:
	
	2.1 centos7.6��libstdc++�汾���͵Ľ���취��

		(1) ��crossover/bin/libstdc++.so.6.0.21���Ƶ�/usr/lib64Ŀ¼
		(2) cd /usr/lib64
			rm libstdc++.so.6
			ln -s libstdc++.so.6.0.21 libstdc++.so.6

			strings /usr/lib64/libstdc++.so.6 | grep GLIBC

	2.2 �������

		cd sh
		chmod -R 750 *
		./automake
		./makeDebug.sh
		./makeRelease.sh

3. ��װmongoDB
	(1) windows 10
		
	(2) centos 7.6
		
4. ִ�е�һ��crossover����

	(1) windows 10
		cd test
		..\crossover.exe hello_world.lua

	(2)	centos 7.6
		cd test
		../crossover hello_world.lua