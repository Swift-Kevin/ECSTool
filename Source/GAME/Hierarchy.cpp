#include "Hierarchy.h"

namespace HIERARCHY
{
	GW::MATH::GMATRIXF ConvertWorldToLocal(GW::MATH::GMATRIXF parent, GW::MATH::GMATRIXF child)
	{
		GW::MATH::GMATRIXF local, parentInv = GW::MATH::GIdentityMatrixF;
		GW::MATH::GMatrix::InverseF(parent, parentInv);
		GW::MATH::GMatrix::MultiplyMatrixF(parentInv, child, local);

		//GW::MATH::GMatrix::MultiplyMatrixF(parent, child, local);

        return local;
	}
}