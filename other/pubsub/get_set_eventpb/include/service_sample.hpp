#ifndef SERVICE_SAMPLE_HPP_
#define SERVICE_SAMPLE_HPP_

#ifndef VSOMEIP_ENABLE_SIGNAL_HANDLING
#include <csignal>
#endif
#include <chrono>
#include <condition_variable>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>
#include <mutex>

#include <vsomeip/vsomeip.hpp>

#include "sample-ids.hpp"

class service_sample
{
public:
    service_sample(uint32_t _cycle) : 
        app_(vsomeip::runtime::get()->create_application("service_notify-sample")),
        is_registered_(false),
        cycle_(_cycle),
        blocked_(false),
        running_(true),
        is_offered_(false),
        offer_thread_(std::bind(&service_sample::run, this)),
        notify_thread_(std::bind(&service_sample::notify, this)) {
        }
    
    bool init();

    void start();

#ifndef VSOMEIP_ENABLE_SIGNAL_HANDLING
     void stop();
#endif

    void offer();

    void stop_offer();

    void on_state(vsomeip::state_type_e _state);

    void on_get(const std::shared_ptr<vsomeip::message> &_message);

    void on_set(const std::shared_ptr<vsomeip::message> &_message);

    void run();

    void notify();

private:
    std::shared_ptr<vsomeip::application> app_;
    bool is_registered_;
    uint32_t cycle_;

    std::mutex mutex_;
    std::condition_variable condition_;
    bool blocked_;
    bool running_;

    std::mutex notify_mutex_;
    std::condition_variable notify_condition_;
    bool is_offered_;

    std::mutex payload_mutex_;
    std::shared_ptr<vsomeip::payload> payload_;

    std::thread offer_thread_;
    std::thread notify_thread_;
};


#endif