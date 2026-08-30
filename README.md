*This project has been created as part of the 42 curriculum by hrasamoe.*

# Codexion

Master the race for resources before the deadline masters you.

## Description

**Codexion** is a concurrent programming project developed in C using POSIX threads (`pthreads`). The simulation models a high-stress software development environment where a set of developers (**Coders**) compete for a limited set of hardware licenses (**Dongles**) in order to compile their code before reaching a state of **Burnout**.

The simulation enforces strict resource sharing rules, precise timing, and scheduled queuing algorithms (**FIFO** and **EDF - Earliest Deadline First**) while preserving data integrity and thread safety.

---

## Project structure

```
.
├── Makefile
├── README.md
└── coders/
    ├── inc/
    │   └── codexion.h
    ├── srcs/
    │   ├── main.c        # entry point, thread creation/joining, coder & monitor routines
    │   ├── action.c       # dongle acquisition/release, heap arbitration check
    │   ├── heap.c          # priority heap internals (sift-down, pop, peek)
    │   └── init.c           # simulation/coder/dongle/heap initialization
    └── tools/
        ├── cleaner.c        # mutex destruction and memory cleanup
        ├── coder_action.c   # compile / debug / refactor phases
        ├── log.c            # timestamped logging, start/finish/error banners
        ├── parsing.c        # CLI argument validation
        ├── request.c        # heap comparator (FIFO/EDF) and push
        └── utils.c          # time helpers, dongle readiness checks, precise sleep
```

---

## Instructions

### Compilation

The project comes with a standard `Makefile` compliant with the 42 Norm and compiled with `-Wall -Wextra -Werror -pthread`.

```bash
make        # build the executable
make clean  # remove object files
make fclean # remove object files and the executable
make re     # full rebuild
```

### Execution

```bash
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

#### Argument details

| #   | Argument                      | Description                                                                            |
| --- | ----------------------------- | -------------------------------------------------------------------------------------- |
| 1   | `number_of_coders`            | Number of coder threads (and available dongles)                                        |
| 2   | `time_to_burnout`             | Time in ms before a coder burns out if they don't start compiling                      |
| 3   | `time_to_compile`             | Duration of the compiling phase (in ms)                                                |
| 4   | `time_to_debug`               | Duration of the debugging phase (in ms)                                                |
| 5   | `time_to_refactor`            | Duration of the refactoring phase (in ms)                                              |
| 6   | `number_of_compiles_required` | Number of successful compiles each coder must reach for the simulation to stop cleanly |
| 7   | `dongle_cooldown`             | Wait time after a dongle is released before it becomes available again (in ms)         |
| 8   | `scheduler`                   | Arbitration policy for dongle requests: `fifo` or `edf`                                |

#### Example

```bash
./codexion 4 800 200 400 100 5 50 edf
```

All 8 arguments are mandatory. Arguments 1-7 must be strictly positive integers - zero, negative values, and non-numeric input are all rejected. Argument 8 must be exactly `fifo` or `edf`. Any invalid input is rejected with an error message and the program exits without running the simulation.

#### Error handling

`parse_arguments` checks the arguments in order and stops at the first problem it finds, so you always know exactly what to fix:

1. **Wrong argument count** - if you don't pass exactly 8 arguments.
2. **Non-numeric or non-positive value** - for arguments 1-7, reported with the argument's position, its name, and the value you actually passed.
3. **Invalid scheduler** - if argument 8 is anything other than `fifo` or `edf`.

Each of these prints a specific message to `stderr`, followed by the full usage line, a reminder of the constraints on every argument, and a working example - so you don't need to check the README to know how to fix your command. For example:

```
Error: argument 3 (time_to_compile) must be greater than 0, got 0.

Usage: ./codexion number_of_coders time_to_burnout time_to_compile time_to_debug time_to_refactor number_of_compiles_required dongle_cooldown scheduler
  ...
Example:
  ./codexion 4 800 200 400 100 5 50 edf
