centos7.6下libstdc++版本过低的解决办法：

(1) 把crossover/bin/libstdc++.so.6.0.21复制到/usr/lib64目录
(2) cd /usr/lib64
	rm libstdc++.so.6
	ln -s libstdc++.so.6.0.21 libstdc++.so.6

	strings /usr/lib64/libstdc++.so.6 | grep GLIBC