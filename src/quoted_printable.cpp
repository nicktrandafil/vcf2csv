#include "quoted_printable.hpp"


namespace vcf {
namespace quoted_printable {


std::string decode(const std::string& x)
{
    std::string decoded;
    for (std::size_t i = 0, begin, end, size = x.size(); i < size; ++i) {
        // Usual char
        if (x[i] != '=') {
            decoded += x[i];
            continue;
        }

        begin = i + 1;
        end = i + 2;

        if (begin >= size) { break; }

        if ('\n' == x[begin]) {
            i = begin;
            continue;
        }

        if (end >= size) { break; }

        const std::string hex{x[begin], x[end]};

        if ("\r\n" == hex) {
            i = end;
            continue;
        }

        try {
            const char c = std::stoi(hex, nullptr, 16);
            decoded += c;
            i = end;
        } catch (...) {
            decoded += x[i];
        }
    }
    return decoded;
}


} // namespace quoted_printable
} // namespace vcf
