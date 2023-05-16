/*
 * (C) 2020 The University of Chicago
 *
 * See COPYRIGHT in top-level directory.
 */
#ifndef __YP_PROVIDER_IMPL_H
#define __YP_PROVIDER_IMPL_H

#include "yp/Backend.hpp"
#include "yp/UUID.hpp"

#include <thallium.hpp>
#include <thallium/serialization/stl/string.hpp>
#include <thallium/serialization/stl/vector.hpp>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <tuple>

#define FIND_PHONEBOOK(__var__) \
        std::shared_ptr<Backend> __var__;\
        do {\
            std::lock_guard<tl::mutex> lock(m_backends_mtx);\
            auto it = m_backends.find(phonebook_id);\
            if(it == m_backends.end()) {\
                result.success() = false;\
                result.error() = "Phonebook with UUID "s + phonebook_id.to_string() + " not found";\
                req.respond(result);\
                spdlog::error("[provider:{}] Phonebook {} not found", id(), phonebook_id.to_string());\
                return;\
            }\
            __var__ = it->second;\
        }while(0)

namespace yp {

using namespace std::string_literals;
namespace tl = thallium;

class ProviderImpl : public tl::provider<ProviderImpl> {

    auto id() const { return get_provider_id(); } // for convenience

    using json = nlohmann::json;

    public:

    tl::engine           m_engine;
    std::string          m_token;
    tl::pool             m_pool;
    // Admin RPC
    tl::remote_procedure m_create_phonebook;
    tl::remote_procedure m_open_phonebook;
    tl::remote_procedure m_close_phonebook;
    tl::remote_procedure m_destroy_phonebook;
    // Client RPC
    tl::remote_procedure m_check_phonebook;
    tl::remote_procedure m_say_hello;
    tl::remote_procedure m_compute_sum;
    // Backends
    std::unordered_map<UUID, std::shared_ptr<Backend>> m_backends;
    tl::mutex m_backends_mtx;

    ProviderImpl(const tl::engine& engine, uint16_t provider_id, const std::string& config, const tl::pool& pool)
    : tl::provider<ProviderImpl>(engine, provider_id)
    , m_engine(engine)
    , m_pool(pool)
    , m_create_phonebook(define("yp_create_phonebook", &ProviderImpl::createPhonebookRPC, pool))
    , m_open_phonebook(define("yp_open_phonebook", &ProviderImpl::openPhonebookRPC, pool))
    , m_close_phonebook(define("yp_close_phonebook", &ProviderImpl::closePhonebookRPC, pool))
    , m_destroy_phonebook(define("yp_destroy_phonebook", &ProviderImpl::destroyPhonebookRPC, pool))
    , m_check_phonebook(define("yp_check_phonebook", &ProviderImpl::checkPhonebookRPC, pool))
    , m_say_hello(define("yp_say_hello", &ProviderImpl::sayHelloRPC, pool))
    , m_compute_sum(define("yp_compute_sum",  &ProviderImpl::computeSumRPC, pool))
    {
        spdlog::trace("[provider:{0}] Registered provider with id {0}", id());
        json json_config;
        try {
            json_config = json::parse(config);
        } catch(json::parse_error& e) {
            spdlog::error("[provider:{}] Could not parse provider configuration: {}",
                    id(), e.what());
            return;
        }
        if(!json_config.is_object()) return;
        if(!json_config.contains("phonebooks")) return;
        auto& phonebooks = json_config["phonebooks"];
        if(!phonebooks.is_array()) return;
        for(auto& phonebook : phonebooks) {
            if(!(phonebook.contains("type") && phonebook["type"].is_string()))
                continue;
            const std::string& phonebook_type = phonebook["type"].get_ref<const std::string&>();
            auto phonebook_config = phonebook.contains("config") ? phonebook["config"] : json::object();
            createPhonebook(phonebook_type, phonebook_config.dump());
        }
    }

    ~ProviderImpl() {
        spdlog::trace("[provider:{}] Deregistering provider", id());
        m_create_phonebook.deregister();
        m_open_phonebook.deregister();
        m_close_phonebook.deregister();
        m_destroy_phonebook.deregister();
        m_check_phonebook.deregister();
        m_say_hello.deregister();
        m_compute_sum.deregister();
        spdlog::trace("[provider:{}]    => done!", id());
    }

