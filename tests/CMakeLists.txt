add_executable(mee-test
    src/ecs-test.cpp
)

target_link_libraries(mee-test PRIVATE mee)

add_test(NAME ecs-tests COMMAND mee-test)
