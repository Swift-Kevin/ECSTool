#ifndef UTILITIES_H_
#define UTILITIES_H_

#include "GameConfig.h"
#include "../DRAW/DrawComponents.h"
#include "../GAME/GameComponents.h"
#include "../APP/ConsoleLog.h"

namespace UTIL
{
	static const std::string PlayerName = "Player";

	enum DebugHierarchy { BaseRender, Rotation, Translation, Scale, Combined, SolarSystem };

	COMPONENT(DebugInfo)
	{
		DebugHierarchy debugMode;
		float theta = 0.0f;
	};

	COMPONENT(Config)
	{
		std::shared_ptr<GameConfig> gameConfig = std::make_shared<GameConfig>();
	};

	COMPONENT(DeltaTime)
	{
		double dtSec = 0.0f;;
		double totalTime = 0.0f;
	};

	COMPONENT(Input)
	{
		GW::INPUT::GController gamePads; // controller support
		GW::INPUT::GInput immediateInput; // twitch keybaord/mouse
		GW::INPUT::GBufferedInput bufferedInput; // event keyboard/mouse
	};

	/// Method declarations
	void UpdateWorldMatrix(GAME::Transform& t, const GW::MATH::GMATRIXF* parentWorld = nullptr);
	void CreateModelEntity(entt::registry& registry, entt::entity entity, std::string entityName, GAME::Transform* _transform = nullptr);
	void SetupCamera(entt::registry& registry, entt::entity entity);

	void PrintVector(GW::MATH::GVECTORF toPrint);
	void CreateOrbiter(entt::registry& registry, entt::entity orbiter, entt::entity orbiting, std::string iniName = "");
	void UpdateWorldPosition(GAME::Transform parentWorld, GAME::Transform& childWorld, float radius);
	
	// Hierarchy
	void UpdateChildren(entt::registry& registry, entt::entity parent);
	void ComputeHierarchy(entt::registry& registry);

	// Math Helpers
	GW::MATH::GVECTORF EulerFromQuaternion(const GW::MATH::GQUATERNIONF& _quat);

} // namespace UTIL


namespace RANDOM 
{
	/// Creates a normalized vector pointing in a random direction on the X/Z plane
	GW::MATH::GVECTORF GetRandomVelocityVector();
	GW::MATH::GMATRIXF GetRandomTransform(GW::MATH::GVECTORF min, GW::MATH::GVECTORF max);
	float GetRandomRange(float min = 0, float max = 0);
	GW::MATH::GVECTORF GetRotationFromMatrix(GW::MATH::GMATRIXF matrix);
}

#endif // !UTILITIES_H_