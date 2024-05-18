cd ./build
cmake ..
make


1.sync_c  sync_s          同步通信示例
2.async_s async_c         异步通信示例
3.async_sn async_cnlogin  多服务异步通信示例
4.async_s_tp   async_c_tp  在3基础上加入线程池  任务交给线程池处理  epoll 线程池 + 线程池


线程池
https://gitee.com/opencv265560/tdpool

测试： 由于测试数据是cpu密集型，导致线程开得 性能不一定越好
async_s_tp :  2 2
./async_c_tp 100000
Greeter received: 10 and encode 0, t: 2361ms
Greeter received: 10 and encode 0, t: 4799ms
Greeter received: 10 and encode 0, t: 7153ms
等待回复，按control c退出， t: 9277ms
Greeter received: 10 and encode 0, t: 9381ms
 need time: 9381ms, tps: 10659.8


./async_c_tp 100000
Greeter received: 10 and encode 0, t: 2798ms
Greeter received: 10 and encode 0, t: 5466ms
Greeter received: 10 and encode 0, t: 8242ms
等待回复，按control c退出， t: 9313ms
Greeter received: 10 and encode 0, t: 10311ms
 need time: 10311ms, tps: 9698.38