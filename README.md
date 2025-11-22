# WITCH
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

WITCH is crossplatform all in one library which provides basic functionality (like libc) for developing thingies. It also contains libraries that supposed to work with WITCH.

## Base features and rules
- stdint.h like types. integers must have s or u in begin like `sint32_t`. floating point data types are `f32_t` `f64_t` `f128_t` `f_t`
- c data types are only allowed if external library uses it.
- `__unreachable();` used if code never gonna reach the place.
- `__unreachable_or(...);` same as `__unreachable();` it expands args if platform or compiler doesnt support `unreachable` hint.
- `__cta(condition);` compile time assert. same as static_assert.
- `__sanit` define means if program gonna use a sanitizer eg valgrind. it will be defined as 0 if WITCH can't find any hint for debug. `__sanit` also can be defined by user.
- `__platform_*` defines. `__platform_unix` is defined for every unix based system including linux. check `internal/WOS.h` for more information about `__platform_*` defines.
- `__compiler_*` defines. currently supported compilers are: `gcc`, `clang`, `tinyc` and `msvc`.
- `__language_*` defines. currently supported languages are: `__language_c` and `__language_cpp`
- `__forceinline` used to force inline a function. inline keyword is not needed.
- `__atomic_*` defines even if compiler doesnt support it. Will give error in compile time if wanted atomic operation is not possible.
- `__restrict_or_nothing` gives restrict keyword if language has it.
- `__abort();` WITCH provides a default implement to abort that can be redefined by `PR` library or by user.
- `__empty_struct` is data type which is struct that doesnt have any members.
- `PRE` used to run code before entry of program comes. not supported in many platforms.
- `WITCH_num_online_cpus()` gives number of available threads. 0 and below is error.

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
