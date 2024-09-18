#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <memory>
#include <string>

#include "CLIParser.hpp"

namespace request_reply{

class Application
{
public:
    virtual ~Application() = default;

    virtual void run() = 0;

    virtual void stop() = 0;

    static std::shared_ptr<Application> make_app(
            const CLIParser::config& config,
            const std::string& service_name);
};
} // namespace request_reply
#endif