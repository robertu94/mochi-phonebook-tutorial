/*
 * (C) 2020 The University of Chicago
 * 
 * See COPYRIGHT in top-level directory.
 */
#include "yp/Exception.hpp"
#include "yp/AsyncRequest.hpp"
#include "AsyncRequestImpl.hpp"

namespace yp {

AsyncRequest::AsyncRequest() = default;

AsyncRequest::AsyncRequest(const std::shared_ptr<AsyncRequestImpl>& impl)
: self(impl) {}

AsyncRequest::AsyncRequest(const AsyncRequest& other) = default;

AsyncRequest::AsyncRequest(AsyncRequest&& other) {
    self = std::move(other.self);
    other.self = nullptr;
}

AsyncRequest::~AsyncRequest() {
    if(self && self.unique()) {
        wait();
    }
}

AsyncRequest& AsyncRequest::operator=(const AsyncRequest& other) {
    if(this == &other || self == other.self) return *this;
    if(self && self.unique()) {
        wait();
    }
    self = other.self;
    return *this;
}

AsyncRequest& AsyncRequest::operator=(AsyncRequest&& other) {
    if(this == &other || self == other.self) return *this;
    if(self && self.unique()) {
        wait();
    }
    self = std::move(other.self);
    other.self = nullptr;
    return *this;
}

void AsyncRequest::wait() const {
    if(not self) throw Exception("Invalid yp::AsyncRequest object");
    if(self->m_waited) return;
    self->m_wait_callback(*self);
    self->m_waited = true;
}

bool AsyncRequest::completed() const {
    if(not self) throw Exception("Invalid yp::AsyncRequest object");
    return self->m_async_response.received();
}

}
