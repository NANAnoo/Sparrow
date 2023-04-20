//
// Created by 29134 on 2023/2/26.
//
#pragma once
#include "EcsFramework/Scene.hpp"
#include "Control/MouseEvent.hpp"
#include "EcsFramework/System/SystemI.h"
#include "EcsFramework/Component/MouseComponent.hpp"

namespace SPW
{
	class MouseControlSystem : public SPW::MouseEventResponder, public SPW::SystemI
	{
	public:
		MouseControlSystem(std::shared_ptr<Scene>& scene): MouseEventResponder(scene), SystemI(scene)
		{
		};

		bool onMouseDown(SPW::MouseEvent* e) override
		{
			mouse_queue.push(e);

			return false;
		}

		bool onMouseHeld(SPW::MouseEvent* e) override
		{
			mouse_queue.push(e);

			return false;
		};

		bool onMouseReleased(MouseEvent* e) override
		{
			mouse_queue.push(e);

			return false;
		};

		bool onMouseScroll(MouseEvent* e) override
		{
			mouse_queue.push(e);

			return false;
		};

		bool cursorMovement(MouseEvent* e) override
		{
			mouse_queue.push(e);

			return false;
		};

		void initial() override
		{
		};

		void beforeUpdate() override
		{
			while (mouse_queue.size() != 0)
			{
				auto e = mouse_queue.front();

				switch (e->type())
				{
				case SPW::EventType::MouseDownType:
					locatedScene.lock()->forEachEntity<MouseComponent>([&e](const Entity& entity)
					{
						if (entity.component<MouseComponent>()->onMouseDownCallBack)
							entity.component<MouseComponent>()->onMouseDownCallBack(entity, e->button_code);
					});
					break;

				case SPW::EventType::MouseHeldType:

					locatedScene.lock()->forEachEntity<MouseComponent>([&e](const Entity& entity)
					{
						if (entity.component<MouseComponent>()->onMouseHeldCallBack)
							entity.component<MouseComponent>()->onMouseHeldCallBack(entity, e->button_code);
					});
					break;

				case SPW::EventType::MouseReleasedType:

					locatedScene.lock()->forEachEntity<MouseComponent>([&e](const Entity& entity)
					{
						if (entity.component<MouseComponent>()->onMouseReleasedCallBack)
							entity.component<MouseComponent>()->onMouseReleasedCallBack(entity, e->button_code);
					});
					break;

				case SPW::EventType::MouseScrollType:

					locatedScene.lock()->forEachEntity<MouseComponent>([&e](const Entity& entity)
					{
						if (entity.component<MouseComponent>()->onMouseScrollCallBack)
							entity.component<MouseComponent>()->onMouseScrollCallBack(entity, e->scroll_offset);
					});
					break;

				case SPW::EventType::CursorMovementType:

					locatedScene.lock()->forEachEntity<MouseComponent>([&e](const Entity& entity)
					{
						if (entity.component<MouseComponent>()->cursorMovementCallBack)
							entity.component<MouseComponent>()->cursorMovementCallBack(
								entity, e->cursor_xpos, e->cursor_ypos, e->cursor_xpos_bias, e->cursor_ypos_bias);
					});
					break;
				default:
					break;
				}
				mouse_queue.pop();
			}
		};

		void onUpdate(TimeDuration dt) override
		{
		};

		void afterUpdate() override
		{
		};

		void onStop() override
		{
		};

	private:
		std::queue<MouseEvent*> mouse_queue;
	};
}
