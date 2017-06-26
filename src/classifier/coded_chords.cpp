#include "coded_chords.hpp"

string trim(string& str) {
      size_t first = str.find_first_not_of(' ');
      if (first == string::npos)
          return "";
      size_t last = str.find_last_not_of(' ');
//      std::cout << "b " << str << std::endl;
      str = str.substr(first, (last-first+1));
//      std::cout << "a " << str << std::endl;
      return str;
}

std::vector<string> split_chords(const string& chordString) {
  std::vector<string> ret;
  std::vector<string> chords = split_by_delim_list<string>(chordString, " ");
  for(string& c : chords) {
    trim(c);
    if(!c.empty()) {
      ret.push_back(c);
    }
  }

  return ret;
}


std::pair<wstring, map<wchar_t, string>> encode_chord_string(const string& cs) {
  map<wchar_t, string> codeMap;
  return encode_chord_string(cs, codeMap);
}

std::pair<wstring, map<wchar_t, string>> encode_chord_string(const string& cs, map<wchar_t, string>& codeMap) {
  map<string, wchar_t> reversecodeMap;
  std::cerr << codeMap.size() << std::endl;
  for(const auto& p: codeMap) {
    reversecodeMap[p.second] = p.first;
  }
  auto chords = split_chords(cs);

  wchar_t cnt;
  if(codeMap.empty())
    cnt = 1;
  else
    cnt = (*codeMap.rbegin()).first + 1;

  wstring encoded;

  for(string& c : chords) {
    auto it = reversecodeMap.find(c);
    if(it == reversecodeMap.end()) {
      reversecodeMap[c] = cnt;
      encoded += cnt;
//      std::cerr << c << "=" << cnt << std::endl;
      ++cnt;
    } else {
      encoded += (*it).second;
//      std::cerr << "enc " << (*it).first << "=" << (*it).second << std::endl;
    }
  }

  for(const auto& p : reversecodeMap) {
    auto it = codeMap.find(p.second);
    if(it == codeMap.end())
      codeMap[p.second] = p.first;
  }

  assert(encoded.size() == chords.size());
  return {encoded, codeMap};
}

string decode_chord_string(std::pair<wstring, map<wchar_t, string>> encoded) {
  string decoded;
  for(wchar_t c : encoded.first) {
    decoded += (encoded.second[c] + ' ');
//    std::cerr << "dec " << c << "=" << encoded.second[c] << std::endl;
  }
  return trim(decoded);
}
