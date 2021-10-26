cmake_minimum_required(VERSION 3.20)
project(asio_basic_client_k)

set(CMAKE_CXX_STANDARD 14)

set(Boost_INCLUDE_DIR /opt/homebrew/Cellar/boost/1.76.0/include)
set(Boost_LIBRARY_DIR /opt/homebrew/Cellar/boost/1.76.0/lib)
set(BOOST_ROOT /opt/homebrew/Cellar/boost/1.76.0)
find_package(Boost 1.76.0)
include_directories(${Boost_INCLUDE_DIR})

add_executable(asio_basic_client_k main.cpp boost_basic_client.cpp boost_basic_client.h)
