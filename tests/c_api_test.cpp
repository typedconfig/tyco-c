#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <nlohmann/json.hpp>

extern "C" {
#include "tyco_c.h"
}

static std::string read_file(const std::string& path) {
    std::ifstream in(path);
    if (!in.is_open()) {
        return {};
    }
    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

int main() {
#ifdef TYCO_TEST_SUITE_DIR
    const std::string suite_root = TYCO_TEST_SUITE_DIR;
#else
    const std::string suite_root = "../tyco-test-suite";
#endif
    const std::string inputs_dir = suite_root + "/inputs";
    const std::string expected_dir = suite_root + "/expected";

    std::error_code ec;
    for (const auto& entry : std::filesystem::directory_iterator(inputs_dir, ec)) {
        if (ec) {
            std::cerr << "Directory iteration failed: " << ec.message() << std::endl;
            return 1;
        }
        if (!entry.is_regular_file()) {
            continue;
        }
        const auto path = entry.path();
        if (path.extension() != ".tyco") {
            continue;
        }

        const auto expected_path = expected_dir + "/" + path.stem().string() + ".json";
        if (!std::filesystem::exists(expected_path)) {
            continue;
        }

        tyco_context* ctx = nullptr;
        char* error = nullptr;
        if (tyco_load_file(path.string().c_str(), &ctx, &error) != TYCO_OK) {
            std::cerr << "Failed to parse " << path << ": " << (error ? error : "unknown error") << std::endl;
            tyco_free_string(error);
            return 1;
        }

        char* json_cstr = nullptr;
        if (tyco_context_dumps_json(ctx, &json_cstr, &error) != TYCO_OK) {
            std::cerr << "Failed to serialize " << path << ": " << (error ? error : "unknown error") << std::endl;
            tyco_free_string(error);
            tyco_context_free(ctx);
            return 1;
        }

        const std::string json_str = json_cstr ? json_cstr : "";
        tyco_free_string(json_cstr);

        const std::string expected_str = read_file(expected_path);
        if (expected_str.empty()) {
            std::cerr << "Unable to read expected JSON file for " << path << std::endl;
            tyco_context_free(ctx);
            return 1;
        }

        try {
            const auto actual_json = nlohmann::json::parse(json_str);
            const auto expected_json = nlohmann::json::parse(expected_str);

            if (actual_json != expected_json) {
                std::cerr << "JSON mismatch for " << path.filename() << "\nExpected:\n"
                          << expected_json.dump(2) << "\nActual:\n"
                          << actual_json.dump(2) << std::endl;
                tyco_context_free(ctx);
                return 1;
            }
        } catch (const std::exception& ex) {
            std::cerr << "JSON parsing failed for " << path.filename() << ": " << ex.what() << std::endl;
            tyco_context_free(ctx);
            return 1;
        }

        tyco_context_free(ctx);
    }

    return 0;
}
