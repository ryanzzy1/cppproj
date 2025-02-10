#ifndef THROUGHPUT_SUB_H_
#define THROUGHPUT_SUB_H_

#include <chrono>
#include <condition_variable>
#include <iostream>
#include <asio.hpp>

#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicData.hpp>
#include <fastdds/rtps/attributes/PropertyPolicy.hpp>
#include <fastdds/dds/topic/TopicDataType.hpp>
#include <fastdds/dds/topic/Topic.hpp>

#include "throughputTypes.hpp"


class ThroughputSubscriber
{
public:
    ThroughputSubscriber();

    ~ThroughputSubscriber();

    bool init(
            bool reliable,
            uint32_t pid,
            bool hostname
            );
    
    void run();

private:

    bool init_dynamic_types();

    bool init_static_types(uint32_t payload);

    bool create_data_endpoints(const eprosima::fastdds::dds::DataReaderQos& dr_qos);

    bool destroy_data_endpoints();

    int total_matches() const;

    // Entities
    eprosima::fastdds::dds::DomainParticipant* participant_ = nullptr;
    eprosima::fastdds::dds::Subscriber* subscriber_ = nullptr;
    eprosima::fastdds::dds::DataReader* data_reader_ = nullptr;
    
    // Time
    std::chrono::steady_clock::time_point t_start_;
    std::chrono::steady_clock::time_point t_end_;
    std::chrono::duration<double, std::micro> t_overhead_;

    // Test synchronization
    std::mutex mutex_;
    std::condition_variable data_discovery_cv_;
    uint32_t data_discovery_count_ = 0;

    // Topics 
    eprosima::fastdds::dds::Topic* data_sub_topic_ = nullptr;

    // Static Data
    ThroughputType* throughput_data_ = nullptr;
    eprosima::fastdds::dds::TypeSupport throughput_data_type_;

    // Dynamic Data
    eprosima::fastdds::dds::DynamicData::_ref_type* dynamic_data_ {nullptr};
    eprosima::fastdds::dds::TypeSupport dynamic_pub_sub_type_;

    // QoS Profiles
    eprosima::fastdds::dds::DataReaderQos dr_qos_;

    // Flags 
    bool dynamic_types_ = false;
    bool ready_ = true;
    bool reliable_ = false;
    bool hostname_ = false;
    uint32_t pid_ = 0;

    // Test configuration
    uint32_t data_size_ = 0;
    uint32_t demand_ = 0;
    
    // Files
    std::string xml_config_file_;

    // Data listener
    class DataReaderListener : public eprosima::fastdds::dds::DataReaderListener
    {
        ThroughputSubscriber& throughput_subscriber_;
        uint32_t last_seq_num_ = 0;
        uint32_t lost_samples_ = 0;
        uint64_t received_samples_ = 0;
        eprosima::fastdds::dds::SampleInfo info_;
        std::atomic_int matched_;
        std::atomic_bool enable_;

    public:
        DataReaderListener(
                ThroughputSubscriber& throughput_subscriber)
            : throughput_subscriber_(throughput_subscriber)
            , matched_(0)
        {
        }

        void on_subscription_matched(
                eprosima::fastdds::dds::DataReader* reader,
                const eprosima::fastdds::dds::SubscriptionMatchedStatus& info) override;

        void on_data_available(eprosima::fastdds::dds::DataReader* reader)override;
    
        void reset();

        void disable();

        void save_numbers();

        int get_matches() const
        {
            return matched_;
        }

        uint32_t saved_last_seq_num_;
        uint32_t saved_lost_samples_;
        uint64_t saved_received_samples_;
    }
    data_reader_listener_;
};

#endif  /* THROUGHPPUT_SUB_H_*/