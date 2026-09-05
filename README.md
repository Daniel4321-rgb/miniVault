# miniVault

A lightweight C++ command-line filesystem explorer for Linux.

## Features

- Recursive directory traversal
- File and directory detection
- Human-readable file sizes
- Total directory size calculation
- Directory/file ordering
- Sort entries by name
- Sort entries by size
- File-only mode
- Directory-only mode
- Non-recursive mode
- Permission error handling
- Command-line help

## Usage

    ./miniVault [command]

To see all available commands:

    ./miniVault --help

## Commands

| Command | Description |
|---|---|
| `--help` | Show available commands |
| `--dirs-first` | Show directories before files |
| `--files-first` | Show files before directories |
| `--files` | Show only files |
| `--dirs` | Show only directories |
| `--no-recursive` | Show only the current directory |
| `--sort-name` | Sort entries by name |
| `--sort-size` | Sort entries by size, smallest first |

## Example

    ./miniVault --sort-size

    [DIR]   something
        └──[FILE]   something.txt 32 B
    [FILE]  main.cpp 11.6 KB
    [FILE]  miniVault 135.5 KB

    The total size is: 147.0 KB

## Built With

- C++
- C++17
- `std::filesystem`
- Linux

## Project Goals

miniVault was built as a learning project to improve my C++ skills and gain experience with filesystem operations, recursion, data structures, sorting, command-line interfaces, and error handling.

## License

This project is licensed under the MIT License.
