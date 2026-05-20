#!/usr/bin/env python3
"""
WHCA* Demo: 8-robot warehouse scenario.
Robots on opposite sides must swap through narrow aisles.

Usage:
  # Terminal 1: Launch WHCA* system
  ros2 launch mapf_base whca_warehouse.launch.py

  # Terminal 2: Run this demo
  python3 scripts/whca_demo.py
"""
import subprocess
import sys
import threading
import time


def pub(topic: str, msg_type: str, data: str) -> None:
    cmd = ["ros2", "topic", "pub", topic, msg_type, data, "--times", "2", "-r", "1"]
    subprocess.run(cmd, capture_output=True, timeout=10)


def pub_once(topic: str, msg_type: str, data: str) -> None:
    cmd = ["ros2", "topic", "pub", topic, msg_type, data, "--once"]
    subprocess.run(cmd, capture_output=True, timeout=10)


def wait_for_topic(topic: str, result: dict, timeout: float = 120.0) -> None:
    """Wait for a message on a topic using ros2 topic echo --once."""
    try:
        r = subprocess.run(
            ["ros2", "topic", "echo", topic, "--once", "--no-arr"],
            capture_output=True,
            text=True,
            timeout=timeout,
        )
        result["stdout"] = r.stdout
        result["ok"] = r.returncode == 0 and len(r.stdout.strip()) > 0
    except subprocess.TimeoutExpired:
        result["ok"] = False


def main() -> None:
    print("=" * 60)
    print("WHCA* Warehouse Dispatch Demo")
    print("=" * 60)
    print()
    print("Map: 30x20 grid (45m x 30m), dense shelf layout")
    print("Window size: 48 time steps")
    print()
    print("Scenario: Robots depart from 2 charging stations to pick items")
    print("  Station A (bottom-left):  4 robots stacked")
    print("  Station B (bottom-right): 4 robots stacked")
    print("  Each robot dispatched to a different aisle/shelf location")
    print()

    # Two charging stations — robots clustered at adjacent cells
    # Station A: bottom-left area (cells around col 1-2, row 1-2)
    # Station B: bottom-right area (cells around col 27-28, row 1-2)
    starts = [
        # Station A — 4 robots in adjacent cells
        (1.5, 1.5),   # cell (1,1)
        (1.5, 3.0),   # cell (1,2)
        (3.0, 1.5),   # cell (2,1)
        (3.0, 3.0),   # cell (2,2)
        # Station B — 4 robots in adjacent cells
        (40.5, 1.5),  # cell (27,1)
        (40.5, 3.0),  # cell (27,2)
        (39.0, 1.5),  # cell (26,1)
        (39.0, 3.0),  # cell (26,2)
    ]

    # Goals: robots cross to the OPPOSITE side of the warehouse
    # Station A robots must cross to far-right aisles
    # Station B robots must cross to far-left aisles
    # This forces paths to tangle in the middle
    goals = [
        # Station A (bottom-left) robots -> far right side
        (40.5, 25.5),  # robot_0: cross entire map to top-right
        (37.5, 13.5),  # robot_1: cross to right-mid aisle
        (40.5, 13.5),  # robot_2: cross to far-right mid
        (33.0, 25.5),  # robot_3: cross to right, top section
        # Station B (bottom-right) robots -> far left side
        (1.5, 25.5),   # robot_4: cross entire map to top-left
        (7.5, 13.5),   # robot_5: cross to left-mid aisle
        (1.5, 13.5),   # robot_6: cross to far-left mid
        (7.5, 25.5),   # robot_7: cross to left, top section
    ]

    print("Dispatching from charging stations to pick locations:")
    for i, ((sx, sy), (gx, gy)) in enumerate(zip(starts, goals)):
        station = "A" if i < 4 else "B"
        print(f"  robot_{i} [Station {station}]: ({sx:.1f}, {sy:.1f}) -> ({gx:.1f}, {gy:.1f})")
    print()
    print("-" * 60)

    # Step 1: Start listening for the plan BEFORE sending goals
    print("\n[1/4] Subscribing to /mapf/global_plan...")
    result: dict = {"ok": False, "stdout": ""}
    listener = threading.Thread(
        target=wait_for_topic,
        args=("/mapf/global_plan", result, 120.0),
    )
    listener.start()
    time.sleep(2)  # Give subscriber time to establish

    # Step 2: Send goals
    print("\n[2/4] Sending goals...")
    pose_tmpl = (
        "{{header: {{frame_id: 'map'}}, "
        "pose: {{position: {{x: {x}, y: {y}, z: 0.0}}, "
        "orientation: {{w: 1.0}}}}}}"
    )

    for i, (gx, gy) in enumerate(goals):
        topic = f"/mapf/robot_{i}/goal"
        data = pose_tmpl.format(x=gx, y=gy)
        pub(topic, "geometry_msgs/msg/PoseStamped", data)
        print(f"  robot_{i} goal: ({gx:.1f}, {gy:.1f})")

    time.sleep(1)

    # Step 3: Trigger planning
    print("\n[3/4] Triggering WHCA* planning...")
    print("       (iterative replanning — may take several seconds)")
    t0 = time.time()
    pub_once("/mapf/goal_init_flag", "std_msgs/msg/Bool", "{data: true}")

    # Step 4: Wait for the plan to be published
    print("\n[4/4] Waiting for plan (up to 2 minutes)...")
    listener.join(timeout=120)
    elapsed = time.time() - t0

    if result["ok"]:
        print(f"\n{'=' * 60}")
        print(f"SUCCESS - WHCA* solved 8-robot scenario in {elapsed:.1f}s!")
        print(f"{'=' * 60}")
        # Extract makespan from output
        for line in result["stdout"].split("\n"):
            if "makespan" in line.lower():
                print(f"  {line.strip()}")
                break
        print()
        print("Robots are now animating in RViz.")
    else:
        print(f"\n  No plan received after {elapsed:.1f}s.")
        print("  Check the launch terminal for planner output.")
        print("  The planner may have succeeded but the subscriber missed it.")
        print()
        print("  Tip: check if the plan was published:")
        print("    ros2 topic echo /mapf/global_plan --once")
        sys.exit(1)


if __name__ == "__main__":
    main()
