# SDF2
### About
Signed Distance Functions for Script Defined FDM

Programatically define forms with basic primitives, transformations, and
boolean operations.

### Build & Run
1. Install Dependencies:
```
git clone https://github.com/AustinJia-code/Mini-HPPs.git /dep/minis
```
2. Build:
```
cd build
cmake ..
make
```
3. Run:
```
./<SCRIPT_NAME> -o <PATH_TO_STL_OUT>
```

### To Do
* Form print debugging
    * Custom scripting language
* Decimation
* Smoothing
* Dual Contouring
* Optimize pattern
* Arbitrarily long boolean op parameters

### Acknowledgements
* Michael Fogelman: [fogleman/sdf](https://github.com/fogleman/sdf)
* Inigo Quilez: https://iquilezles.org/articles/distfunctions/
