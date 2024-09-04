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

class client_app
{
private:
    std::shared_ptr<vsomeip::application> app_;
    std::shared_ptr<vsomeip::message> request_;
    bool use_tcp_;
    bool be_quiet_;
    uint32_t cycle_;
    std::mutex mutex_;
    std::condition_variable condition_;
    bool running_;
    bool blocked_;
    bool is_available_;

    std::thread sender_;

public:
    client_app(bool _use_tcp, bool _be_quiet, uint32_t _cycle)
        : app_(vsomeip::runtime::get()->create_application())
        , request_(vsomeip::runtime::get()->create_request())
        , use_tcp_(_use_tcp)
        , be_quiet_(_be_quiet)
        , cycle_(_cycle)
        , running_(true)
        , blocked_(false)
        , is_available_(false)
        , sender_(std::bind(&client_app::run, this)) {
        }

    bool init() {
        if (!app_->init()) {
            std::cerr << "Couldn't initilize applicaiton." << std::endl;
            return false;
        }

        std::cout << "Client settings [protocol]="
                  << (use_tcp_ ? "TCP" : "UDP")
                  << ":quiet= "
                  << (be_quiet_ ? "true" : "false")
                  << ":cycle="
                  << cycle_
                  << "]"
                  << std::endl;

        app_->register_state_handler(
            std::bind(
                &client_app::on_state,
                this,
                std::placeholders::_1));

        app_->register_message_handler(
                vsomeip::ANY_SERVICE, SAMPLE_INSTANCE_ID, vsomeip::ANY_METHOD,
                std::bind(&client_app::on_message,
                this,
                std::placeholders::_1));

        request_->set_service(SAMPLE_SERVICE_ID);
        request_->set_instance(SAMPLE_INSTANCE_ID);
        request_->set_method(SAMPLE_METHOD_ID);

        std::shared_ptr<vsomeip::payload> its_payload = vsomeip::runtime::get()->create_payload();
        std::vector<vsomeip::byte_t> its_payload_data;
        for (std::size_t i = 0; i < 10; ++i)
            its_payload_data.push_back(vsomeip::byte_t(i % 256));
        its_payload->set_data(its_payload_data);
        request_->set_payload(its_payload);

    }
    
    void start() {

    }

    void stop() {

    }

    void on_state(vsomeip::state_type_e _state) {

    }

    void on_availability(vsomeip::service_t _service, vsomeip::instance_t _instance, bool _is_available) {

    }


    void on_message(const std::shared_ptr<vsomeip::message> &_response) {

    }

    void send() {

    }

    void run() {

    }

}; 