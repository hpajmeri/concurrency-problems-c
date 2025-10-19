# Concurrency Problems: A Systems Programming Deep Dive

A comprehensive C implementation of classic concurrency patterns and synchronization problems, demonstrating real-world solutions to multi-threaded programming challenges using POSIX threads and low-level synchronization primitives.

## Technical Overview

This project showcases four fundamental concurrency problems encountered in operating systems, databases, and real-time systems. Each solution highlights different synchronization strategies and trade-offs in concurrent system design.

### Problems Implemented

1. **Producer-Consumer (Bounded Buffer)** — Coordinating multiple asynchronous producers and consumers with a fixed-capacity buffer
2. **Readers-Writers** — Balancing concurrent read access with exclusive write access to shared data
3. **Cigarette Smokers** — Solving complex inter-thread signaling with asymmetric resource requirements
4. **Barbershop** — Managing queue capacity, thread scheduling, and resource allocation in a real-world scenario

---

## Deep Dive: Synchronization Architecture

### 1. Producer-Consumer Problem

**Core Challenge:** Prevent buffer overflow, buffer underflow, and ensure FIFO ordering with multiple concurrent producers and consumers.

#### Data Structure Design

```c
typedef struct {
    int buffer[10];          // Circular buffer
    int in, out, size;
    pthread_mutex_t mutex;   // Protects buffer indices
    dispatch_semaphore_t empty;  // Counts empty slots
    dispatch_semaphore_t full;   // Counts full slots
} bounded_buffer_t;
```

#### Synchronization Protocol

**Producer Path:**
1. `dispatch_semaphore_wait(empty)` — Block if no empty slots available
2. `pthread_mutex_lock()` — Acquire exclusive access to buffer metadata
3. Write item at `buffer[in]`, increment `in = (in + 1) % size`
4. `pthread_mutex_unlock()` — Release lock
5. `dispatch_semaphore_signal(full)` — Wake waiting consumer

**Consumer Path:**
1. `dispatch_semaphore_wait(full)` — Block if no items available
2. `pthread_mutex_lock()` — Acquire exclusive access to buffer metadata
3. Read item from `buffer[out]`, increment `out = (out + 1) % size`
4. `pthread_mutex_unlock()` — Release lock
5. `dispatch_semaphore_signal(empty)` — Wake waiting producer

#### Why This Works

- **Semaphores** act as counting locks, tracking resource availability without busy-waiting
- **Mutex** ensures atomic buffer updates (critical section protection)
- **Circular indexing** enables efficient reuse of fixed buffer space
- **Order of operations** prevents deadlock: semaphore wait before mutex acquire

#### Performance Characteristics

- **Time Complexity:** O(1) per produce/consume operation
- **Space Complexity:** O(buffer_size) fixed memory footprint
- **Throughput:** Limited by slowest producer or consumer
- **Latency:** Minimal—no polling, pure event-driven signaling

---

### 2. Readers-Writers Problem

**Core Challenge:** Maximize concurrent reader throughput while ensuring writer exclusivity. Prevents reader starvation (readers-first policy) while avoiding write starvation.

#### Data Structure Design

```c
typedef struct {
    int data;
    int read_count;
    pthread_mutex_t read_count_mutex;  // Protects read_count
    dispatch_semaphore_t write_semaphore;  // Controls writer access
} shared_resource_t;
```

#### Synchronization Protocol

**Reader Entry:**
1. `pthread_mutex_lock(&read_count_mutex)` — Acquire counter lock
2. Increment `read_count++`
3. If `read_count == 1` (first reader): `dispatch_semaphore_wait(write_semaphore)`
4. `pthread_mutex_unlock(&read_count_mutex)` — Release counter lock
5. Proceed to read shared data

**Reader Exit:**
1. `pthread_mutex_lock(&read_count_mutex)` — Acquire counter lock
2. Decrement `read_count--`
3. If `read_count == 0` (last reader): `dispatch_semaphore_signal(write_semaphore)`
4. `pthread_mutex_unlock(&read_count_mutex)` — Release counter lock

