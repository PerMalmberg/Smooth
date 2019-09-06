# Contributing to Smooth

Hello and thank you for you interest in this framework. To keep things in order, please follow the following rules when contributing to the project.

## Tests

- Please write unit test for new code. We use Catch2 as the testing framwork. Have a look at the existing [unit tests](https://github.com/PerMalmberg/Smooth/tree/master/test/unit_tests).
- For code that can't be fully tested using unit tests, write integration tests, such as those found in the [test folder)[https://github.com/PerMalmberg/Smooth/tree/master/test]

## Multi-platform

A major goal of Smooth is to ease development, which it does by allowing projects to be compiled for, and run on, Linux. All new code must support this dual platform by using mocks such as [mock-idf](https://github.com/PerMalmberg/Smooth/tree/master/mock-idf) or other abstractions. Hardware mocks must not cause a crash and should pretend to be functioning (e.g. return success-values), but are not expected to full simualtors.

## License and copyright

When changing existing source code, do not change the copyright of the original file(s). The copyright remains with the original author, or those who have been assinged the copyright by the origianl author(s).

Contributors shall be given credit by adding them to the [CONTRIBUTORS.md](CONTRIBUTORS.md).

## Code formatting

- Run [Uncrustify](https://github.com/uncrustify/) on your code for general formatting, a configuration is [found in the root of the project](https://github.com/PerMalmberg/Smooth/blob/master/uncrustify.cfg).
- Follow the same code formatting as the rest of the project, such as:
  - Variables, class members, functions and methods shall use `snake_case` naming.
  - Do __not__ use Hungarian notation for variables, i.e. warts `m_` 
  - Classes shall be named using [UpperCamelCase](https://en.wikipedia.org/wiki/Camel_case) using characters a-z or A-Z. Digits are rarely used.
  - No multiple `return`-statements.
  - No switch, goto, jmp-statements.
  - Prefer positive logic, e.g. `if (ready) {...} else {...}` over  `if (!ready) {...} else {...}`
  - Do not use exceptions for code flow, i.e. use exceptions only for exceptional __uncrecoverable__ situations. That said, prefer error codes over exceptions.
  - Don't mix condition types without parentheses to avoid unintentional logic errors, e.g. `if(a && (b || c))` vs. `if(a && b || c)`
  - Don't call multiple methods in a condition statement; this avoids unintentional short circuits.
  - Place chained `&&` and `||` in if-statements at a new line.


This document is a work in progress and updated as needed.