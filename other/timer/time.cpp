#include <iostream>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <map>
#include <thread>
#include <functional>
#include <iomanip>

class timer{
public:

    timer(std::chrono::seconds t, bool go) : duration(t), running(go){}

    void start(){
        running = true;
        work_thread = std::thread(&timer::work_job, this);
    }

    void stop(){
        running = false;
        if (work_thread.joinable()){
            work_thread.join();
        }
    }
  
private:

    std::thread work_thread;

    std::chrono::steady_clock::time_point start_time;

    std::chrono::seconds duration;

    std::function<void()> callback;

    bool running;

    void work_job(){
        while(running){
            auto now = std::chrono::steady_clock::now();
            
            auto endtime = start_time + duration;

            auto next_run = now + duration;

            if (now >= endtime){
                
                std::cout << "time to go!" << std::endl;
            }
            std::this_thread::sleep_until(next_run);
        }
    }
};
typedef struct {
    uint8_t can_interface_index;    // CAN接口索引 [0:15]
    uint32_t can_id;                // CAN ID
    uint8_t dlc;                    // 数据长度码DLC[0-15]
    uint8_t data[64];               // 报文数据
} Canframe;

Canframe test[2] = {
    {0,1,8,{0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7}},
    {1,2,8,{0x1,0x2,0x3,0x4,0x5,0x7,0x8}}
};

int main()
{
    for (int i = 0; i < test[1].dlc; ++i){
        std::cout << "test[0] = "<< std::hex << std::setw(2)<< "0x0"<<(unsigned int)test[0].data[i] << std::endl;
        std::cout << "test[1] = "<< std::hex << std::setw(2) <<"0x0"<<(unsigned int)test[1].data[i] << std::endl;
    }
    
    timer ttt(std::chrono::seconds(2), true);
    ttt.start();

    
    std::this_thread::sleep_for(std::chrono::seconds(5));
    ttt.stop();

}