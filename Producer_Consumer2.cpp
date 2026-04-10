#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <chrono>

class MonitorBuffer {
    private:
    std::vector<int> buffer;
    int size;
    int count;
    int in;
    int out;
    std::mutex mtx;
    std::condition_variable not_full;
    std::condition_variable not_empty;
    public:
    MonitorBuffer(int size): size(size), count(0), in(0), out(0){
        buffer.resize(size);
    }
    void insert (int item){
        std::unique_lock<std::mutex> lock(mtx);
        while (count == size){
            std::cout<<"Buffer is full. Producer is waiting..."<<std::endl;
            not_full.wait(lock);

        }
        buffer[in]=item;
        in = (in+1)%size;
        count++;
        std::cout<<"[+] Producer added item: "<<item<<std::endl;
        not_empty.notify_one();
    }
    int remove() {
        std::unique_lock<std::mutex> lock(mtx);
        while(count == 0){
            std::cout<<"Buffer is empty. Consumer is waiting..."<<std::endl;
            not_empty.wait(lock);

        }
        int item = buffer[out];
        out = (out+1)%size;
        count--;
        std::cout<<"[-] Consumer removed item: "<<item<<" | Buffer count: "<<count<<std::endl;
        not_full.notify_one();
        return item;
    }
};
void producer(MonitorBuffer& monitor){
    int item =0;
    while(true){
        item++;
        monitor.insert(item);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
void consumer(MonitorBuffer& monitor){
    while(true){
        monitor.remove();
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
}
int main(){
    MonitorBuffer monitor(5);
    std::thread prod(producer, std::ref(monitor));
    std::thread cons(consumer, std::ref(monitor));
    prod.join();
    cons.join();
}