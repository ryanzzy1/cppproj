#include "../include/client_sample.hpp"

bool client_sample::init(){
    if (!app_->init()) {
        std::cerr << "Couldn't initilize application" << std::endl;
        return false;
    }
    std::cout << "Client settings [protocol="
         << (use_tcp_ ? "TCP" : "UDP") << "]"
         << std::endl;

    app_->register_state_handler(
            std::bind(&client_sample::on_state, this, std::placeholders::_1));
    
    app_->register_message_handler(
            vsomeip::ANY_SERVICE, SAMPLE_INSTANCE_ID, vsomeip::ANY_METHOD,
            std::bind(&client_sample::on_message, this,
                    std::placeholders::_1));

    app_->register_availability_handler(SAMPLE_SERVICE_ID,SAMPLE_INSTANCE_ID,
            std::bind(&client_sample::on_availability, this,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    std::set<vsomeip::eventgroup_t> its_groups;
    its_groups.insert(SAMPLE_EVENTGROUP_ID);
    app_->request_event(SAMPLE_SERVICE_ID, 
                        SAMPLE_INSTANCE_ID, 
                        SAMPLE_EVENT_ID, 
                        its_groups,
                        vsomeip::event_type_e::ET_FIELD);
    app_->subscribe(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_EVENTGROUP_ID);

    return true;
}

void client_sample::start() {
    app_->start();
}

#ifndef VSOMEIP_ENABLE_SIGNAL_HANDLING
void client_sample::stop() {
    app_->clear_all_handler();
    app_->unsubscribe(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_EVENTGROUP_ID);
    app_->release_event(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_EVENT_ID);
    app_->release_service(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID);
    app_->stop();
}
#endif

void client_sample::on_state(vsomeip::state_type_e _state) {
    if (_state == vsomeip::state_type_e::ST_REGISTERED) {
        app_->request_service(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID);
    }
}

void client_sample::on_availability(vsomeip::service_t _service, vsomeip::instance_t _instance, bool _is_available) {
    std::cout << "Service [" << std::setw(4) << std::setfill('0') << std::hex
        << _service << "." << _instance << "] is"
        << (_is_available ? "available." : "NOT available.")
        << std::endl;
}

void client_sample::on_message(const std::shared_ptr<vsomeip::message> &_response) {
    std::stringstream client_message;
    client_message << "Received a notification for Event ["
        << std::setfill('0') << std::hex
        << std::setw(4) << _response->get_service() << "."
        << std::setw(4) << _response->get_instance() << "."
        << std::setw(4) << _response->get_method() << "] to Client/Session ["
        << std::setw(4) << _response->get_client() << "/"
        << std::setw(4) << _response->get_session() << "] = ";
    std::shared_ptr<vsomeip::payload> client_payload 
        = _response->get_payload();
    client_message << "(" << std::dec << client_payload->get_length() << ") "
        << std::hex << std::setw(2);
    for (uint32_t i = 0; i < client_payload->get_length(); ++i) {
        client_message << std::setw(2) << (int) client_payload->get_data()[i] << " ";
    }
    std::cout << client_message.str() << std::endl;

    if (_response->get_client() == 0) {
        if ((client_payload->get_length() % 5) == 0) {
            std::shared_ptr<vsomeip::message> client_get = vsomeip::runtime::get()->create_request();
            client_get->set_service(SAMPLE_SERVICE_ID);
            client_get->set_instance(SAMPLE_INSTANCE_ID);
            client_get->set_method(SAMPLE_GET_METHOD_ID);
            client_get->set_reliable(use_tcp_);
            app_->send(client_get);
        }

        if ((client_payload->get_length() % 8) == 0) {
            std::shared_ptr<vsomeip::message> client_set = vsomeip::runtime::get()->create_request();
            client_set->set_service(SAMPLE_SERVICE_ID);
            client_set->set_instance(SAMPLE_INSTANCE_ID);
            client_set->set_method(SAMPLE_SET_METHOD_ID);
            client_set->set_reliable(use_tcp_);
            
            const vsomeip::byte_t client_set_data[] = 
                {0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29};
            std::shared_ptr<vsomeip::payload> client_set_payload
                = vsomeip::runtime::get()->create_payload();
            client_set_payload->set_data(client_set_data, sizeof(client_set_data));
            // without this can't send set payload
            client_set->set_payload(client_set_payload);
            app_->send(client_set);
        }
    }

}