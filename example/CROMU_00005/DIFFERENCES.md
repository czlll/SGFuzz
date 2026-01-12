## Differences from Original (exp/CROMU_00005)

| Aspect | example/CROMU_00005 (SGFuzz) | exp/CROMU_00005 (Original) |
|--------|------------------------------|---------------------------|
| **Purpose** | SGFuzz fuzzing target | Original CGC challenge |
| **Main Entry** | `chess_main()` renamed for harness | Standard `main()` |
| **Instrumentation** | SGFuzz state instrumentation added | No instrumentation |
| **Build System** | Dockerfile for containerized build | Makefile for native build |
| **Harness** | `fuzz_harness.c` with LibFuzzer entry | None |
| **CGC Layer** | `libcgc.h` compatibility header | Uses `<libcgc.h>` system header |
| **Vulnerability Hints** | Removed from source code | Present in comments |
| **Test Cases** | Seed corpus in Dockerfile | `poller/` directory |
| **PoV** | Not included | `pov/` directory with exploit |
| **Library Modifications** | Adapted for fuzzing (stdlib.c, mymath.c) | Original CGC libraries |

## Key Modifications for SGFuzz

### 1. State Instrumentation (`service.c`)
- Added `__sfuzzer_instrument()` calls to track state transitions
- Tracks `current_team`, parse results, and move results
- Helps SGFuzz understand protocol state machine

### 2. Fuzz Harness (`fuzz_harness.c`)
- LibFuzzer entry point `LLVMFuzzerTestOneInput()`
- Game state reset between fuzzing iterations
- CGC I/O redirection to fuzzer input buffer

### 3. CGC Compatibility Layer (`libcgc.h`)
- Redirects CGC syscalls (`transmit`, `receive`, `_terminate`) to fuzzer buffer
- Enables fuzzing without CGC kernel
