require "luaclass"
require "Component"

AudioState = {
    Play = 0,
    Pause = 1,
    Continue = 2,
    Stop = 3
}

AudioComponent = class("AudioComponent", Component)

-- init AudioComponent with an array of audio files
function AudioComponent:init(audioFiles)
    self.audioFiles = audioFiles
    self.audioStates = {}
    -- set up state for each audio file
    for _, path in pairs(self.audioFiles) do
        self.audioStates[path] = AudioState.Stop
    end
end

-- set 3d or 2d
function AudioComponent:set3D(file, enabled)
    if (self.audioStates[file] ~= nil) then
        if enabled then
            self:update("enable3D", file)
        else
            self:update("disable3D", file)
        end
    end
end

-- set loop or not
function AudioComponent:setLoop(file, enabled)
    if (self.audioStates[file] ~= nil) then
        if enabled then
            self:update("enableLoop", file)
        else
            self:update("disableLoop", file)
        end
    end
end

-- set state of a audio file
function AudioComponent:setState(file, state)
    if (self.audioStates[file] ~= nil) then
        shouldUpdate = false
        oldState = self.audioStates[file]
        if oldState == AudioState.Play then
            if state == AudioState.Pause or state == AudioState.Stop then
                shouldUpdate = true
            end
        end
        if oldState == AudioState.Pause then
            if state == AudioState.Continue or state == AudioState.Stop then
                shouldUpdate = true
            end
        end
        if oldState == AudioState.Stop then
            if state == AudioState.Play then
                shouldUpdate = true
            end
        end
        if oldState == AudioState.Continue then
            if state == AudioState.Pause or state == AudioState.Stop then
                shouldUpdate = true
            end
        end
        if shouldUpdate then
            self.audioStates[file] = state
            self:update("setState", {path = file, state = state})
        end
    end
end

-- get state of a audio file
function AudioComponent:getState(file)
    if (self.audioStates[file] ~= nil) then
        return self.audioStates[file]
    end
    return nil
end