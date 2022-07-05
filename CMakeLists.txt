cmake_minimum_required(VERSION 3.10)

project(wav-edit)

set (CMAKE_CXX_STANDARD 11)

add_executable(wav-edit
    main.cpp
    readfile.cpp readfile.h
    writefile.cpp writefile.h
    printhex.cpp printhex.h
    wav-header.cpp wav-header.h
    sound-effects.cpp sound-effects.h
    )
