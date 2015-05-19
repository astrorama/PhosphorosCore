/** 
 * @file PowerFunctions.h
 * @date May 19, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZMODELING_POWERFUNCTIONS_H
#define	PHZMODELING_POWERFUNCTIONS_H

#include <memory>
#include "MathUtils/function/Function.h"

namespace Euclid {
namespace PhzModeling {

extern std::unique_ptr<MathUtils::Function> pow_5_5;

extern std::unique_ptr<MathUtils::Function> pow_4_5;

extern std::unique_ptr<MathUtils::Function> pow_3_7;

extern std::unique_ptr<MathUtils::Function> pow_3_46;

extern std::unique_ptr<MathUtils::Function> pow_3_4;

extern std::unique_ptr<MathUtils::Function> pow_3_3;

extern std::unique_ptr<MathUtils::Function> pow_2_5;

extern std::unique_ptr<MathUtils::Function> pow_2_3;

extern std::unique_ptr<MathUtils::Function> pow_2_1;

extern std::unique_ptr<MathUtils::Function> pow_1_68;

extern std::unique_ptr<MathUtils::Function> pow_1_6;

extern std::unique_ptr<MathUtils::Function> pow_1_5;

extern std::unique_ptr<MathUtils::Function> pow_1_2;

extern std::unique_ptr<MathUtils::Function> pow_m0_3;

extern std::unique_ptr<MathUtils::Function> pow_m0_9;

extern std::unique_ptr<MathUtils::Function> pow_1o3;

extern std::unique_ptr<MathUtils::Function> pow_1o6;

} // end of namespace PhzModeling
} // end of namespace Euclid

#endif	/* PHZMODELING_POWERFUNCTIONS_H */

