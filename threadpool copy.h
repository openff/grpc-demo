#pragma once
#ifndef THREAD_POOL
#define THREAD_POOL
#include <iostream>
#include <vector>
#include <queue>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <unordered_map>
#include <thread>
#include <future>

const int Task_Max_ThreadHold = 1024;  // 任务阈值
const int Thread_Max_ThreadHold = 100; // 线程阈值
const int Thread_Max_TIMEHold = 2;	   // 超过60s关闭线程

class thread_;
class thread_pool;

enum class TPoolMode
{
	MODE_FIXED, // 固定线程池大小
	MODE_CACHED // 动态线程池大小
};
class thread_
{
public:
	// 定义void threadFunc();的函数对象类型
	using Threadfunc = std::function<void(int)>;

	thread_(Threadfunc func);
	~thread_();
	void start();
	int get_threadId() const;

private:
	Threadfunc func_;
	int threadId_;
	static int threadfun_;
};

class thread_pool
{
private:
	void set_Threadpool_initSize(size_t max)
	{
		if (!check_Runing())
			thread_initsize = max;
	} 
	// 设置TpoolSize配置大小
	bool check_Runing() const { return isRuning_Threadspool; } // 查看线程池是否开启

	using Task = std::function<void()>;
	// 由于 std::shared_ptr 的引用计数机制，临时对象在被 shared_ptr 管理时不会被立即释放。
	// c++库内一般是_xxx 避免与c++库相同

	std::queue<Task> task_queue; // 任务队列
	std::atomic_uint task_count; // 当前任务个数
	std::size_t queue_taskMax;	 // 最大任务个数

	std::mutex task_mtx;					 // 线程安全
	std::condition_variable task_isnotempty; // 线程通信
	std::condition_variable task_isnotfull;	 // 线程通信

	TPoolMode pool_mode; // 记录线程模式
	// std::vector<std::unique_ptr<thread_>> threads_;		//线程列表
	std::unordered_map<int, std::unique_ptr<thread_>> threads_;

	std::size_t thread_initsize;		  // 配置大小
	std::size_t thread_maxsize;			  // 线程的上限阈值
	std::atomic_uint32_t thread_size;	  // 当前线程池  线程的数量
	std::atomic_uint32_t idle_threadSize; // 空闲线程

	// 线程池回收
	std::condition_variable pool_destroy;
	std::mutex pool_destroy_mtx;

	std::atomic_bool isRuning_Threadspool; // 记录pool是否开始运行

public:
	thread_pool();

	~thread_pool();
	// 关闭拷贝
	thread_pool(thread_pool &) = delete;
	thread_pool &operator=(thread_pool &) = delete;

	// 配置
	void set_Mode(TPoolMode mode);
	void set_task_Max(size_t max);
	void set_Thread_Max(size_t max);
	// 开启线程池
	void start(size_t size = std::thread::hardware_concurrency());
	// 提交任务给线程
	//  submitTask(sum1,10,20);	特点 任意类型 参数折叠  返回值推导
	/*
	 packaged_task<int(int, int)> task(sum);  // 创建一个 packaged_task，封装 sum 函数
	  future<int> result = task.get_future();  // 获取与 packaged_task 关联的 future
	*/
	//&&折叠引用  可以接受 & and &&
	template <typename _FUNC, typename... _Arg>
	auto submitTask(_FUNC &&func, _Arg &&...arg) -> std::future<decltype(func(arg...))> // auto = std::future<decltype(func(arg))>
	{
		// 打包任务		new 一个任务 （task<Res ()> ( bind(func,arg...))）
		using ResType = decltype(func(arg...));
		auto task = std::make_shared<std::packaged_task<ResType()>>(
			std::bind(std::forward<_FUNC>(func), std::forward<_Arg>(arg)...));
		std::future<ResType> result = task->get_future();

		// 获取锁
		std::unique_lock<std::mutex> lock(task_mtx);
		if (!task_isnotfull.wait_for(lock, std::chrono::seconds(1),
									 [&]() -> bool
									 {
										 return task_queue.size() < queue_taskMax;
									 }))
		{
			// ！1 满了 等
			// 等待1s 依然队列满
			std::cerr << "submit task full error" << std::endl;
			auto task = std::make_shared<std::packaged_task<ResType()>>([]() -> ResType
																		{ return ResType(); });
			// 返回0值  int()| double()... ==> 0
			(*task)(); // 必须调用一次该任务，
			//	如果不调用 (*task)();，任务将不会执行，std::future 对象将无法获得结果。
			return task->get_future();
		}

		// 不满  则添加任务  通知 生产线程处理
		// using Task = function (void())		把void()放进去，封装一下
		task_queue.emplace([task]()
						   {
							   // 真正执行的
							   (*task)(); // packaged_task ()
						   });
		task_count++;
		// 通知 线程处理
		task_isnotempty.notify_all();
		// cache适用于短期、频繁使用的任务类型。
		if (pool_mode == TPoolMode::MODE_CACHED && // 模式
			task_count > idle_threadSize &&		   // 任务个数  >  空闲线程个数
			thread_size < thread_maxsize)		   // 线程个数 < 阈值
		{
			std::cout << "thread create :" << std::this_thread::get_id() << std::endl;
			// 而外创建线程
			// std::unique_ptr<thread_> sp = std::make_unique<thread_>(std::bind(&thread_pool::threadFunc, this));
			// threads_.emplace_back({sp->get_threadId(),std::move(sp)});	 //unique关闭了拷贝构造
			std::unique_ptr<thread_> sp = std::make_unique<thread_>(std::bind(&thread_pool::threadFunc, this, std::placeholders::_1));
			int idx = sp->get_threadId();
			threads_.emplace(idx, std::move(sp)); // unique关闭了拷贝构造

			threads_[idx]->start(); // 开启线程
			// 线程相关变量
			thread_size++;
			idle_threadSize++;
		}

		return result;
	}
	// 从任务队列消费任务
	void threadFunc(int threadId);
};

#endif