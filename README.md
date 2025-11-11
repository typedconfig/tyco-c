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

## Quick Start

Every binding ships the canonical sample configuration as `tyco/example.tyco`
([view on GitHub](https://github.com/typedconfig/tyco-c/blob/main/tyco/example.tyco)).
You can load it directly with the C API:

```c
#include "tyco_c.h"
#include <stdio.h>

int main(void) {
    tyco_context* ctx = NULL;
    char* error = NULL;

    if (tyco_load_file("tyco/example.tyco", &ctx, &error) != TYCO_OK) {
        fprintf(stderr, "parse failed: %s\n", error);
        tyco_free_string(error);
        return 1;
    }

    char* json = NULL;
    if (tyco_context_to_json(ctx, &json, &error) == TYCO_OK) {
        printf("Full document:\n%s\n", json);
        tyco_free_string(json);
    } else {
        fprintf(stderr, "json error: %s\n", error);
        tyco_free_string(error);
    }

    tyco_context_free(ctx);
    return 0;
}
```

Use `tyco_load_string` to parse from memory. All strings returned by the API must be released with
`tyco_free_string`. Once you have the canonical JSON payload you can materialise it with any JSON
library to access globals (`environment`, `debug`, `timeout`) plus the `Database` and `Server`
arrays exactly like the Python example.

### Example Tyco File

```
tyco/example.tyco
```

```tyco
# Global configuration with type annotations
str environment: production
bool debug: false
int timeout: 30

# Database configuration struct
Database:
 *str name:           # Primary key field (*)
  str host:
  int port:
  str connection_string:
  # Instances
  - primary, localhost,    5432, "postgresql://localhost:5432/myapp"
  - replica, replica-host, 5432, "postgresql://replica-host:5432/myapp"

# Server configuration struct  
Server:
 *str name:           # Primary key for referencing
  int port:
  str host:
  ?str description:   # Nullable field (?) - can be null
  # Server instances
  - web1,    8080, web1.example.com,    description: "Primary web server"
  - api1,    3000, api1.example.com,    description: null
  - worker1, 9000, worker1.example.com, description: "Worker number 1"

# Feature flags array
str[] features: [auth, analytics, caching]
```
