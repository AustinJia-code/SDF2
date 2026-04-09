# SDF2
### About
Signed Distance Functions for Script Defined FDM

Programatically define forms with basic primitives, transformations, and
boolean operations.

### Build & Run
1. Install Dependencies:
```
pip install vedo    # if using tools/stl_viewer.py
git clone https://github.com/AustinJia-code/Mini-HPPs.git /dep/minis
```
2. Build:
```
# Debug
cmake -B build/debug -DCMAKE_BUILD_TYPE=Debug && cmake --build build/debug

# Release (O3)
cmake -B build/release -DCMAKE_BUILD_TYPE=Release && cmake --build build/release
```
3. Run:
```
build/[debug|release]/<SCRIPT_NAME> -o <PATH_TO_STL_OUT> [-b] [-c]
```

### To Do
* Live rendering w/ raymarch
* Mirror complex
* Dual Contouring/Marching
* Optimize pattern

### Acknowledgements
* Michael Fogelman: [fogleman/sdf](https://github.com/fogleman/sdf)
* Inigo Quilez: https://iquilezles.org/articles/distfunctions/
