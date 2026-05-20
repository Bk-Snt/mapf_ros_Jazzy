#!/usr/bin/env python3
"""
WHCA* Stress Test: Progressive difficulty to find the planner's limits.

Tests scenarios from easy to hard:
  Level 1: 4 robots, short moves (baseline)
  Level 2: 6 robots, moderate distances
  Level 3: 8 robots, non-crossing goals
  Level 4: 8 robots, partial swap (4 cross)
  Level 5: 8 robots, full swap (hardest)

Usage:
  # Terminal 1: Launch with generous timeout
  ros2 launch mapf_base whca_warehouse.launch.py window_size:=48

  # Terminal 2: Run stress test
  python3 scripts/whca_stress_test.py [level]
"""
import subprocess
import sys
import threading
import time
from dataclasses import dataclass
from typing import Optional


@dataclass(frozen=True)
class Scenario:
    name: str
    description: str
    num_robots: int
    starts: list[tuple[float, float]]
    goals: list[tuple[float, float]]
    suggested_window: int
    suggested_timeout: float


# All positions verified as free cells in the 30x20 complex warehouse map
# Free aisles at columns: 1, 2, 5, 6, 9, 10, 13, 17, 18, 21, 22, 25, 26, 27, 28
# Free rows: 1, 2, 9, 10, 17, 18 (between shelf blocks and borders)
# Robot start positions match the launch file (8 robots)
LAUNCH_STARTS = [
    (1.5, 1.5), (7.5, 1.5), (13.5, 1.5), (28.5, 1.5),
    (1.5, 27.0), (7.5, 27.0), (13.5, 27.0), (28.5, 27.0),
]

SCENARIOS = [
    Scenario(
        name="Level 1: 4 robots, short moves",
        description="4 robots move short distances in open aisles (baseline)",
        num_robots=4,
        starts=LAUNCH_STARTS[:4],
        goals=[(1.5, 13.5), (7.5, 13.5), (13.5, 13.5), (28.5, 13.5)],
        suggested_window=16,
        suggested_timeout=10.0,
    ),
    Scenario(
        name="Level 2: 6 robots, long parallel",
        description="6 robots traverse full map length in parallel aisles",
        num_robots=6,
        starts=LAUNCH_STARTS[:6],
        goals=[
            (1.5, 27.0), (7.5, 27.0), (13.5, 27.0),
            (28.5, 27.0), (1.5, 13.5), (7.5, 13.5),
        ],
        suggested_window=24,
        suggested_timeout=15.0,
    ),
    Scenario(
        name="Level 3: 8 robots, staggered goals (no head-on)",
        description="8 robots move to shuffled positions — some crossing but no direct swap",
        num_robots=8,
        starts=LAUNCH_STARTS,
        goals=[
            # Bottom robots go to various top positions (shifted)
            (7.5, 27.0), (13.5, 27.0), (28.5, 27.0), (1.5, 27.0),
            # Top robots go to various bottom positions (shifted)
            (7.5, 1.5), (13.5, 1.5), (28.5, 1.5), (1.5, 1.5),
        ],
        suggested_window=32,
        suggested_timeout=30.0,
    ),
    Scenario(
        name="Level 4: 8 robots, half swap",
        description="Left 4 robots swap sides, right 4 stay put — partial congestion",
        num_robots=8,
        starts=LAUNCH_STARTS,
        goals=[
            # robot 0-1 swap with robot 4-5 (left side crosses)
            (1.5, 27.0), (7.5, 27.0), (13.5, 13.5), (28.5, 13.5),
            (1.5, 1.5), (7.5, 1.5), (13.5, 13.5), (28.5, 13.5),
        ],
        suggested_window=48,
        suggested_timeout=45.0,
    ),
    Scenario(
        name="Level 5: 8 robots, full swap (maximum stress)",
        description="ALL 8 robots swap sides — hardest possible for WHCA*",
        num_robots=8,
        starts=LAUNCH_STARTS,
        goals=[
            (1.5, 27.0), (7.5, 27.0), (13.5, 27.0), (28.5, 27.0),
            (1.5, 1.5), (7.5, 1.5), (13.5, 1.5), (28.5, 1.5),
        ],
        suggested_window=64,
        suggested_timeout=60.0,
    ),
]


def pub(topic: str, msg_type: str, data: str) -> None:
    cmd = ["ros2", "topic", "pub", topic, msg_type, data, "--times", "2", "-r", "1"]
    subprocess.run(cmd, capture_output=True, timeout=10)


def pub_once(topic: str, msg_type: str, data: str) -> None:
    cmd = ["ros2", "topic", "pub", topic, msg_type, data, "--once"]
    subprocess.run(cmd, capture_output=True, timeout=10)


