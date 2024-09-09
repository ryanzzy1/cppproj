#include <iomanip>
#include <iostream>
#include <sstream>

#include <vsomeip/vsomeip.hpp>


#define SAMPLE_SERVICE_ID 0x1234
#define SAMPLE_INSTANCE_ID 0x5678
#define SAMPLE_METHOD_ID 0x0421

// event handling 
#define SAMPLE_EVENTGROUP_ID 0x4465
#define SAMPLE_EVENT_ID 0x8778

std::shared_ptr<vsomeip::application> service_app;



void on_message(const std::shared_ptr<vsomeip::message> &_request) {
    std::shared_ptr<vsomeip::payload> its_payload = _request->get_payload();
    vsomeip::length_t l = its_payload->get_length();

    // Get payload
    std::stringstream ss;
    // recevive payload type std::vector<vsomeip::byte_t> 
    // for (vsomeip::length_t i = 0; i < l; i++) {
    //     ss << std::setw(2) << std::setfill('0') << std::hex
    //        << (int) * (its_payload->get_data()+i) << " ";
    // }
    // Receive payload type const byte_t *
    ss << "\n" <<(its_payload->get_data()) << " ";
    

    std::cout << "SERVICE: Received message with Client/Session ["
        << std::setw(4) << std::setfill('0') << std::hex << _request->get_client() << "/"
        << std::setw(4) << std::setfill('0') << std::hex << _request->get_session() << "]"
        << ss.str() << std::endl;
    
    // Create response
    std::shared_ptr<vsomeip::message> its_response = vsomeip::runtime::get()->create_response(_request);
    its_payload = vsomeip::runtime::get()->create_payload();
    std::vector<vsomeip::byte_t> its_payload_data;
    for (int i = 9; i >= 0; i--) {
        its_payload_data.push_back(i % 256);
    }
    its_payload->set_data(its_payload_data);
    its_response->set_payload(its_payload);
    service_app->send(its_response);

}

int main() {
    service_app = vsomeip::runtime::get()->create_application("World");

    std::shared_ptr<vsomeip::payload> payload;
    const vsomeip::byte_t its_data[] = {0x10};
    payload = vsomeip::runtime::get()->create_payload();
    payload->set_data(its_data, sizeof(its_data));

    std::set<vsomeip::eventgroup_t> its_groups;
    

    service_app->init();
    service_app->register_message_handler(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_METHOD_ID, on_message);
    service_app->offer_service(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID);

    its_groups.insert(SAMPLE_EVENTGROUP_ID);
    service_app->offer_event(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID,SAMPLE_EVENT_ID, its_groups);
    service_app->notify(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_EVENT_ID, payload);
    service_app->start();
}