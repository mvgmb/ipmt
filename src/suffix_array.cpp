#include "suffix_array.h"

template<class T>
void suffix_array::sort_index(vector<size_t> &index, vector<T> &ranking) {
    size_t n = ranking.size();

    vector<pair<T, size_t>> ordered_ranking(n);
    size_t i = 0;
    for (auto &r : ranking) {
        ordered_ranking[i] = {r, i};
        ++i;
    }
    sort(ordered_ranking.begin(), ordered_ranking.end());

    size_t rank = 0;
    T *prev = nullptr;
    for (auto &ord : ordered_ranking) {
        if (prev && ord.first != *prev)
            ++rank;

        prev = &ord.first;
        index[ord.second] = rank;
    }
}

void suffix_array::build_inv_sa(vector<size_t> &inv_sa) {
    size_t n = strv.size();

    vector<vector<size_t>> index(2, vector<size_t>(n));

    vector<char> str_vector = vector<char>(n);
    char_count = vector<size_t>(127);

    for (size_t i = 0; i < n; ++i) {
        str_vector[i] = strv[i];
        ++char_count[strv[i]];
        ++total_char_count;
    }

    sort_index<char>(index[0], str_vector);

    bool b = false;
    auto ceil_log_n = ceil(log2(n));
    vector<pair<size_t, size_t>> ranking(n);

    for (size_t i = 0; i < ceil_log_n; ++i) {
        size_t k = static_cast<size_t>(1) << i; // two power k
        for (size_t j = 0; j < n; ++j) {
            if (j + k < n)
                ranking[j] = {index[b][j] + 1, index[b][j + k] + 1};
            else
                ranking[j] = {index[b][j] + 1, 0};
        }
        sort_index<pair<size_t, size_t>>(index[!b], ranking);
        b = !b;
    }

    inv_sa = index[b];
}

void suffix_array::invert_inv_sa(vector<size_t> &inv_sa) {
    sa.resize(inv_sa.size());

    for (size_t i = 0; i < inv_sa.size(); ++i)
        sa[inv_sa[i]] = i;
}

void suffix_array::build_lcp(vector<size_t> &lcp, vector<size_t> &inv_sa) {
    const size_t n = strv.size();

    lcp.resize(n - 1);

    for (size_t i = 0, j = 0; i < n; ++i) {
        size_t k = inv_sa[i];
        if (k == n - 1) {
            j = 0;
            continue;
        }

        size_t l = sa[k + 1];
        while (i + j < n && l + j < n && strv[i + j] == strv[l + j])
            ++j;

        lcp[k] = j;
        j -= j > 0;
    }
}

size_t suffix_array::compute_lr_lcp(vector<size_t> &lcp, size_t &l, size_t &r) {
    if (r - l == 1)
        return lcp[l];

    size_t h = (l + r) / 2;

    size_t l_min = compute_lr_lcp(lcp, l, h);
    size_t r_min = compute_lr_lcp(lcp, h, r);

    l_lcp[h] = l_min;
    r_lcp[h] = r_min;

    return min(l_min, r_min);
}

void suffix_array::build_lr_lcp(size_t n, vector<size_t> &lcp) {
    l_lcp.resize(n);
    r_lcp.resize(n);

    size_t l = 0;
    size_t r = n - 1;
    compute_lr_lcp(lcp, l, r);
}

suffix_array::suffix_array(string_view &strv) : strv(strv) {
    vector<size_t> inv_sa;
    vector<size_t> lcp;
    build_inv_sa(inv_sa);

    invert_inv_sa(inv_sa);
    build_lcp(lcp, inv_sa);
    build_lr_lcp(strv.size(), lcp);
}

suffix_array::suffix_array() {}

void suffix_array::recover_str() {
    string _str(total_char_count, ' ');
    size_t si = -1;
    size_t size = 0;

    for (size_t i = 0; i < char_count.size(); ++i) {
        size_t j = 0;
        while (j < char_count[i]) {
            _str[sa[++si]] = char(i);
            ++j;
            ++size;
        }

    }
    str_ref = _str;

    string_view s{str_ref.c_str(), str_ref.size()};
    strv = s;
}

size_t suffix_array::lcp(string_view &str1, string_view &str2, size_t start_from) {
    size_t i = 0;

    const char *str1_it = str1.begin() + start_from;
    const char *str2_it = str2.begin() + start_from;

    while (str1_it != str1.end() && str2_it != str2.end() && *str1_it == *str2_it) {
        ++str1_it;
        ++str2_it;
        ++i;
    }

    return i + start_from;
}

