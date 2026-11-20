"""
Interactive & Automated Robot Car Simulator
Tests the exact Arduino logic for both:
  1. Line Follower Mode
  2. User Follower Mode
"""

import sys

DETECT_LINE = 1   # HIGH
DETECT_WHITE = 0  # LOW

BASE_SPEED = 140
TURN_SPEED = 150

MODE_LINE = "LINE_FOLLOWER"
MODE_USER = "USER_FOLLOWER"

class RobotCar:
    def __init__(self):
        self.mode = MODE_LINE
        self.state = "STOP"
        self.left_motor_speed = 0
        self.right_motor_speed = 0

    def toggle_mode(self):
        self.mode = MODE_USER if self.mode == MODE_LINE else MODE_LINE
        self.stop_motors()
        return self.mode

    def move_forward(self):
        self.state = "MOVING FORWARD"
        self.left_motor_speed = BASE_SPEED
        self.right_motor_speed = BASE_SPEED

    def move_backward(self):
        self.state = "MOVING BACKWARD"
        self.left_motor_speed = BASE_SPEED
        self.right_motor_speed = BASE_SPEED

    def turn_left(self):
        self.state = "TURNING LEFT"
        self.left_motor_speed = -TURN_SPEED
        self.right_motor_speed = TURN_SPEED

    def turn_right(self):
        self.state = "TURNING RIGHT"
        self.left_motor_speed = TURN_SPEED
        self.right_motor_speed = -TURN_SPEED

    def stop_motors(self):
        self.state = "STOPPED"
        self.left_motor_speed = 0
        self.right_motor_speed = 0

    def evaluate_line_follower(self, left_ir, right_ir):
        if left_ir == DETECT_WHITE and right_ir == DETECT_WHITE:
            self.move_forward()
        elif left_ir == DETECT_LINE and right_ir == DETECT_WHITE:
            self.turn_left()
        elif left_ir == DETECT_WHITE and right_ir == DETECT_LINE:
            self.turn_right()
        elif left_ir == DETECT_LINE and right_ir == DETECT_LINE:
            self.stop_motors()
        return self.state

    def evaluate_user_follower(self, distance_cm, left_ir, right_ir):
        if 12 <= distance_cm <= 35:
            if left_ir == 1 and right_ir == 0:
                self.turn_left()
            elif left_ir == 0 and right_ir == 1:
                self.turn_right()
            else:
                self.move_forward()
        elif 0 < distance_cm < 12:
            self.stop_motors()  # Too close
        else:
            self.stop_motors()  # Out of range / too far
        return self.state


def run_automated_tests():
    print("=" * 60)
    print("    RUNNING AUTOMATED TEST SUITE (NO ARDUINO NEEDED)")
    print("=" * 60)

    car = RobotCar()
    all_passed = True

    # Test Line Follower
    car.mode = MODE_LINE
    tests_line = [
        {"desc": "Line centered (White, White)", "left": 0, "right": 0, "expected": "MOVING FORWARD"},
        {"desc": "Car drifted right (Black, White)", "left": 1, "right": 0, "expected": "TURNING LEFT"},
        {"desc": "Car drifted left (White, Black)", "left": 0, "right": 1, "expected": "TURNING RIGHT"},
        {"desc": "End of line/Crossroad (Black, Black)", "left": 1, "right": 1, "expected": "STOPPED"},
    ]

    print("\n--- Testing Mode 1: LINE FOLLOWER ---")
    for t in tests_line:
        action = car.evaluate_line_follower(t["left"], t["right"])
        passed = (action == t["expected"])
        status = "[PASS]" if passed else "[FAIL]"
        if not passed: all_passed = False
        print(f" {status} {t['desc']} -> Action: {action}")

    # Test User Follower
    car.mode = MODE_USER
    tests_user = [
        {"desc": "User at 25cm centered (no IR)", "dist": 25, "l": 0, "r": 0, "expected": "MOVING FORWARD"},
        {"desc": "User at 20cm moved Left (Left IR active)", "dist": 20, "l": 1, "r": 0, "expected": "TURNING LEFT"},
        {"desc": "User at 20cm moved Right (Right IR active)", "dist": 20, "l": 0, "r": 1, "expected": "TURNING RIGHT"},
        {"desc": "User too close (8cm)", "dist": 8, "l": 0, "r": 0, "expected": "STOPPED"},
        {"desc": "User too far away (60cm)", "dist": 60, "l": 0, "r": 0, "expected": "STOPPED"},
    ]

    print("\n--- Testing Mode 2: USER FOLLOWER ---")
    for t in tests_user:
        action = car.evaluate_user_follower(t["dist"], t["l"], t["r"])
        passed = (action == t["expected"])
        status = "[PASS]" if passed else "[FAIL]"
        if not passed: all_passed = False
        print(f" {status} {t['desc']} -> Action: {action}")

    print("\n" + "=" * 60)
    if all_passed:
        print(" RESULT: ALL LOGIC TESTS PASSED SUCCESSFULLY! (100% OK)")
    else:
        print(" RESULT: SOME TESTS FAILED")
    print("=" * 60)


def interactive_mode():
    car = RobotCar()
    print("\nStarting Interactive Test Console:")
    print("Type 'switch' to toggle mode, or enter sensor values.")
    print("Commands:")
    print("  line <left> <right>       e.g.: 'line 0 1' (0=White, 1=Black)")
    print("  user <dist> <left> <right> e.g.: 'user 20 1 0' (Distance in cm)")
    print("  test                      (Run the automated test suite)")
    print("  exit                      (Quit)")

    while True:
        try:
            line = input(f"\n[{car.mode}] > ").strip()
            if not line: continue
            parts = line.split()
            cmd = parts[0].lower()

            if cmd == "exit":
                break
            elif cmd == "switch":
                new_m = car.toggle_mode()
                print(f"Switched mode to: {new_m}")
            elif cmd == "test":
                run_automated_tests()
            elif cmd == "line":
                if len(parts) < 3:
                    print("Usage: line <left_0_or_1> <right_0_or_1>")
                    continue
                l, r = int(parts[1]), int(parts[2])
                act = car.evaluate_line_follower(l, r)
                print(f"-> Robot Action: {act} (Motors: L={car.left_motor_speed}, R={car.right_motor_speed})")
            elif cmd == "user":
                if len(parts) < 4:
                    print("Usage: user <dist_cm> <left_ir_0_or_1> <right_ir_0_or_1>")
                    continue
                d, l, r = int(parts[1]), int(parts[2]), int(parts[3])
                act = car.evaluate_user_follower(d, l, r)
                print(f"-> Robot Action: {act} (Motors: L={car.left_motor_speed}, R={car.right_motor_speed})")
            else:
                print("Unknown command. Type 'test', 'line 0 0', 'user 25 0 0', or 'exit'.")
        except Exception as e:
            print("Error parsing input:", e)

if __name__ == "__main__":
    if len(sys.argv) > 1 and sys.argv[1] == "--interactive":
        interactive_mode()
    else:
        run_automated_tests()
        print("\nTip: Run 'python simulate.py --interactive' to manually test any values!")
