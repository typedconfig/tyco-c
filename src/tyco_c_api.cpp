#include "tyco_c.h"

#include <cstdlib>
#include <cstring>
#include <memory>
#include <string>
#include <utility>

#include "tyco/parser.h"

struct tyco_context {
    std::shared_ptr<tyco::TycoContext> context;
};

namespace {

char* duplicate_string(const std::string& value) {
    auto* buffer = static_cast<char*>(std::malloc(value.size() + 1));
    if (!buffer) {
        return nullptr;
    }
    std::memcpy(buffer, value.c_str(), value.size() + 1);
    return buffer;
}

void assign_error(const std::string& message, char** out_error) {
    if (!out_error) {
        return;
    }
    *out_error = duplicate_string(message);
}

tyco_context* make_context(std::shared_ptr<tyco::TycoContext> ctx) {
    auto* wrapper = new tyco_context();
    wrapper->context = std::move(ctx);
    return wrapper;
}

} // namespace

extern "C" {

tyco_status tyco_load_file(const char* path, tyco_context** out_ctx, char** out_error) {
    if (!path || !out_ctx) {
        assign_error("Invalid argument: null pointer", out_error);
        return TYCO_ERROR_INVALID_ARGUMENT;
    }
    *out_ctx = nullptr;

    try {
        tyco::TycoLexer lexer;
        auto context = lexer.parse_file(path);
        if (!context) {
            assign_error("Failed to parse file", out_error);
            return TYCO_ERROR_PARSING;
        }
        context->render();
        *out_ctx = make_context(context);
        return TYCO_OK;
    } catch (const std::exception& ex) {
        assign_error(ex.what(), out_error);
        return TYCO_ERROR_PARSING;
    } catch (...) {
        assign_error("Unknown error during parsing", out_error);
        return TYCO_ERROR_INTERNAL;
    }
}

tyco_status tyco_load_string(const char* source, const char* source_name, tyco_context** out_ctx, char** out_error) {
    if (!source || !out_ctx) {
        assign_error("Invalid argument: null pointer", out_error);
        return TYCO_ERROR_INVALID_ARGUMENT;
    }
    *out_ctx = nullptr;

    try {
        tyco::TycoLexer lexer;
        auto context = lexer.parse_string(source, source_name ? source_name : "<string>");
        if (!context) {
            assign_error("Failed to parse content", out_error);
            return TYCO_ERROR_PARSING;
        }
        context->render();
        *out_ctx = make_context(context);
        return TYCO_OK;
    } catch (const std::exception& ex) {
        assign_error(ex.what(), out_error);
        return TYCO_ERROR_PARSING;
    } catch (...) {
        assign_error("Unknown error during parsing", out_error);
        return TYCO_ERROR_INTERNAL;
    }
}

tyco_status tyco_context_to_json(const tyco_context* ctx, char** out_json, char** out_error) {
    if (!ctx || !ctx->context || !out_json) {
        assign_error("Invalid argument: null pointer", out_error);
        return TYCO_ERROR_INVALID_ARGUMENT;
    }
    *out_json = nullptr;

    try {
        auto json = ctx->context->to_json();
        auto* copy = duplicate_string(json);
        if (!copy) {
            assign_error("Out of memory", out_error);
            return TYCO_ERROR_INTERNAL;
        }
        *out_json = copy;
        return TYCO_OK;
    } catch (const std::exception& ex) {
        assign_error(ex.what(), out_error);
        return TYCO_ERROR_INTERNAL;
    }
}

void tyco_context_free(tyco_context* ctx) {
    delete ctx;
}

void tyco_free_string(char* str) {
    std::free(str);
}

} // extern "C"
