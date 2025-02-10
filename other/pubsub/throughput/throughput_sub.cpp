#include "throughput_sub.hpp"

#include <chrono>
#include <thread>
#include <vector>
#include <iostream>

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/qos/DomainParticipantQos.hpp>
#include <fastdds/dds/log/Colors.hpp>
#include <fastdds/dds/log/Log.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicPubSubType.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicTypeBuilder.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicTypeBuilderFactory.hpp>
#include <fastdds/rtps/transport/UDPv4TransportDescriptor.hpp>


using namespace eprosima::fastdds::dds;
using namespace eprosima::fastdds::rtps;

void ThroughputSubscriber::DataReaderListener::reset()
{
    last_seq_num_ = 0;
    lost_samples_ = 0;
    received_samples_ = 0;
    matched_ = 0;
    enable_ = true;
}

void ThroughputSubscriber::DataReaderListener::disable()
{
    enable_ = false;
}

void ThroughputSubscriber::DataReaderListener::save_numbers()
{
    saved_last_seq_num_ = last_seq_num_;
    saved_lost_samples_ = lost_samples_;
    saved_received_samples_ = received_samples_;
}

void ThroughputSubscriber::DataReaderListener::on_subscription_matched(
        DataReader* reader,
        const SubscriptionMatchedStatus& info)
{
    if ( 1 == info.current_count)
    {
        std::cout << C_RED << "Sub: DATA Sub Matched" << C_DEF << std::endl;
    }
    else
    {
        std::cout << C_RED << "DATA SUBSCRIBER MATCHING REMOVAL" << C_DEF << std::endl;
    }

    matched_ = info.total_count;
    throughput_subscriber_.data_discovery_cv_.notify_one();
}

void ThroughputSubscriber::DataReaderListener::on_data_available(DataReader* reader)
{
    if(!enable_)
    {
        return;
    }

    auto& sub = throughput_subscriber_;
    void* data = sub.dynamic_types_ ? (void*)sub.dynamic_data_ : (void*)sub.throughput_data_;
    assert(nullptr != data);

    while(RETCODE_OK == reader->take_next_sample(data, &info_))
    {
        if (info_.valid_data)
        {
            uint32_t seq_num {0};

            if (sub.dynamic_types_)
            {
                (*sub.dynamic_data_)->get_uint32_value(seq_num, 0);
            }
            else
            {
                seq_num = sub.throughput_data_->seqnum;
            }

            if ((last_seq_num_ + 1) < seq_num)  // 最后一个接收到的数据序列号 +1 还小于预期的数据序列，丢包至少2包；
            {
                lost_samples_ += seq_num - last_seq_num_ - 1;   // 计算丢包 = 预期数据序列 - 实际接收到包 -1 （3 -2 这种默认为不丢包？）
            }
            last_seq_num_ = seq_num;
            received_samples_ += 1;
        }
        else
        {
            std::cout << "Invalid data received." << std::endl;
        }
    }
}

ThroughputSubscriber::ThroughputSubscriber()
    : data_reader_listener_(*this)
{
}

ThroughputSubscriber::~ThroughputSubscriber()
{
    if (dynamic_types_)
    {
        destroy_data_endpoints();
    }else if (nullptr != data_reader_
              || nullptr != data_sub_topic_
              || throughput_data_type_)
    {
        EPROSIMA_LOG_ERROR(THROUGHPUTSUBSCRIBER, "ERROR unregistering the DATA type");
        return;
    }
}

bool ThroughputSubscriber::init(
        bool reliable,
        uint32_t pid,
        bool hostname)
{
    reliable_ = reliable;
    pid_ = pid;
    hostname_ = hostname;

    // create DomainParticipants
    std::string participant_profile_name = "sub_participant_profile";
    DomainParticipantQos pqos;

    // Default domain
    DomainId_t domainId = pid_ % 230;

    // Default participant name
    pqos.name("throughput_test_subscriber");

    // TBD: load xml configuration file

    // TBD: domainId force or not?
    
    // domain part_property_policy not set

    // shared memory transport disabled
    std::shared_ptr<eprosima::fastdds::rtps::UDPv4TransportDescriptor> udp_transport =
            std::make_shared<eprosima::fastdds::rtps::UDPv4TransportDescriptor>();
    pqos.transport().user_transports.push_back(udp_transport);
    pqos.transport().use_builtin_transports = false;

    // create the participant
    participant_ = 
            DomainParticipantFactory::get_instance()->create_participant(domainId, pqos);
    
    if (participant_ == nullptr)
    {
        std::cout << "ERROR creating participant." << std::endl;
        return false;
    }

    // create Subscriber
    subscriber_ = participant_->create_subscriber(SUBSCRIBER_QOS_DEFAULT, nullptr);
    if (subscriber_ == nullptr)
    {
        EPROSIMA_LOG_ERROR(THGOUGHPUTSUBSCRIBER, "ERROR creating the subscriber");
        return false;
    }
    
    // Reliability
    ReliabilityQosPolicy rp;
    rp.kind =
            reliable_ ? eprosima::fastdds::dds::RELIABLE_RELIABILITY_QOS: eprosima::fastdds::dds::BEST_EFFORT_RELIABILITY_QOS;
    dr_qos_.reliability(rp);

    // Calculate overhead
    t_start_ = std::chrono::steady_clock::now();
    for (int i = 0; i < 1000; ++i)
    {
        t_end_ = std::chrono::steady_clock::now();
    } 
    // 1000 次访问的最后计时点，先放大再做每次的平均统计，准确应该是除以1000次，即为每次的访问时间
    t_overhead_ = std::chrono::duration<double, std::micro>(t_end_ - t_start_) / 1000; // 1000 为以上循环的1000次计时
    std::cout << "Subscriber's clock access overhead: " << t_overhead_.count() << " us" << std::endl;

    return dynamic_types_ ? init_dynamic_types() && create_data_endpoints(dr_qos_) : true;
}

