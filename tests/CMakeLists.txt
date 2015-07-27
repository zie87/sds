PROJECT(sds)

SET(TEST_LIBS ${LIBS} "unittest" "pthread")

ADD_EXECUTABLE(sds_tests  "${CMAKE_CURRENT_SOURCE_DIR}/sds/piece_chain_tests.cpp" 
                          "${CMAKE_CURRENT_SOURCE_DIR}/sds/rope_tests.cpp"

                          "${CMAKE_CURRENT_SOURCE_DIR}/sds/rope.cpp"
)
TARGET_LINK_LIBRARIES(sds_tests ${TEST_LIBS} sds)

ADD_TEST( NAME sds_tests WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} COMMAND "sds_tests" )
ADD_DEPENDENCIES(unittest_modules sds_tests)









ADD_EXECUTABLE(rope_example  "${CMAKE_CURRENT_SOURCE_DIR}/examples/rope_main.cpp"
                             "${CMAKE_CURRENT_SOURCE_DIR}/examples/rope.cpp"
)

ADD_EXECUTABLE(pc_comparsion  "${CMAKE_CURRENT_SOURCE_DIR}/comparsion/piece_chain_compare.cpp"
)
TARGET_LINK_LIBRARIES(pc_comparsion ${LIBS} sds neatpad_pc)