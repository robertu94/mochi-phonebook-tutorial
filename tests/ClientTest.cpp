/*
 * (C) 2020 The University of Chicago
 *
 * See COPYRIGHT in top-level directory.
 */
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_all.hpp>
#include <yp/Client.hpp>
#include <yp/Provider.hpp>
#include <yp/PhonebookHandle.hpp>
#include <yp/Admin.hpp>

static constexpr const char* phonebook_config = "{ \"path\" : \"mydb\" }";
static const std::string phonebook_type = "dummy";

TEST_CASE("Client test", "[client]") {

    auto engine = thallium::engine("na+sm", THALLIUM_SERVER_MODE);
    // Initialize the provider
    yp::Provider provider(engine);
    yp::Admin admin(engine);
    std::string addr = engine.self();
    auto phonebook_id = admin.createPhonebook(addr, 0, phonebook_type, phonebook_config);

    SECTION("Open phonebook") {
        yp::Client client(engine);
        std::string addr = engine.self();

        yp::PhonebookHandle my_phonebook = client.makePhonebookHandle(addr, 0, phonebook_id);
        REQUIRE(static_cast<bool>(my_phonebook));

        auto bad_id = yp::UUID::generate();
        REQUIRE_THROWS_AS(client.makePhonebookHandle(addr, 0, bad_id), yp::Exception);
    }

    admin.destroyPhonebook(addr, 0, phonebook_id);
    engine.finalize();
}
