/**
 * @file tests/src/ChiSquareLikelihoodLogarithm_test.cpp
 * @date dec 15, 2014
 * @author Florian Dubath
 */

#include <boost/test/unit_test.hpp>
#include "ElementsKernel/EnableGMock.h"

#include <vector>
#include <memory>
#include <tuple>

#include "PhzLikelihood/ChiSquareLikelihoodLogarithm.h"

using std::vector;
using namespace Euclid;
using namespace Euclid::PhzLikelihood;
using namespace Euclid::PhzLikelihood::_Impl;
using namespace testing;

class MockResidualCalculator {
public:
  virtual ~MockResidualCalculator() = default;
  MOCK_METHOD3(FunctorCall, double(double source, double model, double scale));
};
std::unique_ptr<MockResidualCalculator> mock_residual_calculator_singleton {};

class TestResidualCalculator {
public:
  double operator()(double source, double model, double scale) {
    if (mock_residual_calculator_singleton == nullptr) {
      BOOST_FAIL("TestResidualCalculator was called when the MockResidualCalculator was not set");
    }
    return mock_residual_calculator_singleton->FunctorCall(source, model, scale);
  }
};

class MockChiSquareFunc {
public:
  virtual ~MockChiSquareFunc() = default;
  MOCK_METHOD3(FunctorCall, double(const SourceCatalog::FluxErrorPair& source,
                                   const SourceCatalog::FluxErrorPair& model,
                                   double scale));
};
std::unique_ptr<MockChiSquareFunc> mock_chi_square_func_singleton {};

class TestChiSquareFunc {
public:
  double operator()(const SourceCatalog::FluxErrorPair& source,
                    const SourceCatalog::FluxErrorPair& model,
                    double scale) {
    if (mock_chi_square_func_singleton == nullptr) {
      BOOST_FAIL("TestChiSquareFunc was called when the MockChiSquareFunc was not set");
    }
    return mock_chi_square_func_singleton->FunctorCall(source, model, scale);
  }
};

struct ChiSquareLikelihoodLogarithm_Fixture {
  
  double close_tolerance = 1E-8;
  
