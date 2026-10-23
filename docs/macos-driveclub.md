# Driveclub on macOS (Apple Silicon, Rosetta 2)

Notes for the `macos-driveclub` branch: Driveclub (CUSA00093, update 1.28) on an x86_64 build
of shadPS4 running under Rosetta 2.

## Build and run

Build as x86_64 (see [building-macos.md](building-macos.md)):

```
cmake -S . -B build-x86 -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_ARCHITECTURES=x86_64
cmake --build build-x86
./build-x86/shadps4 -g <folder>/CUSA00093
```

Put the 1.28 update next to the base game as `CUSA00093-patch` (or `-UPDATE`). No patch file is
needed: fixed mappings in the range Rosetta reserves for itself are moved by the memory manager.

## Game config

Copy [macos-driveclub/CUSA00093.json](macos-driveclub/CUSA00093.json) to
`~/Library/Application Support/shadPS4/custom_configs/`. It turns on red-zone protection
(`redzone_patches`): Driveclub keeps live data below the stack pointer, and without it a memory
fault handled by the emulator can overwrite that data (found by Driveclub-PC).

GPU readbacks (`GPU.readbacks_mode`) are left off. Relaxed and Precise both halve the race frame
rate under Rosetta (see below). Driveclub-PC uses Precise; with readbacks off, some lighting and
shadows may be wrong.

## Measuring

`SHADPS4_AUTOPILOT` scripts the pad and logs the game's frame rate every 5 seconds:

```
SHADPS4_AUTOPILOT="45:press:cross;60:press:cross;75:hold:r2" ./build-x86/shadps4 -g <folder>/CUSA00093
```

Cross at 45 s starts the event on the title screen, the race loads, and R2 is held from 75 s. The
"race" figure below is the mean of the 5-second samples from 100 s to the end of the run. Driveclub
caps itself at 30 FPS.

## Results

Mac mini, M4 Pro, 64 GB, macOS 27.

| Change | Front end FPS | Race FPS | Notes |
|---|---|---|---|
| Baseline (`macos-driveclub` with the memory fix) | 30 | 27.5 | two short dips to ~10 |
| + red-zone protection | 30 | 29.5 | no measurable cost |
| + red-zone, readbacks Relaxed | 30 | 16.0 | |
| + red-zone, readbacks Precise | 30 | 16.1 | |
| + red-zone, readbacks Precise, QoS for game main and GPU command processor | 30 | 16.4 | within noise |

Profile of a race on the earlier reference build (Time Profiler and Metal System Trace, 25 s): the
GPU is busy 57 % of the time. The game's main thread uses 91 % of a core (81 % of it in the game's
own translated code) and the GPU command processor 82 %. A guest WebAPI thread spins on a
semaphore at 100 % of a core.

With Precise readbacks the same threads are mostly idle (game main thread 42 %, command
processor 44 % of a core): each readback makes the CPU wait for a GPU round trip, so that case is
bound by synchronisation, not by CPU time.

## Driveclub-PC changes

[Driveclub-PC](https://github.com/ondrejeder/Driveclub-pc-wrapper) (a Windows fork of GR2fork) lists its
changes in `driveclub-pc/CHANGES.md`. Status on this branch:

| Change | Status |
|---|---|
| Autopilot (scripted input) | ported, with a frame-rate log; screenshots left out (need RenderDoc) |
| Red-zone protection | upstream option, on in the game config |
| Precise readbacks by default | upstream option, left off (halves the race frame rate here) |
| Fast guest mutexes | not needed: Windows-only, replaces a kernel mutex object; on macOS the guest mutex is a pthread mutex whose uncontended path stays in user space |
| Separate queue-submit thread | not ported: queue submits do not show up in the command processor's profile here |
| Texture GC skip fix | already in upstream: skipped images no longer cost a deletion |
| P-core pinning | macOS equivalent: user-interactive QoS, done |
