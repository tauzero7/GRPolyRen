# GRPolyRen

GRPolyRen is a demo implementation of the general-relativistic polygon rendering method proposed in:

**Adaptive polygon rendering for interactive visualization in the Schwarzschild spacetime**  
T. Müller, C. Schultz, and D. Weiskopf  
European Journal of Physics, vol. 0, no. 0, pp. 0-0, 202?  
doi: ...

```BibTeX
@article{muller2021adaptive,
    author={M{\"u}ller, Thomas
    and Schulz, Christoph
    and Weiskopf, Daniel},
    title={Adaptive polygon rendering for interactive visualization in the {Schwarzschild} spacetime},
    journal={European Journal of Physics},
    year={202?},
    volume={0},
    number={0},
    pages={0--0},
    doi={},
    url={}
}
```

## Download

Precompiled binaries and source code are available from:
[github.com/tauzero7/GRPolyRen](https://github.com/tauzero7/GRPolyRen)

## Installation

When using prebuild binaries skip to [quick how-to](#quick-how-to).

If you compile from source code, you have to generate a lookup table. See [generating lookup tables](#generating-lookup-tables) below.

### Prerequisites

To compile the code, you need:   
* CMake 3.15 or higher  
    https://cmake.org/
* A C++11 compiler
  * gcc (Linux) 
  * MSVC (Windows)

### Linux

* Generate build folder
* Switch to build folder and run `cmake ..` or open `cmake-gui`
* Set `CMAKE_BUILD_TYPE` to either `Debug` or `Release`
* Configure and generate the project
* Compile the code using `make`

### Windows

* Open `CMakeLists.txt` using the CMake GUI
* Set the source code path to, e.g., `E:/projects/GRPolyRen` and build the path to `E:/projects/GRPolyRen/build`
* Click 'Configure' (usually default settings are fine)
* Click 'Generate'
* Click 'Open Project' to open the project within Visual Studio
* Select `Release` and `x64` from the dropdown in the toolbar
* Build the entire solution, except for the `ZipIt` target

## Generating lookup tables

By default, a lookup table with a low resolution will be computed when running `GenLookupTable`. To customize settings:

* Open `genlookup/main.cpp` and scroll down to the `main` function.
* Adjust the resolution in radial (Nr) and azimuthal (Nphi) direction.
* Adjust a minimum (rmin) and maximum (rmax) radius value, and set the observer position (rInit).
* Recompile the code and run it... 
* Do not forget to adapt `lutFilename` within `src/main.cpp` and recompile the sources to use the new lookup table.
 
## Quick How-To

* Run ./GRPolyRen from a command console or double click on it (Windows only).
  Note that the LUT is calculated for an observer at `r_obs = 40`. Thus, the 
  camera has to be at `position = (40, 0, 0)` in order to show what an observer
  at that position would see.

* Load a mesh object:  
    Click on 'Load Files' and select `examples/run_sphere.lua`.
    (You might have to resize the window)   

    The mesh object is a colored sphere which is located directly behind the 
    black hole. 

* By default, the mouse controls the object position. With the left mouse button
  pressed, move the object in the yz-plane behind the black hole and see how the
  object is apparently distorted by the black hole.

## Keyboard Shortcuts

- __Esc :__     Quit program
- __1 :__       Flat view mode
- __2 :__       GR view mode
- __3 :__       GRgeom view mode
- __4 :__       GRtess view mode
- __b :__       toggle black hole visibility
- __c :__       set mouse control to 'camera'
- __j :__       increase Euler angle alpha
- __shift-j :__ decrease Euler angle alpha
- __k :__       increase Euler angle beta
- __shift-k :__ decrease Euler angle beta
- __l :__       increase Euler angle gamma
- __shift-l :__ decrease Euler angle gamma
- __o :__       set mouse control to 'object'
- __t :__       toggle crosshairs visibility
- __w :__       toggle wireframe
- __x :__       set camera on x-axis
- __ctrl-x :__  set camera on negative x-axis
- __y :__       set camera on y-axis
- __ctrl-y :__  set camera on negative y-axis
- __z :__       set camera on z-axis
- __ctrl-z :__  set camera on negative z-axis
- __right-arrow :__  rotate object around black hole (positively)
- __left-arrow :__   rotate object around black hole (negatively)

## User Interface

* __Load Files__  
    You can either load a Lua script or an object file or a settings file.

* __Mouse__  
    The mouse can handle either the object or the camera.  
    - Camera: (LMB: rotate around poi, MMB: change distance to poi, RMB: pan view)  
    - Object: (LMB: move within yz-plane, RMB: move along x-axis)

* __Camera__  
    Camera projection can be either perspective or orthographic. The position and
    point of interest (poi) are given in pseudo-Cartesian coordinates. 
    The camera's vertical field-of-view (degrees) can be modified using the slider
    or by pressing Ctrl+LMB.

* __Object__  
    The object position can be modified using, e.g., `trans = (0,0,0)` for setting
    the origin of the object to the center of the black hole. 
    The size of the object can be modified using `scale`. 
    It is also possible to rotate the object using Euler angles. The order 
    of the angles is defined by `euler-order`. 
    In the demo implementation, every object has a checkerboard texture. 
    The frequency of the texture is defined by `patFreq`.
    An object can also rotate around the black hole on a circular orbit with the
    currently set distance. Here, `orbit-rotate` defines an angular velocity.

* __BlackHole__  
    The black hole is represented by a sphere. However, this sphere does not
    undergo transformation by means of the polygon rendering method. Hence, 
    the apparent size must be set manually. A value of `5.05` represents the
    size of the shadow of the black hole as seen by an observer at `r_obs = 40`.

* __View__  
    The view mode can be 
    - `flat`: no distortion
    - `GR`: gr polygon-rendering without subdivision
    - `GRgeom`: gr polygon-rendering without subdivision
    - `GRtess`: gr polygon-rendering with subdivision

* __LightSource__  
    The position of the light source can be set using the spherical 
    angles `theta` and `phi` in degree. `theta` is the colatitude 
    angle measured from the z-axis. `phi` is the azimuth angle. 
    The distance is always equal to the actual observer.

* __Background__  
    Background color is given as R,G,B triplet, where each value is in the 
    range [0,255].

* __other__  
    'Save current state': The current state of all parameters are saved 
    in `setting.cfg`.

## Lua Scripting

Color parameters are given as `r,g,b[,a]` tuples, i.e., red, green, blue, and alpha values between `0.0` and `1.0`.
Enabled parameters can be either `true` or `false`.

* OBJ mesh loading  

        loadObject("filename")

* Camera position and point of interest in pseudo-Cartesian coordinates;
  the camera's field of view is given in degrees

        setCamPos(x, y, z)
        setCamPoI(x, y, z)
        setCamFoV(fov)

* Object position and scaling via pseudo-Cartesian coordinates; the object
  texture can be either `disk`, `sphere` or `triangle`; the checkerboard
  pattern can be influenced by the pattern frequency; Euler rotation angles
  (degree) and order: `z_xs_zss`, `z_ys_zss`, `z_ys_xss`:

        setObjTrans(x, y, z)
        setObjScale(x, y, z)
        setObjTexture("name")
        
        setPatFreq(fx, fy)
        setEulerRot(alpha, beta, gamma)
        setEulerOrder("name")

* Black hole

        setBlackHoleRadius(radius)
        setBlackHoleColor(r, g, b, a)
        SetBlackHoleFlatShading(enabled)

* Background  

        setClearColor(r, g, b)

* View mode (`Flat`, `GR`, `GRgeom`, `GRtess`)

        setViewMode("name")
        setTessFactor(factor)
        setMaxTessLevel(mtl)

* Light source (theta and phi in degrees)

        setLightSourceActive(enabled)
        setLightSourcePos(theta, phi)
        setLightSourceFactor(factor)


* Window size

        setWindowSize(width, height, factor)        

* Other stuff

        setCrossHairsVisible(bool)
        setCoordSysVisible(bool)

* render and save

        renderImage()
        saveImage("filename")

## Dependencies

You do not have to install external dependencies, since they are embedded into the project. We use the following libraries:

* glad  (OpenGL multi-language loader)  
    https://glad.dav1d.de/
* glfw  (API for creating windows, contexts, and surfaces, receiving input and events)  
    https://www.glfw.org
* glm  (C++ mathematics library for graphics software)  
    https://github.com/g-truc/glm
* imgui  (Graphical user interface library for c++)  
    https://github.com/ocornut/imgui    
* Lua  (scripting language)  
    http://www.lua.org/home.html

## Contact

Dr. Thomas Müller  
Max Planck Institute for Astronomy  
Heidelberg, Germany  
EMail: tmueller (at) mpia (dot) de
