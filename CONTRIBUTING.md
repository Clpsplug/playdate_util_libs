# Contributing to Playdate Utility Libraries

Thank you for your interest in contributing to Playdate Utility Libraries!  
This document outlines the guidelines for contributing to the project, including coding style, naming conventions, and more.

(Contents are still under construction and subject to change!)

## Table of Contents
1. [Introduction](#introduction)
2. [Coding Style](#coding-style)
3. [Naming Conventions](#naming-conventions)
4. [Commit Messages](#commit-messages)
5. [Additional Resources](#additional-resources)

## Introduction
This is a collection of libraries that might help you with developing your [Playdate](https://play.date) games.  
Therefore, it is mandatory that you familiarize yourself with coding Playdate games in C.

With that out of the way, this project aims to 
1. make seemingly verbose code more concise, and
2. provide devs with framework that would help organize their game code better.

If you have an idea of what to add to this library, go ahead and make a PR!

## Coding Style
### General Guidelines
* This project is written in C11.
  * This is because example games in Playdate SDK specify C11 as the standard. See CMakeLists.txt of each example project.
* If `PlaydateAPI*` is needed, obtain it using the following methods:
  * If the module itself can be used as a system (e.g., `scene_engine`), define an initialization function.
  * (less recommended) use `pd_get_pd()` in `pd_shorthand` library.
* Install [EditorConfig](https://github.com/editorconfig/editorconfig) plugin if your IDE doesn't already support it.  
  This will help us keep the consistency across all the files.
* Each function call adds to stack (= performance hits), so avoid a one-liner wrapper function.
  * If such a function still improves readability, then you are free to implement it.
  * Consider implementing such a functions as a macro function if it can be considered a syntax sugar.

### Comments
* Prefer the `/* */`-style comments.
  * This is to fail as early as possible when some gnarly charset issues messes up our code and comments.
  * `//`-style comments are not too effective at guarding against charset issues.
* The header should contain a doc comment for each function and type it declares.
  * The doc comment should be in a Doxygen format.
  * We use `@` to refer to the Doxygen control codes, NOT \\ (backslash)

### Language-Specific Guidelines
* Be wary of using the standard C header. Playdate's C library is a subset of the standard,  
  and thus some functions may be found to be unlinkable only when you attempt to build for device.
  * It is often the case that such linkage issues manifest as symbols you don't recognize failing to link,
    making it more difficult to debug.


<!-- Placeholder for any language-specific guidelines or best practices. -->

## Naming Conventions
### Files and Directories
* The base library name and the folder name MUST match.
  * Example: The `scene_engine` library has `scene_engine` as its target name.  
    See the `add_library` call to scene_engine/CMakeLists.txt.
* The simulator build of the library MUST have `_Sim` appended to it.
  * Example: `scene_engine` library's simulator build is called `scene_engine_Sim`

### Variables
* Static variables should start with `s_`

### Functions and Methods
* The functions should be named `<module name>_<method name>`
  * Module name should be camelCase, while method name should be PascalCase.

### Structs, Enums and Unions
* Their names should be PascalCase.
* Avoid anonymous struct/enum/union typedefs. In this project, we append `Tag` to the actual type name.
  ```c
  /* Avoid */
  typedef struct {
    void* member;
  } AvoidThisTypeDef;
  
  /* Prefer */
  typedef struct PreferThisTypeDefTag { /* typedef'd name + "Tag" */
    void* member;
  } PreferThisTypeDef;
  ```

## Commit Messages
* Messages should start with a verb of present tense (e.g., add, modify, fix...)
  and follow it with the content of the change.

## Additional Resources
Please read [Inside Playdate with C](https://sdk.play.date/2.5.0/Inside%20Playdate%20with%20C.html)
to familiarize yourself with coding purely in C on Playdate.

---

Thank you for helping us improve Playdate Utility Libraries!

> Skeleton for this documentation is powered by ChatGPT