  vector<tuple<double, double, double, double>> test_data {
    //         Source flux      Source error     Model flux        Scale factor
    make_tuple(4.87346640756,   0.147328262134,  1.15975981919,    0.0588462623953),                                                                    
    make_tuple(1.35176981808,   0.818651371909,  2.80736165239,    4.30658912186),                                                                      
    make_tuple(3.56239241725,   3.34626954898,   2.00112510102,    0.194488370887),
    make_tuple(2.02626126842,   2.49863410178,   4.19565805995,    4.89778815759),
    make_tuple(2.77986521823,   4.28478822179,   3.60764458216,    1.37614806066),
    make_tuple(2.93047694238,   1.96999654647,   0.653579630354,   1.69567461853),
    make_tuple(4.50089348673,   4.01930257481,   0.134318844251,   1.16184089243),
    make_tuple(4.56908425868,   3.76715564196,   1.01049651946,    4.79732551362),
    make_tuple(2.92428782298,   4.40397398207,   0.51228473317,    3.8448282293),
    make_tuple(0.228713016422,  4.33140549016,   4.07580083892,    4.89995527571),
    make_tuple(2.07324214579,   2.42452539484,   2.58124744224,    0.893175852721),
    make_tuple(4.98337713256,   0.880371019467,  4.71877846589,    2.65497624712),
    make_tuple(1.6309657416,    0.197037372492,  2.023566943,      3.15625407668),
    make_tuple(1.58787356486,   3.69304755144,   0.878717162818,   1.56283004201),
    make_tuple(0.398328228727,  0.564299898515,  3.79091735809,    4.82410751552),
    make_tuple(2.07992087108,   4.92556886158,   0.917230598738,   0.449229862738),
    make_tuple(1.16085472332,   2.87562845331,   2.06250607842,    2.06598996414),
    make_tuple(3.18102218613,   3.05429152558,   1.65367245881,    1.62772201536),
    make_tuple(4.07372580932,   1.38585408264,   4.69803254807,    2.03523658189),
    make_tuple(4.68293923404,   4.24577317772,   3.56890778414,    4.09428618361),
    make_tuple(4.98617868732,   3.8640122922,    3.13933936166,    4.03405744758),
    make_tuple(2.86956068792,   0.324724416732,  2.39457215227,    4.80818726673),
    make_tuple(1.58266771733,   1.07409164944,   2.92139937426,    0.654929060022),
    make_tuple(1.60954754978,   4.6438317375,    3.75839421371,    0.422960497474),
    make_tuple(1.17730464843,   1.58523135853,   3.45065951413,    1.44619977722),
    make_tuple(1.68453939944,   2.42780142448,   1.86277589748,    4.74905856316),
    make_tuple(4.04554456938,   3.73325484813,   0.529747937208,   1.64905696539),
    make_tuple(0.126089820779,  3.88215816106,   0.744215077793,   4.45183624155),
    make_tuple(1.30276469789,   2.71365644204,   0.974422269604,   3.29930177333),
    make_tuple(0.782934304927,  2.5100938609,    1.77795214201,    3.59350997373),
    make_tuple(0.564991165896,  2.95637509935,   4.74422786133,    1.84457164433),
    make_tuple(1.18365486301,   4.47220280123,   4.68357507077,    3.33886475815),
    make_tuple(2.62974940202,   2.43519772705,   2.07904425885,    3.47542427044),
    make_tuple(2.83941024374,   4.34361243332,   4.60190211841,    0.0314237436296),
    make_tuple(4.6252312026,    2.62001049343,   4.75046799088,    1.01631464158),
    make_tuple(3.37085041284,   2.77690164144,   3.54695162423,    1.56280835875),
    make_tuple(1.52502274271,   4.79289911997,   2.66290030799,    1.36070859887),
    make_tuple(0.397940722711,  3.17638994635,   3.46315253814,    1.39115322231),
    make_tuple(1.2855552388,    3.40877464456,   0.0403553704623,  2.50080264508),
    make_tuple(4.1394291559,    0.339745959281,  4.69049995838,    2.92462533794),
    make_tuple(3.55033381757,   0.0583873992341, 2.46186341172,    0.0796350077754),
    make_tuple(3.42879973171,   3.01373899825,   0.025323931248,   3.9159993685),
    make_tuple(2.70658759102,   3.84774542084,   4.15805523496,    2.81718123464),
    make_tuple(1.70335198648,   2.10645755648,   1.61219358302,    4.64574443357),
    make_tuple(3.61020533912,   4.7189685623,    2.27402544139,    3.82950427068),
    make_tuple(4.63870154493,   4.22462081431,   4.66261098982,    0.842389184097),
    make_tuple(1.18651902785,   1.39199052751,   1.40698765948,    1.07447986916),
    make_tuple(1.82084640567,   1.20248948461,   3.2533709604,     4.3634004528),
    make_tuple(1.37688769481,   3.20158478057,   0.495373573602,   0.527503302504),
    make_tuple(0.750672188063,  3.45469951703,   4.05355569547,    4.4385874587),
    make_tuple(0.0194365280162, 1.91533882108,   3.35169082581,    4.64326286372),
    make_tuple(2.84033221222,   3.21721538503,   4.94411822525,    3.59978472028),
    make_tuple(4.92038151094,   1.63046431773,   1.95738521749,    2.34506827715),
    make_tuple(3.15866313183,   4.10716326935,   0.491115846098,   3.58098240291),
    make_tuple(0.497006281291,  1.40926583028,   3.14653251716,    4.1758485722),
    make_tuple(2.2372886136,    3.92132470735,   1.30683140022,    1.0234860921),
    make_tuple(3.03040592465,   4.58365287922,   3.26483182372,    1.40369341701),
    make_tuple(0.896240866505,  4.26909713382,   4.91371256098,    3.77639958522),
    make_tuple(1.70671059599,   1.15664382232,   0.50128935941,    1.46931745845),
    make_tuple(1.60951948527,   0.639401250766,  4.59612221554,    0.431628713816),
    make_tuple(2.36648018484,   0.661842969207,  2.37539101054,    4.20380064821),
    make_tuple(1.95327909544,   4.53343153309,   2.35145387689,    0.502345439337),
    make_tuple(0.737974375153,  4.3568237192,    0.801742284831,   0.0486510907404),
    make_tuple(4.70936421209,   4.12096268483,   2.61537291482,    4.18056004832),
    make_tuple(4.03732819514,   4.8015524323,    3.95306040676,    0.99749271835),
    make_tuple(2.59708065683,   4.64488537506,   3.21035214457,    1.35142162615),
    make_tuple(4.38441919999,   3.80377087515,   4.88786589644,    3.70751504869),
    make_tuple(3.3435405498,    1.658609537,     0.44799160497,    2.83044641239),
    make_tuple(3.36039179898,   0.930348069616,  3.98320952554,    1.73344753449),
    make_tuple(2.9585318665,    4.73968156209,   3.26388552636,    2.32925668764),
    make_tuple(3.39605623893,   2.67379410272,   0.687927611,      3.98701639976),
    make_tuple(2.26177432166,   3.57911917353,   0.831792584336,   2.41112249566),
    make_tuple(4.8997861532,    2.86560068089,   1.27403579741,    1.44269774684),
    make_tuple(3.92227257881,   4.07374181672,   4.26797763142,    0.911837462853),
    make_tuple(0.300009277479,  1.27255645855,   3.2560608213,     3.16604863248),
    make_tuple(2.31971281998,   4.84056461476,   1.60091575413,    1.30750129911),
    make_tuple(3.18607836545,   0.899188949261,  4.02637171909,    2.87483897231),
    make_tuple(1.01596134144,   1.4721668943,    0.604137753106,   4.31030906486),
    make_tuple(3.83009404625,   4.5954276324,    4.72347440055,    2.99878521792),
    make_tuple(2.7548483979,    3.53543138298,   3.92083134992,    2.45846432278),
    make_tuple(2.06431367043,   2.0264737766,    4.97293195015,    0.470686856352),
    make_tuple(1.71082325293,   4.01503162153,   1.15861588245,    4.89205914229),
    make_tuple(4.49026867859,   3.60743884581,   0.882394320117,   4.30720423596),
    make_tuple(2.9815211701,    4.81920454213,   2.98786186757,    1.74474449001),
    make_tuple(2.98504224747,   4.09913580505,   0.923072277643,   1.89569509238),
    make_tuple(2.01768364023,   4.13130368561,   2.29892469602,    3.67549558873),
    make_tuple(4.04685007755,   0.310757020804,  3.99899467621,    4.50992291384),
    make_tuple(0.0655302334554, 0.648505577473,  2.97542994925,    0.821159082215),
    make_tuple(0.973058934152,  0.883918041737,  0.337062555122,   4.89040491613),
    make_tuple(2.11416579353,   2.73580957493,   2.79044278813,    2.14168700794),
    make_tuple(4.60953435105,   0.274662353947,  1.18850999902,    4.88301775738),
    make_tuple(0.622998605333,  3.53870167818,   4.61645903489,    2.95372950293),
    make_tuple(1.90422626176,   3.16171234127,   4.06889646863,    0.620302029171),
    make_tuple(2.83611932865,   1.81675604854,   2.87475807015,    1.95203035126),
    make_tuple(3.19058340807,   3.71374868576,   1.46085866989,    0.824458216425),
    make_tuple(1.66471172336,   4.54969739165,   1.7215101777,     2.60992078457),
    make_tuple(4.15484616448,   1.2501508154,    0.00328150694694, 2.79661768564),
    make_tuple(2.77705562377,   3.82104254934,   4.6328259136,     1.40649732065),
    make_tuple(2.67291417812,   4.8160222852,    0.308371595531,   1.42278986658),
    make_tuple(3.142386677,     1.04986361414,   0.880691306516,   1.46249792769)
  };
  
