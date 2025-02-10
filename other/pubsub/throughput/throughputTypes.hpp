#ifndef THROUGHPUTTYPES_H_
#define THROUGHPUTTYPES_H_

#include <chrono>

#include <fastdds/dds/log/Colors.hpp>
#include <fastdds/dds/log/Log.hpp>
#include <fastdds/dds/topic/TopicDataType.hpp>


typedef struct alignas(4) ThroughputType
{
    // identifies the sample sent
    uint32_t seqnum;

    // actual payload
    uint8_t data[1];

    // This struct overhead
    static const size_t overhead;
    
}ThroughputType;


class ThroughputDataType : public eprosima::fastdds::dds::TopicDataType
{
    // Buffer size for the manage type
    const uint32_t buffer_size_;

public:

    // This size defines the expected ThroughputType buffer size, (size + 3) & ~3
    ThroughputDataType(
            const uint32_t& size)
        : buffer_size_(size)
    {
        set_name(type_name_.c_str());
        max_serialized_type_size = sizeof(decltype(ThroughputType::seqnum)) +
                ((size + 3) & ~3) +                                         
                eprosima::fastdds::rtps::SerializedPayload_t::representation_header_size;
        is_compute_key_provided = false;
    }

    ~ThroughputDataType()
    {
    }

    bool serialize(
            const void* const data,
            eprosima::fastdds::rtps::SerializedPayload_t& payload,
            eprosima::fastdds::dds::DataRepresentationId_t data_representation) override;

    bool deserialize(
            eprosima::fastdds::rtps::SerializedPayload_t& payload,
            void* data) override;

    uint32_t calculate_serialized_size(
            const void* const data,
            eprosima::fastdds::dds::DataRepresentationId_t data_representation) override;

    void* create_data() override;

    void delete_data(
            void* data) override;

    bool compute_key(
            eprosima::fastdds::rtps::SerializedPayload_t& /*payload*/,
            eprosima::fastdds::rtps::InstanceHandle_t& /*ihandle*/,
            bool force_md5 = false) override
    {
        (void)force_md5;
        return false;
    }

    bool compute_key(
            const void* const /*data*/,
            eprosima::fastdds::rtps::InstanceHandle_t& /*ihandle*/,
            bool force_md5 = false) override
    {
        (void)force_md5;
        return false;
    }

    bool compare_data(
            const ThroughputType& lt1,
            const ThroughputType& lt2) const;

    bool is_bounded() const override
    {
        return true;
    }

    bool is_plain(
            eprosima::fastdds::dds::DataRepresentationId_t /*data_representation*/) const override
    {
        // It is plain because the type has a fixed sized
        return true;
    }

    // Name
    static const std::string type_name_;

private:

    using eprosima::fastdds::dds::TopicDataType::is_plain;
};

struct ThroughputResults
{
    uint32_t payload_size;
    uint32_t recovery_time_ms;
    uint32_t demand;
};


#endif