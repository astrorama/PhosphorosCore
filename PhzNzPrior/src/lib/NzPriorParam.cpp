/**
 * @file src/lib/NzPriorParam.cpp
 * @date 2019-03-15
 * @author Florian dubath
 */

#include "PhzNzPrior/NzPriorParam.h"

#include <iterator>

#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"



namespace Euclid {
namespace PhzNzPrior {

void checkInputParam(size_t one_based_group_number, size_t max){
  if (one_based_group_number==0 || one_based_group_number>max){
    throw Elements::Exception() << "PhzNzPrior::NzPriorParam: the group number must be in {1,2,3} but the provided value is"
                                << one_based_group_number;
  }
}

static Elements::Logging logger = Elements::Logging::getLogger("NzPriorParam");

NzPriorParam::NzPriorParam(double z0t1, double kmt1, double alphat1, double kt1, double ft1, double cst1,
             double z0t2, double kmt2, double alphat2, double kt2, double ft2, double cst2,
             double z0t3, double kmt3, double alphat3, double cst3){
  m_z0t.push_back(z0t1);
  m_z0t.push_back(z0t2);
  m_z0t.push_back(z0t3);

  m_kmt.push_back(kmt1);
  m_kmt.push_back(kmt2);
  m_kmt.push_back(kmt3);

  m_alphat.push_back(alphat1);
  m_alphat.push_back(alphat2);
  m_alphat.push_back(alphat3);

  m_kt.push_back(kt1);
  m_kt.push_back(kt2);

  m_ft.push_back(ft1);
  m_ft.push_back(ft2);

  m_cst.push_back(cst1);
  m_cst.push_back(cst2);
  m_cst.push_back(cst3);

}

NzPriorParam NzPriorParam::defaultParam()  {
  return NzPriorParam(0.431,0.091,2.46,0.4,0.30, 0.8869,
                      0.390,0.100,1.81,0.3,0.35, 0.8891,
                      0.300,0.150,2.00,          0.8874);
}

double NzPriorParam::getZ0t(size_t one_based_group_number){
  checkInputParam(one_based_group_number, 3);
  return (m_z0t[one_based_group_number - 1]);
}

double NzPriorParam::getKmt(size_t one_based_group_number){
  checkInputParam(one_based_group_number, 3);
  return (m_kmt[one_based_group_number - 1]);
}

double NzPriorParam::getAlphat(size_t one_based_group_number){
  checkInputParam(one_based_group_number, 3);
  return (m_alphat[one_based_group_number - 1]);
}

double NzPriorParam::getKt(size_t one_based_group_number){
  checkInputParam(one_based_group_number, 2);
  return (m_kt[one_based_group_number - 1]);
}

double NzPriorParam::getFt(size_t one_based_group_number){
  checkInputParam(one_based_group_number, 2);
  return (m_ft[one_based_group_number - 1]);
}

double NzPriorParam::getCst(size_t one_based_group_number){
  checkInputParam(one_based_group_number, 3);
  return (m_cst[one_based_group_number - 1]);
}

}
}
