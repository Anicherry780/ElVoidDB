# ElVoidDB

**ElVoidDB** is a simple, from-scratch database engine written in C++17. It demonstrates core database concepts using a slotted-page storage design, buffer pooling, SQL parsing, and basic query execution. This project is built and tested on a Raspberry Pi 5 (8 GB RAM) but runs equally well on standard x86 systems.

---

## Motivation

This database is inspired by my CSE 562 Database Systems class and the TacoDB project. My goal was to build a real, working database from scratch, step by step, as I learned each concept:

1. **Storage management** with fixed-size (4 KB) pages
2. **Buffer pool** with LRU replacement and background flush
3. **SQL parser** and simple execution engine
4. **Asynchronous I/O threads** for concurrency

---

## Features

* **Basic SQL support**: `SELECT`, `INSERT`, `UPDATE`, `DELETE`
* **Slotted-page storage**: 4 KB pages with record slots
* **LRU buffer pool**: caches pages in memory, flushes dirty pages
* **Asynchronous I/O**: background threads handle disk writes
* **Simple CLI**: interactive prompt for SQL commands

---

## Architecture Overview

1. **Parser & Planner**: Tokenizes SQL text, builds an Abstract Syntax Tree (AST).
2. **Execution Engine**: Converts AST into low-level commands.
3. **Storage Manager**: Reads/writes pages to disk in slotted format.
4. **Buffer Pool**: Manages in-memory page cache with LRU policy.
5. **Concurrency & Logging**: Minimal async I/O threads (no full WAL yet).

---

## Folder Structure

```
ElVoidDB/
├─ include/              # Header files
│   ├─ Exceptions.hpp
│   ├─ Page.hpp
│   ├─ Storage.hpp
│   ├─ BufferPool.hpp
│   ├─ ThreadPool.hpp
│   ├─ BackgroundFlush.hpp
│   ├─ Parser.hpp
│   └─ Commands.hpp
└─ src/                  # Source files
    ├─ main.cpp
    ├─ Exceptions.cpp
    ├─ Page.cpp
    ├─ Storage.cpp
    ├─ BufferPool.cpp
    ├─ ThreadPool.cpp
    ├─ BackgroundFlush.cpp
    ├─ Parser.cpp
    └─ Commands.cpp
```

---

## Requirements

* C++17 compiler (GCC or Clang)
* CMake 3.10+
* Linux or macOS (tested on Raspberry Pi OS and Ubuntu)

---

## Build & Installation

```bash
# From project root:
mkdir build          # create build directory
cd build
cmake ..            # configure project
cmake --build . -j 4  # compile with 4 cores
```

The `elvoiddb` executable will appear in `build/`.

---

## Usage

Run the interactive CLI and enter SQL commands:

```bash
./elvoiddb
> CREATE TABLE users (id INT, name TEXT);
> INSERT INTO users VALUES (1, 'Alice');
> SELECT * FROM users;
```

---

## Performance

Tested on a Raspberry Pi 5 (8 GB RAM, NVMe storage):

* `INSERT` throughput: \~600 ops/sec
* `SELECT` throughput: \~900 ops/sec

---

## Roadmap

* Write-Ahead Logging (WAL) for crash safety
* B⁺-tree indexing for faster lookups
* Predicate push-down and simple query optimizer
* Full transaction support and isolation levels
* Unit tests and continuous integration (CI)

---

## Contributing

Contributions are welcome! Please:

1. Fork this repo
2. Create a feature branch
3. Submit a pull request with tests and documentation updates

---

## License

This project is released under the **MIT License**. See [LICENSE](LICENSE) for details.
