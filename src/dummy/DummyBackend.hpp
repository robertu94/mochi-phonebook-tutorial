/*
 * (C) 2020 The University of Chicago
 *
 * See COPYRIGHT in top-level directory.
 */
#ifndef __DUMMY_BACKEND_HPP
#define __DUMMY_BACKEND_HPP

#include <yp/Backend.hpp>

using json = nlohmann::json;

/**
 * Dummy implementation of an yp Backend.
 */
class DummyPhonebook : public yp::Backend {

    thallium::engine m_engine;
    json             m_config;
    std::unordered_map<std::string, uint64_t> numbers;

    public:

    /**
     * @brief Constructor.
     */
    DummyPhonebook(thallium::engine engine, const json& config);

    /**
     * @brief Move-constructor.
     */
    DummyPhonebook(DummyPhonebook&&) = default;

    /**
     * @brief Copy-constructor.
     */
    DummyPhonebook(const DummyPhonebook&) = default;

    /**
     * @brief Move-assignment operator.
     */
    DummyPhonebook& operator=(DummyPhonebook&&) = default;

    /**
     * @brief Copy-assignment operator.
     */
    DummyPhonebook& operator=(const DummyPhonebook&) = default;

    /**
     * @brief Destructor.
     */
    virtual ~DummyPhonebook() = default;

    /**
     * @brief Get the phonebook's configuration as a JSON-formatted string.
     */
    std::string getConfig() const override;

    /**
     * @brief Prints Hello World.
     */
    void sayHello() override;

    /**
     * @brief Compute the sum of two integers.
     *
     * @param x first integer
     * @param y second integer
     *
     * @return a RequestResult containing the result.
     */
    yp::RequestResult<int32_t> computeSum(int32_t x, int32_t y) override;

    /**
     * @brief Destroys the underlying phonebook.
     *
     * @return a RequestResult<bool> instance indicating
     * whether the database was successfully destroyed.
     */
    yp::RequestResult<bool> destroy() override;

    /**
     * @brief Static factory function used by the PhonebookFactory to
     * create a DummyPhonebook.
     *
     * @param engine Thallium engine
     * @param config JSON configuration for the phonebook
     *
     * @return a unique_ptr to a phonebook
     */
    static std::unique_ptr<yp::Backend> create(const thallium::engine& engine, const json& config);

    /**
     * @brief Static factory function used by the PhonebookFactory to
     * open a DummyPhonebook.
     *
     * @param engine Thallium engine
     * @param config JSON configuration for the phonebook
     *
     * @return a unique_ptr to a phonebook
     */
    static std::unique_ptr<yp::Backend> open(const thallium::engine& engine, const json& config);


    /**
     * @brief insert a phone number
     *
     * @param name 
     * @param phone
     *
     * @return a RequestResult containing the result.
     */
    yp::RequestResult<uint32_t> insert(std::string const& name, uint64_t phone) override;


    /**
     * @brief insert a phone number
     *
     * @param name 
     * @param phone
     *
     * @return a RequestResult containing the result.
     */
    yp::RequestResult<uint64_t> lookup(std::string const& name) override;

};

#endif