  vector<double> chi_square_data {
    1063.785869015169,
    172.0599603523264,
    0.8992329001838121,
    54.95736642114042,
    0.2599919082512001,
    0.8555984676950313,
    1.1685450121842822,
    0.0054692038213795076,
    0.0469884039509647,
    20.77532734336956,
    0.009177340233459896,
    73.44661276127589,
    582.6032489007681,
    0.0033762997891703755,
    1005.0200968362471,
    0.11466039252900344,
    1.1623349268493552,
    0.025664629164904248,
    15.68102696291849,
    5.469077989174785,
    3.9484818861560735,
    708.5959821406087,
    0.09476159959309376,
    1.8354663976251606e-05,
    5.785715067997423,
    8.70220406834054,
    0.7219051506838788,
    0.6739487908941751,
    0.49651625665536187,
    4.988272772285099,
    7.667125669023741,
    10.445837628980895,
    3.5616886373137904,
    0.38490304380797014,
    0.005987815552239399,
    0.6119848833046667,
    0.1916829746715228,
    1.936174542520018,
    0.12077362832664866,
    794.8565413079425,
    3300.3592777680283,
    1.2206214496630676,
    5.480080907694947,
    7.546135418622665,
    1.1671776949671053,
    0.028322085035574287,
    0.05459980956624694,
    105.90630070766044,
    0.1214139811260656,
    24.90714544566146,
    65.856364381918,
    21.61491977836802,
    0.041008859912110465,
    0.11618857000247743,
    80.47773686095353,
    0.05264927987935968,
    0.1147079043724327,
    17.112145606482173,
    0.7035340245931965,
    0.3426809704996635,
    132.52801640264903,
    0.029001607248698606,
    0.025738043339410224,
    2.2813501394205424,
    0.00038472186196538023,
    0.14056472132149544,
    13.043119580373402,
    1.565911942084213,
    14.513370954265085,
    0.9599895710311656,
    0.059695222435737945,
    0.005124782797586656,
    1.1415753588689546,
    5.6314898530910093e-05,
    61.860463421328646,
    0.0021897559878807653,
    87.04182521483295,
    1.163641557237057,
    5.057486573995309,
    3.7917900873351718,
    0.018600788811751322,
    0.971397050484918,
    0.036544065516784234,
    0.2144156286151374,
    0.09079772071347145,
    2.4239204486058705,
    2026.228650260469,
    13.443509384484003,
    0.5836961012507009,
    1.992842085440592,
    18.897163193856198,
    13.522337774153192,
    0.03841883373576135,
    2.333930292692205,
    0.28602649991146584,
    0.38644164119030805,
    10.996752239529465,
    0.9575193449347575,
    0.21520580281696183,
    3.1198270113748054
  };
  
