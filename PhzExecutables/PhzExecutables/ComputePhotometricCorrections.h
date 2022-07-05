/*
 * Copyright (C) 2012-2020 Euclid Science Ground Segment
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/*
 * @file ComputePhotometricCorrections.h
 * @author nikoapos
 */

#ifndef PHZEXECUTABLES_COMPUTEPHOTOMETRICCORRECTIONS_H
#define PHZEXECUTABLES_COMPUTEPHOTOMETRICCORRECTIONS_H

#include "Configuration/ConfigManager.h"
#include "PhzPhotometricCorrection/PhotometricCorrectionCalculator.h"

namespace Euclid {
namespace PhzExecutables {

/**
 * @class ComputeRedshifts
 * @brief
 *
 */
class ComputePhotometricCorrections {

public:
  using ProgressListener = PhzPhotometricCorrection::PhotometricCorrectionCalculator::ProgressListener;

  ComputePhotometricCorrections();

  ComputePhotometricCorrections(ProgressListener progress_listener);

  void run(Configuration::ConfigManager& config_manager);

  /**
   * @brief Destructor
   */
  virtual ~ComputePhotometricCorrections() = default;

private:
  ProgressListener m_progress_listener;

}; /* End of ComputePhotometricCorrections class */

} /* namespace PhzExecutables */
} /* namespace Euclid */

#endif /* PHZEXECUTABLES_COMPUTEPHOTOMETRICCORRECTIONS_H */
