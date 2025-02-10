#ifndef THROUGHPUT_PUB_H_
#define THROUGHPUT_PUB_H_

#include <chrono>
#include <condition_variable>
#include <map>
#include <string>
#include <vector>

#include <asio.hpp>

#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/DataWriterListener.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicData.hpp>
#include <fastdds/rtps/attributes/PropertyPolicy.hpp>

#include "throughputTypes.hpp"

class ThroughputPublisher
{
public:
    
    ThroughputPublisher();

    ~ThroughputPublisher();

    bool init(
            bool reliable,
            uint32_t pid,
            bool hostname);
    
    void run(
            uint32_t test_time,
            uint32_t recovery_time_ms,
            int demand,
            uint32_t msg_size,
            uint32_t subscribers);

private:

    bool init_dynamic_types();

    bool init_static_types(uint32_t payload);

    bool create_data_endpoints(const eprosima::fastdds::dds::DataWriterQos& dw_qos); // create data writer

    bool destroy_data_endpoints(); // delete data writer and listener

    bool test(uint32_t test_time,
              uint32_t recovery_time_ms,
              uint32_t demand,
              uint32_t size);

    // omitted load_demands_payload() and load_recoveries()
    int total_matches() const;

    // Entities
    eprosima::fastdds::dds::DomainParticipant* participant_ = nullptr;
    eprosima::fastdds::dds::Publisher* publisher_ = nullptr;
    eprosima::fastdds::dds::DataWriter* data_writer_ = nullptr;

    // Time
    std::chrono::steady_clock::time_point t_start_;
    std::chrono::steady_clock::time_point t_end_;
    std::chrono::duration<double, std::micro> t_overhead_;

    // Test synchronization
    std::mutex mutex_;
    std::condition_variable data_discovery_cv_;

    // Files
    std::string export_csv_;

    // Topics
    eprosima::fastdds::dds::Topic* data_pub_topic_ = nullptr;

    // omitted commands
    // Static Data
    ThroughputType* throughput_data_ = nullptr;
    eprosima::fastdds::dds::TypeSupport throughput_data_type_;

    // QoS Profiles
    eprosima::fastdds::dds::DataWriterQos dw_qos_;

    // Results
    std::vector<ThroughputResults> results_;
                
    // Flags
    bool dynamic_types_ = false;
    bool reliable_ = false;
    bool hostname_ = false;
    uint32_t pid_ = 0;

    // Test configuration
    uint32_t payload_ = 0;
    std::map<uint32_t, std::vector<uint32_t>> demand_payload_;
    std::vector<uint32_t> recovery_times_;

    uint32_t subscribers_ = 1;

    // Data listener
    class DataWriterListener : public eprosima::fastdds::dds::DataWriterListener
    {
        ThroughputPublisher& throughput_publisher_;
        std::atomic_int matched_;

    public:
        DataWriterListener(
                ThroughputPublisher& throughput_publisher)
            : throughput_publisher_(throughput_publisher)
            , matched_(0)
        {
        }

        void on_publication_matched(
                eprosima::fastdds::dds::DataWriter* writer,
                const eprosima::fastdds::dds::PublicationMatchedStatus& info) override;
        
        int get_matched() const
        {
            return matched_;
        }

        void reset()
        {
            matched_ = 0;
        }

    }
    data_writer_listener_;

};

#endif