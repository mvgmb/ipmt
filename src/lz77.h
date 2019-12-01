#ifndef IMPT_LZ77_H
#define IMPT_LZ77_H

#include <climits>
#include <iostream>
#include <utility>
#include <vector>

using namespace std;

class lz77 {
private:
    static const size_t ls_size = static_cast<size_t>(1) << static_cast<size_t>(9); // 512
    static const size_t la_size = static_cast<size_t>(1) << static_cast<size_t>(7); // 128

    static void build_fsm(vector<size_t> &fsm, const string_view &pat);

    static pair<size_t, size_t> prefix_match(const string_view &win, const string_view &pat);

public:

    static void zip(const string_view &txt, ostream &out);

    static void unzip(istream &in);
};

#endif //IMPT_LZ77_H
