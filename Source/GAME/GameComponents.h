#ifndef GAME_COMPONENTS_H_
#define GAME_COMPONENTS_H_

#include "../UTIL/ComponentReflection.h"

namespace GAME
{
	enum class ORBIT_AXIS : byte
	{
		X,
		Y,
		Z,
	};

	///*** Tags ***///
	COMPONENT(Sun) {};
	COMPONENT(Earth) {};
	COMPONENT(Player) {};
	COMPONENT(Obstacle) {};
	COMPONENT(Collidable) {};
	COMPONENT(GameManager) {};

	///*** Components ***///
	COMPONENT(Transform)
	{
		// aka Position
		GW::MATH::GVECTORF localTranslation = { 0, 0, 0, 1 };
		// rotation in euler
		GW::MATH::GVECTORF localRotation = { 0, 0, 0, 1 };
		// scale relative to parent
		GW::MATH::GVECTORF localScale = { 1, 1, 1, 1 };

		GW::MATH::GMATRIXF GetLocalMatrix()
		{
			GW::MATH::GMATRIXF translationMatrix = GW::MATH::GIdentityMatrixF, rotationMatrix = GW::MATH::GIdentityMatrixF, scaleMatrix = GW::MATH::GIdentityMatrixF, finalMatrix = GW::MATH::GIdentityMatrixF;

			GW::MATH::GMatrix::TranslateLocalF(translationMatrix, localTranslation, translationMatrix);

			float pitchRad = G_DEGREE_TO_RADIAN_F(localRotation.x), yawRad = G_DEGREE_TO_RADIAN_F(localRotation.y), rollRad = G_DEGREE_TO_RADIAN_F(localRotation.z);

			GW::MATH::GMATRIXF rollMatrix = GW::MATH::GIdentityMatrixF, pitchMatrix = GW::MATH::GIdentityMatrixF, yawMatrix = GW::MATH::GIdentityMatrixF;
			GW::MATH::GMatrix::RotateZLocalF(rollMatrix, rollRad, rollMatrix);
			GW::MATH::GMatrix::RotateXLocalF(pitchMatrix, pitchRad, pitchMatrix);
			GW::MATH::GMatrix::RotateYLocalF(yawMatrix, yawRad, yawMatrix);

			GW::MATH::GMatrix::MultiplyMatrixF(pitchMatrix, yawMatrix, rotationMatrix);
			GW::MATH::GMatrix::MultiplyMatrixF(rotationMatrix, rollMatrix, rotationMatrix);
			GW::MATH::GMatrix::ScaleLocalF(scaleMatrix, localScale, scaleMatrix);
			
			GW::MATH::GMatrix::MultiplyMatrixF(rotationMatrix, scaleMatrix, finalMatrix);
			finalMatrix.row4 = translationMatrix.row4;

			localMatrix = finalMatrix;
			return localMatrix;
		}

		GW::MATH::GMATRIXF localMatrix = GW::MATH::GIdentityMatrixF;
		GW::MATH::GMATRIXF worldMatrix = GW::MATH::GIdentityMatrixF;

		entt::entity parentID = entt::null;
	};

	COMPONENT(Inspectable)
	{
		std::string name = "";
	};

	COMPONENT(Orbit)
	{
		entt::entity parent = entt::null;
		float angularSpeed = 1.0f;
	};

	COMPONENT(Velocity)
	{
		GW::MATH::GVECTORF velocity;
	};

}// namespace GAME
#endif // !GAME_COMPONENTS_H_