**Writer Entry:**
- `dispatch_semaphore_wait(write_semaphore)` — Block until no readers/writers

**Writer Exit:**
- `dispatch_semaphore_signal(write_semaphore)` — Wake first waiting thread (reader or writer)

#### Key Design Decisions

- **First Reader Locks:** Initial reader acquires the write semaphore, preventing writers from starting
- **Last Reader Unlocks:** Final reader releases the semaphore, allowing blocked writers to proceed
- **Readers-First Priority:** New readers bypass the semaphore queue as long as one reader holds it
- **Single Counter Lock:** Minimizes critical section with dedicated mutex for `read_count` only

#### Concurrency Patterns

```
Scenario 1: Multiple readers (good throughput)
  Reader1 enters → read_count=1 → acquires write_sem → starts reading
  Reader2 enters → read_count=2 → no semaphore wait → starts reading
  Reader3 enters → read_count=3 → no semaphore wait → starts reading
  (All read concurrently; no mutual exclusion)

Scenario 2: Writer during readers (exclusive write)
  Writer blocks on write_sem (readers still hold it)
  Reader1 exits → read_count=2 → write_sem still held
  Reader2 exits → read_count=1 → write_sem still held
  Reader3 exits → read_count=0 → signals write_sem → writer begins
  (Writer has exclusive access; new readers wait)
```

#### Potential Write Starvation Issue

In readers-first policy, if readers continuously arrive, writers may never acquire the semaphore. Solutions:

1. **Use writer-preference:** Give writers priority when entering (prevents indefinite starvation)
2. **Use fair locks:** Round-robin scheduling between readers and writers
3. **Current implementation:** Accepts potential writer delay; acceptable for read-heavy workloads

---

### 3. Cigarette Smokers Problem

**Core Challenge:** Three asymmetric agents with disjoint resource requirements. Requires careful signaling to prevent deadlock and ensure progress.

#### Resource Requirements

| Agent | Has | Needs | Produces |
|-------|-----|-------|----------|
| Tobacco Smoker | Tobacco | Paper + Matches | Smoke |
| Paper Smoker | Paper | Tobacco + Matches | Smoke |
| Matches Smoker | Matches | Tobacco + Paper | Smoke |

#### Solution Strategy

- **Agent thread:** Provides two items; signals the smoker that can proceed
- **Smoker threads:** Wait for their required items; signal agent when done

#### Synchronization Implementation

```c
void agent_provide_tobacco_paper(cigarette_smokers_t *cs) {
    cs->tobacco_count = 1;
    cs->paper_count = 1;
    dispatch_semaphore_signal(cs->matchesSem);
}
```

Each smoker waits on its respective semaphore; when agent deposits the required pair, that smoker proceeds.

---

### 4. Barbershop Problem

**Core Challenge:** Manage limited waiting room capacity, multiple barbers with different service times, and prevent customer loss when shop is full.

#### Data Structure Design

```c
typedef struct {
    int num_barbers;
    int num_chairs;
    int customers_waiting;
    dispatch_semaphore_t barberSem;     // Signals barber has customer
    dispatch_semaphore_t customerSem;   // Signals customer is waiting
    dispatch_semaphore_t accessSem;     // Protects waiting count
} barbershop_t;
```

#### Protocol: Customer Entry

1. Acquire `accessSem` (mutex)
2. Check if `customers_waiting >= num_chairs`
   - If YES: Release `accessSem`, leave immediately (balk)
   - If NO: Increment `customers_waiting`, signal `customerSem`, release `accessSem`
3. Wait on `barberSem` (blocked until barber ready)

#### Protocol: Barber

1. Wait on `customerSem` (blocked until customer arrives)
2. Acquire `accessSem`, decrement `customers_waiting`, release `accessSem`
3. Perform haircut
4. Loop to next customer or exit

---

## Synchronization Primitives: Technical Details

### dispatch_semaphore_t (macOS GCD Semaphores)

**Advantages over POSIX sem_t:**
- Modern, well-maintained by Apple
- Integrated with Grand Central Dispatch ecosystem
- Better performance on macOS
- No separate initialization file requirement

