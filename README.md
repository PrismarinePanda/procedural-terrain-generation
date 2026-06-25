# Procedural Terrain Generation

A high school independent study on procedurally generating unique 3D terrain in C++ — mountains, oceans, beaches, skies, and trees — built around the textbook *Texturing and Modeling: A Procedural Approach*.

*This was my junior-year-of-high-school independent study (2023–24) — a long project that ran across a full school year, two semesters organized into sprints.*

## Why I did this

I went on a WICS+ field trip to the Oden Institute, decided modeling was very cool, and turned that into a terrain generation project. The goal was to generate terrain that looks random but still follows logical patterns (real terrain isn't hardcoded, but it isn't pure noise either), and along the way to figure out which of these skills I might want to pursue professionally.

## The idea

Hardcoded terrain is ugly — you notice the patterns, and it's the same every time. Truly random terrain is also ugly. What you actually want is something in between: **pseudo-random**.

The approach I used is **value noise smoothed with a smooth-step function**:

1. **Lattice points.** Scatter random height values across a grid at a set spacing.
2. **Interpolation.** Calculate all the values *between* the lattice points so the surface is continuous instead of spiky. I started down the road of cubic interpolation, couldn't track down the bugs, and switched to a smooth-step function — simpler, and the result is smoother anyway. It's a curve from (0,0) to (1,1) that gets scaled and shifted to blend between any two points.

Two knobs control the look of each region:
- **Frequency** — how often a lattice point occurs, which controls how many peaks there are.
- **Amplitude** — the magnitude of the height values, i.e. how tall things get.

## Biomes

Terrain is cool; terrain that isn't all the same is cooler. Each region of the map is assigned a biome type, computed from the random seed.

- **Mountains** — high frequency, very high amplitude. The "simple" ones.
- **Oceans** — the complicated ones. Medium amplitude for waves, and *super* low amplitude for beaches. Beaches form near the edges of an ocean biome by dropping the amplitude there. If an ocean sits on the edge of the whole map it runs all the way to the edge; otherwise a beach forms there. And two ocean biomes next to each other don't get a beach between them — the edge check looks at whether the neighboring biome is also ocean and skips the beach if so.

### The big rewrite

Originally I interpolated the whole map at once, which left me chasing bugs (frequency breaking on the top row, a corner that stayed stuck at 0, oceans coming out symmetrical when they shouldn't). So I rewrote it to **interpolate each biome individually first, then interpolate over the whole heightmap** so the seams between biomes still blend smoothly. More work upfront, but faster than debugging the all-at-once version forever.

## Seeing it

The program prints the height grid as comma-separated rows. To actually *see* the terrain, I paste the data into a spreadsheet and use **conditional formatting** to color each cell by its height — sand and grass low down, rock and snow up high, blues for water. That spreadsheet was effectively my renderer: it's what I used to test and debug everything.

(The textbook approach is to render the height grid as a 3D mesh of triangles, and early on I was using an external renderer for that. My Windows laptop broke partway through the project and the Chromebook I switched to couldn't run it, so the spreadsheet heightmap became my display. It turned out to be plenty for seeing whether the generation was working.)

```bash
g++ terrain.cpp -o terrain
./terrain > terrain_data.csv
```

A new seed is used every run (it's printed at the top), so no two maps are the same. Paste the CSV into the visualization spreadsheet to view it.

## What's in here

| File | What it is |
|------|------------|
| `terrain.cpp` | The main generator. Biomes, beaches (including the correct "no beach between two oceans" logic), and per-biome + whole-map interpolation. Outputs the height grid as CSV. Renamed from `main.cpp`. |
| `terrain_color_wip.cpp` | A work-in-progress version where I started moving the spreadsheet's coloring rules into the C++ itself, so the program could emit colors directly instead of me re-applying conditional formatting by hand. The height-based color ramp (sand → grass → rock → snow, plus water blues) is transcribed in, but it isn't finished or wired into the output yet, so it still prints heights. Renamed from `main 2.2.cp`. |
| `sky.cpp` | A separate generator for sky/clouds — same noise + smooth-step, with amplitude scaled down lower in the frame so there are fewer clouds near the horizon. Renamed from `main_sky.cp`. |
| `tree.scad` | Recursive fractal tree in OpenSCAD (second semester). Renamed from `tree2.scad`. |
| `terrain-heightmaps.xlsx` | My display for the terrain. Each sheet is one generated map: I pasted the program's height output in and used conditional formatting to color the cells by height, so it reads as a map. This is the "renderer" described above. Sheet names encode that map's biome layout. |
| `sky.xlsx` | The sky generator's output, colored the same way. |
| `docs/` | The proposal and the presentations I gave during the project — see Documents below. |

## Documents

The `docs/` folder holds the materials I made over the course of the year. It helps to know what each one is — and isn't:

- **Project proposal** (`project-proposal.docx`) — the plan I wrote at the very beginning, *before I'd written any code*. It's a wishlist of everything I hoped to do, including a bunch of stretch goals (global warming sims, AI pathfinding, importing into Unity) that I never got to. Read it as "where I started," not a description of what I built.
- **Presentations** (`midterm-presentation.pptx`, `wics-presentation.pptx`) — informal talks I gave partway through, explaining the concepts and showing progress. They're casual and a little rough (lots of "here's the part I didn't finish yet"). The midterm deck is the earlier checkpoint; the WiCS+ presentation (given at a Women in Computer Science event) is the updated version, with the trees and some actual renders. They overlap a lot, so the WiCS+ one on its own covers most of it.

## Semester two additions

- **Color (in progress)** — migrating the height-based coloring I'd built with spreadsheet conditional formatting into the generator itself, so colored output comes straight from the program. The rules are ported in `terrain_color_wip.cpp`; finishing it would mean fixing a couple of bugs and actually printing the color array instead of the heights.
- **Sky** — value-noise clouds in shades of blue/white. Scaling up makes the limitation of linear interpolation obvious: the clouds come out visibly square.
- **Trees** — recursive fractals in OpenSCAD. Start with a tapered cylinder trunk, add a sphere "knuckle" so the joints aren't hollow, randomly split into 2–4 branches at chosen angles, and recurse until you hit the iteration limit, then cap the ends with leaves. Noise scales each branch so every tree is unique. I tried 15 recursions once and my computer gave up — the geometry grows exponentially.

## Known issues / things I'd still improve

- **Finish the color migration** — get the generator to output the colors it computes, instead of the spreadsheet doing the coloring.
- **Waves should be ridges, not points.** Real waves form raised lines, not individual peaks.
- **Perlin noise** — I talked about it a lot but never got around to coding it; everything here is value noise.
- **A real forest biome** — the trees generate on their own, but I didn't get to procedurally placing them into the terrain as a biome.
- More biomes generally.

## Source material

Primary reference: *Texturing and Modeling: A Procedural Approach* (Morgan Kaufmann Series in Computer Graphics) — especially the chapters on noise, anti-aliasing, texture design, and cloud generation.
