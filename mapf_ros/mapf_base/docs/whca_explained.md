# Understanding WHCA* from Scratch

## The Problem: Robots Bumping Into Each Other

Imagine a warehouse with 8 robots. Each knows where it wants to go. If they each plan independently (like using Google Maps separately), their paths will cross and they'll collide.

We need a way to plan paths for ALL robots simultaneously, so no two ever occupy the same spot at the same time.

## Why Not Just Plan Everything at Once?

The "correct" solution is to search the joint state space — every possible combination of positions for all robots at every time step. For 8 robots on a 30x20 grid, that's 600^8 possible states per time step. Utterly impossible.

This is why optimal solvers like CBS take exponential time as robots increase.

## WHCA*: The Key Insight

**Windowed Hierarchical Cooperative A*** breaks the problem into two ideas:

### Idea 1: Reservation Table (the "Cooperative" part)

Instead of planning in joint space, robots plan one at a time in priority order. When a robot plans its path, it "reserves" every cell it will occupy at each time step — like booking a meeting room.

The next robot plans around those reservations. It treats reserved cells as temporarily blocked walls.

```
Time step 0:  Robot A at (2,3) — reserved
Time step 1:  Robot A at (2,4) — reserved
Time step 2:  Robot A at (2,5) — reserved

Robot B now plans, seeing:
  (2,3) blocked at t=0
  (2,4) blocked at t=1
  (2,5) blocked at t=2

So Robot B waits, or takes a detour.
```

### Idea 2: Windowed Planning (the "Windowed" part)

Planning a complete path from start to goal for all robots is still expensive. WHCA* only plans W steps ahead (the "window"). After executing those steps, it replans from the new positions.

Think of it like driving with headlights at night — you can only see 100m ahead, but that's enough to make progress safely.

```
Window size = 10:
  - Plan 10 steps for Robot A → reserve those cells
  - Plan 10 steps for Robot B (avoiding A's reservations)
  - Plan 10 steps for Robot C (avoiding A's and B's)
  - Execute all 10 steps
  - Repeat from new positions
```

### Idea 3: Priority Order (the "Hierarchical" part)

Which robot plans first matters enormously. The first robot has total freedom. The last robot must dodge everyone else.

In the simplest version, priority is fixed (robot 0 always goes first). Smarter versions rotate priorities each window so no robot is always disadvantaged.

## How a Single Robot Plans (within the window)

Each robot runs A* — but in 3D space-time instead of 2D space:

```
Normal A*:  state = (x, y)
WHCA* A*:   state = (x, y, t)
```

Neighbors of (x, y, t) are:
- (x+1, y, t+1) — move right
- (x-1, y, t+1) — move left
- (x, y+1, t+1) — move up
- (x, y-1, t+1) — move down
- (x, y, t+1) — **wait** in place

A cell is blocked if:
1. It's a wall (static obstacle), OR
2. Another robot reserved it at time t (collision), OR
3. Two robots would swap positions between t and t+1 (edge collision)

The heuristic is still Manhattan distance (or true distance ignoring other robots), so A* remains efficient.

## Why It Can Fail

WHCA* is **incomplete** — it doesn't always find a solution even when one exists.

### Failure mode: The narrow corridor swap

```
    Robot A →  ████  ← Robot B
               ████
```

If A and B must swap through a single-width corridor, whoever plans first claims the corridor. The second robot has nowhere to go. Neither can "back up" because the window already committed them forward.

An optimal solver would find that one robot should detour around. WHCA* with fixed priorities might not discover this.

### Failure mode: Window too short

If the window is 10 steps but the detour requires 15 steps of coordination, the planner can't "see" far enough ahead to find the solution.

### Failure mode: Too many robots in tight space

With 8 robots in narrow aisles all trying to cross, even with a large window, the priority ordering creates cascading constraints that leave later robots with no viable path.

## Tuning WHCA*

| Parameter | Effect of increasing |
|-----------|---------------------|
| Window size | Finds harder solutions, but exponentially slower |
| Timeout | More search time, but diminishing returns |
| Fewer robots | Dramatically easier (each robot adds constraints for all others) |
| Wider aisles | More routing options, fewer deadlocks |
| Shorter distances | Less overlap in time, fewer conflicts |

## WHCA* vs Other MAPF Algorithms

| Algorithm | Optimal? | Complete? | Speed | Robots |
|-----------|----------|-----------|-------|--------|
| CBS | Yes | Yes | Slow (exponential) | ~10-20 |
| ECBS | Bounded | Yes | Medium | ~20-50 |
| WHCA* | No | No | Fast | ~50-100+ |
| Priority SIPP | No | No | Very fast | ~100+ |

WHCA* trades solution quality and completeness for speed. It's the algorithm you use when you need answers fast and the environment is cooperative enough (wide aisles, not too adversarial).

## Mental Model

Think of WHCA* as robots taking turns booking a shared calendar:

1. Robot 0 books conference rooms (grid cells) for the next hour (window)
2. Robot 1 looks at what's available and books around Robot 0
3. Robot 2 works around both
4. ...
5. After the hour passes, everyone rebooks from their current positions

It's polite, fast, and usually works — but sometimes the last person to book can't find any room at all.
