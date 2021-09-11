--[[
  File:  run_disk.lua
  Author: Thomas Mueller

  Run:
    ./OfflineRend examples/run_disk.lua
  
]]

loadObject("objects/disk.obj")

robs = 40.0
ksiCrit = 7.274
coordRadius = math.tan(math.rad(ksiCrit)) * robs

imgFactor = 1

-- camera position, point of interest, and field of view
setCamPos(robs, 0.0, 0.0)
setCamPoI(0.0, 0.0, 0.0)
setCamFoV(52.0)

-- scale disk and translate
setObjScale(1.1, 1.1, 1.1)
setObjTrans(0.0, 0.0, 0.0)

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
setLightSourceActive(true)
setLightSourcePos(80, 0)
setLightSourceFactor(0.6)


setCrossHairsVisible(false)
setCoordSysVisible(false)

--[[
    Render images
]]

--[[ *****************************
        Face-on view   (90°)
     ******************************]]
setEulerRot(0.0, 90.0, 0.0)
setCamFoV(50.0)

-- observer view
setCamPos(robs, 0.0, 0.0)
setCamPoI(0.0, 0.0, 0.0)     
setWindowSize(840, 720, imgFactor)
setLightSourceActive(1)
renderImage()
saveImage("output/img_disk_incl_90_obs.ppm")

-- external camera view
setCamPos(20.0, -36.0, 0.0)
setCamPoI(0.0, 0.0, 0.0)     
setWindowSize(1080, 720, imgFactor)
setLightSourceActive(0)
renderImage()
saveImage("output/img_disk_incl_90_cam.ppm")

--[[ *****************************
        view  (30°)
     ******************************]]
setEulerRot(0.0, 30.0, 0.0)
setCamFoV(50.0)

-- observer view
setCamPos(robs, 0.0, 0.0)
setCamPoI(0.0, 0.0, 0.0)     
setWindowSize(840, 720, imgFactor)
setLightSourceActive(1)
renderImage()
saveImage("output/img_disk_incl_30_obs.ppm")

-- external camera view
setCamPos(10.0, -40.0, 4.0)
setCamPoI(-2.0, 0.0, 0.0)     
setWindowSize(1080, 720, imgFactor)
setLightSourceActive(0)
renderImage()
saveImage("output/img_disk_incl_30_cam.ppm")

--[[ *****************************
        near edge-on view  (10°)
     ******************************]]
setEulerRot(0.0, 10.0, 0.0)
setCamFoV(50.0)

-- observer view
setCamPos(robs, 0.0, 0.0)
setCamPoI(0.0, 0.0, 0.0)     
setWindowSize(840, 720, imgFactor)
setLightSourceActive(1)
renderImage()
saveImage("output/img_disk_incl_10_obs.ppm")

-- external camera view
setCamPos(0.0, -40.0, 4.0)
setCamPoI(-4.0, 0.0, 0.0)     
setWindowSize(1080, 720, imgFactor)
setLightSourceActive(0)
renderImage()
saveImage("output/img_disk_incl_10_cam.ppm")


--[[
N = 10
phiRange = {90,0}
phiStep = (phiRange[2] - phiRange[1]) / (N - 1)

for i = 0,(N-1) do
    phi = i * phiStep + phiRange[1]
    setEulerRot(0.0, phi, 0.0)
    renderImage()

    filename = "output/img_disk_t" .. tostring(i) .. ".ppm"
    saveImage(filename)
end    
]]
