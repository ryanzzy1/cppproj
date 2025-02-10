#include "throughput_pub.hpp"

#include <chrono>
#include <fstream>
#include <map>
#include <thread>

#include <fastdds/dds/log/Colors.hpp>
#include <fastdds/dds/log/Log.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/DataWriterListener.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicPubSubType.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicTypeBuilder.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicTypeBuilderFactory.hpp>
#include <fastdds/dds/xtypes/dynamic_types/MemberDescriptor.hpp>
#include <fastdds/dds/xtypes/dynamic_types/TypeDescriptor.hpp>
#include <fastdds/rtps/transport/UDPv4TransportDescriptor.hpp>
#include <fastdds/rtps/transport/TCPv4TransportDescriptor.hpp>

using namespace eprosima::fastdds::dds;
using namespace eprosima::fastdds::rtps;

ThroughputPublisher::ThroughputPublisher()
    : data_writer_listener_(*this)
{
}

ThroughputPublisher::~ThroughputPublisher()
{
    if (dynamic_types_)
    {
        destroy_data_endpoints();   // remove dynamic_types_
    }
    else if(data_writer_ != nullptr
            || data_pub_topic_ != nullptr
            || throughput_data_type_)
    {
        EPROSIMA_LOG_ERROR(THROUGHPUTPUBLISHER, "ERROR unregistering the DATA type");
        return;
    }
    participant_->delete_publisher(publisher_);
    DomainParticipantFactory::get_instance()->delete_participant(participant_);
    EPROSIMA_LOG_INFO(THROUGHPUTPUBLISHER, "Pub: Participant removed");
}

bool ThroughputPublisher::init(
                            bool reliable,
                            uint32_t pid,
                            bool hostname)
{
    pid_ = pid;
    hostname_ = hostname;
    reliable_ = reliable;

    // Create DomainParticipant
    std::string participant_profile_name = "pub_participant_profile";
    DomainParticipantQos pqos;

    // Deafult domain
    DomainId_t domainId = pid % 230;

    // Default participant name
    pqos.name("throughput_test_publisher");

    // disable shared memroy, use udp transport
    std::shared_ptr<eprosima::fastdds::rtps::UDPv4TransportDescriptor> udp_transport = 
            std::make_shared<eprosima::fastdds::rtps::UDPv4TransportDescriptor>();
    pqos.transport().user_transports.push_back(udp_transport);
    pqos.transport().use_builtin_transports = false;
    /* udp TransportDescriptor interface configuration */
    // Apply to DomainParticipant entity
    DomainParticipantQos participant_qos;
    // Add new transport to the list of user transports
    std::shared_ptr<eprosima::fastdds::rtps::UDPv4TransportDescriptor> descriptor =
            std::make_shared<eprosima::fastdds::rtps::UDPv4TransportDescriptor>();
    descriptor->sendBufferSize = 9126;
    descriptor->receiveBufferSize = 9126;
    participant_qos.transport().user_transports.push_back(descriptor);
    // set use_builtin_transports to false
    participant_qos.transport().use_builtin_transports = false;
    // [optional] Set ThreadSettings for the builtin transports reception threads
    participant_qos.transport().builtin_transports_reception_threads_ = 
            eprosima::fastdds::rtps::ThreadSettings{2, 2, 2, 2};
    // Set max_msg_size_no_frag to a value > 65500 kB
    participant_qos.transport().max_msg_size_no_frag = 70000;
    // Configure netmask filter
    participant_qos.transport().netmask_filter = 
            eprosima::fastdds::rtps::NetmaskFilterKind::ON;
    // Use modified QoS in the creation of the DomainParticipant entity
    /* end udp configuration*/

    /*TCP configuration*/
    // TODO: TCP configuration
    
    // Create the participant
    participant_ = DomainParticipantFactory::get_instance()->create_participant(domainId, pqos);
    
    if (participant_ == nullptr)
    {
        EPROSIMA_LOG_ERROR(THROUGHPUTPUBLISHER, "ERROR creating participant");
        return false;
    }

    // Create publisher
    publisher_ = participant_->create_publisher(PUBLISHER_QOS_DEFAULT, nullptr);
    if (publisher_ == nullptr)
    {
        EPROSIMA_LOG_ERROR(THROUGHPUTPUBLISHER, "ERROR creating Publisher");
        return false;
    }

    // Reliability
    ReliabilityQosPolicy rp;
    if (reliable_)
    {
        rp.kind = eprosima::fastdds::dds::RELIABLE_RELIABILITY_QOS;
        dw_qos_.reliability(rp);

        RTPSReliableWriterQos rw_qos;
        rw_qos.times.heartbeat_period.seconds = 0;
        rw_qos.times.heartbeat_period.nanosec = 10000000; // 10ms
        rw_qos.times.nack_supression_duration = {0, 0};
        rw_qos.times.nack_response_delay = {0, 0};

        dw_qos_.reliable_writer_qos(rw_qos);
    }
    else 
    {
        rp.kind = eprosima::fastdds::dds::BEST_EFFORT_RELIABILITY_QOS;
        dw_qos_.reliability(rp);
    }

    // data sharing diabled by default do not configured.
    // calculate overhead due to clock calls
    t_start_ = std::chrono::steady_clock::now();
    for (int i = 0; i < 1000; ++i)
    {
        t_end_ = std::chrono::steady_clock::now();
    }
    t_overhead_ = std::chrono::duration<double, std::micro>(t_end_ - t_start_) / 1000; // 计算以上1000次访问的均值
    std::cout << "Publisher's clock access overhead: " << t_overhead_.count() << " us." << std::endl;

    // 端点使用动态数据端点会产生完整的测试时间
    // 每个数据迭代都会创建静态类型和端点
    return dynamic_types_ ? init_dynamic_types() && create_data_endpoints(dw_qos_) : true;
}

