#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <map>

#define TAG_DELIM ';'

bool is_numerical(const std::string &str)
{
    if (str.length() == 0) return false;

    for (size_t i = 0; i < str.length(); i++) {
        char c = tolower(str[i]);
        if (c >= '0' && c <= '9') continue;
        if (c >= 'a' && c <= 'f') continue;
        return false;
    }
    return true;
}

bool process_file(const std::string &in_file, const std::string &out_file, const std::string &module_name)
{
    std::ifstream infile(in_file);
    if (!infile.is_open()) {
        return false;
    }
    std::ofstream outfile(out_file);
    if (!outfile.is_open()) {
        return false;
    }

    std::map<DWORD, std::string> rva_to_cmt;

    std::string line;
    while (std::getline(infile, line)) {
        if (line.length() == 0) continue;

        size_t pos = line.find(TAG_DELIM, 0);
        if (pos == std::string::npos || pos >= (line.length() - 1)) continue;

        std::string rva_str = line.substr(0, pos);
        std::string cmt_str = line.substr(pos + 1, line.length());
        if (rva_str.length() == 0 || cmt_str.length() == 0) continue;

        if (!is_numerical(rva_str)) continue; //skip lines that don't start from RVA
        DWORD rva = std::stoi(rva_str, nullptr, 16);
        rva_to_cmt[rva] = cmt_str;
    }
    infile.close();

    if (module_name.length() > 0) {
        outfile << "bpdll " << "\"" << module_name << "\"" << std::endl;
        outfile << "run " << std::endl;
    }

    std::map<DWORD, std::string>::iterator itr;
    for (itr = rva_to_cmt.begin(); itr != rva_to_cmt.end(); itr++) {
        const std::string cmt_str = itr->second;
        const DWORD rva = itr->first;
        outfile << "cmt :$" << std::hex << rva << "," << "\"" << cmt_str << "\"" << std::endl;
        outfile << "bookmark :$" << std::hex << rva << std::endl;
    }
    outfile.close();
    return true;
}

std::string fetch_module_name(std::string filename)
{
    size_t pos = filename.find_last_of("\\/");
    if (pos != std::string::npos) {
        filename.erase(0, pos + 1);
    }
    pos = filename.find(".tag", 0);
    if (pos == std::string::npos) return "";

    return filename.substr(0, pos);
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "Args: <in_file> [module_name] [out_file]\n";
        return 0;
    }
    std::string module_name = fetch_module_name(argv[1]);
    std::string out = std::string(argv[1]) + ".x64dbg.txt";
    if (argc >= 3) {
        module_name = argv[2];
    }
    if (argc >= 4) {
        out = argv[3];
    }
    std::cout << "Module Name: " << module_name << "\n";
    std::cout << "Output file: " << out << "\n";
    process_file(argv[1], out, module_name);
    return 0;
}
