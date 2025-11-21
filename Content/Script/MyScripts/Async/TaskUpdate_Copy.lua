---@class TaskUpdate_Copy

local TaskUpdate_Copy = {}

function TaskUpdate_Copy:Init(Second)
    self.RunningCoroutine = coroutine.running()
    self.DelayTime = Second
    coroutine.yield()
end

function TaskUpdate_Copy:Update(DeltaTime)
    self.DelayTime = self.DelayTime - DeltaTime
    if self.DelayTime < 0 then
        coroutine.resume(self.RunningCoroutine)
    end
end

return TaskUpdate_Copy