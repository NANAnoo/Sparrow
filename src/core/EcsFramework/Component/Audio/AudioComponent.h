//
// Created by Ouzi Pang  on 2023/2/20.
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
	enum SoundState
	{
		Play, Pause, Continue, Stop
	};

	// get sound data by path
	class SPWSoundDelegateI
	{
	public:
		virtual FMOD::Channel* playSound(const std::string& path, bool i3D, bool isLoop) = 0;
	};

	// wrapper of channels, control the playback of the sound, with a path
	class SPWSound
	{
	public:
		SPWSound() = default;
		explicit SPWSound(const std::string& path)
			: soundPath(path)
		{
		}

		void play(SPWSoundDelegateI* delegate)
		{
			sound_channel = delegate->playSound(soundPath, is3D, isLoop);
			shouldUpdate = false;
		}

		void pause()
		{
			if (sound_channel)
			{
				FMOD_MODE mode = is3D ? FMOD_3D : FMOD_2D;
				mode |= isLoop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
				sound_channel->setMode(mode);
				sound_channel->setPaused(true);
				shouldUpdate = false;
			}
		}

		void resume()
		{
			if (sound_channel)
			{
				sound_channel->setPaused(false);
				shouldUpdate = false;
			}
		}

		void stop()
		{
			sound_channel->stop();
			sound_channel = nullptr;
			shouldUpdate = false;
		}

		void setState(SoundState s)
		{
			shouldUpdate = false;
			if (state == SoundState::Play && (s == SoundState::Pause || s == SoundState::Stop))
			{
				shouldUpdate = true;
			}
			else if (state == SoundState::Pause && (s == SoundState::Stop || s == SoundState::Continue))
			{
				shouldUpdate = true;
			}
			else if (state == SoundState::Stop && (s == SoundState::Play))
			{
				shouldUpdate = true;
			}
			else if (state == SoundState::Continue && (s == SoundState::Pause || s == SoundState::Stop))
			{
				shouldUpdate = true;
			}
			if (shouldUpdate)
			{
				state = s;
			}
		}

		void update()
		{
			if (state == SoundState::Play || state == SoundState::Continue)
			{
				if (!sound_channel)
				{
					state = Stop;
					shouldUpdate = false;
					return;
				}

				bool play = false;
				// update volume
				sound_channel->setVolume(volume);
				// check if sound is playing
				sound_channel->isPlaying(&play);

				if (!play)
				{
					// sound is not playing
					state = Stop;
					shouldUpdate = false;
				}
			}
		}

		void setPos(float x, float y, float z)
		{
			if (is3D && sound_channel)
			{
				FMOD_VECTOR pos = { x, y, z };
				sound_channel->set3DAttributes(&pos, nullptr);
			}
		}

		bool needUpdate()
		{
			return shouldUpdate;
		}

	public:
		SoundState state = Stop;
		bool is3D = false;
		bool isLoop = false;

		std::string soundPath;

		float volume = 1.0f;

	private:
		bool shouldUpdate = true;
		FMOD::Channel* sound_channel = nullptr; // not have to release
	};

	class AudioComponent : public ComponentI
	{
	public:
		AudioComponent() = default;
		AudioComponent(const AudioComponent&) = default;

		explicit AudioComponent(const std::vector<std::string>& soundPaths)
		{
			for (auto& path: soundPaths)
			{
				auto sound = std::make_shared<SPWSound>(path);
				allSounds.insert({ path, sound });
			}
		};

		void AddAudioClip(const std::string& path)
		{
			auto sound = std::make_shared<SPWSound>(path);
			allSounds.insert({ path, sound });
		}

		void set3D(const std::string& path, bool enable)
		{
			if (allSounds.find(path) != allSounds.end())
				allSounds[path]->is3D = enable;
		}

		void setLoop(const std::string& path, bool enable)
		{
			if (allSounds.find(path) != allSounds.end())
				allSounds[path]->isLoop = enable;
		}

		void setVolume(const std::string& path, float volume)
		{
			if (allSounds.find(path) != allSounds.end())
				allSounds[path]->volume = std::max(0.0f, std::min(1.0f, volume));
		}

		SoundState getState(const std::string& path)
		{
			if (allSounds.find(path) != allSounds.end())
				return allSounds[path]->state;
			else
				return Stop;
		}

		void setState(const std::string& path, SoundState s)
		{
			if (allSounds.find(path) != allSounds.end())
				allSounds[path]->setState(s);
		}

		// init from lua
		void initFromLua(const sol::table& value) override
		{
			if (!value["audioFiles"].valid())
				return;
			sol::table audioFiles = value["audioFiles"];
			for (int i = 1; i <= audioFiles.size(); ++i)
			{
				if (!audioFiles[i].valid())
					continue;
				std::string path = audioFiles[i];
				auto sound = std::make_shared<SPWSound>(path);
				allSounds.insert({ path, sound });
			}
		}

		// update from lua
		void update(const std::string& key, const sol::table& value) override
		{
			if (key == "enable3D" && value["value"].valid())
			{
				std::string path = value["value"];
				set3D(path, true);
			}
			else if (key == "disable3D" && value["value"].valid())
			{
				std::string path = value["value"];
				set3D(path, false);
			}
			else if (key == "enableLoop" && value["value"].valid())
			{
				std::string path = value["value"];
				setLoop(path, true);
			}
			else if (key == "enableLoop" && value["value"].valid())
			{
				std::string path = value["value"];
				setLoop(path, false);
			}
			else if (key == "setState" && value["value"].valid())
			{
				if (!value["value"]["path"].valid() || !value["value"]["state"].valid())
					return;
				std::string path = value["value"]["path"];
				int s = value["value"]["state"];
				setState(path, (SoundState)s);
			}
			else if (key == "setVolume" && value["value"].valid())
			{
				if (!value["value"]["path"].valid() || !value["value"]["volume"].valid())
					return;
				std::string path = value["value"]["path"];
				float volume = value["value"]["volume"];
				setVolume(path, volume);
			}
		}

		// getLuaValue
		virtual sol::object getLuaValue(const sol::table& value, const std::string& key) override
		{
			if (key == "audioFiles")
			{
				sol::table audioFiles = sol::state_view(value.lua_state()).create_table();
				for (auto& sound: allSounds)
				{
					audioFiles[sound.first] = sound.second->state;
				}
				return audioFiles;
			}
			return sol::nil;
		}

	public:
		std::unordered_map<std::string, std::shared_ptr<SPWSound>> allSounds;
		std::string currentSoundPath{};
	};

}
#endif //SPARROW_AUDIOCOMPONENT_H
