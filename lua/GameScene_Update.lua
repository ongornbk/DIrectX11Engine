if IsKeyHit(1) == true
then 
SaveInstance("../saves/map.save")
PostQuitMessage(1) 
end
if IsKeyHit(2) == true
then
tileSelected = 0
end
if IsKeyHit(3) == true
then
tileSelected = 1
end
if IsKeyHit(4) == true
then
tileSelected = 2
end
if IsKeyHit(5) == true
then
tileSelected = 3
end
if IsKeyHit(5) == true
then
tileSelected = 4
end
if IsKeyHit(6) == true
then
tileSelected = 5
end
if IsKeyHit(7) == true
then
tileSelected = 6
end
if IsKeyHit(8) == true
then
tileSelected = 7
end
if IsKeyHit(9) == true
then
tileSelected = 8
end
if IsKeyPressed(19) == true
then
GetMousePosition()
SetTile(tileSelected)
end
if IsKeyPressed(20) == true
then
local x,y = GetMousePosition()
CreateUnit()
PickLastCreatedUnit()
SetWalkingStance(1)
SetUnitSpeed(100)
InitializeUnit("enemy.mod",100,18,x,y,0,false)
end
if IsKeyPressed(21) == true
then
PickLastSelectedUnit()
--LockCameraOnUnit()
end
if IsKeyPressed(22) == true
then
GetUnitVariable("hero")
--LockCameraOnUnit()
end
if IsKeyPressed(23) == true
then
EraseUnitVariable("hero")
PickLastSelectedUnit()
PushUnitVariable("hero")
--LockCameraOnUnit()
end
if GetMouseState(0) == true
then
GetUnitVariable("hero")
local x,y = GetMousePosition()
SetTaskGotoPoint(x,y)
end
if GetMousePressed(1) == true
then
GetUnitVariable("hero")
local x,y = GetMousePosition()
SetUnitPosition(x,y)
end
GetUnitVariable("hero")
local x,y = GetUnitPosition()
SetCameraPosition(x,y)