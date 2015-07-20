PROJECT(sds)

ADD_LIBRARY(neatpad_pc SHARED  "${CMAKE_CURRENT_SOURCE_DIR}/linux_port/buffer_control.cpp"
                               "${CMAKE_CURRENT_SOURCE_DIR}/linux_port/piece_chain.cpp"
)

ADD_EXECUTABLE(neatpad_pc_test    "${CMAKE_CURRENT_SOURCE_DIR}/linux_port/main.cpp")
TARGET_LINK_LIBRARIES(neatpad_pc_test ${LIBS} neatpad_pc)