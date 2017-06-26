#include <map>
#include <set>
#include <string>
#include <sstream>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <cassert>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include "coded_chords.hpp"

using std::string;
using std::wstring;
using std::map;
using std::set;

template<typename T>
T replaceAll(T input, const T& find, const T& replace) {
  typename T::size_type n = 0;

  while ( ( n = input.find( find, n ) ) !=  T::npos ) {
      input.replace( n, find.size(), replace);
      n += replace.size();
  }
  return input;
}


bool filter(const wstring& s, const std::vector<wstring>& list) {
  bool found = false;
  for(const wstring& sub : list) {
    if(sub.find(s, 0) != string::npos) {
      found = true;
      break;
    }
  }

  return found;
}

std::vector<std::wstring> repeated_substring(const std::wstring& str, map<wchar_t, string> decodeMap) {
    std::vector<std::wstring> res;
    if(str.empty())
      return res;

    off_t len = str.length();

    off_t **c = new off_t*[len + 1];
    for (off_t i = 0; i <= len; ++i) {
      c[i] = new off_t[len + 1];
    }
    for (off_t i = 0; i <= len; ++i) {
      c[i][0] = 0;
      c[0][i] = 0;
    }

    off_t max_len = 0, index = len + 1;
    for (off_t i = 1; i <= len; ++i) {
      for (off_t j = 1; j <= len; ++j) {
        if (str[i - 1] == str[j - 1] && abs(i - j) > c[i - 1][j - 1]) {
          c[i][j] = c[i - 1][j - 1] + 1;
          if (c[i][j] > max_len) {
            max_len = c[i][j];
            index = std::min(i, j);
          }
        } else {
          c[i][j] = 0;
        }
      }
    }

    for (off_t i = 0; i <= len; ++i) {
      delete[] c[i];
    }
    delete[] c;

    if (max_len > 0) {
      std::wstring ret = str.substr(index - max_len, max_len);
      res.push_back(ret);

      auto rest = split_by_substr<wstring>(str, ret);
      for(const auto r : rest) {
        auto rs = repeated_substring(r, decodeMap);

        if(!rs.empty()) {
          for(const wstring& s : rs) {
            if(!filter(s, res))
              res.push_back(s);
          }
        } else
          res.push_back(r);
      }
    } else
      res.push_back(str);

  return res;
}

template<typename T>
T deleteConsecutiveRepeatingSubstrings(T& str) {
  off_t len = str.size();
  int i, j, counter;
  for (i = 1; i <= len / 2; ++i) {
    for (j = i, counter = 0; j < len; ++j) {
      if (str[j] == str[j - i])
        counter++;
      else
        counter = 0;
      if (counter > 2 && counter == i) {
        counter = 0;
        std::copy(str.begin() + j, str.begin() + len, str.begin() + (j - i));
        j -= i;
        len -= i;
      }
    }
    str = str.substr(0, j);
  }

  size_t start = 0, cnt = 0;
  char c, l = str[0];

  for (size_t i = 1; i < str.length(); ++i) {
    c = str[i];
    if(c == l) {
      if(cnt == 0)
        start = i - 1;

      ++cnt;
    } else {
      if(cnt > 1) {
        str.erase(str.begin() + start + 1, str.begin() + start + cnt);
        if(start + cnt >= str.length())
          break;
      }
    }
    l = c;
  }
  return str;
}


std::vector<std::pair<std::vector<string>, std::vector<std::pair<string, string>>>> findScaleSequences(const std::vector<string>& chords, const map<string, map<string, std::set<string>>>& scaleLookup) {
  std::list<std::pair<string, string>> candidates;
  std::vector<std::pair<std::vector<string>, std::vector<std::pair<string, string>>>> res;
  std::vector<string> seqChords;
  std::vector<std::pair<string, string>> seqScales;

  for(const string& chord : chords) {
    if(candidates.empty()) {
      auto it = scaleLookup.find(chord);
      if(it != scaleLookup.end()) {
        for(auto& scale : (*it).second) {
          for(auto& root : scale.second)
          candidates.push_back({scale.first, root});
        }
      }
      seqChords.push_back(chord);
    } else {
      std::vector<std::list<std::pair<string, string>>::iterator> rem;
      auto itsl = scaleLookup.find(chord);
      if (itsl != scaleLookup.end()) {
        for (auto itc = candidates.begin(); itc != candidates.end(); ++itc) {
          bool found = false;
          for (auto& scale : (*itsl).second) {
            for(auto& root : scale.second) {
              if ((*itc).first == scale.first && (*itc).second == root) {
                found = true;
                break;
              }
            }
          }
          if (!found) {
            rem.push_back(itc);
          }
        }
      }

      if(candidates.size() == rem.size()) {
        for(auto& can : candidates) {
          seqScales.push_back({can.first, can.second});
        }
        res.push_back({seqChords,seqScales});
        candidates.clear();
        seqChords.clear();
        seqScales.clear();
        seqChords.push_back(chord);

        auto itsl = scaleLookup.find(chord);
        if(itsl != scaleLookup.end()) {
          for(auto& scale : (*itsl).second) {
            for(auto& root : scale.second)
            candidates.push_back({scale.first, root});
          }
        }

      } else {
        seqChords.push_back(chord);
        for(auto itr = rem.begin(); itr != rem.end(); ++itr) {
          candidates.erase(*itr);
        }
      }
      rem.clear();
    }
  }

  for(auto& can : candidates) {
    seqScales.push_back({can.first, can.second});
  }
  res.push_back({seqChords,seqScales});
  return res;
}

void variant2(map<string, map<string, std::set<string>>> & scaleLookup) {
  string line;
  std::vector<string> chords;
  while(getline(std::cin, line)) {
    auto encoded = encode_chord_string(line);
    encoded.first = deleteConsecutiveRepeatingSubstrings(encoded.first);
    auto repSub = repeated_substring(encoded.first, encoded.second);
    std::stringstream substream;
    for(auto sub : repSub) {
      auto decoded = decode_chord_string({sub, encoded.second});
      substream << decoded << " ";
    }

    string ss = substream.str();
    chords = split_chords(trim(ss));
    auto scaleSequences = findScaleSequences(chords, scaleLookup);

    std::stringstream chordStream;
    bool empty = true;
    for(auto seq : scaleSequences) {
      chordStream.str("");
      if(!seq.first.empty() && !seq.second.empty()) {
        for(auto& chord : seq.first) {
          chordStream << chord << " ";
        }
        string cs = chordStream.str();
        std::cout << trim(cs) << '=';

        std::stringstream scaleStream;
        for(auto& scale : seq.second) {
          scaleStream << scale.first << ' ' << scale.second << ';';
          empty = false;
        }
        std::cout << scaleStream.str() << std::endl;
      }
    }
    if(!empty)
      std::cout << "###" << std::endl;
  }
}

int main(int argc, char* argv[]) {
  if(argc != 2) {
    std::cerr << "Usage: classifier <scaletable-file>" << std::endl;
    exit(1);
  }

  std::ifstream tf(argv[1]);
  if(!tf) {
    std::cerr << "scale table file not found" << std::endl;
    exit(2);
  }

  string line;
  map<string, map<string, std::set<string>>> scaleLookup;

  while(getline(tf, line)) {
    for(auto& c : line)
      c = toupper(c);


    auto tokens = split_by_delim_list<string>(line, ";");
    assert(tokens.size() == 3);
    string chord = tokens[0];
    string root = tokens[1];
    string scale = tokens[2];
    scaleLookup[chord][scale].insert(root);
  }

  variant2(scaleLookup);
}
