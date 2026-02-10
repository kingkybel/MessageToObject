# MessageToObjects

A library to enable generation of polymorph objects from message-strings.

# How It Works

The core type is a template `util::generator_map<N, KeyGenerator>`. It stores factory functions keyed by a hash derived
from a message string. At lookup time, the same key generator is applied to the incoming message and used to find the
correct factory.

Template parameters:
- `N`: number of characters used to build the key (limited to `sizeof(size_t)`).
- `KeyGenerator`: a type that can be constructed from `std::string` or `std::string_view` and provides `hash()` returning
  an integral type. The default is `util::prefix_key<N>`.

Key generators in `include/string_to_hash.h` (accept `std::string`, `std::string_view`, `char*`, and `char[]` without
creating a `std::string`):
- `prefix_key<N>`: uses the first `N` characters of the message.
- `postfix_key<N>`: uses the last `N` characters of the message.
- `enumerated_key<Idx...>`: uses characters at the specified zero-based indices, in that order.

Adding your own key generator:
- Provide a type constructible from `std::string` or `std::string_view` that exposes a `hash()` method returning an
  integral type.
- Ensure the internal key fits in an `size_t` if you want to reuse the same map storage and compare
  behavior as the built-in generators.
- Implement any extraction logic you need (prefix, suffix, pattern-based, etc.).

Minimal example:

```c++
struct my_key
{
    explicit my_key(const std::string &str) : hash_(0)
    {
        // Example: use the first 2 chars and the last 2 chars
        char buf[4] = {'\0', '\0', '\0', '\0'};
        if(!str.empty())
        {
            buf[0] = str[0];
            if(str.size() > 1) buf[1] = str[1];
            if(str.size() > 2) buf[2] = str[str.size() - 2];
            if(str.size() > 3) buf[3] = str[str.size() - 1];
        }
        std::memcpy(&hash_, buf, sizeof(buf));
    }

    size_t hash() const { return hash_; }

    size_t hash_;
};

using CustomMap = util::generator_map<4, my_key>;
```

Examples:

```c++
// Default: first 4 characters
using Map = util::generator_map<4>;
Map::registerGenerator("TEST", [](const std::string &msg) { /* ... */ return obj; });
auto obj = Map::get("TEST:payload");

// Last 3 characters
using SuffixMap = util::generator_map<3, util::postfix_key<3>>;
SuffixMap::registerGenerator("ABC-END", [](const std::string &msg) { /* ... */ return obj; });
auto obj2 = SuffixMap::get("XYZ-END");

// Selected indices
using EnumMap = util::generator_map<3, util::enumerated_key<0, 2, 4>>;
EnumMap::registerGenerator("abcdef", [](const std::string &msg) { /* ... */ return obj; });
auto obj3 = EnumMap::get("aXcXeX");
```

# Installation

The installation and build is tested on *ubuntu24.04 LTS*

## dependencies

googletest:

```bash
# create a directory where you like to clone googletest, eg: ~/Repos and change to it
mkdir ~/Repos ; cd ~/Repos
git clone https://github.com/google/googletest.git
mkdir -p googletest/build
cd googletest/build
cmake -Wno-dev -DCMAKE_INSTALL_PREFIX=${INSTALL_PREFIX} ..
cmake --build . --parallel $(nproc)
sudo cmake --install .
```

## use cmake to install the library

```bash
# change the next line to change the install prefix to your liking
INSTALL_PREFIX=/usr
mkdir ./build
cd build
cmake -Wno-dev -DCMAKE_INSTALL_PREFIX=${INSTALL_PREFIX} ..
cmake --build . --parallel $(nproc)
sudo cmake --install .
```

This will install the headers from the include-folder to `${INSTALL_PREFIX}/dkyb`

To use the headers in your code, make sure that `${INSTALL_PREFIX}` is in the include directories of your project.
Include the file in your code e.g:

```c++
#include <dkyb/generator_map.h>
```