void ThroughputPublisher::run(
                            uint32_t test_time,
                            uint32_t recovery_time_ms,
                            int demand,
                            uint32_t msg_size,
                            uint32_t subscriber)
{
    subscribers_ = subscriber;
    if(demand == 0 || msg_size == 0)
    {
        return;
    }
    else
    {
        payload_ = msg_size;
        // 8 指 header + seqnum = 4 + 4
        demand_payload_[msg_size - (uint32_t)(dynamic_types_ ? 8 : ThroughputType::overhead)].push_back(demand);
    }

    // recovery times vector
    recovery_times_.push_back(recovery_time_ms);

    std::cout << "Recovery times: ";
    for (uint16_t i = 0; i < recovery_times_.size(); i++)
    {
        std::cout << recovery_times_[i] << ", ";
    }
    std::cout << std::endl;

    // create export_csv_ file and add the header
    if (export_csv_ != "")
    {
        std::ofstream data_file;
        data_file.open(export_csv_);
        data_file << "Payload [Bytes],Demand [sample/burst],Recovery time [ms],Sent [samples],Publication time [us],"
            << "Publication sample rate [Sample/s],Publication throughput [Mb/s],Received [samples],"
            << "Lost [samples],Subscription time [us],Subscription sample rate [Sample/s],"
            << "Subscription throughput [Mb/s]" << std::endl;
        data_file.flush();
        data_file.close();
    }

    // command type msg omitted
    // SampleInfo info;  // subscriber inlcude
    bool test_failure = false;

    // Iterate over message sizes
    for (auto sit = demand_payload_.begin(); sit != demand_payload_.end(); ++sit)
    {
        auto dw_qos = dw_qos_;

        // Check history resources depending on the history kind and demand
        uint32_t max_demand = 0;
        for (auto current_demand : sit->second)
        {
            max_demand = std::max(current_demand, max_demand);
        }
        if (dw_qos.history().kind == KEEP_LAST_HISTORY_QOS)
        {
            // Ensure that the history depth is at least the demand
            if (dw_qos.history().depth < 0 ||
                static_cast<uint32_t>(dw_qos.history().depth) < max_demand)
            {
                EPROSIMA_LOG_WARNING(THROUGHPUTPUBLISHER, "Setting history depth to " << max_demand);
                dw_qos.resource_limits().max_samples = max_demand;
                dw_qos.history().depth = max_demand;
            }   
        }
     
        // KEEP_ALL case
        else
        {
            // Ensure that the max samples is at least the demand
            if(dw_qos.resource_limits().max_samples <= 0 ||
                    static_cast<uint32_t>(dw_qos.resource_limits().max_samples) < max_demand)
            {
                EPROSIMA_LOG_WARNING(THROUGHPUTPUBLISHER, "Setting resource limit max samples to " << max_demand);
                dw_qos.resource_limits().max_samples = max_demand;
            }
        }
        // allocated_samples to max_samples, must be <= max_samples
        dw_qos.resource_limits().allocated_samples = dw_qos.resource_limits().max_samples;
        if (dynamic_types_)
        {
            // do nothing
        }
        else{
            assert(nullptr == throughput_data_);
            auto m_size = sit->first;
            // Create the data endpoints if using static types (right after modifying the QoS)
            if (init_static_types(m_size) && create_data_endpoints(dw_qos))
            {
                // Wait for the data endpoints descovery
                std::unique_lock<std::mutex> data_disc_lock(mutex_);
                data_discovery_cv_.wait(data_disc_lock, [&]()
                        {
                            // all command and data endpoints must be discovered
                            // subscriber 3 至少3个的原因在于设计中包含了数据发布和订阅， 命令发布和订阅 * 2， 总共3个订阅者；
                            // 这里因为没有增加命令处理内容，只保留了数据订阅1个
                            return total_matches() == static_cast<int>(subscribers_ * 1);
                            
                        });
            }
            else
            {
                EPROSIMA_LOG_ERROR(THROUGHPUTPUBLISHER, "Error preparing static types and endpoints for testing");
                test_failure = true;
            }
            
            // no setting data_loans, so create the data sample.
            throughput_data_ = static_cast<ThroughputType*>(throughput_data_type_.create_data());
        }

        // Iterator over burst of messages to send
        // std::map<uint32_t, std::vector<uint32_t>>demand_payload_ 中，sit->first 表示size部分，sit->second 表示demand部分。
        for (auto dit = sit->second.begin(); dit != sit->second.end(); ++dit)
        {
            auto m_size = sit->first;
            auto m_demand = *dit;

            for (uint16_t i = 0; i < recovery_times_.size(); i++)
            {
                uint32_t subscribers_ready = 0;
                while(subscribers_ready < subscribers_)
                {
                    // subscriber SampleInfo info increase information
                    ++subscribers_ready;
                }

                // run this test iteration test
                if (!test(test_time, recovery_times_[i], *dit, sit->first))
                {
                    test_failure = true;
                    break;
                }
            }
        }

        // 删除数据类型
        // 如果是动态数据类型先删除动态数据，没有就删除静态数据
        if (dynamic_types_)
        {
            // 不会进入            
        }
        else 
        {
            if(1) // no data_loans_
            {
                throughput_data_type_.delete_data(throughput_data_);
            }
            throughput_data_ = nullptr;

            // Destroy the data endpoints if using static types
            if (!destroy_data_endpoints())
            {
                EPROSIMA_LOG_ERROR(THROUGHPUTPUBLISHER, "Error removing static types and endpoints for testing");
                test_failure = true;
                break;
            }

            // wait till subscribers are rigged
            uint32_t subscribers_ready = 0;
            while(subscribers_ready < subscribers_)
            {
                // 命令接收处理， 未定义，不做处理

            }
        }
    }

    // 等待命令处理线程接收部分删去，仅保留对数据收发的处理，命令部分处理有点复杂，尽量简化处理逻辑

}

