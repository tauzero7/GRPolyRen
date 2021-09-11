--[[
  File:   run_sphere_movie.lua
  Author: Thomas Mueller

  Run:
    ./OfflineRen[d] examples/run_sphere_movie.lua
    
]]

loadObject("objects/sphere.obj")

robs = 40.0
ksiCrit = 7.2
coordRadius = math.tan(math.rad(ksiCrit)) * robs

imgFactor = 2.0

N = 30
N = 360

-- camera position, point of interest, and field of view
setCamPos(robs, 0.0, 0.0)
setCamPoI(0.0, 0.0, 0.0)
setCamFoV(20.0)

-- scale sphere
setObjScale(1.0, 1.0, 1.0)

-- rotate sphere
setEulerRot(0.0, 0.0, 0.0)
setEulerOrder("z_ys_xss")
setPatFreq(12, 12)

-- set sphere texture
setObjTexture("col_sphere")

-- set apparent black hole radius (in coordinates)
setBlackHoleRadius(coordRadius)
setBlackHoleColor(0.0,0.0,0.0, 1.0)
setBlackHoleFlatShading(true)

-- set background color to white
setClearColor(1.0, 1.0, 1.0)

-- set view mode and tessellation factor
setViewMode("GRtess")
setTessFactor(5.0)
setMaxTessLevel(64)
setTessDistRelation(100)

-- ignore light source
setLightSourceActive(false)

-- set window size for offscreen rendering
setWindowSize(1024, 720, imgFactor)

setCrossHairsVisible(false)
setCoordSysVisible(false)

--[[
    Render image sequence
]]

phiStart = 0.0
phiEnd   = 2.0 * math.pi
phiStep  = (phiEnd - phiStart) / N

for i = 0,(N-1) do
    phi = phiStart + i * phiStep
    x = 6.0 * math.cos(phi)
    y = 6.0 * math.sin(phi)
    setObjTrans(x, y, 0.0001)
    renderImage()

    filename = "output/img_sphere_grpr_" .. string.format("%04d", i) .. ".ppm"
    saveImage(filename)
end
