#ifndef VSOMEIP_ENABLE_SIGNAL_HANDLING
#include <csignal>
#endif
#include <iostream>
#include <condition_variable>
#include <iomanip>
#include <chrono>
#include <sstream>
#include <thread>
#include <vsomeip/vsomeip.hpp>

static vsomeip::service_t service_id = 0x0001;
static vsomeip::instance_t service_instance_id = 0x0002;
static vsomeip::method_t service_method_id = 0x0003;

class sample_service
{
public:
    sample_service(bool _use_static_routing_)
    : app_(vsomeip::runtime::get()->create_application())
    , is_registered_(false)
    , use_static_routing_(_use_static_routing_)
    , blocked_(false)
    , running_(true)
    , offer_thread_(std::bind(&sample_service::run, this)){}

    bool init() {
        std::lock_guard<std::mutex> its_lock(mutex_);

        if (!app_->init()) {
            std::cerr << "Could't initilize application." << std::endl;
            return false;
        }
        app_->register_state_handler(
            std::bind(&sample_service::on_state, this, std::placeholders::_1));
        
        app_->register_message_handler(service_id, service_instance_id, service_method_id,
            std::bind(&sample_service::on_message, this, std::placeholders::_1));

        std::cout << "Starting routing " << (use_static_routing_ ? "ON" : "OFF")
                  << std::endl;
        return true;
    }

    void start() {
        app_->start();
    }

#ifndef VSOMEIP_ENABLE_SIGNAL_HANDLING
    void stop() {
        running_ = false;
        blocked_ = true;
        app_->clear_all_handler();
        stop_offer();
        condition_.notify_one();
        offer_thread_.join();
        app_->stop();
    }
#endif 
    
    void offer() {
        app_->offer_service(service_id, service_instance_id);
    }

    void stop_offer() {
        app_->stop_offer_service(service_id, service_instance_id);
    }

    void on_state(vsomeip::state_type_e _state) {
        std::cout << "Application " << app_->get_name() << "is"
                  << (_state == vsomeip::state_type_e::ST_REGISTERED ? "registered." : "deregistered.")
                  << std::endl;
        
        if (_state == vsomeip::state_type_e::ST_REGISTERED) {
            if (!is_registered_) {
                is_registered_ = true;
                blocked_ = true;
                condition_.notify_one();
            }
        }else {
            is_registered_ = false;
        }
    }

    void on_message(const std::shared_ptr<vsomeip::message>&_request) {
        std::cout << "Received a message with Client/Session ["
            << std::setfill('0') << std::hex
            << std::setw(4) << _request->get_client() << "/"
            << std::setw(4) << _request->get_session() << "]"
            << std::endl;
        
        std::shared_ptr<vsomeip::message> its_response 
            = vsomeip::runtime::get()->create_response(_request);

        std::shared_ptr<vsomeip::payload> its_payload
            = vsomeip::runtime::get()->create_payload();
        std::vector<vsomeip::byte_t> its_payload_data;
        for (std::size_t i = 0; i < 120; ++i) 
            its_payload_data.push_back(vsomeip::byte_t(i % 256));
        its_payload->set_data(its_payload_data);
        its_response->set_payload(its_payload);

        app_->send(its_response);
    }

    void run() {
        std::unique_lock<std::mutex> its_lock(mutex_);
        while (!blocked_)
            condition_.wait(its_lock);
        
        bool is_offer(true);

        if (use_static_routing_) {
            offer();
            while (running_);            
        } else {
            while (running_)
            {
                if (is_offer)
                    offer();
                else
                    stop_offer();
                
                for (int i = 0; i < 10 && running_; i++ )
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                is_offer = !is_offer;
            }
            
        }
    }

private:
    std::shared_ptr<vsomeip::application> app_;
    bool is_registered_;
    bool use_static_routing_;

    std::mutex mutex_;
    std::condition_variable condition_;
    bool blocked_;
    bool running_;

    std::thread offer_thread_;
};

#ifndef VSOMEIP_ENABLE_SIGNAL_HANDLING
    sample_service* its_sample_ptr(nullptr);
    void handle_signal(int _signal) {
        if (its_sample_ptr != nullptr && (_signal == SIGINT || _signal == SIGTERM)) 
            its_sample_ptr->stop();
    }
#endif

int main(int argc, char **argv)
{
    // create vsomeip instance
    bool use_static_routing(false);

    std::string static_routing_enable("--static-routing");

    for (int i = 1; i < argc; i++) {
        if (static_routing_enable == argv[i]) {
            use_static_routing = true;
        }
    }

    sample_service its_sample(use_static_routing);
#ifndef VSOMEIP_ENABLE_SIGNAL_HANDLING
    its_sample_ptr = &its_sample;
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);
#endif
    if (its_sample.init()) {
        its_sample.start();
        return 0;
    } else {
        return 1;
    }
}