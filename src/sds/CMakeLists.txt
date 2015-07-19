PROJECT(sds)


ADD_EXECUTABLE(sds_benchmark  "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
                              "${CMAKE_CURRENT_SOURCE_DIR}/array_method.cpp"
                              "${CMAKE_CURRENT_SOURCE_DIR}/gap_method.cpp"
                              "${CMAKE_CURRENT_SOURCE_DIR}/list_method.cpp"
                              # "${CMAKE_CURRENT_SOURCE_DIR}/line_span_method.cpp"
)
TARGET_LINK_LIBRARIES(sds_benchmark ${LIBS} boost_serialization boost_program_options)