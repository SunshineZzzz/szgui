#pragma once

#include "InputControl.h"

#include <SDL3/SDL.h>

namespace sz_gui
{
	InputControl::InputControl() {}

	InputControl::~InputControl() {}

	void InputControl::OnMouseButton(std::any eventContainer)
	{
		const SDL_Event* event = std::any_cast<SDL_Event*>(eventContainer);
		if (!event)
		{
			return;
		}

		m_currentX = event->button.x;
		m_currentY = event->button.y;

		switch (event->button.button)
		{
		case SDL_BUTTON_LEFT:
		{
			m_mouseLeftIsDown = event->type == SDL_EVENT_MOUSE_BUTTON_DOWN;
		}
		break;
		case SDL_BUTTON_RIGHT:
		{
			m_mouseRightIsDown = event->type == SDL_EVENT_MOUSE_BUTTON_DOWN;
		}
		break;
		case SDL_BUTTON_MIDDLE:
		{
			m_mouseMiddleIsDown = event->type == SDL_EVENT_MOUSE_BUTTON_DOWN;
		}
		break;
		}
	}

	void InputControl::OnMouseMotion(std::any eventContainer)
	{
		const SDL_Event* event = std::any_cast<SDL_Event*>(eventContainer);
		if (!event)
		{
			return;
		}

		m_currentX = event->motion.x;
		m_currentY = event->motion.y;
	}
}