size_t suffix_array::pred(string_view &pat) {
    size_t n = strv.size();
    size_t m = pat.size();

    size_t _min;

    string_view l_str = strv.substr(sa[0]);
    _min = min(m, l_str.size());
    if (l_str.substr(0, _min) > pat.substr(0, _min)) {
        return -1;
    }

    string_view r_str = strv.substr(sa[n - 1]);
    _min = min(m, r_str.size());
    if (r_str.substr(0, _min) <= pat.substr(0, _min)) {
        return n - 1;
    }

    size_t l, r, L, R, H;
    string_view aux;

    l = 0;
    r = n - 1;
    L = lcp(l_str, pat, 0);
    R = lcp(r_str, pat, 0);
    while (r - l > 1) {
        size_t h = (l + r) / 2;
        if (L >= R) {
            if (L < l_lcp[h])
                H = L;
            else if (L == l_lcp[h]) {
                aux = strv.substr(sa[h]);
                H = lcp(aux, pat, L);
            } else
                H = l_lcp[h];
        } else {
            if (R < r_lcp[h])
                H = R;
            else if (R == r_lcp[h]) {
                aux = strv.substr(sa[h]);
                H = lcp(aux, pat, R);
            } else
                H = r_lcp[h];
        }

        if (H == m || (H < n - sa[h] && strv[sa[h] + H] <= pat[H])) {
            l = h;
            L = H;
        } else {
            r = h;
            R = H;
        }
    }

    return l;
}


size_t suffix_array::succ(string_view &pat) {
    size_t n = strv.size();
    size_t m = pat.size();

    size_t _min;

    string_view l_str = strv.substr(sa[0]);
    _min = min(m, l_str.size());
    if (l_str.substr(0, _min) >= pat.substr(0, _min)) {
        return 0;
    }

    string_view r_str = strv.substr(sa[n - 1]);
    _min = min(m, r_str.size());
    if (r_str.substr(0, _min) < pat.substr(0, _min)) {
        return n;
    }

    size_t l, r, L, R, H;
    string_view aux;

    l = 0;
    r = n - 1;
    L = lcp(l_str, pat, 0);
    R = lcp(r_str, pat, 0);

    while (r - l > 1) {
        size_t h = (l + r) / 2;
        if (L >= R) {
            if (L < l_lcp[h])
                H = L;
            else if (L == l_lcp[h]) {
                aux = strv.substr(sa[h]);
                H = lcp(aux, pat, L);
            } else
                H = l_lcp[h];
        } else {
            if (R < r_lcp[h])
                H = R;
            else if (R == r_lcp[h]) {
                aux = strv.substr(sa[h]);
                H = lcp(aux, pat, R);
            } else
                H = r_lcp[h];
        }

        if (H == m || (sa[h] + H < n && pat[H] <= strv[sa[h] + H])) {
            r = h;
            R = H;
        } else {
            l = h;
            L = H;
        }
    }

    return r;
}


void suffix_array::save(const string &indexFilePath) {
    ofstream out(indexFilePath, ios::out | ios::binary | ios::trunc);
    size_t size;

    size = sa.size();
    out.write(reinterpret_cast<const char *>(&size), sizeof(size_t));
    out.write(reinterpret_cast<const char *>(sa.data()), sizeof(size_t) * size);

    size = l_lcp.size();
    out.write(reinterpret_cast<const char *>(&size), sizeof(size_t));
    out.write(reinterpret_cast<const char *>(l_lcp.data()), sizeof(size_t) * size);

    size = r_lcp.size();
    out.write(reinterpret_cast<const char *>(&size), sizeof(size_t));
    out.write(reinterpret_cast<const char *>(r_lcp.data()), sizeof(size_t) * size);

    out.write(reinterpret_cast<const char *>(char_count.data()), sizeof(size_t) * 127);

    out.write(reinterpret_cast<const char *>(&total_char_count), sizeof(size_t));
}

size_t suffix_array::search(bool print, string &indexFilePath, list<string> &patterns) {
    ifstream in(indexFilePath, ios::in | ios::binary);

    size_t no_occ = 0;
    size_t size, char_count_size = 127;

    in.read(reinterpret_cast<char *>(&size), sizeof(size_t));
    sa.resize(size);
    in.read(reinterpret_cast<char *>(sa.data()), sizeof(size_t) * size);

    in.read(reinterpret_cast<char *>(&size), sizeof(size_t));
    l_lcp.resize(size);
    in.read(reinterpret_cast<char *>(l_lcp.data()), sizeof(size_t) * size);

    in.read(reinterpret_cast<char *>(&size), sizeof(size_t));
    r_lcp.resize(size);
    in.read(reinterpret_cast<char *>(r_lcp.data()), sizeof(size_t) * size);

    char_count.resize(127);
    in.read(reinterpret_cast<char *>(char_count.data()), sizeof(size_t) * 127);

    in.read(reinterpret_cast<char *>(&total_char_count), sizeof(size_t));

    recover_str();

    for (auto &p : patterns) {
        string_view pv{p.c_str(), p.size()};

        size_t rp = pred(pv);
        size_t lp = succ(pv);

        if (lp + 1 > rp + 1)
            continue;

        if (print) {
            while (lp <= rp) {
                size_t sa_value = sa[lp];
                while (strv.at(--sa_value) != '\n');

                while (strv.at(++sa_value) != '\n')
                    cout << strv[sa_value];
                cout << endl;

                ++lp;
            }
        } else {
            no_occ += rp - lp + 1;
        }
    }

    return no_occ;
}
