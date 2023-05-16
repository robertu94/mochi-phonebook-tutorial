/*
 * (C) 2020 The University of Chicago
 *
 * See COPYRIGHT in top-level directory.
 */
#include "DummyBackend.hpp"
#include <iostream>

YP_REGISTER_BACKEND(dummy, DummyPhonebook);

DummyPhonebook::DummyPhonebook(thallium::engine engine, const json& config)
: m_engine(std::move(engine)),
  m_config(config) {

}

void DummyPhonebook::sayHello() {
    std::cout << "Hello World" << std::endl;
}

std::string DummyPhonebook::getConfig() const {
    return m_config.dump();
}

yp::RequestResult<int32_t> DummyPhonebook::computeSum(int32_t x, int32_t y) {
    yp::RequestResult<int32_t> result;
    result.value() = x + y;
    return result;
}

yp::RequestResult<bool> DummyPhonebook::destroy() {
    yp::RequestResult<bool> result;
    result.value() = true;
    // or result.success() = true
    return result;
}

std::unique_ptr<yp::Backend> DummyPhonebook::create(const thallium::engine& engine, const json& config) {
    (void)engine;
    return std::unique_ptr<yp::Backend>(new DummyPhonebook(engine, config));
}

std::unique_ptr<yp::Backend> DummyPhonebook::open(const thallium::engine& engine, const json& config) {
    (void)engine;
    return std::unique_ptr<yp::Backend>(new DummyPhonebook(engine, config));
}
