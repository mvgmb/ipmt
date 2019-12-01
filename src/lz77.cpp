#include <tuple>
#include "lz77.h"


void lz77::build_fsm(vector<size_t> &fsm, const string_view &pat) {
    size_t m = pat.length();

    fsm.resize((m + 1) * 256);
    fsm[0 + static_cast<unsigned char>(pat[0])] = 1;

    size_t brd = 0, _i = 256, _brd = 0;
    for (size_t i = 1; i < m; ++i) {
        for (size_t c = 0; c < 256; ++c)
            fsm[_i + static_cast<unsigned char>(c)] = fsm[_brd + static_cast<unsigned char>(c)];

        auto c = static_cast<unsigned char>(pat[i]);
        fsm[_i + static_cast<unsigned char>(c)] = i + 1;

        _i += 256;
        _brd = 256 * brd;
    }

    for (size_t c = 0; c < 256; ++c)
        fsm[256 * m + static_cast<unsigned char>(c)] = fsm[_brd + static_cast<unsigned char>(c)];
}

pair<size_t, size_t> lz77::prefix_match(const string_view &win, const string_view &pat) {
    const size_t n = win.length();
    const size_t m = pat.length();

    vector<size_t> fsm;
    build_fsm(fsm, pat);

    size_t cur = 0, len = 0, pos = 0, i = 0;

    for (auto &w : win) {
        cur = fsm[256 * cur + static_cast<unsigned char>(w)];
        if (cur > len && i - cur + 1 < n - m) {
            len = cur;
            pos = i - cur + 1;
            if (len == m - 1)
                break;
        }
        ++i;
    }

    return pair<size_t, size_t>(pos, len);
}

void lz77::zip(const string_view &txt, ostream &out) {
    const size_t n = txt.size();
    out.write(reinterpret_cast<const char *>(&n), sizeof(size_t));

    string pre_txt;
    pre_txt.resize(ls_size + n);
    fill(pre_txt.begin(), pre_txt.begin() + ls_size, '\0');
    copy(txt.begin(), txt.end(), pre_txt.begin() + ls_size);

    string_view pre_txtv{pre_txt.c_str(), pre_txt.size()};

    string_view win, pat;
    size_t i = ls_size;
    while (i < ls_size + n) {
        win = pre_txtv.substr(i - ls_size, la_size + ls_size);
        pat = pre_txtv.substr(i, la_size);

        size_t pos, len;
        tie(pos, len) = prefix_match(win, pat);
        char c = pat[len];

        size_t store = 0;
        store |= pos << static_cast<size_t>(7);
        store |= len;

        out.write(reinterpret_cast<const char *>(&store), 2);
        out.write(reinterpret_cast<const char *>(&c), sizeof(char));

        i += len + 1;
    }
}

void lz77::unzip(istream &in) {
    size_t n = 0;
    in.read(reinterpret_cast<char *>(&n), sizeof(size_t));

    string txt;
    txt.resize(n + ls_size);

    fill(txt.begin(), txt.begin() + ls_size, 'x');

    size_t pos, len;
    char c;
    size_t store = 0;
    size_t i = ls_size;
    while (i < txt.size()) {
        in.read(reinterpret_cast<char *>(&store), 2);
        in.read(reinterpret_cast<char *>(&c), sizeof(char));

        pos = store >> static_cast<size_t>(7);
        len = store & (static_cast<size_t>(-1) >> static_cast<size_t>(64 - 7));

        for (size_t j = 0; j < len; ++j) {
            txt[i] = txt[i - ls_size + pos];
            ++i;
        }

        txt[i] = c;
        ++i;
    }
    string_view txtv{txt.c_str(), txt.size()};
    cout << txtv.substr(ls_size) << endl;
}
