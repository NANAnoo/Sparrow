//
// Created by Shawwy on 4/11/2023.
//

#ifndef SPARROW_THREADPOOL_H
#define SPARROW_THREADPOOL_H

#include <stdlib.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <atomic>
#include <future>
#include <vector>
#include <condition_variable>
namespace SPW{
    const int NUMBER = 2;
    using namespace std;

    using callback = void(*)(void*);
    using Task = function<void()>;

    class ThreadPool
    {
    public:
        ThreadPool(int min, int max)
        {
            do
            {
                minNum = min;
                maxNum = max;
                busyNum = 0;
                liveNum = min;
                exitNum = 0;

                shutdown = false;
                managerID = thread(manager,this);

                threadIDs.resize(max);
                for(int i = 0; i < min; ++i)
                {
                    threadIDs[i] = thread(worker,this);
                }
                return;
            } while (0);
        }


        template<class F, class... Args>
        void add(F&& f, Args&&... args)
        {
            if(shutdown)
            {
                return;
            }
            using RetType = decltype(f(args...)); // typename std::result_of<F(Args...)>::type, 函数 f 的返回值类型

            auto task = make_shared<packaged_task<RetType()>>(
                    bind(forward<F>(f), forward<Args>(args)...)
            ); // 把函数入口及参数,打包(绑定)

            future<RetType> future = task->get_future();

            {    // 添加任务到队列
                lock_guard<mutex> lock{ mutexPool };//对当前块的语句加锁  lock_guard 是 mutex 的 stack 封装类，构造的时候 lock()，析构的时候 unlock()
                taskQ.push([task]() { (*task)(); });
            }

            cond.notify_all();
        }

        int BusyNum()
        {
            //mutexPool.lock();
            int busy = busyNum;
           // mutexPool.unlock();
            return busy;
        }

        int AliveNum()
        {
            //mutexPool.lock();
            int alive = liveNum;
            //mutexPool.unlock();
            return alive;
        }

        ~ThreadPool()
        {
            shutdown = true;
            //阻塞回收管理者线程
            if(managerID.joinable()) managerID.join();
            //唤醒阻塞的消费者线程
            cond.notify_all();
            for(int i = 0; i < maxNum; ++i)
            {
                if(threadIDs[i].joinable()) threadIDs[i].join();
            }
        }

    private:

        queue<Task> taskQ;
        thread managerID;
        vector<thread> threadIDs;
        int minNum;
        int maxNum;
        int busyNum;
        int liveNum;
        int exitNum;

        mutex mutexPool;
        condition_variable cond;
        bool shutdown;
        static void manager(void* arg)
        {
            ThreadPool* pool = static_cast<ThreadPool*>(arg);
            while(!pool->shutdown)
            {
                //每隔3秒检测一次
                //取出线程池中任务的数量和当前线程的数量
                unique_lock<mutex> lk(pool->mutexPool);
                int queuesize = pool->taskQ.size();
                int livenum = pool->AliveNum();
                int busynum = pool->BusyNum();

                lk.unlock();

                //添加线程
                //任务的个数>存活的线程个数 && 存活的线程数 < 最大线程数
                if(queuesize > livenum && livenum <pool->maxNum)
                {
                    lk.lock();
                    int count = 0;
                    for(int i = 0; i < pool->maxNum && count < NUMBER && pool->liveNum < pool->maxNum; ++i)
                    {
                        if(pool->threadIDs[i].get_id() == thread::id())
                        {
                            pool->threadIDs[i] = thread(worker,pool);
                            count++;
                            pool->liveNum++;
                        }
                    }
                    lk.unlock();
                }

                //销毁线程
                //忙的线程*2 < 存活的线程数 && 存活的线程数 >  最小的线程数
                if(busynum*2 < livenum && livenum > pool->minNum)
                {
                    lk.lock();
                    pool->exitNum = NUMBER;
                    lk.unlock();
                    //让工作的线程自杀
                    for(int i = 0; i < NUMBER; ++i) pool->cond.notify_all();
                }
            }
        }


        static void worker(void* arg)
        {
            ThreadPool* pool = static_cast<ThreadPool*>(arg);
            while(true) {
                unique_lock<mutex> lk(pool->mutexPool);
                //当前任务队列是否为空
                while (pool->taskQ.empty() && !pool->shutdown) {
                    //阻塞工作线程
                    pool->cond.wait(lk);
                    if (pool->exitNum > 0) {
                        pool->exitNum--;
                        if (pool->liveNum > pool->minNum) {
                            pool->liveNum--;
                            lk.unlock();
                            return;
                        }
                    }
                }
                //判断线程池是否关闭了
                if (pool->shutdown) {
                    return;
                }

                //从任务队列中去除一个任务
                Task task = pool->taskQ.front();
                pool->taskQ.pop();
                pool->busyNum++;
                //解锁
                lk.unlock();

                task();

                lk.lock();
                pool->busyNum--;
                lk.unlock();
            }
        }

    };
}
#endif //SPARROW_THREADPOOL_H
