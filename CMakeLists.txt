cmake_minimum_required(VERSION 3.21)
project(dansappledouble)

set(CMAKE_CXX_STANDARD 23)

add_executable(dansappledouble
        main.cpp
        ReClassicfication/InterfaceLib/FakeHandles.c
        ReClassicfication/InterfaceLib/FakeHandles.h
        ReClassicfication/InterfaceLib/FakeResources.c
        ReClassicfication/InterfaceLib/FakeResources.h
        ReClassicfication/InterfaceLib/EndianStuff.h
        )
