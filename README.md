# RISC-V Hifive1 Speculative Attacks

This repository contains files to avaliate speculative attacks on Hifive boards.


L1 Sets 16384 
L1 Ways 2
L1 Linesize 32 bytes
MSHR File Entries 4
BTB Sets    512
BTB Banks   2
BTB Ways    4
GShare History Bits 23 
GShare Counter Table Entries 4096

## Suported boards

| Name | Revision |
| ---- | ---------|
|Hifive1| revb|

## Build

Install [platformio](https://platformio.org/)

Build and upload
```bash
platformio run --targer upload
``` 

## Monitor
```bash
platformio device monitor
``` 
