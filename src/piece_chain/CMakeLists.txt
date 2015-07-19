PROJECT(sds)


ADD_EXECUTABLE(piece_chain    "${CMAKE_CURRENT_SOURCE_DIR}/linux_port/main.cpp"
                              "${CMAKE_CURRENT_SOURCE_DIR}/linux_port/buffer_control.cpp"
                              "${CMAKE_CURRENT_SOURCE_DIR}/linux_port/piece_chain.cpp"
)
TARGET_LINK_LIBRARIES(piece_chain ${LIBS})