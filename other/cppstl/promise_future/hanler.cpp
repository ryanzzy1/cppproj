#include "ara/com/types.h"

using Proxy = ara::com::sample::proxy::MyInterfaceProxy;

// ara::com::ServiceHandleContainer<Handle>
template <typename Handle>
using ServiceHandleContainer = ara::core::Vector<Handle>;


// ara::com::FindServiceHandler<T>;

template <typename T>
using FindServiceHandler = std::function<void(ServiceHandleContainer<T>, FindServiceHandle)>;

// ara::com::FindServiceHandle
struct FindServiceHandle
{
    internal::ServiceId service_id;
    internal::InstanceId instance_id;
    std::uint32_t uid;
};

// StartFindServie(ara::com::FindServiceHandler<<ProxyClassNmae>::HandleType> handler, ara::com::InstanceIdentifier instance = ara::com::InstanceIdentifier::Any);

static ara::com::FindServieHandle StartFindService(ara::com::FindServiceHandler<Proxy::HandleType> handler);