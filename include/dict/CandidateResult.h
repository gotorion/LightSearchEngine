#ifndef __CANDIDATERESULT_H_
#define __CANDIDATERESULT_H_
#include <string>
using std::string;

struct CandidateResult {
    string _word;
    int _freq;
    int _dist;
    CandidateResult(const string& str, int freq, int dist)
        : _word(str), _freq(freq), _dist(dist) {}
    bool operator<(const CandidateResult& rhs) {
        if (_dist < rhs._dist) {
            return true;
        } else if (_dist == rhs._dist) {
            if (_freq > rhs._freq) {
                return true;
            } else if (_freq == rhs._freq) {
                if (_word < rhs._word) {
                    return true;
                } else {
                    return false;
                }
            } else {
                return false;
            }
        } else {
            return false;
        }
    }
};

template <>
struct std::less<CandidateResult> {
   public:
    bool operator()(const CandidateResult& lhs, const CandidateResult& rhs) {
        if (lhs._dist < rhs._dist) {
            return true;
        } else if (lhs._dist == rhs._dist) {
            if (lhs._freq > rhs._freq) {
                return true;
            } else if (lhs._freq == rhs._freq) {
                if (lhs._word < rhs._word) {
                    return true;
                } else {
                    return false;
                }
            } else {
                return false;
            }
        } else {
            return false;
        }
    }
};

struct BetterCandidate {
    bool operator()(const CandidateResult& lhs, const CandidateResult& rhs) {
        if (lhs._dist < rhs._dist) {
            return false;
        } else if (lhs._dist == rhs._dist) {
            if (lhs._freq > rhs._freq) {
                return false;
            } else if (lhs._freq == rhs._freq) {
                if (lhs._word < rhs._word) {
                    return false;
                } else {
                    return true;
                }
            } else {
                return true;
            }
        } else {
            return true;
        }
    }
};

#endif