    std::string getConfig() const {
        auto config = json::object();
        config["phonebooks"] = json::array();
        for(auto& pair : m_backends) {
            auto phonebook_config = json::object();
            phonebook_config["__id__"] = pair.first.to_string();
            phonebook_config["type"] = pair.second->name();
            phonebook_config["config"] = json::parse(pair.second->getConfig());
            config["phonebooks"].push_back(phonebook_config);
        }
        return config.dump();
    }

    RequestResult<UUID> createPhonebook(const std::string& phonebook_type,
                                       const std::string& phonebook_config) {

        auto phonebook_id = UUID::generate();
        RequestResult<UUID> result;

        json json_config;
        try {
            json_config = json::parse(phonebook_config);
        } catch(json::parse_error& e) {
            result.error() = e.what();
            result.success() = false;
            spdlog::error("[provider:{}] Could not parse phonebook configuration for phonebook {}",
                    id(), phonebook_id.to_string());
            return result;
        }

        std::unique_ptr<Backend> backend;
        try {
            backend = PhonebookFactory::createPhonebook(phonebook_type, get_engine(), json_config);
        } catch(const std::exception& ex) {
            result.success() = false;
            result.error() = ex.what();
            spdlog::error("[provider:{}] Error when creating phonebook {} of type {}:",
                    id(), phonebook_id.to_string(), phonebook_type);
            spdlog::error("[provider:{}]    => {}", id(), result.error());
            return result;
        }

        if(not backend) {
            result.success() = false;
            result.error() = "Unknown phonebook type "s + phonebook_type;
            spdlog::error("[provider:{}] Unknown phonebook type {} for phonebook {}",
                    id(), phonebook_type, phonebook_id.to_string());
            return result;
        } else {
            std::lock_guard<tl::mutex> lock(m_backends_mtx);
            m_backends[phonebook_id] = std::move(backend);
            result.value() = phonebook_id;
        }

        spdlog::trace("[provider:{}] Successfully created phonebook {} of type {}",
                id(), phonebook_id.to_string(), phonebook_type);
        return result;
    }

    void createPhonebookRPC(const tl::request& req,
                           const std::string& token,
                           const std::string& phonebook_type,
                           const std::string& phonebook_config) {

        spdlog::trace("[provider:{}] Received createPhonebook request", id());
        spdlog::trace("[provider:{}]    => type = {}", id(), phonebook_type);
        spdlog::trace("[provider:{}]    => config = {}", id(), phonebook_config);

        RequestResult<UUID> result;

        if(m_token.size() > 0 && m_token != token) {
            result.success() = false;
            result.error() = "Invalid security token";
            req.respond(result);
            spdlog::error("[provider:{}] Invalid security token {}", id(), token);
            return;
        }

        result = createPhonebook(phonebook_type, phonebook_config);

        req.respond(result);
    }

    void openPhonebookRPC(const tl::request& req,
                         const std::string& token,
                         const std::string& phonebook_type,
                         const std::string& phonebook_config) {

        spdlog::trace("[provider:{}] Received openPhonebook request", id());
        spdlog::trace("[provider:{}]    => type = {}", id(), phonebook_type);
        spdlog::trace("[provider:{}]    => config = {}", id(), phonebook_config);

        auto phonebook_id = UUID::generate();
        RequestResult<UUID> result;

        if(m_token.size() > 0 && m_token != token) {
            result.success() = false;
            result.error() = "Invalid security token";
            req.respond(result);
            spdlog::error("[provider:{}] Invalid security token {}", id(), token);
            return;
        }

        json json_config;
        try {
            json_config = json::parse(phonebook_config);
        } catch(json::parse_error& e) {
            result.error() = e.what();
            result.success() = false;
            spdlog::error("[provider:{}] Could not parse phonebook configuration for phonebook {}",
                    id(), phonebook_id.to_string());
            req.respond(result);
            return;
        }

        std::unique_ptr<Backend> backend;
        try {
            backend = PhonebookFactory::openPhonebook(phonebook_type, get_engine(), json_config);
        } catch(const std::exception& ex) {
            result.success() = false;
            result.error() = ex.what();
            spdlog::error("[provider:{}] Error when opening phonebook {} of type {}:",
                    id(), phonebook_id.to_string(), phonebook_type);
            spdlog::error("[provider:{}]    => {}", id(), result.error());
            req.respond(result);
            return;
        }

        if(not backend) {
            result.success() = false;
            result.error() = "Unknown phonebook type "s + phonebook_type;
            spdlog::error("[provider:{}] Unknown phonebook type {} for phonebook {}",
                    id(), phonebook_type, phonebook_id.to_string());
            req.respond(result);
            return;
        } else {
            std::lock_guard<tl::mutex> lock(m_backends_mtx);
            m_backends[phonebook_id] = std::move(backend);
            result.value() = phonebook_id;
        }

        req.respond(result);
        spdlog::trace("[provider:{}] Successfully created phonebook {} of type {}",
                id(), phonebook_id.to_string(), phonebook_type);
    }

