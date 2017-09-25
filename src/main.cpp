// local
#include "csv.hpp"
#include "quoted_printable.hpp"
#include "vcf.hpp"
#include "vcf_iterator.hpp"

#include "meta/any.hpp"

// 3rd
#include <folly/ScopeGuard.h>

// boost
#include <boost/format.hpp>

#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <boost/range/adaptor/indexed.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/range/algorithm/for_each.hpp>

// std
#include <algorithm>
#include <codecvt>
#include <cstring>
#include <iostream>
#include <fstream>

// win
#ifdef WIN32
#include <windows.h>
#endif


namespace fs = boost::filesystem;


using boost::wformat;


std::string fromWString(const std::wstring& x)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cv;
    return cv.to_bytes(x);
}


std::wstring toWString(const std::string& x)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cv;
    return cv.from_bytes(x);
}


void err(const std::wstring& text)
{
#ifdef WIN32
    std::wcerr << text;
#else
    std::cerr << fromWString(text);
#endif
}


void out(const std::wstring& text)
{
#ifdef WIN32
    std::wcerr << text;
#else
    std::cout << fromWString(text);
#endif
}


void help(const std::wstring& app_name)
{
    const auto x = wformat(
        L"Использование:\n"
        L"    1) Поместите *.vcf файлы в одну папку\n"
        L"с %1% и запустите программу. После завершения\n"
        L"работы программы, в папку появятся файлы с теми\n"
        L"же названиями, но с расширением *.csv\n"
        L"    2) %1% file1.vcf file2.vcf ... fileN.vcf\n"
        L"Запустите программу с командной строки и задайте\n"
        L"несколько входных файлов. Пути могут быть относительными\n"
        L"(относительной текущей папки) или абсолютными.\n"
   ) % app_name;

   out(x.str());
}


std::string vcardHeaders()
{
    std::vector<std::string> headers;
    headers.push_back("Версия");
    headers.push_back("ФИО");
    headers.push_back("Фамилия");
    headers.push_back("Имя");
    headers.push_back("Отчество");
    headers.push_back("Префикс");
    headers.push_back("Суфикс");
    headers.push_back("Тип телефона");
    headers.push_back("Телефон");
    headers.push_back("Тип адреса");
    headers.push_back("Почтовый ящик");
    headers.push_back("Расширенный адрес");
    headers.push_back("Улица, дом");
    headers.push_back("Город");
    headers.push_back("Регион");
    headers.push_back("Почтовый индекс");
    headers.push_back("Страна");
    headers.push_back("Неизвестные аттрибуты");
    return vcf::csv::serialize(headers);
}


using Decoder = std::function<std::string(std::string)>;


Decoder makeDecoder(
    const boost::optional<std::string>& outer_coding,
    const boost::optional<std::string>& inner_coding)
{
    const auto ic = boost::to_lower_copy(inner_coding.value_or("utf-8"));
    if (ic != "utf-8") {
        // TODO warning non utf-8 coding not supported
    }

    if (outer_coding &&
        boost::to_lower_copy(*outer_coding) == "quoted-printable")
    {
        return &vcf::quoted_printable::decode;
    }

    if (outer_coding) {
        // TODO warning non quoted-printable coding not supported
    }

    return [](auto&& x) { return std::forward<decltype(x)>(x); };
}


std::string toCsv(const boost::optional<vcf::attribute::Version>& x)
{
    return x ? x->value : "";
}


std::string toCsv(const boost::optional<vcf::attribute::FormattedName>& x)
{
    if (!x) { return {}; }
    const auto decode = makeDecoder(x->encoding, x->charset);
    return decode(x->value);
}


std::vector<std::string> toCsv(const boost::optional<vcf::attribute::Name>& x)
{
    std::vector<std::string> row(5);
    if (!x) { return row; }

    const auto decode = makeDecoder(x->encoding, x->charset);
    row[0] = decode(x->surname);
    row[1] = decode(x->first_name);
    row[2] = decode(x->patronymic);
    row[3] = decode(x->prefix);
    row[4] = decode(x->suffix);

    return row;
}


