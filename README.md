*This project has been created as part of the 42 curriculum by hrasamoe.*

# Codexion

## Description

**Codexion** is a concurrent programming project developed in C using POSIX threads (`pthreads`). The simulation models a high-stress software development environment where a set of developers (**Coders**) compete for a limited set of hardware licenses (**Dongles**) in order to compile their code before reaching a state of **Burnout**.

The simulation enforces strict resource sharing rules, precise timing, and scheduled queuing algorithms (**FIFO** and **EDF - Earliest Deadline First**) while preserving data integrity and thread safety through proper mutual exclusion and condition variables.

---

## Project structure

```
.
├── README.md
└── coders/
    ├── Makefile
    ├── inc/
    │   └── codexion.h
    ├── srcs/
    │   ├── action.c            # dongle acquisition/release + priority arbitration
    │   ├── heap.c              # priority heap internals (sift-down, pop, peek)
    │   ├── init.c              # simulation / coder / dongle / heap initialization
    │   └── main.c              # entry point, thread creation/joining, coder & monitor routines
    └── tools/
        ├── cleaner.c           # mutex/cond destruction and memory cleanup
        ├── coder_action.c      # compile / debug / refactor phases
        ├── log.c               # timestamped logging, start/finish/error banners
        ├── parsing.c           # CLI argument validation
        ├── request.c           # heap comparator (FIFO/EDF) and push
    	├── time.c              # time helpers and timeout calculation
        └── utils.c             # time helpers, dongle readiness checks, precise sleep
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
./codexion number_of_coders time_to_burnout time_to_compile  time_to_debug time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

#### Argument details

| # | Argument                      | Description                                                                 |
|---|-------------------------------|-----------------------------------------------------------------------------|
| 1 | `number_of_coders`            | Number of coder threads (and available dongles)                             |
| 2 | `time_to_burnout`             | Time in ms before a coder burns out if they don't start compiling           |
| 3 | `time_to_compile`             | Duration of the compiling phase (in ms)                                     |
| 4 | `time_to_debug`               | Duration of the debugging phase (in ms)                                     |
| 5 | `time_to_refactor`            | Duration of the refactoring phase (in ms)                                   |
| 6 | `number_of_compiles_required` | Number of successful compiles each coder must reach                         |
| 7 | `dongle_cooldown`             | Wait time after a dongle is released before it becomes available again (ms) |
| 8 | `scheduler`                   | Arbitration policy: `fifo` or `edf`                                         |

#### Example

```bash
./codexion 4 1500 150 150 80 5 30 edf
```

All 8 arguments are mandatory. Arguments 1-7 must be strictly positive integers. Argument 8 must be exactly `fifo` or `edf`. Any invalid input is rejected with a detailed error message and the program exits without starting the simulation.

---

## Resources

### Documentation & articles

- [Thread Management Functions in C](https://www.geeksforgeeks.org/c/thread-functions-in-c-c/)
- Earliest Deadline First (EDF) scheduling - theoretical background on deadline-based real-time scheduling
- man pages: `pthread_mutex_lock`, `pthread_cond_wait`, `pthread_cond_timedwait`, `pthread_cond_broadcast`, `gettimeofday`, `clock_gettime`

### Use of AI assistance

In compliance with 42 project policy, AI tools were used during development for:

- **Architecture design & review** - discussing synchronization strategy, Coffman's deadlock conditions, and the design of the shared priority-queue (heap) used for FIFO/EDF arbitration.
- **Guided implementation** - the heap, dongle acquisition/release logic, coder and monitor routines were built incrementally through a question-driven walkthrough. Each function was written and reviewed line by line for correctness.
- **Debugging support** - identifying concrete bugs (mutex self-deadlock on the single-coder edge case, data races on dongle state, missing wake-ups after cooldown, incorrect stop propagation) and explaining the underlying causes.

No code was copy-pasted from AI output into the submission without being fully understood, rewritten, and reviewed.

---

## Blocking cases handled

### 1. Deadlock prevention (Coffman's conditions)

- **Mutual exclusion** is required (one coder per dongle at a time).
- **Hold-and-wait** and **circular wait** are broken through **centralized arbitration**:
  - A coder never locks a dongle while still deciding whether it can obtain the second one.
  - A request (coder id, arrival time, deadline) is first pushed onto a shared priority heap.
  - The coder is only allowed to lock its two dongles after an atomic `heap_try_pop_if_mine` succeeds **and** both dongles are ready.
- **Single-coder edge case**: when `number_of_coders == 1`, left and right point to the same dongle. `take_dongles` detects this and performs a single `pthread_mutex_lock`.
- When the two dongles are distinct, they are always acquired in ascending ID order, eliminating any possibility of circular wait.

### 2. Starvation prevention

- **FIFO**: requests are granted strictly in arrival order.
- **EDF**: priority is given to the coder with the closest burnout deadline (`last_compilation + time_to_burnout`), recomputed on every new request.

### 3. Cooldown handling

Each dongle stores `unavailable_until` (set to `release_time + dongle_cooldown`). A dongle can only be taken once `current_time > unavailable_until`. Waiting coders use `pthread_cond_timedwait` so they periodically re-evaluate readiness after the cooldown expires.

### 4. Precise burnout detection

A dedicated **monitor thread** continuously checks every coder’s deadline. As soon as a coder exceeds `time_to_burnout` without compiling, the monitor logs the burnout and sets the global stop flag (which also broadcasts on the allocation condition variable to wake every waiting coder).

### 5. Log serialization

All output goes through `print_log`, protected by `log_lock`, guaranteeing non-interleaved messages and chronological timestamps. Logging is suppressed once the stop flag is set.

### 6. Safe shutdown ordering

`clean_simulation` is called from `main` only after every coder thread and the monitor thread have been joined with `pthread_join`. No thread destroys mutexes or frees memory while other threads may still be running.

---

## Thread synchronization mechanisms

| Synchronization object      | Purpose                                                                 |
|-----------------------------|-------------------------------------------------------------------------|
| `log_lock`                  | Serializes all stdout output                                            |
| `stop_lock`                 | Protects the global `stop` flag                                         |
| `alloc_lock` + `alloc_cond` | Protects the allocation decision and allows efficient waiting for dongles |
| `lock` inside `t_heap`      | Protects the shared priority queue (push / try-pop)                     |
| `lock` per dongle           | Protects `is_available`, `held_by` and `unavailable_until`              |
| `lock` per coder            | Protects a coder’s own state and compilation counter                    |

### How dongle acquisition works

A coder that needs dongles proceeds as follows:

1. Pushes a request into the priority heap.
2. Locks `alloc_lock`.
3. While the condition “both dongles are ready **and** I am the highest-priority request” is false, it calls `pthread_cond_timedwait` on `alloc_cond` (with a short timeout). This releases `alloc_lock` while sleeping and guarantees periodic re-evaluation so that cooldowns can expire.
4. When the condition becomes true, it unlocks `alloc_lock` and proceeds to `take_dongles`.

When a coder finishes compiling and releases its dongles, it updates their state and performs a `pthread_cond_broadcast` on `alloc_cond`. All waiting coders wake up, re-evaluate the condition under `alloc_lock`, and only the legitimate next coder proceeds. The same broadcast is also performed when the global stop flag is set, ensuring a clean shutdown.

This design:

- Eliminates data races on dongle state.
- Avoids busy-waiting while still handling time-based cooldowns.
- Guarantees that the FIFO/EDF ordering is respected.
- Uses a single global condition variable for the allocation decision, keeping the synchronization logic simple and correct.

**Example - race condition avoided**  
Without proper locking, two coders could both observe a dongle as available and both try to take it. By protecting state changes with the dongle mutex and coordinating the high-level decision with `alloc_lock` + `alloc_cond`, only one coder can ever transition a dongle from available to taken.
