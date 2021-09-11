--[[
  File:   run_triangle_movie.lua
  Author: Thomas Mueller

  Run:
    ./OfflineRen[d] examples/run_triangle_movie.lua
  
]]

loadObject("objects/triangle.obj")

robs = 40.0
ksiCrit = 7.274
coordRadius = math.tan(math.rad(ksiCrit)) * robs

imgFactor = 2.0

N = 10
N = 300

-- camera position, point of interest, and field of view
setCamPos(robs, 0.0, 0.0)
setCamPoI(0.0, 0.0, 0.0)
setCamFoV(20.0)

-- scale triangle
setObjScale(5.0, 5.0, 5.0)

-- rotate triangle
setEulerRot(0.0, -90.0, 0.0)
setEulerOrder("z_ys_xss")
setPatFreq(10, 10)

-- set triangle texture
setObjTexture("triangle")

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
setTessDistRelation(10)

-- ignore light source
setLightSourceActive(false)

-- set window size for offscreen rendering
setWindowSize(1280, 720, imgFactor)

setCrossHairsVisible(false)
setCoordSysVisible(false)

--[[
    Render image sequence
]]

yStart = 6.0
yEnd   = -9.0
yStep  = (yEnd - yStart) / (N - 1)

for i = 0,(N-1) do
    y = yStart + i * yStep
    setObjTrans(-5.0, y, -2.0)
    renderImage()

    filename = "output/img_triangle_grpr_" .. string.format("%04d", i) .. ".ppm"
    saveImage(filename)
end
