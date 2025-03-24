#include <iostream>


#include "ara/com/sample_ptr.h"


template<typename T>
class ara::com::SamplePtr
{
public:
    using E2ECheckStatus = ara::com::e2e::ProfileCheckStatus;

    constexpr SamplePtr() noexcept
        : dataPtr_(nullptr)
        , e2eCheckStatus_(E2ECheckStatus::kOk)
        {}

    constexpr SamplePtr(std::nullptr_t) noexcept
        :SamplePtr()
        {}
    
    explicit SamplePtr(T* ptr, E2ECheckStatus e2eCheckState = E2ECheckStatus::kok)
        : dataPtr_(ptr, Deleter)
        , e2eCheckStatus_(e2eCheckState)
        {}
    
    SamplePtr(SamplePtr<T>&& r) noexcept
        : dataPtr_(std::move(r.dataPtr_))
        , e2eCheckStatus_(std::move(r.e2eCheckStatus_))
        {            
        }

    SamplePtr(const SamplePtr<T>& r) = delete;

    SamplePtr& operator=(SamplePtr<T>&& r) noexcept;

    SamplePtr& operator=(const SamplePtr<T>& r) = delete;

    SamplePtr& operator=(std::nullptr_t) noexcept;

    T* operator->() const noexcept;

    T& operator*() const noexcept;

    explicit operator bool() const noexcept;

    T* Get() const noexcept;

    void Swap(SamplePtr& other) noexcept;

    void Reset(std::nullptr_t);

    E2ECheckStatus GetProfileCheckStatus() const noexcept;

private:

    std::unique_ptr<T, std::function<void(T*)>> dataPtr_;
    
    E2ECheckStatus e2eCheckStatus_;
};