

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../inc ${CMAKE_CURRENT_BINARY_DIR})

create_test_sourcelist(tests
    all_test.c
    test_mem.c
    test_list.c
    test_array.c
    test_map.c
    test_event.c
    test_auto.c
    test_stream.c
)

add_executable(all-gef-tests ${tests})
target_link_libraries(all-gef-tests gef-core)

set(all-tests-to-run ${tests})
remove(all-tests-to-run all_test.c)

foreach(test ${all-tests-to-run})
    get_filename_component(tname ${test} NAME_WE)
    add_test(NAME ${tname} COMMAND all-gef-tests ${tname})
endforeach()