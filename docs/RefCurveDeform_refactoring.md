# RefCurveDeform: YAML export, curve normals and MHA mesh import

## UI and API names

| Previous button | New button | Mode method |
| --- | --- | --- |
| Export config | Export YAML config... | `ExportDeformationConfig` |
| Draw normals | Toggle curve normals | `ToggleSelectedCurveNormals` |
| Flip normals | Flip curve normals | `FlipSelectedCurveNormals` |
| Load MHA | Import MHA meshes... | `ImportMeshSequenceFromMha` |

Normal actions affect the selected curve. For a shared curve, they affect every
frame. Toggling visibility changes display only; flipping direction changes the
deformation constraints and can now be undone.

MHA import generates meshes from masks. It keeps the existing curve constraints;
it does not replace the loaded image sequence. Files are assigned to frames in
numeric file-name order (`frame1`, `frame2`, `frame10`). `Finish Segmentation`
applies the imported mesh sequence to the selected mask, including frames without
curve constraints.

## Bugs fixed

- YAML export omitted shared curves. It now includes their interpolated curves
  in every frame, matching the constraints used by `DeformAllFrame`.
- Apostrophes in folder paths produced invalid YAML. Paths are escaped, folder
  strings are encoded as UTF-8, and an empty curve collection is written as `[]`
  rather than YAML null. Coordinates retain float round-trip precision regardless
  of the process locale.
- Export allocated three unmanaged path strings without freeing them. Path
  conversion now uses owned strings, and dialogs have scoped disposal. Export
  errors are returned to the form and displayed; invalid curve data is checked
  before an existing output file can be truncated.
- Normal flips were absent from Undo history. Standard and shared curve flips
  now record snapshots. Drawing and deformation now use the same normal
  calculation, with checks for degenerate tangents and non-finite normals.
- The MHA reader treated compressed `MET_SHORT` data as `MET_UCHAR`. It now
  allocates the correct element type and requires the exact decompressed size.
  Previously a short decompressed payload could leave unread voxels in the buffer.
- Invalid/missing MHA dimensions could reach allocation without validation. The
  parser now rejects invalid sizes, overflow, unsupported types/byte order,
  multiple channels, and truncated or invalid compressed payloads. Optional
  spacing defaults to `(1, 1, 1)`.
- Mesh import accepted mismatched voxel spacing and could silently create empty
  meshes when binary foreground labels differed from the selected mask ID. It
  now checks dimensions and spacing, accepts byte and short labels in `[0, 255]`,
  and maps single-label binary masks (`1` or `255`) to the target ID when needed.
- Mesh replacement retained debug vertex indices from the previous topology,
  which could cause out-of-range access while showing matching lines. Both mesh
  generation paths now clear those indices when replacing the meshes.
- Imported meshes were skipped by `Finish Segmentation` in frames with no curves.
  The mode now tracks imported meshes separately so those frames are applied too.
- An empty mesh in the first frame prevented deformation in every other frame.
  The guard now checks the requested frame. Related mesh access before generation
  is guarded, and mode restart clears meshes from the preceding session.
- Parallel deformation wrote the modification flag concurrently. Writes now use
  an OpenMP critical section.

## Structure

`RefCurveDeformData.h` contains the YAML serialization, mask sampling and numeric
file-name comparison. `MhaReader.h` parses MHA data independently of file opening.
The form handles dialogs and text encoding; the mode validates imported masks and
builds meshes. `RebuildMeshSequence` is shared by mask-to-mesh creation and MHA
import. New meshes and debug arrays are prepared before replacing the current
state, so read or generation failures preserve the existing mesh sequence.

The YAML keys and point layout remain compatible with the previous export:
`data`, `volume`, and `curves`, with points stored as `[x, y, z, frame]`. Normal
visibility and normal-side flags are not additional YAML fields.

## Validation

Build the application with Visual Studio, Release / x64. Then run:

```text
python RoiPainter4D/tests/run_refcurve_data_tests.py
```

The native tests require MinGW `g++` on PATH and the application's built
`zlib1.dll`. They cover actual production data helpers and the MHA parser,
including compressed/uncompressed 8/16-bit fixtures, malformed/truncated input,
YAML escaping/precision/shared frames, sampling boundaries and file ordering.
If PyYAML is installed, the generated YAML is also parsed and checked.

Additional isolated tests exercised the actual mode method bodies with UI,
file-reader and mesher stubs: standard/shared normal flips and Undo, visibility
independence, label mapping, frame ordering, import failures, mesh replacement,
and applying imported meshes on Finish without curves. The previous nine
selection/frame-switch regression groups also passed.

Full interaction with real image data and OpenGL rendering has not been tested.
MHA import supports local little-endian scalar data. It does not transform
`TransformMatrix`/`Offset` coordinates or resample masks; masks must use the loaded
image's axis convention, dimensions and spacing.
