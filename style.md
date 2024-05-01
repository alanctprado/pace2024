# Style Guide

## Formatting

Formatting is automatically handled by GitHub actions on open Pull Requests. If any code must be formatted, an automatic commit with the user `Bananalyzer` is added to make these changes. If you want to format your source code locally, install `clang` and run the following command on the project's root.

``` shell
clang-format -i src/*
```

## Naming Conventions

* Global variables and classes should be named in CamelCase. 
* Local variables should be named in snake_case. 
* Class members names should be prefixed with `m_`.
