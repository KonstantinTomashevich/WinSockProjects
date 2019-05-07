add_executable(SimpleMailServer Main.cpp Mail.hpp Mail.cpp)
target_link_libraries(SimpleMailServer SocketHelpers)