bool ThroughputPublisher::test(
            uint32_t test_time,
            uint32_t recovery_time_ms,
            uint32_t demand,
            uint32_t size )
{
    assert(total_matches() == static_cast<int>(subscribers_ * 1)); //只有一个数据订阅

    // Declare test time variables
    std::chrono::duration<double, std::micro> clock_overhead(0);
    std::chrono::duration<double, std::nano> test_time_ns = std::chrono::seconds(test_time);
    std::chrono::duration<double, std::nano> recovery_duration_ns = std::chrono::milliseconds(recovery_time_ms);
    std::chrono::steady_clock::time_point batch_start;

    // Send a TEST_STARTS and sleep for a while to give the subscriber time to set up
    uint32_t samples = 0;

    // mark test start time
    std::chrono::steady_clock::time_point test_start_sent_up = std::chrono::steady_clock::now();

    // 统计订阅方确认接收到报文并响应给发布方的时间，可以放在数据订阅上实现

    // 在给定时间内发送一定数量的样本数据，
    t_start_ = std::chrono::steady_clock::now();
    uint32_t seqnum = 0;
    while((t_end_ - t_start_) < test_time_ns)
    {
        // 获取开始时间
        batch_start = std::chrono::steady_clock::now();
        // 发送一定数量的demand
        for(uint32_t sample = 0; sample < demand; sample++)
        {
            if (dynamic_types_) // dynamic_types_ = false;
            {
                //not implemented
            }
            else // no data_loans_ 
            {
                throughput_data_->seqnum = ++seqnum;
                data_writer_->write(throughput_data_);
            }
        }
        // 获取结束时间
        t_end_ = std::chrono::steady_clock::now();

        // 添加demand number to samples
        samples += demand;

        // 如果样本发送时间小于回复时间，休眠时间是回复时间 - batch 发送时间
        // 否则就会导致在while内执行完for循环，立即执行下一轮的for循环，样本
        // 发送没有恢复时间。
        // 如果以上差值为负数，根据sleep_for() 的实现，返回值是不会设置线程休眠的。
        // 另外，设计指出，如果durtaion 小于线程调度的时间片，休眠会持续到OS下次调度为止。
        std::this_thread::sleep_for(recovery_duration_ns - (t_end_ - batch_start));

        clock_overhead += t_overhead_ * 2; // 每个batch 访问时钟两次
    }

    size_t removed = 0;
    data_writer_->clear_history(&removed); // ?

    // command writer omitted
    // 写入测试结束命令，统计测试结果

    // Results processing
    uint32_t num_results_received = 0;
    bool results_error = false;
    while (!results_error && num_results_received < subscribers_)
    {
        
    }
}

