#include "include/Application.hpp"

#include "include/CLIParser.hpp"
#include "include/ServerApp.hpp"
#include "include/ClientApp.hpp"

using namespace eprosima::fastdds::dds;

namespace request_reply{

std::shared_ptr<Application> Application::make_app(
        const CLIParser::config& config,
        const std::string& service_name)
{
    std::shared_ptr<Application> entity;
    switch (config.entity)
    {
        case CLIParser::EntityKind::SERVER:
        {
            entity = std::make_shared<ServerApp>(service_name);
            break;
        }

        case CLIParser::EntityKind::CLIENT:
        {
            entity = std::make_shared<ClientApp>(config, service_name);
            break;
        }

        case CLIParser::EntityKind::UNDEFINED:
        default:
            throw std::runtime_error("Entity initialization failed");
            break;
    }

    return entity;
}


}