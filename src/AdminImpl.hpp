/*
 * (C) 2020 The University of Chicago
 * 
 * See COPYRIGHT in top-level directory.
 */
#ifndef __YP_ADMIN_IMPL_H
#define __YP_ADMIN_IMPL_H

#include <thallium.hpp>

namespace yp {

namespace tl = thallium;

class AdminImpl {

    public:

    tl::engine           m_engine;
    tl::remote_procedure m_create_phonebook;
    tl::remote_procedure m_open_phonebook;
    tl::remote_procedure m_close_phonebook;
    tl::remote_procedure m_destroy_phonebook;

    AdminImpl(const tl::engine& engine)
    : m_engine(engine)
    , m_create_phonebook(m_engine.define("yp_create_phonebook"))
    , m_open_phonebook(m_engine.define("yp_open_phonebook"))
    , m_close_phonebook(m_engine.define("yp_close_phonebook"))
    , m_destroy_phonebook(m_engine.define("yp_destroy_phonebook"))
    {}

    AdminImpl(margo_instance_id mid)
    : AdminImpl(tl::engine(mid)) {
    }

    ~AdminImpl() {}
};

}

#endif
