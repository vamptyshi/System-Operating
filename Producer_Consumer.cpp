#include <iostream>
#include <thread>
#include <condition_variable>
#include <queue>
#include <chrono>

class MySemaphore{
    private:
    std::mutex mtx;
    std::condition_variable cv;
    int count;
    public:
    MySemaphore(int init_count): count(init_count){}

    void acquire(){
        std::unique_lock<std::mutex> lock (mtx);
        while(count<=0){
            cv.wait(lock);
        }
        count--;
    }
    
    void release(){
        std::unique_lock<std::mutex> lock(mtx);
        count++;
        cv.notify_one();
    }
};


std::queue<int> buffer;
const int CAPACITY_BUFFER = 5;

MySemaphore sem_empty(CAPACITY_BUFFER);
MySemaphore sem_full(0);
MySemaphore sem_mutex(1);

void producer() {
    for (int i=0; i<=10; ++i){
        sem_empty.acquire();
        sem_mutex.acquire();
        buffer.push(i);
        std::cout<<"[+] Producer added item: "<<i<<std::endl;
        sem_mutex.release();
        sem_full.release();
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
}

void consumer() {
    for (int i=0; i<=10; ++i){
        sem_full.acquire();
        sem_mutex.acquire();
        int item = buffer.front();
        buffer.pop();
        std::cout<<"[-] Consumer removed item: "<<item<<std::endl;
        sem_mutex.release();
        sem_empty.release();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

    }
}
int main (){
    std::cout<<"Starting Producer-Consumer simulation..."<<std::endl;

    std::thread producer_thread(producer);
    std::thread consumer_thread(consumer);

    producer_thread.join();
    consumer_thread.join();
}