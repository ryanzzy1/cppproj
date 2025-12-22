#include <iostream>
#include <iomanip>
#include <atomic>
#include <thread>
#include <chrono>

using namespace std;

int main()
{
    std::uint8_t percentage = 0x64;

    // std::thread tt([](){
    //     while(true){
    //         std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            
    //     }
    // });

    std::atomic<bool> flag{false};

    int count = 10;
    while(count-- > 0)
    {
        if (count >8) {
            flag = true;

            cout << "Flag is set to true." << endl;
        }

        cout << "Current count: " << count << endl;
        cout << "Flag value: " << std::boolalpha << flag.load() << endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));        
    }

    std::cout << "dec " << std::dec << (percentage) << "%" << std::endl;
    std::cout << "hex " << std::hex << (percentage) << "%" << std::endl;
    std::cout << "dec " << +percentage << "%" << std::endl;
    std::cout << "hex 0x" << std::hex << +percentage << "%" << std::endl;

        // 重置输出格式（重要！）
    std::cout << std::dec;
    
    // 验证重置成功
    std::cout << "重置后: " << +percentage << std::endl;
    
    std::cout << "Hexadecimal representation: 0x" 
              << std::hex << std::uppercase << std::setw(2) << std::setfill('0') 
              << static_cast<int>(percentage) << std::endl;

    std::cout << "static_cast<double>: " 
              << static_cast<double>(percentage) / 100.0 << std::endl;

    double dectest = static_cast<double>(percentage) / 100.0;


    std::cout << "convert result uint:" << static_cast<int>(dectest * 100) << "\nconvert result int:" << static_cast<int>(dectest * 100) << std::endl;
    //  ipcf_can_frame_sent.data[1] = static_cast<uint8_t>(drvpos * 100)

    return 0;
}