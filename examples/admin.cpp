/*
 * (C) 2020 The University of Chicago
 *
 * See COPYRIGHT in top-level directory.
 */
#include <yp/Admin.hpp>
#include <spdlog/spdlog.h>
#include <tclap/CmdLine.h>
#include <iostream>
#include <vector>

namespace tl = thallium;

static std::string g_address;
static std::string g_protocol;
static std::string g_phonebook;
static std::string g_type;
static std::string g_config;
static std::string g_token;
static std::string g_operation;
static unsigned    g_provider_id;
static std::string g_log_level = "info";

static void parse_command_line(int argc, char** argv);

int main(int argc, char** argv) {
    parse_command_line(argc, argv);
    spdlog::set_level(spdlog::level::from_str(g_log_level));

    // Initialize the thallium server
    tl::engine engine(g_protocol, THALLIUM_CLIENT_MODE);

    try {

        // Initialize an Admin
        yp::Admin admin(engine);

        if(g_operation == "create") {
            auto id = admin.createPhonebook(g_address, g_provider_id,
                g_type, g_config, g_token);
            spdlog::info("Created phonebook {}", id.to_string());
        } else if(g_operation == "open") {
            auto id = admin.openPhonebook(g_address, g_provider_id,
                g_type, g_config, g_token);
            spdlog::info("Opened phonebook {}", id.to_string());
        } else if(g_operation == "close") {
            admin.closePhonebook(g_address, g_provider_id,
                yp::UUID::from_string(g_phonebook.c_str()), g_token);
            spdlog::info("Closed phonebook {}", g_phonebook);
        } else if(g_operation == "destroy") {
            admin.destroyPhonebook(g_address, g_provider_id,
                yp::UUID::from_string(g_phonebook.c_str()), g_token);
            spdlog::info("Destroyed phonebook {}", g_phonebook);
        }

        // Any of the above functions may throw a yp::Exception
    } catch(const yp::Exception& ex) {
        std::cerr << ex.what() << std::endl;
        exit(-1);
    }

    return 0;
}

void parse_command_line(int argc, char** argv) {
    try {
        TCLAP::CmdLine cmd("Yp admin", ' ', "0.1");
        TCLAP::ValueArg<std::string> addressArg("a","address","Address or server", true,"","string");
        TCLAP::ValueArg<unsigned>    providerArg("p", "provider", "Provider id to contact (default 0)", false, 0, "int");
        TCLAP::ValueArg<std::string> tokenArg("s","token","Security token", false,"","string");
        TCLAP::ValueArg<std::string> typeArg("t","type","Phonebook type", false,"dummy","string");
        TCLAP::ValueArg<std::string> phonebookArg("r","phonebook","Phonebook id", false, yp::UUID().to_string(),"string");
        TCLAP::ValueArg<std::string> configArg("c","config","Phonebook configuration", false,"","string");
        TCLAP::ValueArg<std::string> logLevel("v","verbose", "Log level (trace, debug, info, warning, error, critical, off)", false, "info", "string");
        std::vector<std::string> options = { "create", "open", "close", "destroy" };
        TCLAP::ValuesConstraint<std::string> allowedOptions(options);
        TCLAP::ValueArg<std::string> operationArg("x","exec","Operation to execute",true,"create",&allowedOptions);
        cmd.add(addressArg);
        cmd.add(providerArg);
        cmd.add(typeArg);
        cmd.add(tokenArg);
        cmd.add(configArg);
        cmd.add(phonebookArg);
        cmd.add(logLevel);
        cmd.add(operationArg);
        cmd.parse(argc, argv);
        g_address = addressArg.getValue();
        g_provider_id = providerArg.getValue();
        g_token = tokenArg.getValue();
        g_config = configArg.getValue();
        g_type = typeArg.getValue();
        g_phonebook = phonebookArg.getValue();
        g_operation = operationArg.getValue();
        g_log_level = logLevel.getValue();
        g_protocol = g_address.substr(0, g_address.find(":"));
    } catch(TCLAP::ArgException &e) {
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
        exit(-1);
    }
}
