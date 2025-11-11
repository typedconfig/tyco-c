# Tyco C API

Thin C wrapper over the canonical C++ Tyco parser (`../tyco-cpp`). This exposes a stable C ABI so
projects written in C (or other FFI consumers) can parse `.tyco` files without reimplementing the
parser.

## Building

```bash
cd tyco-c
cmake -S . -B build
cmake --build build
```

This builds the shared library `libtyco_c.so` plus a small `tyco_c_tests` executable.

## Testing

The C wrapper reuses the shared `tyco-test-suite`. From `tyco-c`:

```bash
cmake -S . -B build -DTYCO_TEST_SUITE_DIR=../tyco-test-suite
cmake --build build
ctest --test-dir build
```

The default test already assumes the repo layout (`../tyco-test-suite`), so the extra definition is
optional.

## API Overview

tyco_context* ctx = NULL;
tyco_context_free(ctx);
```c
#include "tyco_c.h"

tyco_context* ctx = NULL;
char* error = NULL;


// Parse the bundled example.tyco file
if (tyco_load_file("example.tyco", &ctx, &error) != TYCO_OK) {
    fprintf(stderr, "parse failed: %s\n", error);
    tyco_free_string(error);
    return;
}

// Access global configuration values (as JSON or via C API)
char* globals_json = NULL;
if (tyco_get_globals_json(ctx, &globals_json, &error) == TYCO_OK) {
    printf("Globals: %s\n", globals_json);
    tyco_free_string(globals_json);
} else {
    fprintf(stderr, "globals error: %s\n", error);
    tyco_free_string(error);
}

// Access objects/instances (as JSON or via C API)
char* objects_json = NULL;
if (tyco_get_objects_json(ctx, &objects_json, &error) == TYCO_OK) {
    printf("Objects: %s\n", objects_json);
    tyco_free_string(objects_json);
} else {
    fprintf(stderr, "objects error: %s\n", error);
    tyco_free_string(error);
}

tyco_context_free(ctx);
```

Use `tyco_load_string` to parse from memory. All strings returned by the API must be released with
`tyco_free_string`.
