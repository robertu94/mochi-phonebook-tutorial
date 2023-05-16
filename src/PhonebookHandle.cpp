/*
 * (C) 2020 The University of Chicago
 * 
 * See COPYRIGHT in top-level directory.
 */
#include "yp/PhonebookHandle.hpp"
#include "yp/RequestResult.hpp"
#include "yp/Exception.hpp"

#include "AsyncRequestImpl.hpp"
#include "ClientImpl.hpp"
#include "PhonebookHandleImpl.hpp"

#include <thallium/serialization/stl/string.hpp>
#include <thallium/serialization/stl/pair.hpp>

namespace yp {

PhonebookHandle::PhonebookHandle() = default;

PhonebookHandle::PhonebookHandle(const std::shared_ptr<PhonebookHandleImpl>& impl)
: self(impl) {}

PhonebookHandle::PhonebookHandle(const PhonebookHandle&) = default;

PhonebookHandle::PhonebookHandle(PhonebookHandle&&) = default;

PhonebookHandle& PhonebookHandle::operator=(const PhonebookHandle&) = default;

PhonebookHandle& PhonebookHandle::operator=(PhonebookHandle&&) = default;

PhonebookHandle::~PhonebookHandle() = default;

PhonebookHandle::operator bool() const {
    return static_cast<bool>(self);
}

Client PhonebookHandle::client() const {
    return Client(self->m_client);
}

void PhonebookHandle::sayHello() const {
    if(not self) throw Exception("Invalid yp::PhonebookHandle object");
    auto& rpc = self->m_client->m_say_hello;
    auto& ph  = self->m_ph;
    auto& phonebook_id = self->m_phonebook_id;
    rpc.on(ph)(phonebook_id);
}

void PhonebookHandle::computeSum(
        int32_t x, int32_t y,
        int32_t* result,
        AsyncRequest* req) const
{
    if(not self) throw Exception("Invalid yp::PhonebookHandle object");
    auto& rpc = self->m_client->m_compute_sum;
    auto& ph  = self->m_ph;
    auto& phonebook_id = self->m_phonebook_id;
    if(req == nullptr) { // synchronous call
        RequestResult<int32_t> response = rpc.on(ph)(phonebook_id, x, y);
        if(response.success()) {
            if(result) *result = response.value();
        } else {
            throw Exception(response.error());
        }
    } else { // asynchronous call
        auto async_response = rpc.on(ph).async(phonebook_id, x, y);
        auto async_request_impl =
            std::make_shared<AsyncRequestImpl>(std::move(async_response));
        async_request_impl->m_wait_callback =
            [result](AsyncRequestImpl& async_request_impl) {
                RequestResult<int32_t> response =
                    async_request_impl.m_async_response.wait();
                    if(response.success()) {
                        if(result) *result = response.value();
                    } else {
                        throw Exception(response.error());
                    }
            };
        *req = AsyncRequest(std::move(async_request_impl));
    }
}

void PhonebookHandle::insert(std::string const& name, uint64_t const phone, uint32_t* result, AsyncRequest* req) {
    if(not self) throw Exception("Invalid yp::PhonebookHandle object");
    auto& rpc = self->m_client->m_compute_sum;
    auto& ph  = self->m_ph;
    auto& phonebook_id = self->m_phonebook_id;
    if(req == nullptr) { // synchronous call
        RequestResult<int32_t> response = rpc.on(ph)(phonebook_id, name, phone);
        if(response.success()) {
            if(result) *result = response.value();
        } else {
            throw Exception(response.error());
        }
    } else { // asynchronous call
        auto async_response = rpc.on(ph).async(phonebook_id, name, phone);
        auto async_request_impl =
            std::make_shared<AsyncRequestImpl>(std::move(async_response));
        async_request_impl->m_wait_callback =
            [result](AsyncRequestImpl& async_request_impl) {
                RequestResult<int32_t> response =
                    async_request_impl.m_async_response.wait();
                    if(response.success()) {
                        if(result) *result = response.value();
                    } else {
                        throw Exception(response.error());
                    }
            };
        *req = AsyncRequest(std::move(async_request_impl));
    }
}

void PhonebookHandle::lookup(std::string const& name, uint64_t* result, AsyncRequest* req) {
    if(not self) throw Exception("Invalid yp::PhonebookHandle object");
    auto& rpc = self->m_client->m_compute_sum;
    auto& ph  = self->m_ph;
    auto& phonebook_id = self->m_phonebook_id;
    if(req == nullptr) { // synchronous call
        RequestResult<int32_t> response = rpc.on(ph)(phonebook_id, name);
        if(response.success()) {
            if(result) *result = response.value();
        } else {
            throw Exception(response.error());
        }
    } else { // asynchronous call
        auto async_response = rpc.on(ph).async(phonebook_id, name);
        auto async_request_impl =
            std::make_shared<AsyncRequestImpl>(std::move(async_response));
        async_request_impl->m_wait_callback =
            [result](AsyncRequestImpl& async_request_impl) {
                RequestResult<int32_t> response =
                    async_request_impl.m_async_response.wait();
                    if(response.success()) {
                        if(result) *result = response.value();
                    } else {
                        throw Exception(response.error());
                    }
            };
        *req = AsyncRequest(std::move(async_request_impl));
    }
}



}
