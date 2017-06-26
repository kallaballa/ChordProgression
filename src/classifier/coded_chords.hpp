#ifndef SRC_CODED_CHORDS_HPP_
#define SRC_CODED_CHORDS_HPP_

#include <string>
#include <map>
#include <cassert>
#include <boost/algorithm/string.hpp>

using std::wstring;
using std::string;
using std::map;

string trim(string& str);

template<typename T>
std::vector<T> split_by_delim_list(const T& input, const T& delims) {
  std::vector<T> res;
  boost::algorithm::split(res, input, boost::is_any_of(delims));

  return res;
}

template<typename T>
std::vector<T> split_by_substr(T input, const T& substring) {
  std::vector<T> res;
  size_t pos = 0;
  T token;
  while ((pos = input.find(substring)) != T::npos) {
    token = input.substr(0, pos);
    if(!token.empty())
      res.push_back(token);
    input.erase(0, pos + substring.length());
  }
  res.push_back(input);
  return res;
}

std::vector<string> split_chords(const string& chordString);
std::pair<wstring, map<wchar_t, string>> encode_chord_string(const string& cs, map<wchar_t, string>& codeMap);
std::pair<wstring, map<wchar_t, string>> encode_chord_string(const string& cs);
string decode_chord_string(std::pair<wstring, map<wchar_t, string>> encoded);
#endif
