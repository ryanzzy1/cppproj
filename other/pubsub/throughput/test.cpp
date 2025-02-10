#include <iostream>
#include <asio.hpp>
#include <fstream>
#include <cstddef>
#include <vector>
#include <map>
#include <thread>
#include <chrono>

// #include "ThroughputTypes.hpp"
#include "ThroughputPublisher.hpp"
// #include "optionarg.hpp"

// dds header
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/log/Colors.hpp>
#include <fastdds/dds/log/Log.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/DataWriterListener.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/DataReaderListener.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicData.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicPubSubType.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicTypeBuilder.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicTypeBuilderFactory.hpp>
#include <fastdds/dds/xtypes/dynamic_types/MemberDescriptor.hpp>
#include <fastdds/dds/xtypes/dynamic_types/TypeDescriptor.hpp>
#include <fastdds/rtps/transport/shared_mem/SharedMemTransportDescriptor.hpp>
#include <fastdds/rtps/transport/UDPv4TransportDescriptor.hpp>
#include <fastdds/rtps/attributes/PropertyPolicy.hpp>

using namespace eprosima::fastdds::dds;


// Entities
eprosima::fastdds::dds::DataWriter* data_writer_ = nullptr;
eprosima::fastdds::dds::DataWriter* command_writer_ = nullptr;
eprosima::fastdds::dds::DataReader* command_reader_ = nullptr;


// Time
std::chrono::steady_clock::time_point t_start_;
std::chrono::steady_clock::time_point t_end_;
std::chrono::duration<double, std::micro> t_overhead_;

// Flags
bool dynamic_types_ = false;
Arg::EnablerValue data_sharing_ = Arg::EnablerValue::NO_SET;
bool data_loans_ = false;
Arg::EnablerValue shared_memory_ = Arg::EnablerValue::NO_SET;
bool ready_ = true;
bool reliable_ = false;
bool hostname_ = false;
uint32_t pid_ = 0;

// Static Data
ThroughputType* throughput_data_ = nullptr;

// Dynamic Data
eprosima::fastdds::dds::DynamicData::_ref_type* dynamic_data_ {nullptr};
eprosima::fastdds::dds::TypeSupport dynamic_pub_sub_type_;

// Results 
std::vector<TroughputResults> results_;


std::string recoveries_file_;

uint32_t subscribers_ = 1;

// load_recoveries()
bool load_recoveries()
{
    std::ifstream fi(recoveries_file_);

    std::cout << "Reading recoveries file: " << recoveries_file_ << std::endl;
    std::string DELIM = ";";
    if (!fi.is_open())
    {
        std::cout << "Could not open recoveries file: " << recoveries_file_ << ", closing." << std::endl;
        return false;
    }
    return true;
}


std::string export_csv_ = "./test.csv";
void export_csv_file() {
    if (export_csv_ != "")
    {
        std::cout << "###run here.\n";
        std::ofstream data_file;
        data_file.open(export_csv_);
        data_file << "Payload [Bytes],Demand [sample/burst],Recovery time [ms],Sent [samples],Publication time [us],"
                  << "Publication sample rate [Sample/s],Publication throughput [Mb/s],Received [samples],"
                  << "Lost [samples],Subscription time [us],Subscription sample rate [Sample/s],"
                  << "Subscription throughput [Mb/s]" << std::endl;
                  
        data_file.flush();
        data_file.close();
    }
}

// Log data to csv file
std::map<uint32_t, std::vector<uint32_t>> demand_payload_;

void demad_payload()
{
    for(auto sit = demand_payload_.begin(); sit != demand_payload_.end(); ++sit)
    {
        // TODO: file read and close
    }
}


