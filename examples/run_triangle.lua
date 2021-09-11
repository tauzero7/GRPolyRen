--[[
  File:  run_triangle.lua
  Author: Thomas Mueller

  Run:
    ./OfflineRend examples/run_triangle.lua
  ...
   compare OGLView/triangle.py
]]

loadObject("objects/triangle.obj")

robs = 40.0
ksiCrit = 7.274
coordRadius = math.tan(math.rad(ksiCrit)) * robs

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
setBlackHoleColor(0.7,0.7,0.7, 1.0)
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
imgFactor = 1
setWindowSize(1280, 720, imgFactor)

setCrossHairsVisible(false)

--[[
    Render multiple cases, starting with flat geometry for comparison
]]
setObjTrans(-5.0, 100.0, -2.0)
setViewMode("Flat")
renderImage()
saveImage("output/img_triangle_bh.ppm")

setBlackHoleColor(0.7,0.7,0.7, 0.0)

--
setObjTrans(-5.0, 3.0, -2.0)

setViewMode("Flat")
renderImage()
saveImage("output/img_triangle_pos1_tl0.ppm")

setViewMode("GRtess")
setMaxTessLevel(1)
renderImage()
saveImage("output/img_triangle_pos1_tl1.ppm")

setMaxTessLevel(4)
renderImage()
saveImage("output/img_triangle_pos1_tl2.ppm")

setMaxTessLevel(64)
renderImage()
saveImage("output/img_triangle_pos1_tl3.ppm")

--
setObjTrans(-5.0, 0.5, -2.0)

setViewMode("Flat")
renderImage()
saveImage("output/img_triangle_pos2_tl0.ppm")

setViewMode("GRtess")
setMaxTessLevel(1)
renderImage()
saveImage("output/img_triangle_pos2_tl1.ppm")

setMaxTessLevel(4)
renderImage()
saveImage("output/img_triangle_pos2_tl2.ppm")

setMaxTessLevel(64)
renderImage()
saveImage("output/img_triangle_pos2_tl3.ppm")
