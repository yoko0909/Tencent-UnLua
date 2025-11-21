---@class TaskUpdate

local TaskUpdate = {}

function TaskUpdate:Init(Second)
    self.RunningCoroutine = coroutine.running()
    self.DelayTime = Second
    coroutine.yield()
end

function TaskUpdate:Update(DeltaTime)
    self.DelayTime = self.DelayTime - DeltaTime
    if self.DelayTime < 0 then
        coroutine.resume(self.RunningCoroutine)
    end
end

return TaskUpdate