bool test(uint32_t test_time,
          uint32_t recovery_time_ms,
          uint32_t demand,
          uint32_t msg_size)
{
    // Declare test time variables
    std::chrono::duration<double, std::micro> clock_overhead(0);
    std::chrono::duration<double, std::nano> test_time_ns = std::chrono::seconds(test_time);
    std::chrono::duration<double, std::nano> recovery_duration_ns = std::chrono::milliseconds(recovery_time_ms);
    std::chrono::steady_clock::time_point batch_start;

    // 发送测试开始命令，并休眠一会儿，给订阅者时间完成设置
    uint32_t samples = 0;
    ThroughputCommandType command_sample;
    SampleInfo info; // SampleInfo.hpp, 引入dds subscriber 相关头文件即可
    command_sample.m_command = e_Command::TEST_STARTS;
    command_writer_->write(&command_sample);

    // 如果订阅者不能即时确认TEST_STARTS，会被认为有出错的地方
    std::chrono::steady_clock::time_point test_start_sent_tp = std::chrono::steady_clock::now();
    if (RETCODE_OK != command_writer_->wait_for_acknowledgments({20,0})) 
    {
        EPROSIMA_LOG_ERROR(THROUGHPUTPUBLISHER,
            "Somethind went wrong: The subscriber has not acknowledged the TEST_STARTS command.");
        return false;
    }
    // 计算订阅者花费多长时间确认 TEST_STARTS
    std::chrono::duration<double, std::micro> test_start_ack_duration =
        std::chrono::duration<double, std::micro>(std::chrono::steady_clock::now() - test_start_sent_tp);

    // 发送样本批次，直到达到测试时间
    t_start_ = std::chrono::steady_clock::now();
    uint32_t seqnum = 0;
    while ((t_end_ - t_start_) < test_time_ns)
    {
        // Get start time 
        batch_start = std::chrono::steady_clock::now();
        // Send a batch of size demand
        for (uint32_t sample = 0; sample < demand; sample++)
        {
            if (dynamic_types_) // 动态数据类型
            {
                (*dynamic_data_)->set_uint32_value(0, ++seqnum);
                data_writer_->write(dynamic_data_);
            }
            else if (data_loans_) // loan 标志flag
            {
                // Try loan a sample
                void* data = nullptr;
                if (RETCODE_OK
                        == data_writer_->loan_sample(
                           data,
                           DataWriter::LoanInitializationKind::NO_LOAN_INITIALIZATION)) 
                {
                    // initialize and send the sample
                    static_cast<ThroughputType*>(data)->seqnum = ++seqnum;

                    if (RETCODE_OK != data_writer_->write(data))
                    {
                        data_writer_->discard_loan(data);
                    }
                }
                else
                {
                    std::this_thread::yield();
                    // try again this sample
                    --sample;
                    continue;
                }
            }
            else
            {
                throughput_data_->seqnum = ++seqnum;
                data_writer_->write(throughput_data_);
            }
        }
        
        // Get end time
        t_end_ = std::chrono::steady_clock::now();
        // Add the number of sent samples
        samples += demand;

        // 如果样本发送时间小于回复时间，休眠时间是回复时间 - batch 发送时间
        // 否则就会导致在while内执行完for循环，立即执行下一轮的for循环，样本
        // 发送没有恢复时间。
        // 如果以上差值为负数，根据sleep_for() 的实现，返回值是不会设置线程休眠的。
        // 另外，设计指出，如果durtaion 小于线程调度的时间片，休眠会持续到OS下次调度为止。
        std::this_thread::sleep_for(recovery_duration_ns - (t_end_ - batch_start));
        
        // 每个批次样本时钟访问翻倍， 前面会有t_overhead 计算转换为
        //  这个计算意义没明白：t_overhead_ = std::chrono::duration<double, std::micro>(t_end_ - t_start_) / 1001;
        clock_overhead += t_overhead_ * 2;
    }

    size_t removed = 0;
    data_writer_->clear_history(&removed);

    command_sample.m_command = e_Command::TEST_ENDS;
    command_writer_->write(&command_sample);

    // 如果订阅者不能即时确认TEST_ENDS，同前面TEST_STARTS 一样的处理，报出错处理
    if (RETCODE_OK != command_writer_->wait_for_acknowledgments({20,0}))
    {
        EPROSIMA_LOG_ERROR(THROUGHPUTPUBLISHER, "Something went wrong: The subscriber ha not ack the TEST_ENDS CMD.");
        return false;
    }

    // 结果处理
    uint32_t num_results_received = 0;
    bool results_error = false;
    while (!results_error && num_results_received < subscribers_)
    {
        if (command_reader_->wait_for_unread_message({20,0})
                && RETCODE_OK == command_reader_->take_next_sample(&command_sample, &info)
                && info.valid_data)
        {
            if(command_sample.m_command == e_Command::TEST_RESULTS)
            {
                num_results_received++;

                TroughputResults result;
                result.payload_size = msg_size + (uint32_t)(dynamic_types_ ? 8 : ThroughputType::overhead);
                result.demand = demand;
                result.recovery_time_ms = recovery_time_ms;

                result.publisher.send_samples = samples;
                result.publisher.totaltime_us = std::chrono::duration<double, std::micro>(t_end_ - t_start_) - clock_overhead;

                result.subscriber.recv_samples = command_sample.m_receivedsamples;
                assert(samples >= command_sample.m_receivedsamples);
                result.subscriber.lost_samples = samples - (uint32_t)command_sample.m_receivedsamples;
                // 订阅方的时间统计：总时间-测试开始的命令确认时间-时钟开销（）
                result.subscriber.totaltime_us =
                        std::chrono::microseconds(command_sample.m_totaltime) - test_start_ack_duration - clock_overhead;
            
                result.compute();

                if(num_results_received == 1)
                {
                    results_.push_back(result);
                }
                else
                {
                    auto& results_entry = results_.back();
                    results_entry.publisher.send_samples += result.publisher.send_samples;
                    results_entry.subscriber.recv_samples += result.subscriber.recv_samples;
                    results_entry.subscriber.lost_samples += result.subscriber.lost_samples;
                    results_entry.subscriber.MBitssec += result.subscriber.MBitssec;
                    results_entry.subscriber.Packssec += result.subscriber.Packssec;
                }

                // Log data to CSV file
                if (export_csv_ != "")
                {
                    std::ofstream data_file;
                    data_file.open(export_csv_, std::fstream::app);
                    data_file << std::fixed << std::setprecision(3)
                              << result.payload_size << ","
                              << result.demand << ","
                              << result.recovery_time_ms << ","
                              << result.publisher.send_samples << ","
                              << result.publisher.totaltime_us.count() << ","
                              << result.publisher.Packssec << ","
                              << result.publisher.MBitssec << ","
                              << result.subscriber.recv_samples << ","
                              << result.subscriber.lost_samples << ","
                              << result.subscriber.totaltime_us.count() << ","
                              << result.subscriber.Packssec << ","
                              << result.subscriber.MBitssec << std::endl;
                    data_file.flush();
                    data_file.close();
                }
                command_writer_->clear_history(&removed);
            }
            else
            {
                std::cout << "The test expected results, stopping" << std::endl;
                results_error = true;
            }
        }
        else
        {
            std::cout << "Reading results;" << std::endl;
            results_error = true;
        }
    }

    return !results_error;

}

