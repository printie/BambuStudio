---
summary: "Headless CLI slicing fixes and build notes."
read_when:
  - When debugging Bambu Studio CLI slicing crashes.
  - When rebuilding Bambu Studio for CLI usage.
---

# CLI Slicing Fixes (Headless)

## Symptoms
- CLI slicing of some `.3mf` files crashed with errors like:
  - `3207: nozzle_volume_type not found ... new_printer_name , extruder_count 1`
- The CLI hit GUI-only code paths when run headless, leading to null access.

## Fixes Applied
- `src/slic3r/GUI/PartPlate.cpp`
  - Guard GUI-only `wxGetApp()`/`plater` usage in `generate_print_polygon` and `get_right_icon_offset_bed`.
  - Skip icon/gridline generation in `PartPlateList::set_shapes` when no GUI is present.
- `src/libslic3r/PrintObject.cpp`
  - Guard `print()` null access in `calc_estimated_filament_print_time`.
  - Handle missing filament options before dereferencing (`filament_extruder_variant`, `filament_self_index`,
    `filament_max_volumetric_speed`, `filament_flow_ratio`).
- `src/libslic3r/Fill/FillFloatingConcentric.cpp`
  - Guard missing width entries when interpolating/merging lines to avoid out-of-range access.

## Build Notes (macOS)
- Release build: `./BuildMac.sh -s`
- CLI binary path:
  - `build/arm64/src/Release/BambuStudio.app/Contents/MacOS/BambuStudio`

## CLI Usage
Example:
```
build/arm64/src/Release/BambuStudio.app/Contents/MacOS/BambuStudio \
  --allow-newer-file \
  --no-check \
  --slice 0 \
  --export-3mf /tmp/out.sliced.3mf \
  /path/to/input.3mf
```

### Faster multi-plate slicing
- New CLI option: `--skip-precheck`
- Behavior:
  - Default (`false`): with `--slice 0`, CLI runs a validation-only pre-check pass over all plates, then runs slicing.
  - Enabled (`true`): skips that pre-check pass and slices directly.
- Why:
  - The pre-check pass duplicates expensive per-plate setup for multi-plate projects.
  - For grams-estimation workflows, skipping pre-check cuts wall time.

Example:
```
build/arm64/src/Release/BambuStudio.app/Contents/MacOS/BambuStudio \
  --allow-newer-file \
  --no-check \
  --skip-precheck \
  --slice 0 \
  --outputdir /tmp/slice-out \
  --export-slicedata plates \
  /path/to/input.3mf
```

### Estimate-only fast path
- New CLI option: `--skip-gcode-export`
- Behavior:
  - Skips writing `plate_*.gcode` files after slicing.
  - Keeps slicing, validation/apply, and print statistics generation.
  - Intended for workflows that only need grams/timing metadata from `result.json`.

Example:
```
build/arm64/src/Release/BambuStudio.app/Contents/MacOS/BambuStudio \
  --allow-newer-file \
  --no-check \
  --skip-gcode-export \
  --slice 0 \
  --outputdir /tmp/slice-out \
  /path/to/input.3mf
```

## Tech Debt
- `--no-check` is currently required to avoid upstream CLI crashes. Remove once the upstream issue is resolved.
