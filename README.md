# Onion Event

A lightweight, header-only, thread-safe C++20 event system.

It provides a simple way to subscribe, unsubscribe, and trigger strongly-typed events with automatic cleanup when subscription handles go out of scope.

---

## Features

* Header-only (INTERFACE library)
* Thread-safe
* C++20 compliant
* No external dependencies

---

## Adding to Your CMake Project

If the library is included in your repository:

```cmake
add_subdirectory(libs/event)

target_link_libraries(your_target
    PRIVATE
        onion_event
)
```

---

## Basic Usage

```cpp
#include <Event.hpp>

onion::Event<MyEventArgs> event;

auto handle = event.Subscribe([](const MyEventArgs& args)
{
    std::cout << args.value << std::endl;
});

event.Trigger(MyEventArgs{42});

// Optional manual unsubscribe
event.Unsubscribe(handle);

// Or automatic unsubscribe when handle goes out of scope
```


---


## Disable Demo

Disable demo:

```bash
cmake -DONION_BUILD_DEMO=OFF ..
```

---

## Design Notes

* Subscriptions are represented by `EventHandle` tokens.
* When a token is destroyed, the associated handler is automatically ignored.
* Expired handles are cleaned up lazily.
* Events can be stack-allocated.

---
