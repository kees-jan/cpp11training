# cpp 11 training 

Exercises to train your C++11/14 (and then some).

[![Build Status](https://travis-ci.org/xtofl/cpp11training.svg?branch=master)](https://travis-ci.org/xtofl/cpp11training)

## Getting started

### Linux

On Linux all you need is a compiler with C++14 support, and CMake, and conan (so... python3 and pip)

    git clone git@github.com:xtofl/cpp11training
    mkdir build
    cd build
    conan install ..
    cmake ..

    make



### Windows

    
Prerequisite: currently I have it up and running for Visual Studio 2017 Community edition.  It relies on NuGet package manager, which should be installed explicitly (cf. [stackoverflow](http://stackoverflow.com/questions/42875451/cant-find-the-nuget-package-manager-in-visual-studio-2017))

Open the `cpp11training/cpp11training.sln` solution.


## Exercising

The project contains a bunch of failing unit tests.  The idea is that you un-DISABLE each one of them, and make it pass (using C++11, obviously).  (the gtest library by default ignores tests starting with `DISABLED_`.)

I tried to add a TODO: comment hinting what's expected from you to fulfill the exercise.

Once all tests pass, I have nothing more to teach you :)
