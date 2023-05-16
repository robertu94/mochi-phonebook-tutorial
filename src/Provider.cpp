/*
 * (C) 2020 The University of Chicago
 *
 * See COPYRIGHT in top-level directory.
 */
#include "yp/Provider.hpp"

#include "ProviderImpl.hpp"

#include <thallium/serialization/stl/string.hpp>

namespace tl = thallium;

namespace yp {

Provider::Provider(const tl::engine& engine, const tl::provider_handle& yokan_ph, uint16_t provider_id, const std::string& config, const tl::pool& p)
: self(std::make_shared<ProviderImpl>(engine, yokan_ph, provider_id, config, p)) {
    self->get_engine().push_finalize_callback(this, [p=this]() { p->self.reset(); });
}

Provider::Provider(margo_instance_id mid, const tl::provider_handle& yokan_ph, uint16_t provider_id, const std::string& config, const tl::pool& p)
: self(std::make_shared<ProviderImpl>(mid, yokan_ph, provider_id, config, p)) {
    self->get_engine().push_finalize_callback(this, [p=this]() { p->self.reset(); });
}

Provider::Provider(Provider&& other) {
    other.self->get_engine().pop_finalize_callback(this);
    self = std::move(other.self);
    self->get_engine().push_finalize_callback(this, [p=this]() { p->self.reset(); });
}

Provider::~Provider() {
    if(self) {
        self->get_engine().pop_finalize_callback(this);
    }
}

std::string Provider::getConfig() const {
    return self ? self->getConfig() : "{}";
}

Provider::operator bool() const {
    return static_cast<bool>(self);
}

void Provider::setSecurityToken(const std::string& token) {
    if(self) self->m_token = token;
}

}