  vector<double> upper_limit_data {
    -0.0,
    1416.7928370645282,
    0.37626177515910625,
    60.83746752577336,
    2.374474870197735,
    0.3907812132380387,
    0.3012974413302126,
    1.5078188858710364,
    1.0695350528176633,
    25.733736611643284,
    1.545072107434156,
    1416.7928370645282,
    1416.7928370645282,
    1.295705756179427,
    1416.7928370645282,
    0.9159911918190872,
    3.9252249581715284,
    1.1466827502555423,
    20.38307338533184,
    9.275817134627959,
    7.505186594768413,
    1416.7928370645282,
    1.9398871384859706,
    1.3794693914567073,
    9.637107533422759,
    12.88914016909725,
    0.44069454832003113,
    3.1613206891675723,
    2.849925430381747,
    8.722905907490604,
    11.747861952438061,
    14.788977769006236,
    7.042410623259736,
    0.6225713449063192,
    1.5136218247465534,
    3.0555248745559136,
    2.212725789857238,
    5.001041853413574,
    0.9054231050760341,
    1416.7928370645282,
    -0.0,
    0.2891716530000675,
    9.288407488094496,
    11.613660830256878,
    3.9323549457993643,
    1.135416085771069,
    1.7948293504699662,
    1416.7928370645282,
    0.9043367649577074,
    30.033672635773787,
    72.087306778234307,
    26.609873238386502,
    1.0886283509450232,
    0.9133151817565195,
    1416.7928370645282,
    1.0527515215697787,
    2.002481312425727,
    21.89316484255294,
    0.4482865425332616,
    2.5520629277665443,
    1416.7928370645282,
    1.132633196883628,
    1.1463627903734765,
    5.452359431518844,
    1.3552387939416117,
    2.077707150243211,
    17.58062605165467,
    0.22275906890956518,
    19.145948234388882,
    3.6207258587282487,
    1.0333157027523332,
    1.2752927243521024,
    0.30784105356668373,
    1.3743550109296725,
    67.86688037754114,
    1.313007168072548,
    1416.7928370645282,
    3.927149361948556,
    8.802845169722621,
    7.318465063372369,
    1.6159555450534713,
    3.6382695198632247,
    1.1039833876931522,
    2.268494655768685,
    0.9611837071918397,
    5.635246852102243,
    1416.7928370645282,
    18.00782927522669,
    3.0062532837755223,
    5.0760575721029095,
    23.768591909964023,
    18.091851638174106,
    1.7240679592498567,
    5.520025571530638,
    0.7030595026292126,
    2.6403549907508195,
    0.0009129251521245301,
    3.6169202884767486,
    0.7753251218473064,
    0.07888026843934465
  };
  
