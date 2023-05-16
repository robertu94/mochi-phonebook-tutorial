/*
 * (C) 2020 The University of Chicago
 *
 * See COPYRIGHT in top-level directory.
 */
#ifndef __YP_ADMIN_HPP
#define __YP_ADMIN_HPP

#include <nlohmann/json.hpp>
#include <thallium.hpp>
#include <string>
#include <memory>
#include <yp/Exception.hpp>
#include <yp/UUID.hpp>

namespace yp {

namespace tl = thallium;

class AdminImpl;

/**
 * @brief Admin interface to a YP service. Enables creating
 * and destroying phonebooks, and attaching and detaching them
 * from a provider. If YP providers have set up a security
 * token, operations from the Admin interface will need this
 * security token.
 */
class Admin {

    public:

    using json = nlohmann::json;

    /**
     * @brief Default constructor.
     */
    Admin();

    /**
     * @brief Constructor using a margo instance id.
     *
     * @param mid Margo instance id.
     */
    Admin(margo_instance_id mid);

    /**
     * @brief Constructor.
     *
     * @param engine Thallium engine.
     */
    Admin(const tl::engine& engine);

    /**
     * @brief Copy constructor.
     */
    Admin(const Admin&);

    /**
     * @brief Move constructor.
     */
    Admin(Admin&&);

    /**
     * @brief Copy-assignment operator.
     */
    Admin& operator=(const Admin&);

    /**
     * @brief Move-assignment operator.
     */
    Admin& operator=(Admin&&);

    /**
     * @brief Destructor.
     */
    ~Admin();

    /**
     * @brief Check if the Admin instance is valid.
     */
    operator bool() const;

    /**
     * @brief Creates a phonebook on the target provider.
     * The config string must be a JSON object acceptable
     * by the desired backend's creation function.
     *
     * @param address Address of the target provider.
     * @param provider_id Provider id.
     * @param type Type of the phonebook to create.
     * @param config JSON configuration for the phonebook.
     */
    UUID createPhonebook(const std::string& address,
                        uint16_t provider_id,
                        const std::string& type,
                        const std::string& config,
                        const std::string& token="") const;

    /**
     * @brief Creates a phonebook on the target provider.
     * The config string must be a JSON object acceptable
     * by the desired backend's creation function.
     *
     * @param address Address of the target provider.
     * @param provider_id Provider id.
     * @param type Type of the phonebook to create.
     * @param config JSON configuration for the phonebook.
     */
    UUID createPhonebook(const std::string& address,
                        uint16_t provider_id,
                        const std::string& type,
                        const char* config,
                        const std::string& token="") const {
        return createPhonebook(address, provider_id, type, std::string(config), token);
    }

    /**
     * @brief Creates a phonebook on the target provider.
     * The config object must be a JSON object acceptable
     * by the desired backend's creation function.
     *
     * @param address Address of the target provider.
     * @param provider_id Provider id.
     * @param type Type of the phonebook to create.
     * @param config JSON configuration for the phonebook.
     */
    UUID createPhonebook(const std::string& address,
                        uint16_t provider_id,
                        const std::string& type,
                        const json& config,
                        const std::string& token="") const {
        return createPhonebook(address, provider_id, type, config.dump(), token);
    }

    /**
     * @brief Opens an existing phonebook in the target provider.
     * The config string must be a JSON object acceptable
     * by the desired backend's open function.
     *
     * @param address Address of the target provider.
     * @param provider_id Provider id.
     * @param type Type of the phonebook to create.
     * @param config JSON configuration for the phonebook.
     */
    UUID openPhonebook(const std::string& address,
                      uint16_t provider_id,
                      const std::string& type,
                      const std::string& config,
                      const std::string& token="") const;

    /**
     * @brief Opens an existing phonebook to the target provider.
     * The config object must be a JSON object acceptable
     * by the desired backend's open function.
     *
     * @param address Address of the target provider.
     * @param provider_id Provider id.
     * @param type Type of the phonebook to create.
     * @param config JSON configuration for the phonebook.
     */
    UUID openPhonebook(const std::string& address,
                      uint16_t provider_id,
                      const std::string& type,
                      const json& config,
                      const std::string& token="") const {
        return openPhonebook(address, provider_id, type, config.dump(), token);
    }

    /**
     * @brief Closes an open phonebook in the target provider.
     *
     * @param address Address of the target provider.
     * @param provider_id Provider id.
     * @param phonebook_id UUID of the phonebook to close.
     */
    void closePhonebook(const std::string& address,
                        uint16_t provider_id,
                        const UUID& phonebook_id,
                        const std::string& token="") const;

    /**
     * @brief Destroys an open phonebook in the target provider.
     *
     * @param address Address of the target provider.
     * @param provider_id Provider id.
     * @param phonebook_id UUID of the phonebook to destroy.
     */
    void destroyPhonebook(const std::string& address,
                         uint16_t provider_id,
                         const UUID& phonebook_id,
                         const std::string& token="") const;

    /**
     * @brief Shuts down the target server. The Thallium engine
     * used by the server must have remote shutdown enabled.
     *
     * @param address Address of the server to shut down.
     */
    void shutdownServer(const std::string& address) const;

    private:

    std::shared_ptr<AdminImpl> self;
};

}

#endif
