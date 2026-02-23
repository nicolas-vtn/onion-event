add_executable(onion_event_demo
    "event_demo.cpp"
)

target_link_libraries(onion_event_demo
    PRIVATE
        onion::event
)

target_compile_features(onion_event_demo PRIVATE cxx_std_20)

set_target_properties(onion_event_demo PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)