void ThroughputSubscriber::run()
{
    std::cout << "Sub waiting for command discovery" << std::endl;
    {
        // TODO: may do not need command discovery
        if(dynamic_types_)  // TODO： 因为没有cmd_discovery, 还需验证是否正确
        {
            std::cout << "dynamic_types_ total_matches: " << (total_matches() == 3) << std::endl;
        }
        else
        {
            std::cout << "dynamic_types_ total_matches: " << (total_matches() == 2) << std::endl;
        }
    }
    std::cout << "Sub waiting for command discovery" << std::endl;

    auto dr_qos = dr_qos_;
    if (dynamic_types_){
        // do nothing
    }
    else
    {
        // Validate QoS settings
        uint32_t max_demand;    // 需要固定或者参数传入
        if (dr_qos.history().kind == KEEP_LAST_HISTORY_QOS)
        {
            // Ensure that the history depth is at least the demand
            if(dr_qos.history().depth < 0 ||
                    static_cast<uint32_t>(dr_qos.history().depth) < max_demand)
            {
                EPROSIMA_LOG_WARNING(THGOUGHPUTSUBSCRIBER, "Setting history depth to" << max_demand);
                dr_qos.resource_limits().max_samples = max_demand;
                dr_qos.history().depth = max_demand;
            }
        }
        // KEEP_ALL case
        else
        {
            // Ensure that the max samples is at least the demand
            // <= 0 means infinite resources. By default, 5000.
            if(dr_qos.resource_limits().max_samples <= 0 ||
                    static_cast<uint32_t>(dr_qos.resource_limits().max_samples) < max_demand) 
            {
                EPROSIMA_LOG_WARNING(THROUGHPUTSUBSCRIBER, "Setting resource limit max samples to " << max_demand);
                dr_qos.resource_limits().max_samples = max_demand;
            }
        }
        // 对allocated_samples 设置为max_samples. 因为分配的allocated_samples 必须 <= max_samples
        dr_qos.resource_limits().allocated_samples = dr_qos.resource_limits().max_samples; //Number of samples currently allocated. By default, 100.
        uint32_t size;
        if(init_static_types(size) && create_data_endpoints(dr_qos))
        {
            assert(nullptr == throughput_data_);
            if (1)  // no data_loans_
            {
                // create data sample, 后面需要释放资源
                throughput_data_ = static_cast<ThroughputType*>(throughput_data_type_.create_data());
            }

            // wait for data endpoint discovery
            {
                std::cout << "Waiting for data discovery" << std::endl;
                std::unique_lock<std::mutex> data_disc_lock(mutex_);
                data_discovery_cv_.wait(data_disc_lock, [&]()
                        {
                        return total_matches() == 3;
                        });
                std::cout << "Discovery data complete" << std::endl;
            }
        }
        else
        {
            EPROSIMA_LOG_ERROR(THROUGHPUTSUBSCRIBER,
                    "Error preparing static types and endpoints for testing");
        }
    }
    std::cout << "Last Received Sample: " << data_reader_listener_.saved_last_seq_num_ << std::endl;
    std::cout << "Lost Samples: " << data_reader_listener_.saved_lost_samples_ << std::endl;
    std::cout << "Received Samples: " << data_reader_listener_.saved_received_samples_ << std::endl;
    // omited test samples/s and test size statistics calculations due to lack of command designing.

    return;    
}