```

After this detailed message, the program also prints the generic red `Error: invalid arguments` banner and exits with status `1` without starting the simulation - no threads are created, and no partial state is left behind.

---

## Resources

### Documentation & articles

- [Thread Management Functions in C](https://www.geeksforgeeks.org/c/thread-functions-in-c-c/)
- Earliest Deadline First (EDF) scheduling - theoretical background on deadline-based real-time scheduling
- man pages: `pthread_mutex_lock`, `gettimeofday`, `usleep`

### Use of AI assistance

In compliance with 42 project policy, AI tools (Claude) were used during development for:

- **Architecture design & review** - discussing synchronization strategy, Coffman's deadlock conditions, and the design of the shared priority-queue (heap) used for FIFO/EDF arbitration, before any code was written.
- **Guided implementation** - the heap (push/pop/peek/sift-down), the dongle acquisition/release logic, the coder and monitor thread routines, and `main` were built incrementally through a question-driven walkthrough rather than by having code generated directly; each function was written by me and reviewed line by line for correctness (types, memory ownership, mutex ordering) before moving to the next one.
- **Debugging support** - identifying concrete bugs introduced during development (e.g. a mutex self-deadlock on the single-coder edge case, an inconsistent "available" sentinel value between dongle initialization and release, missing `pthread_mutex_destroy` calls on coder locks) and explaining the underlying cause rather than only supplying a fix.

No code was copy-pasted directly from AI output into the submission without being understood, rewritten, and reviewed by me, in accordance with the project's AI usage guidelines.

---

## Blocking cases handled

### 1. Deadlock prevention (Coffman's conditions)

- **Mutual exclusion** is required by the subject (one coder per dongle at a time) and is not removable.
- **Hold-and-wait** and **circular wait** are both broken through **centralized arbitration**: a coder never locks a dongle while still deciding whether it can get the second one. Instead, a request (coder id, arrival time, and computed deadline) is pushed onto a shared priority heap (`push_heap`). While waiting, the coder repeatedly calls `heap_try_pop_if_mine`, which - atomically, under the heap's own mutex - checks whether its request currently sits at the head of the heap _and_ only pops it if so. A coder is only allowed to actually lock its two dongles once that atomic pop succeeded and `are_dongles_ready` confirmed both dongles are free and past cooldown.
- **Single-coder edge case**: when `number_of_coders == 1`, a coder's left and right dongle are the same dongle. `take_dongles` explicitly checks for this and issues a single `pthread_mutex_lock` call instead of two, so the same mutex is never locked twice by the same thread, which would otherwise deadlock immediately.
- As an additional safeguard, when the two dongles are distinct, `take_dongles` always acquires them in ascending dongle-ID order (`min(left, right)` before `max(left, right)`), removing any possibility of a circular wait chain.

### 2. Starvation prevention

- Under **FIFO**, requests are granted strictly in arrival order - no coder can be skipped indefinitely.
- Under **EDF**, priority is given to the coder with the closest burnout deadline (`last_compilation + time_to_burnout`), recomputed at every new request rather than reused from a stale value, preventing indefinite postponement of any coder under feasible parameters.

### 3. Cooldown handling

Each dongle stores `unvailable_until`, set to `release_time + dongle_cooldown` when the dongle is released. A coder is only allowed to take a dongle once `current_time > unvailable_until` (checked by `is_dongle_ready`). This check is combined with the heap-arbitration check on every polling iteration inside `aquire_dongles` (see Thread synchronization mechanisms below).

### 4. Precise burnout detection

A dedicated **monitor thread** (`monitor_routine`) scans every coder's deadline (`last_compilation + time_to_burnout`) on a 300 µs polling interval (`usleep(300)`). As soon as a coder's deadline has passed, the monitor logs the burnout and sets the global stop flag, keeping the detection well within the required 10 ms precision window.

### 5. Log serialization

All log output goes through a single function (`print_log`) protected by a dedicated `log_lock` mutex, guaranteeing that two messages are never interleaved and that timestamps remain in strict chronological order. `print_log` also checks the global stop flag before printing, so no state-change message is emitted after the simulation has already been told to stop.

### 6. Safe shutdown ordering

`clean_simulation` (destroying mutexes and freeing all shared arrays) is called exactly once, from `main`, only **after** every coder thread and the monitor thread have been joined with `pthread_join`. Neither the coder routine nor the monitor routine ever calls it themselves, which avoids destroying mutexes or freeing memory that another still-running thread might be using.

---

## Thread synchronization mechanisms

- **`pthread_mutex_t log_lock`** - serializes all stdout output across coder threads and the monitor thread.
- **`pthread_mutex_t stop_lock`** - protects the shared `stop` flag read/written by all threads to detect simulation termination.
- **`pthread_mutex_t lock` (inside `t_heap`)** - protects the shared priority queue used for FIFO/EDF arbitration from concurrent push/pop operations.
- **`pthread_mutex_t lock` per dongle** - synchronizes state changes (`is_available`, `held_by`, `unvailable_until`) for each individual dongle; held for the full duration a coder uses that dongle to compile. Each dongle also declares a `pthread_cond_t cond`, initialized alongside the lock; coordination is currently done through polling rather than condition-variable signaling.
- **`pthread_mutex_t lock` per coder** - protects a coder's own `state` and `nb_compilation` fields against concurrent access.
- **Bounded polling instead of blocking waits** - a coder waiting for its turn (heap priority) and for both dongles to be ready polls the shared state on a short `usleep` interval rather than spinning without pause, and the monitor does the same when scanning for burnouts. This keeps CPU usage bounded while staying well inside the timing precision the subject requires; it is a deliberate simplification over using condition variables for these waits.

**Example - race condition avoided:** without the per-dongle mutex, two coder threads could both read `is_available == true` at the same instant and both proceed to "take" the same dongle. By locking the dongle before checking _and_ updating its state (inside `take_dongles`), only one coder can ever transition a dongle from available to taken.

**Example - avoiding a check-then-act race on the heap:** `heap_try_pop_if_mine` performs "is my request at the head?" and "pop it" as a single operation under the heap's mutex, rather than as two separate calls (peek, then pop). This prevents a window where two coder threads could both observe themselves at the head of the heap and both attempt to proceed.

**Thread-safe coder ↔ monitor communication:** the monitor thread only reads `last_compilation`, a field written exclusively by each coder's own thread during its own compile cycle; the monitor never writes to coder-owned fields, and any field it does write (the global `stop` flag) goes through `stop_lock`.
