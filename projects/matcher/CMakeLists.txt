cmake_minimum_required(VERSION 3.12)

# Basic pattern matcher example
add_executable(pattern_matcher
    pattern_matcher.c
)
target_link_libraries(pattern_matcher PRIVATE librift_static)

# Thread safety demonstration
add_executable(thread_safe_demo
    thread_safe_demo.c
)
target_link_libraries(thread_safe_demo PRIVATE librift_static pthread)

# R'' syntax demonstration
add_executable(rift_syntax_demo
    rift_syntax_demo.c
)
target_link_libraries(rift_syntax_demo PRIVATE librift_static)

# Log analyzer example
add_executable(log_analyzer
    log_analyzer.c
)
target_link_libraries(log_analyzer PRIVATE librift_static)

# Install examples
install(TARGETS pattern_matcher thread_safe_demo rift_syntax_demo log_analyzer
    RUNTIME DESTINATION bin/examples
)