--[[
  File:  run_sphere.lua
  Author: Thomas Mueller

  Run:
    ./OfflineRend examples/run_sphere.lua
  
]]

loadObject("objects/sphere.obj")

robs = 40.0
ksiCrit = 7.2
coordRadius = math.tan(math.rad(ksiCrit)) * robs
--coordRadius = 3.0

imgFactor = 1

-- camera position, point of interest, and field of view
setCamPos(robs, 0.0, 0.0)
setCamPoI(0.0, 0.0, 0.0)
setCamFoV(19.0)

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
setBlackHoleColor(0.7,0.7,0.7, 1.0)
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
setWindowSize(880, 700, imgFactor)

setCrossHairsVisible(false)
setCoordSysVisible(false)

--[[
    Render multiple cases, starting with flat geometry for comparison
]]

posList = {0.0, 36.0, 72.0, 108.0, 144.0, 180.0}

for i = 1,6 do
    alpha = math.rad(posList[i])
    x = 6.0 * math.cos(alpha)
    y = 6.0 * math.sin(alpha)
    setObjTrans(x, y, 0.0001)
    renderImage()

    filename = "output/img_sphere_t" .. tostring(i) .. ".ppm"
    saveImage(filename)
end




