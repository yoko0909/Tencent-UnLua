--[[
    说明：在Lua协程中可以方便的使用UE4的Latent函数实现延迟执行的效果
]] --

local Screen = require "Tutorials.Screen"

local M = UnLua.Class()

local function run(self, name)
    Screen.Print(string.format("协程%s：启动", name))
    for i = 1, 5 do
        UE.UKismetSystemLibrary.Delay(self, 1)
        Screen.Print(string.format("协程%s：%d", name, i))
    end
    Screen.Print(string.format("协程%s：结束", name))
end

local taskUpdateA = require("MyScripts.Async.TaskUpdate")
local taskUpdateB = require("MyScripts.Async.TaskUpdate_Copy")

function M:ReceiveBeginPlay()
    local msg = [[
    —— 本示例来自 "Content/Script/Tutorials.07_CallLatentFunction.lua"
    ]]
    Screen.Print(msg)

    -- coroutine.resume(coroutine.create(run), self, "A")
    -- coroutine.resume(coroutine.create(run), self, "B")

    coroutine.resume(coroutine.create(function()
        for i = 1, 5 do
            taskUpdateA:Init(1)
            Screen.Print(string.format("协程A：%d", i))
        end
        Screen.Print(string.format("协程A：结束"))
    end))

    coroutine.resume(coroutine.create(function()
        for i = 1, 5 do
            taskUpdateB:Init(1)
            Screen.Print(string.format("协程B：%d", i))
        end
        Screen.Print(string.format("协程B：结束"))
    end))

    Screen.Print(string.format("主协程结束"))
end

function M:ReceiveTick(DeltaTime)
    Screen.Print(string.format("ReceiveTick"))
    taskUpdateA:Update(DeltaTime)
    taskUpdateB:Update(DeltaTime)
end

function M:ReceiveEndPlay()
    Screen.Print(string.format("ReceiveEndPlay"))
end

return M