**Operations:**
```c
dispatch_semaphore_t sem = dispatch_semaphore_create(initial_count);
dispatch_semaphore_wait(sem, DISPATCH_TIME_FOREVER);
dispatch_semaphore_signal(sem);
```

### pthread_mutex_t (POSIX Mutex)

**Properties:**
- Recursive locking not allowed (non-recursive mutex)
- No priority inheritance in basic implementation
- Fair scheduling (FIFO order for waiters)
- Zero-copy; minimal memory overhead

**Critical Operations:**
```c
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_lock(&mutex);
// Critical section
pthread_mutex_unlock(&mutex);
```

---

## Avoiding Deadlock: Design Principles

### 1. Lock Ordering

**Problem:** Circular wait creates deadlock
```
Thread A: locks M1 → waits for M2
Thread B: locks M2 → waits for M1
Result: Deadlock
```

**Solution:** Always acquire locks in consistent order
```
Thread A: locks M1 → locks M2
Thread B: locks M1 → locks M2
Result: No deadlock (serialized access)
```

### 2. Semaphore Before Mutex Pattern

In producer-consumer:
```c
dispatch_semaphore_wait(empty);      // Check resource availability
pthread_mutex_lock(&mutex);          // Then enter critical section
// Access shared data
pthread_mutex_unlock(&mutex);
dispatch_semaphore_signal(full);     // Signal completion
```

Why it works: Semaphore wait prevents mutex contention; thread only enters critical section when resource is likely available.

### 3. Minimal Critical Sections

```c
// WRONG: Long critical section
pthread_mutex_lock(&mutex);
expensive_computation();              // Can't be parallelized
update_shared_data();
pthread_mutex_unlock(&mutex);

// RIGHT: Compute outside critical section
expensive_computation();
pthread_mutex_lock(&mutex);
update_shared_data();                 // Quick update
pthread_mutex_unlock(&mutex);
```

---

## Thread Safety: Implementation Details

### Thread-Safe Printing Macro

```c
#define THREAD_SAFE_PRINT(fmt, ...) \
    do { \
        pthread_mutex_lock(&print_mutex); \
        printf(fmt, ##__VA_ARGS__); \
        fflush(stdout); \
        pthread_mutex_unlock(&print_mutex); \
    } while(0)
```

**Why needed:** Without synchronization, `printf` output from multiple threads can interleave, producing corrupted output.

### Resource Cleanup

Each module provides a `*_destroy()` function:
```c
void bounded_buffer_destroy(bounded_buffer_t *bb) {
    pthread_mutex_destroy(&bb->mutex);
}
```

Proper cleanup prevents resource leaks and allows graceful shutdown.

---

## Architecture Insights

### Circular Buffer Implementation

Efficient reuse of fixed memory:
```c
in = (in + 1) % size;
out = (out + 1) % size;
```

Advantages:
- No dynamic allocation (predictable latency)
- Cache-friendly (linear memory access)
- O(1) insertion and removal

### Read-First vs Write-First Policies

**Read-First (Current Implementation):**
- Prioritizes readers; writers may starve
- High read throughput
- Suitable for read-dominant workloads (caches, configuration data)

**Write-First Alternative:**
- Prioritizes writers; readers may starve
- Ensures write timeliness
- Suitable for write-critical workloads (logs, updates)

---

## Lessons from Implementation

1. **Semaphores are powerful but subtle** — Easy to deadlock; requires careful ordering
2. **Mutexes don't scale** — Each critical section serializes execution; minimize time inside locks
3. **Counters need protection** — Reading and incrementing a shared counter is NOT atomic
4. **Order matters** — Consistent lock acquisition order prevents deadlock
5. **Test concurrency carefully** — Race conditions may only appear under specific timing; stress test with many threads

---

## Real-World Applications

- **Producer-Consumer:** Message queues (RabbitMQ), thread pools, pipeline stages
- **Readers-Writers:** Database caches, configuration management, read-heavy indexes
- **Cigarette Smokers:** Complex resource allocation with asymmetric requirements
- **Barbershop:** Queue management, resource pooling, capacity control