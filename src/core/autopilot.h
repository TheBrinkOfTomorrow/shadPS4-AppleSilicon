// SPDX-FileCopyrightText: Copyright 2026 Driveclub-PC contributors
// SPDX-License-Identifier: GPL-2.0-or-later

// Scripted controller input for unattended test runs, from Driveclub-PC's autopilot.
//
// SHADPS4_AUTOPILOT holds a schedule of steps "<seconds>:<action>" separated by ';', timed
// from the start of the emulator. Actions:
//   press:<button>[:<ms>]   press and release (default 150 ms)
//   hold:<button>           press and keep pressed
//   release:<button>
//   axis:<axis>:<value>     set an axis (0..255; sticks centre at 128)
//   repeat:<seconds>:<action>  from now on, also run <action> every <seconds>
// Buttons: cross circle square triangle up down left right l1 r1 l2 r2 l3 r3 options touchpad.
// Axes: lx ly rx ry l2 r2.
// The input goes to the emulated pad of player 1, exactly like a physical controller's;
// nothing is sent to the host's windows or input devices. While a schedule runs, the game's
// flip rate is logged every 5 seconds, for comparing performance between runs.
//
// Example: "45:press:cross;60:hold:r2;90:repeat:20:press:cross"

#pragma once

namespace Core::Autopilot {

/// Starts the schedule thread when SHADPS4_AUTOPILOT is set. Call after the controllers exist.
void Start();

} // namespace Core::Autopilot
