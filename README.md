# Roblox Runtime Offset Dumper

A runtime offset dumper for **Roblox** using **pattern scanning** with **Zydis** and **ZYCore**.  
Dumps **external offsets**, **internal offsets**, and **Hyperion (Roblox AC) offsets**.

## Features

- Dump **external offsets** for Roblox.
- Dump **internal offsets** (in-game memory offsets).
- Dump **Hyperion AC offsets**.
- Reliable **pattern scanning** with Zydis and ZYCore.

## Requirements

- Windows OS (x64 recommended)
- C++17 compatible compiler
- [Zydis](https://github.com/zyantific/zydis) and [ZYCore](https://github.com/zyantific/zycore) libraries

## Installation

1. Clone the repository:
- bash
- git clone https://github.com/nyra-workspace/RobloxDumper.git
- cd RobloxDumper
- cmake .
- cmake --build .
