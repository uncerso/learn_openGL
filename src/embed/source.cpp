#include <fstream>
#include <iostream>
#include <vector>
#include <cstdint>
#include <filesystem>

namespace fs = std::filesystem;

std::vector<uint8_t> readFile(fs::path const& file_name) {
    std::ifstream input(file_name, std::ios::binary);
    if (!input.is_open()) {
        std::cerr << "Cannot open file: '" << file_name << "'" << std::endl;
        throw 1;
    }
    return {std::istreambuf_iterator<char>(input), {}};
}

void write(std::vector<uint8_t> const& resource, fs::path const& output_file) {
    std::ofstream out(output_file, std::ios::out);
    if (!out.is_open()) {
        std::cerr << "Cannot open output file: '" << output_file << "'" << std::endl;
        throw 1;
    }

    if (!resource.empty())
        out << (int)resource[0];
    for (size_t i = 1; i < resource.size(); ++i)
        out << ',' << (int)resource[i];
}

int main(int argc, char const* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <binary_file> <output_dir>" << std::endl;
        return 1;
    }

    const auto file_path = fs::path(argv[1]);
    const auto file_content = readFile(file_path);

    const auto output_file = fs::path(argv[2]) / file_path.stem().concat(".h");
    write(file_content, output_file);

    return 0;
}
