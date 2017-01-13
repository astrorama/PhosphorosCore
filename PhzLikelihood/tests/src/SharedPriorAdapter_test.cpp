/**
 * @file tests/src/SharedPriorAdapter_test.cpp
 * @date Nov 4, 2015
 * @author Florian Dubath
 */
#include <boost/test/unit_test.hpp>
#include <boost/test/test_tools.hpp>
#include "ElementsKernel/EnableGMock.h"

#include "XYDataset/QualifiedName.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzDataModel/ScaleFactorGrid.h"
#include "PhzDataModel/LikelihoodGrid.h"

#include "PhzLikelihood/SharedPriorAdapter.h"

using namespace testing;
using namespace Euclid;


struct SharedPriorAdaptor_Fixture {

  class PriorMock{
  public:

    void operator()(PhzDataModel::LikelihoodGrid& likelihoodGrid,
            const SourceCatalog::Photometry& sourcePhotometry,
            const PhzDataModel::PhotometryGrid& modelGrid,
            const PhzDataModel::ScaleFactordGrid& scaleFactorGrid) const {
      internal_call(likelihoodGrid, sourcePhotometry, modelGrid, scaleFactorGrid);
    }

    MOCK_CONST_METHOD4(internal_call, void(PhzDataModel::LikelihoodGrid&,
        const SourceCatalog::Photometry&,
        const PhzDataModel::PhotometryGrid&,
        const PhzDataModel::ScaleFactordGrid&));

  };

  XYDataset::QualifiedName luminosityFilterName{"group/FilterName"};

  std::vector<double> zs{0.0,0.2,0.6,1.0};
  std::vector<double> ebvs{0.0,0.1,0.2};
  std::vector<Euclid::XYDataset::QualifiedName> reddeing_curves{{"reddeningCurves/Curve1"}};
  std::vector<Euclid::XYDataset::QualifiedName> seds{{"sed/Curve_1"},{"sed/Curve_2"},{"sed/Curve_3"}};

  Euclid::PhzDataModel::ModelAxesTuple parameter_space= Euclid::PhzDataModel::createAxesTuple(zs,ebvs,reddeing_curves,seds);

  // Create the Grids
  PhzDataModel::ScaleFactordGrid scale_factor_grid{parameter_space};

  PhzDataModel::PhotometryGrid photometry_grid {parameter_space};

  PhzDataModel::LikelihoodGrid likelihood_grid {parameter_space};


  // Shared pointer of filter name vector
  std::shared_ptr<std::vector<std::string>> filter_name_vector_ptr {
      new std::vector<std::string> { "group/FilterName1" ,"group/FilterName2" } };

  // photometry values vector
  std::vector<SourceCatalog::FluxErrorPair> photometry_vector { SourceCatalog::FluxErrorPair(1.0,
      0.1), SourceCatalog::FluxErrorPair(2.0, 0.3) };

  SourceCatalog::Photometry  ref_photometry = SourceCatalog::Photometry{filter_name_vector_ptr,photometry_vector};


  SharedPriorAdaptor_Fixture(){


    double value = 1.7;
    for (auto& photometry : photometry_grid){
      std::vector<SourceCatalog::FluxErrorPair> photometry_vector { SourceCatalog::FluxErrorPair(value,
           0.1), SourceCatalog::FluxErrorPair(0.1, 0.3) };
      photometry = SourceCatalog::Photometry{filter_name_vector_ptr,photometry_vector};
      value+=1.;
    }

    value = 1.3;
    for (auto& scale_factor : scale_factor_grid){
      scale_factor = value;
      value+=1.;
    }

    value = 1.9;
    for (auto& likelihood : likelihood_grid){
      likelihood=value;
      value+=1.;
    }


  }

};



BOOST_AUTO_TEST_SUITE (SharedPriorAdapter_test)

BOOST_FIXTURE_TEST_CASE( functorCall, SharedPriorAdaptor_Fixture ) {

  std::shared_ptr<PriorMock> prior_ptr{new PriorMock()};

  EXPECT_CALL(*prior_ptr,internal_call(_, _, _, _));

  PhzLikelihood::SharedPriorAdapter<PriorMock> adaptor{prior_ptr};

  adaptor(likelihood_grid,ref_photometry,photometry_grid,scale_factor_grid);

}

BOOST_FIXTURE_TEST_CASE( copy, SharedPriorAdaptor_Fixture ) {

  std::shared_ptr<PriorMock> prior_ptr{new PriorMock()};

  EXPECT_CALL(*prior_ptr,internal_call(_, _, _, _)).Times(2);

  PhzLikelihood::SharedPriorAdapter<PriorMock> adaptor{prior_ptr};

  PhzLikelihood::SharedPriorAdapter<PriorMock> adaptor_copied{adaptor};

  adaptor_copied(likelihood_grid,ref_photometry,photometry_grid,scale_factor_grid);
  adaptor(likelihood_grid,ref_photometry,photometry_grid,scale_factor_grid);

}

BOOST_FIXTURE_TEST_CASE( assigned, SharedPriorAdaptor_Fixture ) {

  std::shared_ptr<PriorMock> prior_ptr{new PriorMock()};

  EXPECT_CALL(*prior_ptr,internal_call(_, _, _, _)).Times(2);

  PhzLikelihood::SharedPriorAdapter<PriorMock> adaptor{prior_ptr};

  PhzLikelihood::SharedPriorAdapter<PriorMock> adaptor_assigned =adaptor;

  adaptor_assigned(likelihood_grid,ref_photometry,photometry_grid,scale_factor_grid);
  adaptor(likelihood_grid,ref_photometry,photometry_grid,scale_factor_grid);

}


BOOST_AUTO_TEST_SUITE_END ()
