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
    if (tyco_context_dumps_json(ctx, &json, &error) == TYCO_OK) {
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
library to access globals (`timezone`) plus the `Application`, `Host`, and `Port`
arrays exactly like the Python example.

### Example Tyco File

```
tyco/example.tyco
```

```tyco
str timezone: UTC  # this is a global config setting

Application:       # schema defined first, followed by instance creation
  str service:
  str profile:
  str command: start_app {service}.{profile} -p {port.number}
  Host host:
  Port port: Port(http_web)  # reference to Port instance defined below
  - service: webserver, profile: primary, host: Host(prod-01-us)
  - service: webserver, profile: backup,  host: Host(prod-02-us)
  - service: database,  profile: mysql,   host: Host(prod-02-us), port: Port(http_mysql)

Host:
 *str hostname:  # star character (*) used as reference primary key
  int cores:
  bool hyperthreaded: true
  str os: Debian
  - prod-01-us, cores: 64, hyperthreaded: false
  - prod-02-us, cores: 32, os: Fedora

Port:
 *str name:
  int number:
  - http_web,   80  # can skip field keys when obvious
  - http_mysql, 3306
```
