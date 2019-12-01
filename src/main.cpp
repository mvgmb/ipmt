#include <string_view>
#include "suffix_array.h"
#include "lz77.h"
#include <getopt.h>
#include <list>
#include <functional>
#include <iostream>
#include <memory>
#include <queue>
#include <sstream>

using namespace std;

enum options {
    DEFAULT = 0x00,
    COUNT = 0x01,
};

void help_index(char *s) {
    cerr
            << "Usage: " << s << " index [options] textfile" << endl
            << "Create an indexfile named after the given textfile with suffix '.idx' using the suffix-array algorithm"
            << endl
            << "Options:" << endl
            << "  -h, --help    display this information" << endl
            << endl
            << "Example: " << s << " index moby-dick.txt" << endl
            << endl;
}

void help_search(char *s) {
    cerr
            << "Usage: " << s << " search [options] pattern indexfile" << endl
            << endl
            << "Search for pattern using indexfile" << endl
            << endl
            << "Options:" << endl
            << "  -p, --pattern FILE    obtain patterns (per line) from FILE" << endl
            << "  -c, --count           only print the total count of occurrences" << endl
            << "  -l, --line_count      only print the total count of lines that has occurrences" << endl
            << "  -h, --help            display this information" << endl
            << endl
            << "Example: " << s << " search whale moby-dick.idx" << endl
            << endl;
}

void help_zip(char *s) {
    cerr
            << "Usage: " << s << " zip [options] textfile" << endl
            << endl
            << "zip textfile using lz77 algorithm producing textfile.lz77" << endl
            << endl
            << "Options:" << endl
            << "  -h, --help    display this information" << endl
            << endl
            << "Example: " << s << " zip moby-dick.txt" << endl
            << endl;
}

void help_unzip(char *s) {
    cerr
            << "Usage: " << s << " unzip [options] textfile.lz77" << endl
            << endl
            << "Unzip textfile.lz77 using lz77 algorithm producing textfile" << endl
            << endl
            << "Options:" << endl
            << "  -h, --help    display this information" << endl
            << endl
            << "Example: " << s << " unzip moby-dick.txt.lz77" << endl;
}

void help(char *s) {
    cerr
            << "This project is able to:"
            << endl
            << endl
            << "Create an indexfile named after the given textfile with suffix '.idx' using the suffix-array algorithm"
            << endl
            << "For more info run: " << s << " index -h"
            << endl
            << endl
            << "Search for pattern using indexfile"
            << endl
            << "For more info run: " << s << " search -h"
            << endl
            << endl
            << "Zip textfile using lz77 algorithm producing textfile.lz77"
            << endl
            << "For more info run: " << s << " zip -h"
            << endl
            << endl
            << "Unzip textfile.lz77 using lz77 algorithm producing textfile"
            << endl
            << "For more info run: " << s << " unzip -h"
            << endl;
}

int main(int argc, char *argv[]) {
    cin.tie(nullptr);
    ios::sync_with_stdio(false);

    if (argc < 2) {
        help(argv[0]);
        return 1;
    }

    switch (argv[1][0]) {
        case 'i': { // index
            const char *short_options = ":h";
            const option long_options[] = {
                    {"help",  no_argument, nullptr, 'h'},
                    {nullptr, no_argument, nullptr, '\0'},
            };
            int option_index = -1;

            int c;
            while ((c = getopt_long(argc, argv, short_options, long_options, &option_index)) != -1) {
                switch (c) {
                    case 'h':
                    case '?':
                    default: {
                        help_index(argv[0]);
                        return 1;
                    }
                }
            }

            string in_file = argv[++optind];
            string out_file = in_file.substr(0, in_file.find_last_of('.')) + ".idx";

            stringstream ss;
            ss << ifstream(in_file).rdbuf();
            string str = ss.str();
            string_view strv{str.c_str(), str.size()};

            auto sa = new suffix_array(strv);
            sa->save(out_file);

            return 0;
        }

        case 's': { // search
            const char *short_options = "p:ch";
            const option long_options[] = {
                    {"pattern", required_argument, nullptr, 'p'},
                    {"count",   no_argument,       nullptr, 'c'},
                    {"help",    no_argument,       nullptr, 'h'},
                    {nullptr,   no_argument,       nullptr, '\0'},
            };
            int option_index = -1;

            size_t e = 0;
            size_t m = options::DEFAULT;

            list <string> patterns;
            queue < unique_ptr < istream, function < void(istream * ) >> > t;

            int c;
            while ((c = getopt_long(argc, argv, short_options, long_options, &option_index)) != -1)
                switch (c) {
                    case 'p': {
                        ifstream ifs(optarg);
                        string s;
                        while (ifs >> s)
                            patterns.emplace_back(s);
                        break;
                    }

                    case 'c': {
                        m |= options::COUNT;
                        break;
                    }

                    case 'h':
                    case '?':
                    default: {
                        help_search(argv[0]);
                        return EXIT_FAILURE;
                    }
                }

            for (auto i = static_cast<size_t>(optind) + 1; i < argc; ++i) {
                if (patterns.empty())
                    patterns.emplace_back(argv[i]);
            }

            if (patterns.empty()) {
                help(argv[0]);
                return 1;
            }

            string idx_file;
            for (auto i = static_cast<size_t>(optind) + 1; i < argc; ++i)
                idx_file = argv[i];

            auto sa = new suffix_array();

            if (m & options::COUNT)
                cout << sa->search(false, idx_file, patterns);
            else
                sa->search(true, idx_file, patterns);

            return 0;
        }

        case 'z': { // zip
            const char *short_options = ":h";
            const option long_options[] = {
                    {"help",  no_argument, nullptr, 'h'},
                    {nullptr, no_argument, nullptr, '\0'},
            };
            int option_index = -1;

            int c;
            while ((c = getopt_long(argc, argv, short_options, long_options, &option_index)) != -1)
                switch (c) {
                    case 'h':
                    case '?':
                    default: {
                        help_zip(argv[0]);
                        return EXIT_FAILURE;
                    }
                }

            string in_file = argv[++optind];
            string out_file = in_file + ".lz77";

            stringstream ss;
            ss << ifstream(in_file).rdbuf();
            string str = ss.str();
            string_view strv{str.c_str(), str.size()};

            ofstream(out_file, ios::trunc); // clear file
            ofstream out(out_file, ios::out | ios::binary | ios::app);

            lz77::zip(strv, out);

            return 0;
        }

        case 'u': { // unzip
            const char *short_options = ":h";
            const option long_options[] = {
                    {"help",  no_argument, nullptr, 'h'},
                    {nullptr, no_argument, nullptr, '\0'},
            };
            int option_index = -1;

            int c;
            while ((c = getopt_long(argc, argv, short_options, long_options, &option_index)) != -1)
                switch (c) {
                    case 'h':
                    case '?':
                    default: {
                        help_unzip(argv[0]);
                        return EXIT_FAILURE;
                    }
                }

            string in_file = argv[++optind];

            ifstream in(in_file);
            lz77::unzip(in);

            return 0;
        }

        default:
            help(argv[0]);
            return 1;
    }
}
