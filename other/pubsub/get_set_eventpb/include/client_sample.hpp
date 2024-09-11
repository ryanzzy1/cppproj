#ifndef CLIENT_SAMPLE_HPP_
#define CLIENT_SAMPLE_HPP_

#ifndef VSOMEIP_ENABLE_SIGNAL_HANDLING
#include <csignal>
#endif

#include <chrono>
#include <condition_variable>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>

#include <vsomeip/vsomeip.hpp>

#include "sample-ids.hpp"

class client_sample {
public:
    client_sample(bool _use_tcp) :
        app_(vsomeip::runtime::get()->create_application()),
        use_tcp_(_use_tcp) {}
    
    bool init();

    void start();

#ifndef VSOMEIP_ENABLE_SIGNAL_HANDLING
    void stop();
#endif

    void on_state(vsomeip::state_type_e _state);

    void on_availability(vsomeip::service_t _service, vsomeip::instance_t _instance, bool _is_available);

    void on_message(const std::shared_ptr<vsomeip::message> &_response);
private:
    std::shared_ptr<vsomeip::application> app_;
    bool use_tcp_;
};

#endif

