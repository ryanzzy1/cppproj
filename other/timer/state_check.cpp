#include <iostream>
#include <thread>
#include <string>
#include <chrono>
#include <csignal>
#include <atomic>

enum class pwrState : std::uint8_t{
    OFF = 0,
    ACC = 1,
    ON = 2,
    START = 3
};

bool flag = true;
std::atomic<bool> auto_slpmod_flag;

pwrState current_state;
pwrState last_state;
int i = 0;

pwrState state_change()
{
    while(i < 4 && flag)
    {
        std::cout << "i = " << *(&i) << std::endl;
        switch (i) {
            case 3 : 
                current_state = pwrState::OFF;
                std::cout << "TH1 Current_state: OFF" << std::endl;
            break;            
            case 2 : 
                current_state = pwrState::ACC;
                std::cout << "TH1 Current_state: ACC" << std::endl;
            break;            
            case 1 : 
                current_state = pwrState::ON;
                std::cout << "TH1 Current_state: ON" << std::endl;
            break;            
            case 0 : 
                current_state = pwrState::START;
                std::cout << "TH1 Current_state: START" << std::endl;
            break;
        }

        i++;
        if (i == 4) i = 0;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    return current_state;
}

pwrState state_check()
{
    last_state = current_state;
    
    while (flag)
    {
        if (last_state != current_state)
        {
            if((auto_slpmod_flag.load() == false) && (last_state != pwrState::OFF) && (current_state == pwrState::OFF)){
                auto_slpmod_flag.store(true);

                switch (last_state)
                {
                case pwrState::OFF:
                    std::cout << "TH2 *** last_state: OFF" << std::endl;
                    break;
                case pwrState::ACC:
                    std::cout << "TH2 *** last_state: ACC" << std::endl;
                    break;
                case pwrState::ON:
                    std::cout << "TH2 *** last_state: ON" << std::endl;
                    break;
                case pwrState::START:
                    std::cout << "TH2 *** last_state: START" << std::endl;
                    break;
                default:
                    break;
                }

                switch (current_state) {
                case pwrState::OFF:
                    std::cout << "TH2 *** Current_state: OFF" << std::endl;
                    break;            
                case pwrState::ACC:
                    std::cout << "TH2 *** Current_state: ACC" << std::endl;
                    break;            
                case pwrState::ON:
                    std::cout << "TH2 *** Current_state: ON" << std::endl;
                    break;            
                case pwrState::START:
                    std::cout << "TH2 *** Current_state: START" << std::endl;
                    break;
                }                
            }
/*
            switch (last_state)
            {
            case pwrState::OFF:
                std::cout << "TH2 *** last_state: OFF" << std::endl;
                break;
            case pwrState::ACC:
                std::cout << "TH2 *** last_state: ACC" << std::endl;
                break;
            case pwrState::ON:
                std::cout << "TH2 *** last_state: ON" << std::endl;
                break;
            case pwrState::START:
                std::cout << "TH2 *** last_state: START" << std::endl;
                break;
            default:
                break;
            }

            switch (current_state) {
            case pwrState::OFF:
                std::cout << "TH2 *** Current_state: OFF" << std::endl;
                break;            
            case pwrState::ACC:
                std::cout << "TH2 *** Current_state: ACC" << std::endl;
                break;            
            case pwrState::ON:
                std::cout << "TH2 *** Current_state: ON" << std::endl;
                break;            
            case pwrState::START:
                std::cout << "TH2 *** Current_state: START" << std::endl;
                break;
            }*/

        }else{
            switch (current_state) {
            case pwrState::OFF:
                std::cout << "TH2 *** last_state = Current_state: OFF" << std::endl;
                break;            
            case pwrState::ACC:
                std::cout << "TH2 *** last_state = Current_state: ACC" << std::endl;
                break;            
            case pwrState::ON:
                std::cout << "TH2 *** last_state = Current_state: ON" << std::endl;
                break;            
            case pwrState::START:
                std::cout << "TH2 *** last_state = Current_state: START" << std::endl;
                break;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    return last_state;
}

void state_flag()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(10000));

    flag = false;
}

void slpmod_nonoff_run()
{
    while (flag) {
        if(auto_slpmod_flag.load() == true){
            std::cout << "atuo_slpmod_flag set to true." << std::endl;
            
            return;
        }
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }

    std::cout << "auto slpmod_flag set to false and quit." << std::endl; 
} 

int main()
{    
    auto_slpmod_flag.store(false);

    std::thread th1(state_change), th2(state_check), th3(state_flag), th4(slpmod_nonoff_run);
    // state_change();

    if (th2.joinable()){
        th2.join();
    }

    if (th1.joinable()){
        th1.join();
    }

    if (th3.joinable()){
        th3.join();
    }

    if (th4.joinable()){
        th4.join();
    }

    return 0;
}
