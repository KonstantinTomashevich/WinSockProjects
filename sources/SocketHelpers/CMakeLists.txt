add_library(SocketHelpers STATIC ClientSocket.cpp ClientSocket.hpp ServerSocket.cpp ServerSocket.hpp Init.cpp Init.hpp InputMessageBuffer.cpp InputMessageBuffer.hpp OutputMessageBuffer.cpp OutputMessageBuffer.hpp)
target_link_libraries(SocketHelpers Ws2_32)
