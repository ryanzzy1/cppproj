#include <iomanip>
#include <iostream>
#include <sstream>

#include <condition_variable>
#include <thread>

#include <vsomeip/vsomeip.hpp>

#define SAMPLE_SERVICE_ID 0x1234
#define SAMPLE_INSTANCE_ID 0x5678
#define SAMPLE_METHOD_ID 0x0421
#define SAMPLE_EVENTGROUP_ID 0x4465
#define SAMPLE_EVENT_ID 0x8778

std::shared_ptr<vsomeip::application> app;
std::mutex mutex;
std::condition_variable condition;

void run() {
    std::unique_lock<std::mutex> its_lock(mutex);
    condition.wait(its_lock);

    std::set<vsomeip::eventgroup_t> its_groups;
    its_groups.insert(SAMPLE_EVENTGROUP_ID);
    app->request_event(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_EVENT_ID, its_groups);
    app->subscribe(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_EVENTGROUP_ID);
    
    // process subscribed event content. 
    // std::shared_ptr<vsomeip::payload> its_payload;
    // its_payload = vsomeip::runtime::get()->create_payload();    
    // for (int i = 0; i< its_payload->get_length(); i++)
    //     std::cout << "Subscribed data: " <<  std::setw(4) << std::setfill('0') << std::hex << its_payload->get_data()[i] << " ";
}

void on_event(vsomeip::service_t _service, vsomeip::instance_t _instance ) {

}

void on_message(const std::shared_ptr<vsomeip::message> &_response) {
    std::stringstream its_message;
    its_message << "CLIENT: Received a notification for event [" 
        << std::setw(4) << std::setfill('0') << std::hex
        << _response->get_service() << "."
        << std::setw(4) << std::setfill('0') << std::hex
        << _response->get_instance() << "."
        << std::setw(4) << std::setfill('0') << std::hex
        << _response->get_method() << "."
        << std::setw(4) << std::setfill('0') << std::hex
        << _response->get_client() << "."
        << std::setw(4) << std::setfill('0') << std::hex
        << _response->get_session() << "] = ";
    
    std::shared_ptr<vsomeip::payload> its_payload = _response->get_payload();
    its_message << "(" << std::dec << its_payload->get_length() << ")";
    for (uint32_t i = 0; i < its_payload->get_length(); ++i)
        its_message << std::hex << std::setw(2) << std::setfill('0')
            <<(int) its_payload->get_data()[i] << " ";
    std::cout << its_message.str() << std::endl;
}

void on_availability(vsomeip::service_t _service, vsomeip::instance_t _instance, bool _is_available) {
    std::cout << "CLIENT Service [" << std::setw(4) << std::setfill('0') << std::hex << _service << "."
              << _instance << "] is " << (_is_available ? "available." : "NOT available.") << std::endl;
    condition.notify_one();
}

int main() {
    app = vsomeip::runtime::get()->create_application("Test_Client_Sub");
    app->init();
    app->register_availability_handler(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, on_availability);
    app->request_service(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID);

    app->register_message_handler(vsomeip::ANY_SERVICE, vsomeip::ANY_INSTANCE, vsomeip::ANY_METHOD, on_message);

    std::thread sender(run);
    app->start();
}