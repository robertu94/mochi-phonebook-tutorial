/*
 * (C) 2020 The University of Chicago
 *
 * See COPYRIGHT in top-level directory.
 */
#include <yp/Admin.hpp>
#include <yp/Provider.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_all.hpp>

static const std::string phonebook_type = "dummy";
static constexpr const char* phonebook_config = "{ \"path\" : \"mydb\" }";

TEST_CASE("Admin tests", "[admin]") {

    auto engine = thallium::engine("na+sm", THALLIUM_SERVER_MODE);
    // Initialize the provider
    yp::Provider provider(engine);

    SECTION("Create an admin") {
        yp::Admin admin(engine);
        std::string addr = engine.self();

        SECTION("Create and destroy phonebooks") {
            yp::UUID phonebook_id = admin.createPhonebook(addr, 0, phonebook_type, phonebook_config);

            REQUIRE_THROWS_AS(admin.createPhonebook(addr, 0, "blabla", phonebook_config),
                              yp::Exception);

            admin.destroyPhonebook(addr, 0, phonebook_id);

            yp::UUID bad_id;
            REQUIRE_THROWS_AS(admin.destroyPhonebook(addr, 0, bad_id), yp::Exception);
        }
    }
    // Finalize the engine
    engine.finalize();
}