std::vector<std::string> toCsv(const std::vector<vcf::attribute::Tel>& telephones)
{
    std::vector<std::string> row(2);

    using namespace boost::adaptors;
    const int last = telephones.size() - 1;
    for (const auto& x: telephones | indexed(0)) {
        const auto eol = x.index() == last ? "" : "\n";
        const auto tmp = boost::format("=\"%1%\"") % x.value().value;
        row[0] += x.value().type.value_or("") + eol;
        row[1] += tmp.str()                   + eol;
    }

    return row;
}


std::vector<std::string> toCsv(const std::vector<vcf::attribute::Addr>& addresses)
{
    std::vector<std::string> row(8);

    using namespace boost::adaptors;
    const int last = addresses.size() - 1;
    for (const auto& node : addresses | indexed(0)) {
        const auto eol = node.index() == last ? "" : "\n";
        const auto x = node.value();
        const auto decode = makeDecoder(x.encoding, x.charset);
        row[0] += x.type.value_or("")        + eol;
        row[1] += decode(x.postal_box)       + eol;
        row[2] += decode(x.extended_address) + eol;
        row[3] += decode(x.street)           + eol;
        row[4] += decode(x.town)             + eol;
        row[5] += decode(x.region)           + eol;
        row[6] += decode(x.postal_code)      + eol;
        row[7] += decode(x.country)          + eol;
    }

    return row;
}


std::string toCsv(const vcf::VCard& x)
{
    std::vector<std::string> row;
    row.push_back(toCsv(x.version));
    row.push_back(toCsv(x.formatted_name));
    boost::copy(toCsv(x.name), std::back_inserter(row));
    boost::copy(toCsv(x.tel), std::back_inserter(row));
    boost::copy(toCsv(x.address), std::back_inserter(row));
    row.push_back(boost::trim_copy(x.unkonwn));
    return vcf::csv::serialize(row);
}


void process(const fs::path& path)
{
    out((wformat(L"Обработка файла %1% ... ") % path.wstring()).str());
    std::wstring info{L"успешно"};
    SCOPE_EXIT { out(info + L"\n"); };

    std::ifstream inf{path.string<std::string>()};
    if (!inf) {
        info = (wformat(L"ошибка: не могу открыть файл %1% для чтения")
            % path.wstring())
            .str();
        return;
    }

    auto out_path = path;
    out_path.replace_extension(".csv");
    std::ofstream outf{out_path.string<std::string>()};
    if (!outf) {
        info = (wformat(L"ошибка: не могу открыть файл %1% для записи")
            % out_path.wstring())
            .str();
        return;
    }

    vcf::Vcf parser{
        std::istreambuf_iterator<char>{inf},
        std::istreambuf_iterator<char>{}};

    const auto vcards = 
        boost::make_iterator_range(
            vcf::VcfIterator{parser}, vcf::VcfIterator{});

    outf << vcardHeaders() << '\n';

    using namespace boost::adaptors;
    boost::for_each(
        vcards | transformed(
            static_cast<std::string(*)(const vcf::VCard &)>(&toCsv)),
        [&outf](const std::string &x) {
            outf << x << '\n';
        });
}


void processFilesInCurrentDir()
{
    const auto current = fs::current_path();
    for (const auto &entry: fs::directory_iterator{current}) {
        if (fs::is_directory(entry)) { continue; }
        const auto path = entry.path();
        if (path.extension() != ".vcf") { continue; }
        process(path.filename());
    }
    out(L"Нажмите \"Ввод\" для завершения\n");
    std::getchar();
}


int main(int argc, char *argv[])
{
#ifdef WIN32
    setlocale(LC_CTYPE, "Russian");
    SetConsoleOutputCP(1251);
#endif

    // Help
    if (2 == argc && argv[1] == std::string{"-h"}) {
        help(toWString(argv[0]));
        return 0;
    }

    // First use case
    if (1 == argc) {
        processFilesInCurrentDir();
        return 0;
    }

    // Second use case
    std::for_each(
        argv + 1, argv + argc,
        [](auto x) {
            const fs::path path{x};
            if (path.extension() != ".vcf") { return; }
            process(path);
        });

    return 0;
}
