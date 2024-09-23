// Fast-DDS request_reply ClientApp.hpp

/**
 * @file ClientApp.hpp
 * 
 */

#ifndef CLIENTAPP_HPP
#define CLIENTAPP_HPP

#include <atomic>
#include <condition_variable>
#include <map>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantListener.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/topic/ContentFilteredTopic.hpp>
#include <fastdds/dds/topic/Topic.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/rtps/common/SampleIdentity.hpp>

#include "app_utils.hpp"
#include "Application.hpp"
#include "CLIParser.hpp"
#include "types/Calculator.hpp"
#include "types/CalculatorPubSubTypes.hpp"

namespace eprosima{
namespace fastdds{
namespace request_reply{

using namespace eprosima::fastdds::dds;


class ClientApp : public Application, public DomainParticipantListener
{
public:

    ClientApp(
            const CLIParser::config& config,
            const std::string& service_name);

    ~ClientApp();

    //! Run subscriber
    void run() override;

    //! Trigger the end of execution
    void stop() override;

    //! Participant discovery method
    void on_participant_discovery(
            DomainParticipant* participant,
            rtps::ParticipantDiscoveryStatus status,
            const ParticipantBuiltinTopicData& info,
            bool& should_be_ignored) override;

    //! Publication matched method
    void on_publication_matched(
            DataWriter* writer,
            const PublicationMatchedStatus& info) override;

    //! Subscription matched method
    void on_subscription_matched(
            DataReader* reader,
            const SubscriptionMatchedStatus& info) override;

    //! Reply received method
    void on_data_available(
            DataReader* reader) override;

private:

    void create_participant();

    template<typename TypeSupportClass>
    Topic* create_topic(
            const std::string& topic_name,
            TypeSupport& type);

    void create_request_entities(
            const std::string& service_name);

    void create_reply_entities(
            const std::string& service_name);

    bool send_requests();

    bool send_request(
            const CalculatorRequestType& request);

    bool is_stopped();

    void wait_for_replies();

    std::pair<std::int16_t, std::int16_t> request_input_;

    DomainParticipant* participant_;

    TypeSupport request_type_;

    Topic* request_topic_;

    Publisher* publisher_;

    DataWriter* request_writer_;

    TypeSupport reply_type_;

    Topic* reply_topic_;

    ContentFilteredTopic* reply_cf_topic_;

    std::string reply_topic_filter_expression_;

    std::vector<std::string> reply_topic_filter_parameters_;

    Subscriber* subscriber_;

    DataReader* reply_reader_;

    mutable std::mutex mtx_;

    std::condition_variable cv_;

    RemoteServerMatchedStatus server_matched_status_;

    std::atomic<bool> stop_;

    std::map<rtps::SampleIdentity, bool> requests_status_;
};

template<>
Topic* ClientApp::create_topic<CalculatorRequestTypePubSubType>(
        const std::string& topic_name,
        TypeSupport& type);

template<>
Topic* ClientApp::create_topic<CalculatorReplyTypePubSubType>(
        const std::string& topic_name,
        TypeSupport& type);



} // namespace request_reply
} // namespace fastdds
} // namespace eprsima
#endif

