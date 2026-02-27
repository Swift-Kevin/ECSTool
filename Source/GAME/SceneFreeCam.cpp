#include "GAMEComponents.h"
#include "../CCL.h"

#include "../UTIL/Utilities.h"
#include "../APP/Window.hpp"

namespace GAME
{
	bool UpdateSceneFreeCamInput(entt::registry& registry, entt::entity entity, GW::MATH::GVECTORF& out)
	{
		auto& input = registry.ctx().get<UTIL::Input>();
		auto& deltaTime = registry.ctx().get<UTIL::DeltaTime>();

		std::shared_ptr<const GameConfig> config = registry.ctx().get<UTIL::Config>().gameConfig;
		unsigned int width = config->at("Window").at("width").as<unsigned int>();
		unsigned int height = config->at("Window").at("height").as<unsigned int>();
		float aspectRatio = (float)height / (float)width;
		float fov = config->at("Render").at("fov").as<float>();

		// MOVE ( X / Y )
		{
			float moveSpeed = config->at(UTIL::PlayerName).at("moveSpeed").as<float>();
			float keyboardPositive = 0, keyboardNegative = 0;

			input.immediateInput.GetState(G_KEY_D, keyboardPositive);
			input.immediateInput.GetState(G_KEY_A, keyboardNegative);
			out.x = (keyboardPositive - keyboardNegative) * (moveSpeed * deltaTime.dtSec);

			input.immediateInput.GetState(G_KEY_W, keyboardPositive);
			input.immediateInput.GetState(G_KEY_S, keyboardNegative);
			out.y = (keyboardPositive - keyboardNegative) * (moveSpeed * deltaTime.dtSec);
		}

		// LOOK ( Z / W )
		{
			float camSpeed = config->at(UTIL::PlayerName).at("camSpeed").as<float>() * deltaTime.dtSec;
			GW::MATH::GVECTORF states = { 0, 0, 0, 0 };

			auto res = input.immediateInput.GetMouseDelta(states.x, states.y);
			input.gamePads.GetState(0, G_RIGHT_TRIGGER_AXIS, states.z);

			if (res == GW::GReturn::REDUNDANT)
				states.x = states.y = 0.0f;
			else if (abs(states.x) <= 0.15f)
				states.x = 0.0f;
			else if (abs(states.y) <= 0.15f)
				states.y = 0.0f;

			// Update total pitch
			out.z = G_PI / 2 * states.y / height + states.z * -camSpeed;

			// Update total yaw
			out.w = G_PI / 2 * aspectRatio * states.x / width + states.z * camSpeed;
		}

		float mouse2State = 0.0f;
		input.immediateInput.GetState(G_BUTTON_RIGHT, mouse2State);
		return mouse2State;
	}

	void UpdateSceneFreeCamPosition(entt::registry& registry, entt::entity entity, GW::MATH::GVECTORF inputStates)
	{
		// Translate Locally on X and Z
		GW::MATH::GVECTORF deltaPos = { inputStates.x, 0, inputStates.y, 1 };
		GW::MATH::GVECTORF& playerTransform = registry.get<Transform>(entity).localTranslation;
		GW::MATH::GVector::AddVectorF(playerTransform, deltaPos, playerTransform);
	}

	void UpdateSceneFreeCamRotation(entt::registry& registry, entt::entity entity, GW::MATH::GVECTORF inputStates)
	{
		GW::MATH::GVECTORF& playerRot = registry.get<Transform>(entity).localRotation;
		playerRot.x = std::clamp(playerRot.x + G_RADIAN_TO_DEGREE_F(inputStates.z), -89.f, 89.f);
		playerRot.y += G_RADIAN_TO_DEGREE_F(inputStates.w);
	}

	void UpdateSceneFreeCam(entt::registry& registry, entt::entity entity)
	{
		bool focused = false;
		auto& win = registry.get<GW::SYSTEM::GWindow>(registry.group<GW::SYSTEM::GWindow>().front());
		win.IsFocus(focused);
		if (!focused)
			return;

		GW::MATH::GVECTORF input = { 0, 0, 0, 0 };
		if (UpdateSceneFreeCamInput(registry, entity, input))
		{
			UpdateSceneFreeCamPosition(registry, entity, input);
			UpdateSceneFreeCamRotation(registry, entity, input);
		}

		auto& camera = registry.get<DRAW::Camera>(registry.view<DRAW::Camera>().front());
		camera.camMatrix = registry.get<Transform>(entity).GetLocalTransform();
	}

	CONNECT_COMPONENT_LOGIC() {
		registry.on_update<Player>().connect<UpdateSceneFreeCam>();
	}
} // namespace DRAW