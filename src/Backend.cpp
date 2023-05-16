/*
 * (C) 2020 The University of Chicago
 * 
 * See COPYRIGHT in top-level directory.
 */
#include "yp/Backend.hpp"

namespace tl = thallium;

namespace yp {

using json = nlohmann::json;

std::unordered_map<std::string,
                std::function<std::unique_ptr<Backend>(const tl::engine&, const json&)>> PhonebookFactory::create_fn;

std::unordered_map<std::string,
                std::function<std::unique_ptr<Backend>(const tl::engine&, const json&)>> PhonebookFactory::open_fn;

std::unique_ptr<Backend> PhonebookFactory::createPhonebook(const std::string& backend_name,
                                                         const tl::engine& engine,
                                                         const json& config) {
    auto it = create_fn.find(backend_name);
    if(it == create_fn.end()) return nullptr;
    auto& f = it->second;
    return f(engine, config);
}

std::unique_ptr<Backend> PhonebookFactory::openPhonebook(const std::string& backend_name,
                                                       const tl::engine& engine,
                                                       const json& config) {
    auto it = open_fn.find(backend_name);
    if(it == open_fn.end()) return nullptr;
    auto& f = it->second;
    return f(engine, config);
}

}
