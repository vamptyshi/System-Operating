#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <chrono>

int women_in=0;
int man_in=0;

std::mutex mtx_counts;
std::condition_variable cv_bathroom;
bool bathroom_empty =true;

void woman(int id){
    std::unique_lock<std::mutex> lock(mtx_counts);
    if (women_in == 0){
        while(!bathroom_empty){
            cv_bathroom.wait(lock);
        }
        bathroom_empty = false;
    }
    women_in++;
    lock.unlock();
    {
        std::lock_guard<std::mutex> console_lock(mtx_counts);
        std::cout<<"[Women]The women"<<id<<" entered. Total:"<<women_in<<std::endl;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    lock.lock();
    women_in--;
    std::cout<<"[Women]The women"<<id<<" left. Total:"<<women_in<<std::endl;
    if (women_in ==0){
        std::cout<<" The bathroom is now empty."<<std::endl;
        bathroom_empty = true;
        cv_bathroom.notify_all();
    }
    lock.unlock();
}

void man(int id){
    std::unique_lock<std::mutex> lock (mtx_counts);
    if (man_in == 0){
        while(!bathroom_empty){
            cv_bathroom.wait(lock);
        }
        bathroom_empty = false;
    }
    man_in++;
    lock.unlock();

    {
        std::lock_guard<std::mutex> console_lock(mtx_counts);
        std::cout<<"[Man] The man "<<id<<" entered. Total:"<<man_in<<std::endl;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    lock.lock();
    man_in--;
    std::cout<<"[Man] The man "<<id<<" left. Total:"<<man_in<<std::endl;
    if (man_in ==0){
        std::cout<<" The bathroom is now empty."<<std::endl;
        bathroom_empty =true;
        cv_bathroom.notify_all();
    }
    lock.unlock();

}
int main (){
    std::cout<<"Starting the unisex bathroom simulation..."<<std::endl;
    std::vector<std::thread> person;

    person.push_back(std::thread(woman,1));
    person.push_back(std::thread(woman,2));
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    person.push_back(std::thread(man,1));
    person.push_back(std::thread(man,2));
    person.push_back(std::thread(man,3));
    person.push_back(std::thread(woman,3));
    for (auto& th: person){
        th.join();
    }
    std::cout<<"Simulation finished."<<std::endl;
    return 0;
}