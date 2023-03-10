//
// Created by 惠普 on 2023/2/20.
//

#ifndef SPARROW_AUDIOCOMPONENT_H
#define SPARROW_AUDIOCOMPONENT_H

#include "../ComponentI.h"
#include "fmod.hpp"
#include "string"
#include <map>
#include <unordered_map>

namespace SPW
{
    enum SoundState {Play,Pause,Continue,Stop};
    // 管理每个sound数据对象
    // 依据path获取sound数据
    class SPWSoundDelegateI {
    public:
        virtual FMOD::Channel* playSound(const std::string &path, bool i3D, bool isLoop) = 0;
    };

    //包装channdel, 控制声音的播放
    // 绑定一个path
    class SPWSound {
    public:
        SPWSound() = default;
        explicit SPWSound(const std::string &path) : soundPath(path) {
        }
        void play(SPWSoundDelegateI *delegate) {
            chan = delegate->playSound(soundPath, is3D, isLoop);
            shouldUpdate = false;
        }
        void pause() {
            if (chan) {
                FMOD_MODE mode = is3D ? FMOD_3D : FMOD_2D;
                mode |= isLoop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
                chan->setMode(mode);
                chan->setPaused(true);
                shouldUpdate = false;
            }
        }
        void resume() {
            if (chan) {
                chan->setPaused(false);
                shouldUpdate = false;
            }
        }
        void stop() {
            chan->stop();
            chan = nullptr;
            shouldUpdate = false;
        }
        void setState(SoundState s) {
            shouldUpdate = false;
            if (state == SoundState::Play && (s == SoundState::Pause || s == SoundState::Stop)) {
                shouldUpdate = true;
            } else if (state == SoundState::Pause && (s == SoundState::Stop || s == SoundState::Continue)) {
                shouldUpdate = true;
            } else if (state == SoundState::Stop && (s == SoundState::Play)) {
                shouldUpdate = true;
            } else if (state == SoundState::Continue && (s == SoundState::Pause || s == SoundState::Stop)) {
                shouldUpdate = true;
            }
            if (shouldUpdate) {
                state = s;
            }
        }
        void update() {
            if (state == SoundState::Play || state == SoundState::Continue) {
                if (!chan) {
                    state = Stop;
                    shouldUpdate = false;
                    return;
                }
                bool play = false;
                chan->isPlaying(&play);
                if (!play) {
                    state = Stop;
                    shouldUpdate = false;
                }
            }
        }

        void setPos(float x, float y, float z) {
            if (is3D && chan) {
                FMOD_VECTOR pos = {x, y, z};
                chan->set3DAttributes(&pos, nullptr);
            }
        }
        bool needUpdate() {return shouldUpdate;}
        SoundState state = Stop;
        bool is3D = false;
        bool isLoop = false;

    private:
        bool shouldUpdate = true;
        std::string soundPath;
        FMOD::Channel *chan = nullptr; // 控制声音播放暂停，结束, 不需要释放
    };

    class AudioComponent : public ComponentI {
    public:

        AudioComponent() = default;
        AudioComponent(const AudioComponent&) = default;

        explicit AudioComponent(const std::vector<std::string> &soundPaths) {
            for (auto &path : soundPaths) {
                auto sound = std::make_shared<SPWSound>(path);
                allSounds.insert({path, sound});
            }
        };
        void set3D(const std::string &path, bool enable) {
            if (allSounds.find(path) != allSounds.end())
                allSounds[path]->is3D = enable;
        }
        void setLoop(const std::string &path, bool enable) {
            if (allSounds.find(path) != allSounds.end())
                allSounds[path]->isLoop = enable;
        }
        std::unordered_map<std::string, std::shared_ptr<SPWSound>> allSounds;

        SoundState getState(const std::string &path) {
            if (allSounds.find(path) != allSounds.end())
                return allSounds[path]->state;
            else
                return Stop;
        }
        void setState(const std::string &path, SoundState s) {
            if (allSounds.find(path) != allSounds.end())
                allSounds[path]->setState(s);
        }
    };


}
#endif //SPARROW_AUDIOCOMPONENT_H
