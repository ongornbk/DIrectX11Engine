require "core/Unit"
require "core/Tree"
require "core/Doodads"
require "core/Camera"
require "core/Game"
require "core/Chat"
require "core/Pointer"

function AttackSound()

    local rn = math.random(1,3)
    if rn == 1
    then

    PlaySound("attack1")
    else if rn == 2
    then

    PlaySound("attack2")
    else
    PlaySound("attack3")
    end
    end
end
end

local x,y = GetMousePosition()
if GetMouseState(0) == true
then
local x,y = GetMousePosition()
local selected = GetLastSelectedUnit()
Unit.Pick(hero)



if selected > 0 and selected ~= hero and IsSelected(selected)
then

if Unit.Attack(selected) == false
then
AttackSound()
else
Unit.Goto(x,y)
end
--BeginRunning()
else

Unit.Pick(hero)
Unit.CleanTasks()
--EndRunning()
end

if GetMousePressed(1) == true
then
local x,y = GetMousePosition()
Unit.Pick(hero)
Unit.SetPosition(x,y)
Unit.CleanTasks()
PlaySound("teleport")
end
--
--
--
if IsKeyHit(1) == true
then 
Game.Save()
Game.Exit()
end

if IsKeyHit(20) == true
then 
Unit.Pick(hero)
Unit.ToggleRunning()
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
SetTile(tileSelected,2)
end
if IsKeyPressed(20) == true
then
local rs = math.random(0,50)
local x,y = GetMousePosition()

Unit.Pick(Unit.new())
Object.SetRenderingFlag(true)
Object.SetSelectableFlag(true)
Object.SetShadowFlag(true)
Object.SetPushableFlag(true)
Object.SetCollisionPriority(1)
Unit.SetWalkingStance(0)

local rn = math.random(3,3)

if rn == 0
then
Unit.SetSpeed(180)
Unit.Initialize("amazon",96,23,x,y,0,true)
end

if rn == 1
then
    Unit.SetSpeed(200)
Unit.Initialize("amazon_light",96,22,x,y,0,true)
end

if rn == 2
then
    Unit.SetSpeed(150)
Unit.Initialize("enemy",96,23,x,y,0,true)
end

if rn == 3
then
    Unit.SetSpeed(180)
Unit.Initialize("barbarian",96,23,x,y,0,true)
end



end

if IsKeyHit(21) == true
then
local niu = GetLastSelectedUnit()
Object.Pick(niu)

if niu > 0 and niu ~= hero and IsSelected(niu)
then
Object.Delete()
end
end

--if IsKeyHit(22) == true
--then
--local x,y = GetMousePosition()
--Doodads.new()
--SetRenderingFlag(true)
--SetPushableFlag(false)
--Doodads.Initialize("well0",125,50,x,y,0)
--Chat.Print("well0 Has Been Created!")
--PlaySound("place")
--end
--if IsKeyPressed(23) == true
--then
--local x,y = GetMousePosition()
--Doodads.new()
--SetRenderingFlag(true)
--SetPushableFlag(true)
--Doodads.Initialize("barell0",105,18,x,y,0)
--Chat.Print("barell0 Has Been Created!")
--end
--if IsKeyPressed(24) == true
--then
--local x,y = GetMousePosition()
--CreateAnimatedDoodads()
--SetRenderingFlag(true)
--SetPushableFlag(false)
--SetCollisionPriority(2)
--InitializeAnimatedDoodads("fire0",120,10,x,y,0)
--SetNumberOfFrames(25)
--Chat.Print("fire0 Has Been Created!")
--end
--if IsKeyHit(26) == true
--then
--local x,y = GetMousePosition()
--Doodads.new()
--SetRenderingFlag(true)
--Doodads.Initialize("cow_corpse0",120,20,x,y,-1)
--Chat.Print("cow_corpse0 Has Been Created!")
--Music.Play("place")
----SetZ(1)
--end
if IsKeyHit(27) == true
then
local x,y = GetMousePosition()
local tree = Tree.new()

Tree.Pick(Tree.new())
Object.SetRenderingFlag(true)
Object.SetSelectableFlag(false)
Object.SetShadowFlag(true)
Object.SetPushableFlag(false)
Object.SetCollisionPriority(5)

Tree.Initialize("tree0",400,23,x,y,0)
Chat.Print("tree0 Has Been Created!")
Music.Play("place")
end
--if IsKeyHit(28) == true
--then
--local x,y = GetMousePosition()
--Tree.new()
--SetRenderingFlag(true)
--SetPushableFlag(false)
--SetShadowFlag(true)
--SetCollisionPriority(0)
--InitializeTree("fountain0",250,70,x,y,0)
--Chat.Print("fountain0 Has Been Created!")
--Music.Play("place")
--end

GetUnitsInGroup(pullgroup)
local unit = PopGroup()
while(unit > 0)
do
Unit.Pick(unit)
local distance = GetDistanceBetweenUnits(unit,hero)

if distance > 300.0
then
RemoveUnitFromGroup(pullgroup,unit)
Unit.CleanTasks()
else

end

unit = PopGroup()
end