    void closePhonebookRPC(const tl::request& req,
                          const std::string& token,
                          const UUID& phonebook_id) {
        spdlog::trace("[provider:{}] Received closePhonebook request for phonebook {}",
                id(), phonebook_id.to_string());

        RequestResult<bool> result;

        if(m_token.size() > 0 && m_token != token) {
            result.success() = false;
            result.error() = "Invalid security token";
            req.respond(result);
            spdlog::error("[provider:{}] Invalid security token {}", id(), token);
            return;
        }

        {
            std::lock_guard<tl::mutex> lock(m_backends_mtx);

            if(m_backends.count(phonebook_id) == 0) {
                result.success() = false;
                result.error() = "Phonebook "s + phonebook_id.to_string() + " not found";
                req.respond(result);
                spdlog::error("[provider:{}] Phonebook {} not found", id(), phonebook_id.to_string());
                return;
            }

            m_backends.erase(phonebook_id);
        }
        req.respond(result);
        spdlog::trace("[provider:{}] Phonebook {} successfully closed", id(), phonebook_id.to_string());
    }

    void destroyPhonebookRPC(const tl::request& req,
                            const std::string& token,
                            const UUID& phonebook_id) {
        RequestResult<bool> result;
        spdlog::trace("[provider:{}] Received destroyPhonebook request for phonebook {}", id(), phonebook_id.to_string());

        if(m_token.size() > 0 && m_token != token) {
            result.success() = false;
            result.error() = "Invalid security token";
            req.respond(result);
            spdlog::error("[provider:{}] Invalid security token {}", id(), token);
            return;
        }

        {
            std::lock_guard<tl::mutex> lock(m_backends_mtx);

            if(m_backends.count(phonebook_id) == 0) {
                result.success() = false;
                result.error() = "Phonebook "s + phonebook_id.to_string() + " not found";
                req.respond(result);
                spdlog::error("[provider:{}] Phonebook {} not found", id(), phonebook_id.to_string());
                return;
            }

            result = m_backends[phonebook_id]->destroy();
            m_backends.erase(phonebook_id);
        }

        req.respond(result);
        spdlog::trace("[provider:{}] Phonebook {} successfully destroyed", id(), phonebook_id.to_string());
    }

    void checkPhonebookRPC(const tl::request& req,
                          const UUID& phonebook_id) {
        spdlog::trace("[provider:{}] Received checkPhonebook request for phonebook {}", id(), phonebook_id.to_string());
        RequestResult<bool> result;
        FIND_PHONEBOOK(phonebook);
        result.success() = true;
        req.respond(result);
        spdlog::trace("[provider:{}] Code successfully executed on phonebook {}", id(), phonebook_id.to_string());
    }

    void sayHelloRPC(const tl::request& req,
                     const UUID& phonebook_id) {
        spdlog::trace("[provider:{}] Received sayHello request for phonebook {}", id(), phonebook_id.to_string());
        RequestResult<bool> result;
        FIND_PHONEBOOK(phonebook);
        phonebook->sayHello();
        spdlog::trace("[provider:{}] Successfully executed sayHello on phonebook {}", id(), phonebook_id.to_string());
    }

    void computeSumRPC(const tl::request& req,
                       const UUID& phonebook_id,
                       int32_t x, int32_t y) {
        spdlog::trace("[provider:{}] Received computeSum request for phonebook {}", id(), phonebook_id.to_string());
        RequestResult<int32_t> result;
        FIND_PHONEBOOK(phonebook);
        result = phonebook->computeSum(x, y);
        req.respond(result);
        spdlog::trace("[provider:{}] Successfully executed computeSum on phonebook {}", id(), phonebook_id.to_string());
    }

};

}

#endif
