# GPS Log Checker

![build](https://github.com/d-chin-xl/gps-log-checker/actions/workflows/build.yml/badge.svg)

A small C++17 command-line tool that checks GPS logs. It reads NMEA GGA sentences (the text a GPS module sends), verifies each line's checksum, and reports the position, the number of satellites, and any lines that are corrupted or have no GPS fix.

## Why I built this
During my internship at Ascent Solutions I tested GPS on IoT asset trackers, and some units had a GPS fault that kept coming back. I wanted to understand what the GPS module actually sends, and how to tell a corrupted reading from a real "no fix".

## Build and run
```bash
cmake -S . -B build
cmake --build build
./build/gps_log_checker data/sample.nmea
```

## Example output
```
Line 1: OK      48.11730, 11.51667 (8 satellites)
Line 4: NO FIX  GPS had no position
Line 5: OK      1.29200, 103.85550 (9 satellites)
Line 6: BAD     checksum does not match
Line 7: BAD     not a readable GGA sentence

Summary: 4 valid, 1 no fix, 1 bad checksum, 1 malformed
```

## How it works
- **Checksum:** XORs every character between `$` and `*` and compares the result with the two hex digits the GPS sent. If one character changed on the way, they don't match.
- **Position:** NMEA writes `4807.038` meaning 48° 7.038′. Dividing the minutes by 60 gives 48.1173°. South and West become negative.
- **Exit code:** returns 1 if any line was corrupted, so scripts and CI can detect bad logs.

## Tests
7 Catch2 test cases run on every push with GitHub Actions:
```bash
ctest --test-dir build --output-on-failure
```

## Tools
C++17 · CMake · Catch2 · GitHub Actions · Linux (Ubuntu)