def echo_topic(topic: str, result: dict, timeout: float) -> None:
    try:
        r = subprocess.run(
            ["ros2", "topic", "echo", topic, "--once", "--no-arr"],
            capture_output=True,
            text=True,
            timeout=timeout,
        )
        result["stdout"] = r.stdout
        result["ok"] = True
    except subprocess.TimeoutExpired:
        result["ok"] = False


def run_scenario(scenario: Scenario) -> tuple[bool, float]:
    """Run a single scenario. Returns (success, elapsed_time)."""
    print(f"\n{'=' * 60}")
    print(f"  {scenario.name}")
    print(f"  {scenario.description}")
    print(f"  Robots: {scenario.num_robots} | Window: {scenario.suggested_window}")
    print(f"{'=' * 60}")

    for i in range(scenario.num_robots):
        sx, sy = scenario.starts[i]
        gx, gy = scenario.goals[i]
        print(f"  robot_{i}: ({sx:.1f},{sy:.1f}) -> ({gx:.1f},{gy:.1f})")

    # Subscribe before sending goals
    result: dict = {"ok": False, "stdout": ""}
    timeout = scenario.suggested_timeout + 15
    listener = threading.Thread(
        target=echo_topic, args=("/mapf/global_plan", result, timeout)
    )
    listener.start()
    time.sleep(1)

    # Send goals
    pose_tmpl = (
        "{{header: {{frame_id: 'map'}}, "
        "pose: {{position: {{x: {x}, y: {y}, z: 0.0}}, "
        "orientation: {{w: 1.0}}}}}}"
    )
    for i in range(scenario.num_robots):
        gx, gy = scenario.goals[i]
        topic = f"/mapf/robot_{i}/goal"
        data = pose_tmpl.format(x=gx, y=gy)
        pub(topic, "geometry_msgs/msg/PoseStamped", data)

    time.sleep(0.5)

    # Trigger planning
    t0 = time.time()
    pub_once("/mapf/goal_init_flag", "std_msgs/msg/Bool", "{data: true}")

    # Wait for result
    listener.join(timeout=timeout)
    elapsed = time.time() - t0

    success = result["ok"] and "makespan" in result.get("stdout", "")
    if success:
        print(f"\n  PASSED in {elapsed:.1f}s")
        # Extract makespan from output
        for line in result["stdout"].split("\n"):
            if "makespan" in line:
                print(f"  {line.strip()}")
                break
    else:
        print(f"\n  FAILED after {elapsed:.1f}s")

    return success, elapsed


def main() -> None:
    print("=" * 60)
    print("  WHCA* Progressive Stress Test")
    print("=" * 60)
    print()
    print("This test runs increasingly difficult scenarios to find")
    print("where WHCA* breaks down on the complex warehouse map.")
    print()
    print("NOTE: The launch file must be running with enough agents.")
    print("  ros2 launch mapf_base whca_warehouse.launch.py window_size:=48")
    print()

    # Select level(s)
    if len(sys.argv) > 1:
        try:
            level = int(sys.argv[1])
            if 1 <= level <= 5:
                scenarios = [SCENARIOS[level - 1]]
            else:
                print(f"Invalid level {level}. Use 1-5.")
                sys.exit(1)
        except ValueError:
            print("Usage: python3 whca_stress_test.py [level]")
            print("  level: 1-5 (default: run all until failure)")
            sys.exit(1)
    else:
        scenarios = SCENARIOS

    results: list[tuple[str, bool, float]] = []

    for scenario in scenarios:
        success, elapsed = run_scenario(scenario)
        results.append((scenario.name, success, elapsed))

        if not success and len(scenarios) > 1:
            print(f"\n  Stopping — WHCA* limit reached at this level.")
            break

        # Brief pause between scenarios to let system settle
        if scenario != scenarios[-1]:
            print("\n  Waiting 5s before next scenario...")
            time.sleep(5)

    # Summary
    print(f"\n\n{'=' * 60}")
    print("  RESULTS SUMMARY")
    print(f"{'=' * 60}")
    for name, success, elapsed in results:
        status = "PASS" if success else "FAIL"
        print(f"  [{status}] {name} ({elapsed:.1f}s)")

    passed = sum(1 for _, s, _ in results if s)
    total = len(results)
    print(f"\n  {passed}/{total} scenarios passed")

    if passed < total:
        print("\n  To push further, try:")
        print("    - Increase window_size (48, 64, 96)")
        print("    - Increase planner timeout in params")
        print("    - Use a simpler map layout:")
        print("      python3 maps/warehouse.py simple")
        sys.exit(1)
    else:
        print("\n  All scenarios passed! WHCA* handled everything.")


if __name__ == "__main__":
    main()
