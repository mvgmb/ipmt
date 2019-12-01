#ifndef IPMT_SUFFIX_ARRAY_H
#define IPMT_SUFFIX_ARRAY_H

#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <fstream>
#include <list>

using namespace std;

class suffix_array {
private:
    template<class T>
    void sort_index(vector<size_t> &index, vector<T> &ranking);

    void build_inv_sa(vector<size_t> &inv_sa);

    void invert_inv_sa(vector<size_t> &inv_sa);

    void build_lcp(vector<size_t> &lcp, vector<size_t> &inv_sa);

    void build_lr_lcp(size_t n, vector<size_t> &lcp);

    size_t compute_lr_lcp(vector<size_t> &lcp, size_t &l, size_t &r);

    void recover_str();

public:
    vector<size_t> sa;
    vector<size_t> l_lcp;
    vector<size_t> r_lcp;
    vector<size_t> char_count;
    size_t total_char_count = 0;
    string_view strv;
    string str_ref;

    size_t lcp(string_view &str1, string_view &str2, size_t start_from);

    size_t pred(string_view &pat);

    size_t succ(string_view &pat);

    explicit suffix_array(string_view &str);

    explicit suffix_array();

    void save(const string &indexFilePath);

    size_t search(bool print, string &indexFilePath, list<string> &patterns);
};


#endif //IPMT_SUFFIX_ARRAY_H
