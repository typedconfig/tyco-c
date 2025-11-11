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

```c
#include "tyco_c.h"

tyco_context* ctx = NULL;
char* error = NULL;

if (tyco_load_file("config.tyco", &ctx, &error) != TYCO_OK) {
    fprintf(stderr, "parse failed: %s\n", error);
    tyco_free_string(error);
    return;
}

char* json = NULL;
if (tyco_context_to_json(ctx, &json, &error) == TYCO_OK) {
    printf("%s\n", json);
    tyco_free_string(json);
} else {
    fprintf(stderr, "serialize failed: %s\n", error);
    tyco_free_string(error);
}

tyco_context_free(ctx);
```

Use `tyco_load_string` to parse from memory. All strings returned by the API must be released with
`tyco_free_string`.