bool ThroughputPublisher::init_dynamic_types()
{

}

bool ThroughputPublisher::init_static_types(uint32_t payload)
{
    assert(participant_ != nullptr);

    // Check if it has been initialized
    if (throughput_data_type_)
    {
        EPROSIMA_LOG_ERROR(THROUGHPUTPUBLISHER, "ERROR STATIC DATA type already initialized");
        return false;
    }
    else if(participant_->find_type(ThroughputDataType::type_name_))
    {
        EPROSIMA_LOG_ERROR(THROUGHPUTPUBLISHER, "ERROR STATIC DATA type already registered");
        return false;
    }
    
    // Create the static type
    throughput_data_type_.reset(new ThroughputDataType(payload));
    // Register the static type
    if (RETCODE_OK != throughput_data_type_.register_type(participant_))
    {
        return false;
    }

    return true;
}

bool ThroughputPublisher::create_data_endpoints(const DataWriterQos& dw_qos)
{
    if (nullptr != data_pub_topic_)
    {
        std::cout << "already initialized\n";
        return false;
    }

    if (nullptr != data_writer_)
    {
        std::cout << "data_writer_ already initialized.\n";
        return false;
    }

    // Create the topic
    std::ostringstream topic_name;
    topic_name << "ThroughputTest_";
    if(hostname_)
    {
        topic_name << asio::ip::host_name() << "_";
    }
    topic_name << pid_ << "_UP";

    data_pub_topic_ = participant_->create_topic(
        topic_name.str(),
        ThroughputDataType::type_name_,
        TOPIC_QOS_DEFAULT);
    
    if (nullptr == data_pub_topic_)
    {
        std::cout << "data_pub_topic_ creating failed.\n";
        return false;
    }

    // Create the endpoint 即是create_datawriter
    if (nullptr == 
            (data_writer_ = publisher_->create_datawriter(
                data_pub_topic_,
                dw_qos,
                &data_writer_listener_)))
    {
        return false;
    }

    return true;

}

// 删除端点即是删除data_writer_
bool ThroughputPublisher::destroy_data_endpoints()
{
    assert(nullptr != participant_);
    assert(nullptr != publisher_);

    // Delete the endpoint(datawriter)
    if (nullptr == data_writer_
            || RETCODE_OK != publisher_->delete_datawriter(data_writer_))
    {
        EPROSIMA_LOG_ERROR(THROUGHPUTPUBLISHER, "ERROR destroying the DataWriter");
        return false;
    }
    data_writer_ = nullptr;
    data_writer_listener_.reset();

    // Delete topic
    if (nullptr == data_pub_topic_
            || RETCODE_OK != participant_->delete_topic(data_pub_topic_))
    {
        EPROSIMA_LOG_ERROR(THROUGHPUTPUBLISHER, "ERROR destroying the pub topic");
        return false;
    }
    data_pub_topic_ = nullptr;

    // Delete the Type
    if (RETCODE_OK != participant_->unregister_type(ThroughputDataType::type_name_))
    {
        EPROSIMA_LOG_ERROR(THROUGHPUTPUBLISHER, "ERROR destroying the DataWriter");
        return false;
    }

    throughput_data_type_.reset();

    return true;
}

int ThroughputPublisher::total_matches() const
{
    int count = data_writer_listener_.get_matched();    // omitted command_reader_listener_ and command_writer_listener_
    assert(count >= 0 && count <= 3 * (int)subscribers_);

    return count;
}

void ThroughputPublisher::DataWriterListener::on_publication_matched(
        DataWriter* writer,
        const PublicationMatchedStatus& info)
{
    if(info.current_count == 1)
    {
        EPROSIMA_LOG_INFO(THROUGHPUTPUBLISHER, C_RED << "Pub: DATA Pub Matched "
                    << info.total_count << "/" << throughput_publisher_.subscribers_ << C_DEF);
    }

    matched_ = info.total_count;
    throughput_publisher_.data_discovery_cv_.notify_one();
}