bool ThroughputSubscriber::init_dynamic_types()
{
    assert(participant_ != nullptr);

    // Check if it has been initialized before
    if (dynamic_pub_sub_type_)
    {
        EPROSIMA_LOG_ERROR(THROUGHPUTSUBSCRIBER, "ERROR DYNAMIC DATA type already initialized");
        return false;
    }
    else if (participant_->find_type(ThroughputDataType::type_name_))
    {
        EPROSIMA_LOG_ERROR(THROUGHPUTSUBSCRIBER, "ERROR DYNAMIC DATA type already registered");
        return false;
    }

    // Dummy type registration
    DynamicTypeBuilderFactory::_ref_type factory {DynamicTypeBuilderFactory::get_instance()};
    // Create basic builders
    TypeDescriptor::_ref_type type_descriptor {traits<TypeDescriptor>::make_shared()};
    type_descriptor->kind(TK_STRUCTURE);
    type_descriptor->name(ThroughputDataType::type_name_);

    DynamicTypeBuilder::_ref_type struct_type_builder {factory->create_type(type_descriptor)};

    // 为结构体添加成员
    MemberDescriptor::_ref_type member_descriptor {traits<MemberDescriptor>::make_shared()};
    member_descriptor->name("seqnum");
    member_descriptor->type(factory->get_primitive_type(TK_UINT32));
    struct_type_builder->add_member(member_descriptor);
    member_descriptor->name("data");
    member_descriptor->type(factory->create_sequence_type(
                factory->get_primitive_type(TK_BYTE), static_cast<uint32_t>(LENGTH_UNLIMITED))->build());
    struct_type_builder->add_member(member_descriptor);
    dynamic_pub_sub_type_.reset(new DynamicPubSubType(struct_type_builder->build()));

    //Register the data type
    if (RETCODE_OK != dynamic_pub_sub_type_.register_type(participant_))
    {
        EPROSIMA_LOG_ERROR(THROUGHPUTSUBSCRIBER, "ERROR registering the DYNAMIC DATAtopic");
        return false;
    }

    return true;
}

bool ThroughputSubscriber::init_static_types(uint32_t payload)
{
    assert(participant_ != nullptr);

    // Check if has initialized before
    if(throughput_data_type_){
        EPROSIMA_LOG_ERROR(THROUGHPUTSUBSCRIBER, "ERROR STATIC DATA type already initialized");
        return false;
    }
    else if (participant_->find_type(ThroughputDataType::type_name_))
    {
        EPROSIMA_LOG_ERROR(THROUGHPUTSUBSCRIBER, "ERROR STATIC DATA type already initialized");
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

bool ThroughputSubscriber::create_data_endpoints(const DataReaderQos& dr_qos)
{
    if(nullptr != data_sub_topic_)
    {
        EPROSIMA_LOG_ERROR(THROUGHPUTSUBSCRIBER, "ERROR topic type already initialized");
        return false;
    }
    
    if(nullptr != data_reader_)
    {
        EPROSIMA_LOG_ERROR(THROUGHPUTSUBSCRIBER, "ERROR data_reader_ already initialized");
        return false;
    }

    // Create the topic
    std::ostringstream topic_name;
    topic_name << "ThroughputTest_";
    if (hostname_)
    {
        topic_name << asio::ip::host_name() << "_";
    }
    topic_name << pid_ << "_UP";

    data_sub_topic_ = participant_->create_topic(
        topic_name.str(),
        ThroughputDataType::type_name_,
        TOPIC_QOS_DEFAULT);
    if(nullptr == data_sub_topic_)
    {
        std::cout << "Creating data topic error." << std::endl;
        return false;
    }

    // Create the endpoint
    if (nullptr == 
        (data_reader_ = subscriber_->create_datareader(
            data_sub_topic_,
            dr_qos,
            &data_reader_listener_)))
    {
        return false;
    }

    return true;
}

bool ThroughputSubscriber::destroy_data_endpoints()
{
    
    assert(nullptr != participant_);
    assert(nullptr != subscriber_);

    // Delete the endpoint/ DataReader or DataWriter
    if (nullptr == data_reader_
            || RETCODE_OK != subscriber_->delete_datareader(data_reader_))
    {
        EPROSIMA_LOG_ERROR(THROUGHPUTSUBSCRIBER, "ERROR destroying the DataReader");
        return false;
    }
    data_reader_ = nullptr;
    data_reader_listener_.reset();

    // Delete the Topic
    if(nullptr == data_sub_topic_
            || RETCODE_OK != participant_->delete_topic(data_sub_topic_))
    {
        EPROSIMA_LOG_ERROR(THROUGHPUTSUBSCRIBER, "ERROR destroying the DATA topic");
        return false;
    }
    data_sub_topic_ = nullptr;

    // Delete the Type
    if (RETCODE_OK 
            != participant_->unregister_type(ThroughputDataType::type_name_))
    {
        EPROSIMA_LOG_ERROR(THROUGHPUTSUBSCRIBER, "ERROR unregistering the DATA type");
        return false;      
    }
    throughput_data_type_.reset();

    // throughput_data_ 资源的释放
    throughput_data_type_.delete_data(throughput_data_);
    throughput_data_ = nullptr;
}

int ThroughputSubscriber::total_matches() const
{
    // 省掉了命令行写入者和读取者的匹配数据
    int count = data_reader_listener_.get_matches();
    assert(count >= 0 && count <= 3);

    return count;
}