/*
bool ThroughputPublisher::init_dynamic_types()
{
    assert(participant_ != nullptr);

    // 检查是否已经被初始化过
    if (dynamic_pub_sub_type_)
    {
        EPROSIMA_LOG_ERROR(THROUGHPUTPUBLISHER, "ERROR DYNAMIC DATA type already initialized");
        return false;
    }
    else if (participant_->find_type(ThroughputDataType::type_name_))
    {
        EPROSIMA_LOG_ERROR(THROUGHPUTPUBLISHER, "ERROR DYNAMIC DATA type already registered");
        return false;
    }

    // Dummy type registration
    DynamicTypeBuilderFactory::_ref_type factory {DynamicTypeBuilderFactory::get_instance()};
    // Create basic builders 
    TypeDescriptor::_ref_type type_descriptor {traits<TypeDescriptor>::make_shared()};
    type_descriptor->kind(TK_STRUCTURE);
    type_descriptor->name(ThroughputDataType::type_name_);

    DynamicTypeBuilder::_ref_type struct_type_builder {factory->create_type(type_descriptor)};

    // Add members to the struct.
    MemberDescriptor::_ref_type member_descriptor {traits<MemberDescriptor>::make_shared()};
    member_descriptor->name("seqnum");
    member_descriptor->type(factory->get_primitive_type(TK_UINT32));
    struct_type_builder->add_member(member_descriptor);
    member_descriptor->name("data");
    member_descriptor->type(
        factory->create_sequence_type(factory->get_primitive_type(TK_BYTE), static_cast<uint32_t>(LENGTH_UNLIMITED))->build());
    struct_type_builder->add_member(member_descriptor);
    dynamic_pub_sub_type_.reset(new DynamicPubSubType(struct_type_builder->build()));

    // Register the data type
    if (RETCODE_OK
            != dynamic_pub_sub_type_.register_type(participant_))
    {
        EPROSIMA_LOG_ERROR(THROUGHPUTPUBLISHER, "ERROR registering the DYNAMIC DATA topic");
        return false;
    }

    return true;
} */