  vector<double> upper_limit_fast_data {
    0.,
    1416.7928370645282,
    0.,
    1416.7928370645282,
    1416.7928370645282,
    0.,
    0.,
    1416.7928370645282,
    0.,
    1416.7928370645282,
    1416.7928370645282,
    1416.7928370645282,
    1416.7928370645282,
    0.,
    1416.7928370645282,
    0.,
    1416.7928370645282,
    0.,
    1416.7928370645282,
    1416.7928370645282,
    1416.7928370645282,
    1416.7928370645282,
    1416.7928370645282,
    0.,
    1416.7928370645282,
    1416.7928370645282,
    0.,
    1416.7928370645282,
    1416.7928370645282,
    1416.7928370645282,
    1416.7928370645282,
    1416.7928370645282,
    1416.7928370645282,
    0.,
    1416.7928370645282,
    1416.7928370645282,
    1416.7928370645282,
    1416.7928370645282,
    0.,
    1416.7928370645282,
    0.,
    0.,
    1416.7928370645282,
    1416.7928370645282,
    1416.7928370645282,
    0.,
    1416.7928370645282,
    1416.7928370645282,
    0.,
    1416.7928370645282,
    1416.7928370645282,
    1416.7928370645282,
    0.,
    0.,
    1416.7928370645282,
    0.,
    1416.7928370645282,
    1416.7928370645282,
    0.,
    1416.7928370645282,
    1416.7928370645282,
    0.,
    0.,
    1416.7928370645282,
    0.,
    1416.7928370645282,
    1416.7928370645282,
    0.,
    1416.7928370645282,
    1416.7928370645282,
    0.,
    0.,
    0.,
    0.,
    1416.7928370645282,
    0.,
    1416.7928370645282,
    1416.7928370645282,
    1416.7928370645282,
    1416.7928370645282,
    1416.7928370645282,
    1416.7928370645282,
    0.,
    1416.7928370645282,
    0.,
    1416.7928370645282,
    1416.7928370645282,
    1416.7928370645282,
    1416.7928370645282,
    1416.7928370645282,
    1416.7928370645282,
    1416.7928370645282,
    1416.7928370645282,
    1416.7928370645282,
    0.,
    1416.7928370645282,
    0.,
    1416.7928370645282,
    0.,
    0.
  };
  
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (ChiSquareLikelihoodLogarithm_test)

//-----------------------------------------------------------------------------
// Check the functor ChiSquareLikelihoodLogarithm_Impl
//-----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(ChiSquareLikelihoodLogarithm_Impl_test) {
  
  // Given
  vector<double> source_values {1, 2, 3, 4, 5};
  vector<double> model_values {.1, .2, .3, .4, .5};
  double scale = 17.;
  ChiSquareLikelihoodLogarithm_Impl<TestResidualCalculator> chi_square_functor {};
  
  // Expect
  mock_residual_calculator_singleton.reset(new MockResidualCalculator{});
  for (auto s_it=source_values.begin(), m_it=model_values.begin();
                                    s_it!=source_values.end(); ++s_it, ++m_it) {
    EXPECT_CALL(*mock_residual_calculator_singleton, 
                FunctorCall(*s_it, *m_it, scale)).WillOnce(Return(1));
  }
  
  // When 
  double sum = chi_square_functor(source_values.begin(), source_values.end(),
                                  model_values.begin(), scale);
  
  // Then
  BOOST_CHECK_EQUAL(sum, -0.5 * source_values.size());
  
  // The following will delete the mock object and report its tests
  mock_residual_calculator_singleton.reset();
  
}

//-----------------------------------------------------------------------------
// Check the functor ChiSquareNormal
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(ChiSquareNormal_test, ChiSquareLikelihoodLogarithm_Fixture) {
  
  // Given
  ChiSquareNormal chi_square_functor {};
  
  auto data_it = test_data.begin();
  auto expected_it = chi_square_data.begin();
  for (; data_it!=test_data.end(); ++data_it, ++expected_it) {
    
    // Given
    SourceCatalog::FluxErrorPair source {get<0>(*data_it), get<1>(*data_it), false, false};
    SourceCatalog::FluxErrorPair model {get<2>(*data_it), 0., false, false};
    double scale = get<3>(*data_it);
    
    // When
    auto result = chi_square_functor(source, model, scale);
    
    // Then
    BOOST_CHECK_CLOSE_FRACTION(result, *expected_it, close_tolerance);
    
  }
  
}

