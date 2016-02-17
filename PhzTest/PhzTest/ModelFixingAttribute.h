/**
 * @file PhzTest/ModelFixingAttribute.h
 *
 * Created on: 2016-02-08
 *     Author: Florian Dubath
 */
#ifndef MODELFIXING_H_
#define MODELFIXING_H_

#include "SourceCatalog/Attribute.h"
#include "XYDataset/QualifiedName.h"

namespace Euclid {
namespace PhzTest {

/**
 * @class Coordinates
 * @brief Store the SED, Redshift, ReddeningCurve and
 * E(B-V) of the model fitting the source.
 */
class ModelFixingAttribute : public SourceCatalog::Attribute {
public:
  ModelFixingAttribute(XYDataset::QualifiedName sed, double z, XYDataset::QualifiedName red_curve, double ebv) :
    m_sed(sed), m_z(z), m_red_curve(red_curve), m_ebv(ebv) {}
  virtual ~ModelFixingAttribute() {}

  XYDataset::QualifiedName getSed() const { return m_sed; }

  double getZ() const { return m_z; }

  XYDataset::QualifiedName getRedCurve() const { return m_red_curve; }

  double getEbv() const { return m_ebv; }

private:
  XYDataset::QualifiedName m_sed;
  double m_z {};
  XYDataset::QualifiedName m_red_curve;
  double m_ebv {};

};

} // namespace PhzTest
} // end of namespace Euclid

#endif // MODELFIXING_H_
