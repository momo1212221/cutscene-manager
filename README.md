# Cutscene Manager

Conservative cutscene handling for RAC1.

Modes:
- `0` — automatic skipping disabled.
- `1` — safe-repeat mode.

A scene becomes eligible for repeat skipping only after it has completed naturally and no level/load transition follows during the safety window. Level-ending and progression-triggering scenes are therefore classified as unsafe and are never auto-skipped.

Default mode is `0`.

Better Pause Menu / QoL Hub can display the current configuration; the safety logic remains in this standalone background mod.

No game assets or disc files are included.

Built for OpenRAC RAC1 PAL/NTSC-U. AI-assisted implementation. GPL-3.0-or-later.