//-----------------------------------------------------------------------------
// Check the functor ChiSquareUpperLimit with the upper limit flag set to false
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(ChiSquareUpperLimit_flag_false_test, ChiSquareLikelihoodLogarithm_Fixture) {
  
  // Given
  ChiSquareUpperLimit chi_square_functor {};
  ChiSquareNormal chi_square_functor_normal {};
  bool upper_limit_flag = false;
  
  for (auto data_it=test_data.begin(); data_it!=test_data.end(); ++data_it) {
    
    // Given
    SourceCatalog::FluxErrorPair source {get<0>(*data_it), get<1>(*data_it), false, upper_limit_flag};
    SourceCatalog::FluxErrorPair model {get<2>(*data_it), 0., false, false};
    double scale = get<3>(*data_it);
    
    // When
    auto result = chi_square_functor(source, model, scale);
    double expected = chi_square_functor_normal(source, model, scale);
    
    // Then
    BOOST_CHECK_CLOSE_FRACTION(result, expected, close_tolerance);
    
  }
}

//-----------------------------------------------------------------------------
// Check the functor ChiSquareUpperLimit with the upper limit flag set to true
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(ChiSquareUpperLimit_flag_true_test, ChiSquareLikelihoodLogarithm_Fixture) {
  
  // Given
  ChiSquareUpperLimit chi_square_functor {};
  bool upper_limit_flag = true;
  
  auto data_it = test_data.begin();
  auto expected_it = upper_limit_data.begin();
  for (; data_it!=test_data.end(); ++data_it, ++expected_it) {
    
    // Given
    SourceCatalog::FluxErrorPair source {get<0>(*data_it), get<1>(*data_it), false, upper_limit_flag};
    SourceCatalog::FluxErrorPair model {get<2>(*data_it), 0., false, false};
    double scale = get<3>(*data_it);
    
    // When
    auto result = chi_square_functor(source, model, scale);
    
    // Then
    BOOST_CHECK_CLOSE_FRACTION(result, *expected_it, close_tolerance);
    
  }
}



//-----------------------------------------------------------------------------
// Check the functor ChiSquareMissingData
//-----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(ChiSquareMissingData_test) {
  
  // Given
  vector<SourceCatalog::FluxErrorPair> sources {
    {1, .1, false, false},
    {2, .2, false, false},
    {3, .3, true, false},
    {4, .4, false, false},
    {5, .5, true, false}
  };
  vector<SourceCatalog::FluxErrorPair> models {
    {.01, 0, false, false},
    {.02, 0, false, false},
    {.03, 0, false, false},
    {.04, 0, false, false},
    {.05, 0, false, false}
  };
  vector<double> scales {
    0.001, 0.002, 0.003, 0.004, 0.005
  };
  vector<double> expected {
    10, 20, 0, 40, 0
  };
  ChiSquareMissingData<TestChiSquareFunc> missing_data_functor {};
  
  // Expect
  mock_chi_square_func_singleton.reset(new MockChiSquareFunc{});
  EXPECT_CALL(*mock_chi_square_func_singleton, 
              FunctorCall(sources[0], models[0], scales[0])).WillOnce(Return(expected[0]));
  EXPECT_CALL(*mock_chi_square_func_singleton, 
              FunctorCall(sources[1], models[1], scales[1])).WillOnce(Return(expected[1]));
  EXPECT_CALL(*mock_chi_square_func_singleton, 
              FunctorCall(sources[3], models[3], scales[3])).WillOnce(Return(expected[3]));
  
  for (size_t i=0; i<sources.size(); ++i) {
    
    // When
    double result = missing_data_functor(sources[i], models[i], scales[i]);
    
    // Then
    BOOST_CHECK_EQUAL(result, expected[i]);
    
  }
  
  // The following will delete the mock object and report its tests
  mock_chi_square_func_singleton.reset();
  
}

BOOST_AUTO_TEST_SUITE_END ()
