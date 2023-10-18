# Gem C++ Style Guide

## Why C++?

Gem is written in [C++](https://en.wikipedia.org/wiki/C%2B%2B).
Here, the choice was very easy.
C++ is the de facto standard for game engine programming in the game development industry, not only for indie developers, but also for AAA developers.
C++ being the standard, allows us to leverage a **lot** of libraries that have been used by a plethora of other game programmers before us.
This is a big plus, since we don't have to reinvent the wheel, writing more code than necessary, and obviously, creating more bugs for ourselves.

## Why a Style Guide?

C++ has been around for about 38 years, at the time of writing.
The language has drastically evolved during this time span.
This means that there are, inevitably, a lot of ways to write C++.
Each way has its own pros and cons.
Since this project aims at being a collaborative effort, it is a good idea to start stating a way in which the code should be written, style wise.
We do so, in order to keep the codebase consistent.
Hence, the need for a style guide arises.

## Rationale

The style guide is not meant to be a list of, often opinionated, DOs and DO NOTs.
We find that kind of lists boring and of little use.
That kind of approach is also quite rigid and clashes with the continuously evolving C++ landscape.
The aim of the style guide is to give a sense of direction when writing code.

## What C++ Standard?

We use [C++20](https://en.wikipedia.org/wiki/C%2B%2B20), since it is well [supported](https://en.cppreference.com/w/cpp/compiler_support/20) by our target platforms.

## What C++ Style?

We use **Modern C++** following the spirit of the [C++ core guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines).

## What should I keep in mind when writing code for Gem?

- Don't reinvent the wheel.
- Think before writing code.
- Think where your code is going to be used.
- When implementing a feature, get it working as soon as possible.
- Write the least amount of code to achieve the required feature.
- Optimize only after you benchmark your code and understand its use cases.
- After implementing a non trivial algorithm, document it.
- Use types to express intent.
- Think about ownership of data.
- Don't overgeneralize.
