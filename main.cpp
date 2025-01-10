#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <filesystem>

#include <chrono>
#include <cassert>

namespace chrono = std::chrono;

/// @brief turn RIR Record into std::vector
/// @param record pipe separated record
/// @param vector std::vector record
void split(std::string in, std::vector<std::string> &out);

int main()
{
    /* File Streams */
    std::vector<std::ifstream> fins;
    for (const auto entry : std::filesystem::directory_iterator("data")) {
        fins.emplace_back(std::ifstream(entry.path()));
    }
    // std::ifstream fin("data\\delegated-apnic-latest.psv");
    // if (!fin) {
    //     std::cerr << "Failed to open the file. Aborting..." << std::endl;
    //     exit(EXIT_FAILURE);
    // }
    std::ofstream fout4("cidr\\ipv4_cidr.csv", std::ios_base::out | std::ios_base::trunc);
    if (!fout4) {
        std::cerr << "Failed to open the file. Aborting..." << std::endl;
        exit(EXIT_FAILURE);
    }
    std::ofstream fout6("cidr\\ipv6_cidr.csv", std::ios_base::out | std::ios_base::trunc);
    if (!fout6) {
        std::cerr << "Failed to open the file. Aborting..." << std::endl;
        exit(EXIT_FAILURE);
    }

    auto start = chrono::system_clock::now();


    std::string line_buf;
    std::vector<std::string> record;
    static std::size_t region_len;
    int header_count, asn_count, ipv4_count, ipv6_count;


    for (auto& fin : fins) {
        /* Header */
        while (true) {
            // count comment lines
            std::getline(fin, line_buf);
            if (line_buf[0] == '#') {
                header_count++;
                continue;
            }

            split(line_buf, record);
            // count version lines
            if (line_buf[0] == '2') {
                region_len = record.at(1).size();
                header_count++;
                continue;
            }

            // count summary lines
            if (record.at(5) == "summary") {
                header_count++;
                // get amount of each address
                if (record.at(2) == "asn")  asn_count  = std::stoi(record.at(4));
                if (record.at(2) == "ipv4") ipv4_count = std::stoi(record.at(4));
                if (record.at(2) == "ipv6") ipv6_count = std::stoi(record.at(4));
                continue;
            }
            break;
        }

        // seek to begining of file
        fin.seekg(0, std::ios_base::beg);

        // the largest possible size of std::string
        const size_t string_max_size = std::string().max_size();

        /* Ignore Header */
        for (int i = header_count; i > 0; i--) {
            fin.ignore(string_max_size, '\n');
        }

        /* Convert */
        while (std::getline(fin, line_buf)) {
            /* ASN */
            if (line_buf.at(region_len + 4) == 'a') {
                continue;
            }
            /* IPv4 */
            if (line_buf.at(region_len + 7) == '4') {
                split(line_buf, record);
                fout4 << record.at(1) << ','
                    << record.at(3) << '/'
                    << (32 - static_cast<int>(std::log2(std::stoi(record.at(4))))) << '\n';
            }
            /* IPv6 */
            if (line_buf.at(region_len + 7) == '6') {
                split(line_buf, record);
                fout6 << record.at(1) << ',' << record.at(3) << '/' << record.at(4) << '\n';
            }
        }
    }

    auto end = chrono::system_clock::now();

    std::cout << "Elapsed: "
    << (double)chrono::duration_cast<chrono::microseconds>(end - start).count()/1000.0 << "ms" << std::endl;

    fout4 << std::flush;
    fout6 << std::flush;

    for (auto& fin : fins) {
        fin.close();
    }
    fout4.close();
    fout6.close();
    return 0;
}

void split(std::string instring, std::vector<std::string> &outvector) {
    std::size_t first = 0, last = instring.find_first_of('|');
    const std::size_t size = instring.size();
    outvector.clear();

    if (last == std::string::npos) {
        std::cerr << "Invalid string given" << std::endl;
        exit(EXIT_FAILURE);
    }

    while (first < size) {
        outvector.push_back(std::string(instring, first, last-first));
        first = last + 1;
        last = instring.find_first_of('|', first);

        if (last == std::string::npos) {
            last = size;
        }
    }
}
