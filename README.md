# LiteDir

> Lightweight alternative to Active Directory for BSD environments.  
> User, group, and policy management in pure C — no external dependencies.

```
 _     _ _       ____  _
| |   (_) |_ ___|  _ \(_)_ __
| |   | | __/ _ \ | | | | '__|
| |___| | ||  __/ |_| | | |
|_____|_|\__\___|____/|_|_|
```

---

<p align="center">

[![License: GPL-3.0](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
<img src="https://img.shields.io/badge/made%20by-nerdemma-blue.svg" >
<img src="https://img.shields.io/badge/CC-11.0.1-red.svg">
<img src="https://img.shields.io/badge/OPENBSD-7.0-green.svg">
<img src="https://badges.frapsoft.com/os/v1/open-source.svg?v=103" >
<img src="https://img.shields.io/github/stars/nerdemma/litedir.svg?style=flat">
<img src="https://img.shields.io/github/languages/top/nerdemma/litedir.svg">
<img src="https://img.shields.io/github/issues/nerdemma/litedir.svg">
<img src="https://img.shields.io/badge/PRs-welcome-brightgreen.svg?style=flat">
</p>

## What is LiteDir?

LiteDir is a user and group directory designed to replace Active Directory in environments where Active Directory's requirements are excessive: legacy hardware, embedded BSD systems, or small networks that do not justify a Windows server.

**It is not an AD clone.** It is a purpose-specific tool that solves the core problem: authenticating users, assigning them to groups, and applying policies — with the smallest possible footprint.

---

## Features

- **Pure C99** — no C++, no Rust, no scripting
- **Zero external dependencies** — only the base system's libc
- **Custom binary format** — `ldstore.db` file, no SQLite, no LDAP
- **Compilable on i386 32-bit** — hardware from 15+ years ago
- **Minimal footprint** — binary < 2 MB, memory at rest < 8 MB
- **Strict POSIX** — compatible with FreeBSD, OpenBSD, NetBSD
- **Secure by design** — `pledge(2)` / `unveil(2)` on OpenBSD, no root required to operate

---

## Reference Hardware

The project was designed and validated on:

| Component | Specification |
|---|---|
| CPU | Intel Celeron D 2.4 GHz (i386, 32-bit) |
| RAM | 2 GB DDR |
| Disk | 300 GB HDD |
| System | OpenBSD 7.0 — i386 architecture |
| Compiler | cc (clang 11.0.1) — base system |

If it runs on a 2004 Celeron D, it runs on anything.

---

## Project Status

```
Phase 1 — ldstore (data engine)          ████████░░  In development
Phase 2 — Network and LDP protocol       ░░░░░░░░░░  Planned
Phase 3 — Policy engine                  ░░░░░░░░░░  Planned
Phase 4 — Administration tools           ░░░░░░░░░░  Planned
Phase 5 — Validation and hardening       ░░░░░░░░░░  Planned
```

---

## Quick Start

### Requirements

- OpenBSD 7.x, FreeBSD 12+, NetBSD 9+ — or any POSIX system with clang/gcc
- `cc` and `make` — included in the base system of all BSDs
- No `pkg_add`, no `pkg install`, no `apt`

### Compile

```sh
git clone https://github.com/youruser/litedir.git
cd litedir
make
```

To explicitly compile in 32-bit mode from an amd64 machine:

```sh
make CFLAGS="-std=c99 -Wall -Wextra -m32"
```

Verify the binary:

```sh
file ldctl
# ldctl: ELF 32-bit LSB executable, Intel 80386
```

### Basic Usage

```sh
# Users
./ldctl user add  alice secret123
./ldctl user add  bob   pass456
./ldctl user list
./ldctl user auth alice secret123   # -> OK
./ldctl user del  bob

# Groups
./ldctl group add admins
./ldctl group adduser admins alice
./ldctl group members admins
./ldctl group list
```

---

## Architecture

```
ldstore.db (binary file)
     │
     ├── Header (32 bytes)   — magic "LDST", version, counters
     ├── Users (× 1024)      — uid, name, bcrypt hash, groups
     └── Groups (× 256)      — gid, name, member list
          │
          └── db.c           — POSIX pread/pwrite, no mmap
               │
               ├── users.c   — CRUD for users
               ├── groups.c  — CRUD for groups and memberships
               ├── auth.c    — crypt_newhash / crypt_checkpass (OpenBSD)
               └── main.c    — ldctl CLI, argv[] parsing
```

The `ldstore.db` file is always created in the working directory. The binary does not write outside its execution directory.

---

## Repository Structure

```
litedir/
├── Makefile
├── include/
│   └── store.h          — on-disk structures and public API
├── src/
│   ├── db.c / db.h      — storage engine
│   ├── users.c          — user management
│   ├── groups.c         — group management
│   ├── auth.c           — hashing and authentication
│   └── main.c           — ldctl (CLI)
├── tests/
│   └── test_users.c     — unit tests in C
└── docs/
    └── litedir_phase1.pdf  — detailed getting started guide
```

---

## Running Tests

```sh
make test
# All tests passed.
```

Tests are pure C using `assert()`. No frameworks, no dependencies.

---

## Security

- Passwords are stored with **bcrypt** via OpenBSD's `crypt_newhash(3)`. Never in plain text or with MD5/SHA-1.
- The `ldstore.db` file is created with `0600` permissions — only the owner can read it.
- In production, the daemon will use `pledge(2)` and `unveil(2)` to restrict syscalls to the bare minimum (Phase 4).
- The binary operates without root privileges. It does not use `setuid`.

---

## Roadmap

### Phase 1 — ldstore (current)
Pure C storage engine. `ldctl` CLI for local user and group management. No networking.

### Phase 2 — Network and LDP Protocol
TCP server with an event loop based on `kqueue(2)`. Custom binary protocol (LiteDir Protocol) with TLV messages. Optional TLS via embedded BearSSL.

### Phase 3 — Policy Engine
Rules in `.ini` files, group inheritance, mapping to POSIX file system permissions.

### Phase 4 — Administration Tools
`ldird` (daemon with rc.d), `ldisync` (active/passive replication between nodes).

### Phase 5 — Hardening
Fuzzing with AFL++, static analysis with `scan-build`, testing on real legacy hardware.

---

## Differences with Active Directory / OpenLDAP

| | Active Directory | OpenLDAP | **LiteDir** |
|---|---|---|---|
| Dependencies | Hundreds | Dozens | **Zero** |
| Minimum RAM | ~2 GB | ~128 MB | **< 8 MB** |
| Architecture | x86_64 | x86_64 / ARM | **i386 / x86_64 / ARM** |
| Protocol | LDAP + Kerberos | LDAP | **LDP (custom binary)** |
| Language | C / .NET | C | **Pure C99** |
| BSD Native | No | Partial | **Yes** |

---

## Contributing

The project is in Phase 1. The most useful contributions right now are:

1. **Testing on real hardware** — especially i386 and low-end ARM
2. **Portability** — testing on NetBSD, DragonFlyBSD
3. **Security review** — file format and argument parser
4. **Documentation** — man pages in `mandoc` format

Before opening a PR, ensure `make test` passes and `cc -Wall -Wextra` emits no warnings.

---

## License

GPL V3

---

<sub>Developed on OpenBSD 7.0 · Intel Celeron D 2.4 GHz · cc (clang 11.0.1) · C99</sub>
