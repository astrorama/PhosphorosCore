#include <ElementsKernel/Real.h>
#include <XYDataset/XYDataset.h>
#include <iomanip>

namespace Euclid {
namespace XYDataset {

boost::test_tools::predicate_result checkAllClose(const XYDataset& a, const XYDataset& b) {
  boost::test_tools::predicate_result res(true);

  if (a.size() != b.size()) {
    res = false;
    res.message() << "Different sizes";
  } else {
    for (auto i = a.begin(), j = b.begin(); i != a.end() && j != b.end(); ++i, ++j) {
      if (Elements::isNotEqual(i->first, j->first) || Elements::isNotEqual(i->second, j->second)) {
        res = false;
        res.message() << '<' << i->first << ',' << i->second << '>' << " != " << '<' << j->first << ',' << j->second
                      << ">\n";
      }
    }
  }

  return res;
}

}  // namespace XYDataset
}  // namespace Euclid
