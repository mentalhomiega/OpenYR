#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-or-later
#
# Runs mapinfo on the hand-written sample map and checks the decoded fields it must print.
# Usage: check_mapinfo.py <mapinfo executable> <sample.map>

import json
import subprocess
import sys


def run(exe, *args):
    return subprocess.run([exe, *args], capture_output=True, text=True)


def fail(message):
    print("check_mapinfo: " + message)
    sys.exit(1)


def expect(condition, message):
    if not condition:
        fail(message)


def main(argv):
    exe, sample = argv[0], argv[1]

    result = run(exe, "--json", sample)
    expect(result.returncode == 0, "--json exited %d" % result.returncode)
    data = json.loads(result.stdout)

    expect(data["map"]["player"] == "Americans", "player house")
    trigger = data["triggers"][0]
    expect(trigger["id"] == "0AB" and trigger["name"] == "Rocket Trigger", "trigger id and name")
    expect(trigger["house"] == "Americans" and trigger["linked"] == "" and trigger["disabled"] is False, "trigger house, link, disabled flag")
    expect(trigger["tags"][0]["id"] == "0D1" and trigger["tags"][0]["repeat"] == "Volatile", "tag attached to trigger")
    event = trigger["events"][0]
    expect(event["number"] == 4 and event["name"] == "DISCOVERED", "DISCOVERED event")
    change = trigger["actions"][0]
    expect(change["name"] == "CHANGE_HOUSE" and change["params"]["house"] == "Russians", "CHANGE_HOUSE target")
    expect(change["params"]["waypoint"] == "A cell (3,2)", "waypoint letter to cell")
    sound = trigger["actions"][1]
    expect(sound["name"] == "PLAY_SOUND" and sound["params"]["sound"] == "Bonus", "sound name")
    disable = trigger["actions"][2]
    expect(disable["params"]["trigger"] == "Linked Trigger (0AC)", "trigger reference")

    expect(data["tags"][0]["name"] == "Rocket Tag", "tag name")
    expect(data["teams"][0]["script"] == "0E2" and data["teams"][0]["name"] == "Test Team", "team script ID and name")
    expect(data["scripts"][0]["steps"][0]["name"] == "MOVE", "script step name")
    expect(data["scripts"][0]["steps"][0]["params"]["waypoint"] == "A cell (3,2)", "script waypoint")
    expect(data["taskForces"][0]["members"][0] == {"count": 2, "type": "HTNK"}, "task force member")
    carriers = data["tagCarriers"][0]["carriers"]
    expect(len(carriers) == 2 and carriers[0]["type"] == "HTNK", "reverse index lists the tag's carriers")
    expect(data["units"][0]["tagName"] == "Rocket Tag (0D1)", "unit carries the tag")
    expect(data["structures"][0]["tag"] == "", "structure without a tag")

    found = run(exe, "--find", "Rocket", sample)
    expect(found.returncode == 0 and "Triggers (1 of 2 match)" in found.stdout, "--find keeps matching triggers")
    found = run(exe, "--find", "HTNK", sample)
    expect("Task forces (1 of 1 match)" in found.stdout and "Starting units (1 of 1 match)" in found.stdout, "--find matches a type")

    missing = run(exe, sample + ".missing")
    expect(missing.returncode == 2, "a missing file exits with 2")

    print("check_mapinfo: ok")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
