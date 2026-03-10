# dsh
A minimal Unix-like shell written in C++20. **dsh** runs commands, supports pipes, and handles I/O redirection—built from scratch with a lexer, recursive-descent parser, and AST-based execution.

## Features

- **Command execution** — Run any program on your `PATH` 
- **Pipes** — Chain commands with `|` (e.g. `ls | wc -l`)
- **I/O redirection**
  - Redirect stdout: `command > file`
  - Append stdout: `command >> file`
  - Redirect stdin: `command < file`
- **REPL** — Read-eval-print loop with a `$ ` prompt; exit with **Ctrl+D**

## Build & Run

**Requirements:** CMake 3.20+, C++20-capable compiler (e.g. GCC 10+, Clang 10+)

```bash
mkdir build && cd build
cmake ..
make
./dsh
```

Then type commands as you would in bash or zsh:

```bash
$ ls -la
$ echo hello world
$ cat README.md | head -5
$ ls > out.txt
$ sort < out.txt
$ echo appended >> out.txt
```

## Development

- **C++ standard:** C++20 (see `CMakeLists.txt`).
- **Linting/checks:** [clang-tidy](https://clang.llvm.org/extra/clang-tidy/) is configured (see `.clang-tidy`) with a strict set of checks and warnings-as-errors.

## License

See repository license file (if present).
