PROJECT(sds)

ADD_LIBRARY(sds SHARED  "${CMAKE_CURRENT_SOURCE_DIR}/array_method.cpp"
                        "${CMAKE_CURRENT_SOURCE_DIR}/gap_method.cpp"
                        "${CMAKE_CURRENT_SOURCE_DIR}/list_method.cpp"
                        "${CMAKE_CURRENT_SOURCE_DIR}/piece_chain_method.cpp"
)

ADD_EXECUTABLE(sds_benchmark  "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")
TARGET_LINK_LIBRARIES(sds_benchmark ${LIBS} sds boost_serialization boost_program_options)