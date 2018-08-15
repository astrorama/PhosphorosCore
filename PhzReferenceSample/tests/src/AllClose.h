namespace Euclid {
namespace XYDataset {

boost::test_tools::predicate_result checkAllClose(const XYDataset& a, const XYDataset &b) {
  boost::test_tools::predicate_result res(true);
  boost::test_tools::close_at_tolerance<double> is_close(boost::test_tools::percent_tolerance(1e-5));

  if (a.size() != b.size()) {
    res = false;
    res.message() << "Different sizes";
  }
  else {
    for (auto i = a.begin(), j = b.begin(); i != a.end(); ++i, ++j) {
      if (!is_close(i->first, j->first) || !is_close(i->second, j->second)) {
        res = false;
        res.message()
          << '<' << i->first << ',' << i->second << '>'
          << " != "
          << '<' << j->first << ',' << j->second << ">\n";
      }
    }
  }

  return res;
}

}
}

