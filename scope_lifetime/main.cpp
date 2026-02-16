#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <utility>
#include <chrono>
#include <random>

class TempFile {
public:
    TempFile() {
        filename_ = generate_unique_filename();
        ofs_.open(filename_, std::ios::out | std::ios::app);
        if (!ofs_) throw std::runtime_error("Failed to create temp file");
        std::cout << "TempFile created: " << filename_ << "\n";
    }

    ~TempFile() {
        if (ofs_.is_open()) ofs_.close();
        if (!filename_.empty()) {
            std::filesystem::remove(filename_);
            std::cout << "TempFile removed: " << filename_ << "\n";
        }
    }

    TempFile(TempFile&& other) noexcept
        : filename_(std::move(other.filename_)), ofs_(std::move(other.ofs_)) {
        other.filename_.clear();
    }

    TempFile& operator=(TempFile&& other) noexcept {
        if (this != &other) {
            if (ofs_.is_open()) ofs_.close();
            if (!filename_.empty()) std::filesystem::remove(filename_);
            filename_ = std::move(other.filename_);
            ofs_ = std::move(other.ofs_);
            other.filename_.clear();
        }
        return *this;
    }

    TempFile(const TempFile&) = delete;
    TempFile& operator=(const TempFile&) = delete;

    void write(const std::string& text) {
        if (!ofs_) throw std::runtime_error("File not open");
        ofs_ << text << "\n";
    }

private:
    std::string filename_;
    std::ofstream ofs_;

    std::string generate_unique_filename() {
        auto tmp = std::filesystem::temp_directory_path();
        auto time_now = std::chrono::steady_clock::now().time_since_epoch().count();
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(1000, 9999);
        return tmp.string() + "/tempfile_" + std::to_string(time_now) + "_" + std::to_string(dist(gen)) + ".txt";
    }
};

void create_file() {
    TempFile file;
    file.write("inside function");
}

int main() {
    create_file();
}

