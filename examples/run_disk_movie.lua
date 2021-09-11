--[[
  File:   run_disk_movie.lua
  Author: Thomas Mueller

  Run:
    ./OfflineRend examples/run_disk_movie.lua
  
]]

loadObject("objects/disk.obj")

robs = 40.0
ksiCrit = 7.274
coordRadius = math.tan(math.rad(ksiCrit)) * robs

imgFactor = 2.0

--N = 10
N = 180

-- camera position, point of interest, and field of view
setCamPos(robs, 0.0, 0.0)
setCamPoI(0.0, 0.0, 0.0)
setCamFoV(52.0)

-- scale disk
setObjScale(1.1, 1.1, 1.1)

-- rotate disk
setEulerRot(0.0, 80.0, 0.0)
setEulerOrder("z_ys_xss")
setPatFreq(20, 18)

-- set disk texture
setObjTexture("disk")

-- set apparent black hole radius (in coordinates)
setBlackHoleRadius(coordRadius)
setBlackHoleColor(0.3,0.3,0.3, 1.0)
setBlackHoleFlatShading(false)

-- set background color to white
setClearColor(1.0, 1.0, 1.0)

-- set view mode and tessellation factor
setViewMode("GRtess")
setTessFactor(5.0)
setMaxTessLevel(64)
setTessDistRelation(1000)

-- ignore light source
setLightSourceActive(false)
setLightSourcePos(80, 0)
setLightSourceFactor(0.6)

-- set window size for offscreen rendering
setWindowSize(800, 720, imgFactor)

setCrossHairsVisible(false)
setCoordSysVisible(false)

--[[
    Render images
]]

phiRange = {90,1}
phiStep = (phiRange[2] - phiRange[1]) / (N - 1)

for i = 0,(N-1) do
    phi = i * phiStep + phiRange[1]
    setEulerRot(0.0, phi, 0.0)
    renderImage()

    filename = "output/img_disk_grpr_" .. string.format("%04d", i) .. ".ppm"
    saveImage(filename)
end    
