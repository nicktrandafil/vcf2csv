#include "quoted_printable.hpp"


namespace vcf::quoted_printable {


std::string decode(const std::string& x)
{
    std::string decoded;
    for (std::size_t i = 0, j, size = x.size(); i < size; ++i) {
        // Usual char
        if (x[i] != '=') {
            decoded += x[i];
            continue;
        }

        j = i + 1;
        i = i + 2;

        if (i >= size) { break; }

        const std::string hex{x[j], x[i]};
        try {
            const char c = std::stoi(hex, nullptr, 16);
            decoded += c;
        } catch (...) {
            decoded += hex;
        }
    }
    return decoded;
}


} // namespace vcf::quoted_printable
