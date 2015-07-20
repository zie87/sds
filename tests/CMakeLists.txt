PROJECT(sds)

SET(TEST_LIBS ${LIBS} "unittest" "pthread")

ADD_EXECUTABLE(sds_tests  "${CMAKE_CURRENT_SOURCE_DIR}/sds/piece_chain_tests.cpp"


                          "${CMAKE_CURRENT_SOURCE_DIR}/sds/chain.cpp"
                          "${CMAKE_CURRENT_SOURCE_DIR}/sds/redesign_tests.cpp"
)
TARGET_LINK_LIBRARIES(sds_tests ${TEST_LIBS} sds)

ADD_TEST( NAME sds_tests WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} COMMAND "sds_tests" )
ADD_DEPENDENCIES(unittest_modules sds_tests)