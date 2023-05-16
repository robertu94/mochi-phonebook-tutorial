/*
 * (C) 2020 The University of Chicago
 *
 * See COPYRIGHT in top-level directory.
 */
#include "yp/Client.hpp"
#include "yp/Provider.hpp"
#include "yp/ProviderHandle.hpp"
#include <bedrock/AbstractServiceFactory.hpp>
#include <yokan/provider-handle.h>

namespace tl = thallium;

class YpFactory : public bedrock::AbstractServiceFactory {

    public:

    YpFactory() {}

    void *registerProvider(const bedrock::FactoryArgs &args) override {
        auto it = args.dependencies.find("yokan_ph");
        yk_provider_handle_t yokan_ph =
          it->second.dependencies[0]->getHandle<yk_provider_handle_t>();
        tl::provider_handle ph {
            args.engine, yokan_ph->addr, yokan_ph->provider_id, false
        };
        auto provider = new yp::Provider(args.mid, args.provider_id,
                args.config, tl::pool(args.pool), ph);
        return static_cast<void *>(provider);
    }

    void deregisterProvider(void *p) override {
        auto provider = static_cast<yp::Provider *>(p);
        delete provider;
    }

    std::string getProviderConfig(void *p) override {
        auto provider = static_cast<yp::Provider *>(p);
        return provider->getConfig();
    }

    void *initClient(const bedrock::FactoryArgs& args) override {
        return static_cast<void *>(new yp::Client(args.mid));
    }

    void finalizeClient(void *client) override {
        delete static_cast<yp::Client *>(client);
    }

    std::string getClientConfig(void* c) override {
        auto client = static_cast<yp::Client*>(c);
        return client->getConfig();
    }

    void *createProviderHandle(void *c, hg_addr_t address,
            uint16_t provider_id) override {
        auto client = static_cast<yp::Client *>(c);
        auto ph = new yp::ProviderHandle(
                client->engine(),
                address,
                provider_id,
                false);
        return static_cast<void *>(ph);
    }

    void destroyProviderHandle(void *providerHandle) override {
        auto ph = static_cast<yp::ProviderHandle *>(providerHandle);
        delete ph;
    }

    const std::vector<bedrock::Dependency> &getProviderDependencies() override {
        static const std::vector<bedrock::Dependency> dependency {
            {"yokan_ph", "yokan", BEDROCK_REQUIRED}
        };
        return dependency;
    }

    const std::vector<bedrock::Dependency> &getClientDependencies() override {
        static const std::vector<bedrock::Dependency> no_dependency;
        return no_dependency;
    }
};

BEDROCK_REGISTER_MODULE_FACTORY(yp, YpFactory)