/*
bool ThroughputPublisher::init_static_types(
        uint32_t payload)
{
    assert(participant_ != nullptr);

    // 检查是否已经初始化
    if(throughput_data_type_)
    {
        EPROSIMA_LOG_ERROR(THROUGHPUTPUBLISHER, "ERROR STATIC DATA type already initialized");
        return false;
    }
    else if (participant_->find_type(ThroughputDataType::type_name_))
    {
        EPROSIMA_LOG_ERROR(THROUGHPUTPUBLISHER, "ERROR STATIC DATA type already registered");
        return false;
    }

    // 创建静态类型
    throughput_data_type_.reset(new ThroughputDataType(payload));
    // Register the static type
    if (RETCODE_OK
            != throughput_data_type_.register_type(participant_))
    {
        return false;
    }

    return true;
}
*/

/*
// create data endpoints
bool ThroughputPublisher::create_data_endpoints(
        const DataWriterQos& dw_qos)
{
    if (nullptr != data_pub_topic_)
    {
        EPROSIMA_LOG_ERROR(THROUGHPUTPUBLISHER, "ERROR topic already initilized");
        return false;
    }

    if(nullptr != data_writer_)
    {
        EPROSIMA_LOG_ERROR(THROUGHPUTPUBLISHER, "ERROR data_writer_ already initialized");
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

    data_pub_topic_ = participant_->create_topic(
        topic_name.str(),
        ThroughputDataType::type_name_,
        TOPIC_QOS_DEFAULT);
    if (nullptr == data_pub_topic_)
    {
        EPROSIMA_LOG_ERROR(THROUGHPUTPUBLISHER, "ERROR creating the DATA topic");
        return false;
    }

    // Create the endpoint
    if (nullptr == (data_writer_ = publisher_->create_datawriter(
                        data_pub_topic_,
                        dw_qos,
                        &data_writer_listener_)))
    {
        return false;
    }

    return true;
}
*/

