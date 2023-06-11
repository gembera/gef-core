

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
    test_strfuncs.c
    test_utils.c
    test_value.c
    test_json.c
    test_string.c
)

add_executable(all-gef-core-tests ${tests})
target_link_libraries(all-gef-core-tests gef-core)

set(all-gef-core-tests-to-run ${tests})
remove(all-gef-core-tests-to-run all_test.c)

foreach(test ${all-gef-core-tests-to-run})
    get_filename_component(tname ${test} NAME_WE)
    add_test(NAME ${tname} COMMAND all-gef-core-tests ${tname})
endforeach()