/*
// delete data_endpoints
bool ThroughputPublisher::destroy_data_endpoints()
{
    assert(nullptr != participant_);
    assert(nullptr != publisher_);

    // Delete the endpoint
    if (nullptr == data_writer_
            || RETCODE_OK != publisher_->delete_datawriter(data_writer_))
    {
        EPROSIMA_LOG_ERROR(THROUGHPUTPUBLISHER, "ERROR destroying the DataWriter");
        return false;
    }
    data_writer_ = nullptr;
    data_writer_listener_.reset();

    // Delete the Topic
    if (nullptr == data_pub_topic_
            || RETCODE_OK != participant_->delete_topic(data_pub_topic_))
    {
        EPROSIMA_LOG_ERROR(THROUGHPUTPUBLISHER, "ERROR destroying the DATA topic");
        return false;
    }
    data_pub_topic_ = nullptr;

    // Delete the type
    if (RETCODE_OK != participant_->unregister_type(ThroughputDataType::type_name_))
    {
        EPROSIMA_LOG_ERROR(THROUGHPUTPUBLISHER, "ERROR unregistering the DATA type");
        return false;
    }

    throughput_data_type_.reset();

    return true;
}
*/
int main()
{
    std::ostringstream topic_name;

    uint32_t pid;

    topic_name << "this is hostname: " << asio::ip::host_name() << "_";

    topic_name << pid << "_SUB2PUB";

    std::cout << "topic_name: " << topic_name.str() << std::endl;

    topic_name.str("");
    topic_name.clear();
    topic_name << "ThroughputTest_Command_";

    topic_name << asio::ip::host_name() << "_" << pid << "_PUB2SUB";

    std::cout << "topic_name: " << topic_name.str() << std::endl;

    int i ;
    std::cout << "i = " <<( (3 < 2) ? 1 : 2) << std::endl;

    // load demands file 
    std::string demands_file_ = "./test.txt";
    std::ifstream fi(demands_file_);
    std::cout << "Reading demands files: " << demands_file_ << std::endl;
    std::string DELIM = ";";
    if (!fi.is_open())
    {
        std::cout << "Could't open demands file: " << demands_file_ << ", closing." << std::endl;

        return false;
    }

    std::string line;
    size_t start;
    size_t end;
    bool first = true;
    bool more = true;
    auto overhead = uint32_t(true ? 8 : 1);
    uint32_t payload_;
    std::map<uint32_t, std::vector<uint32_t>> demand_payload_;

    std::cout << "befor while\n";
    while(std::getline(fi, line))
    {
        start = 0;
        end = line.find(DELIM);
        std::cout << "line.find(DELIM) END:" << end << std::endl;
        first = true;
        uint32_t demand;
        more = true;
        std::cout << "entering while1:\n";
        while(more)
        {
            std::cout << "entering while2:\n";
            std::istringstream iss(line.substr(start, end - start));
            if(first)
            {
                iss >> payload_;
                std::cout << "entering while2_if(first) paylod_:" << payload_ << " overhead:" << overhead << "\n";
                std::cout << " first iss.str():" << iss.str() << std::endl;
                if (payload_ < overhead)
                {
                    std::cout << "Minimum payload is 16 bytes." << std::endl;
                    return false;
                }
                payload_ -= overhead;
                first = false;
            } else {
                iss >> demand;
                std::cout << "first_else iss.str():" << iss.str() << std::endl;
                demand_payload_[payload_].push_back(demand);
            }
            start = end + DELIM.length();
            std::cout << "first start:" << start << " first end:" << end << std::endl;
            end = line.find(DELIM, start);
            std::cout << "first find end:" << end << std::endl;           

            if (end == std::string::npos)
            {
                more = false;
                std::istringstream n_iss(line.substr(start, end -start));
                if ( n_iss >> demand)
                {
                    
                }
            }
        }
    }

    fi.close();
    payload_ += overhead;
    
    std::cout << "Performing test wiht this payloads/demands: " << std::endl;
    for (auto sit = demand_payload_.begin(); sit != demand_payload_.end(); ++sit)
    {
        printf("payload: %6d; Demands: ",sit->first + overhead);
        for (auto dit = sit->second.begin(); dit != sit->second.end(); ++dit)
        {
            printf("%6d, ", *dit);
        }
        printf("\n");
    }

    
    export_csv_file();

    print_results(results_);
    

    return 0;
}