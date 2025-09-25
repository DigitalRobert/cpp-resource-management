#include <iostream>
#include <fstream>
#include <string>
#include <filesystem> // C++17
#include <utility>    // std::move
#include <chrono>
#include <random>

class TempFile {
public:
    // Constructor: create unique filename and open file
    TempFile() {
        filename_ = generate_unique_filename();
        ofs_.open(filename_, std::ios::out | std::ios::app);
        if (!ofs_) {
            throw std::runtime_error("Failed to create temp file");
        }
        std::cout << "TempFile created: " << filename_ << "\n";
    }

    // Destructor: close and remove file
    ~TempFile() {
        if (ofs_.is_open()) {
            ofs_.close();
        }
        if (!filename_.empty()) {
            std::filesystem::remove(filename_);
            std::cout << "TempFile removed: " << filename_ << "\n";
        }
    }

    // Move constructor
    TempFile(TempFile&& other) noexcept
        : filename_(std::move(other.filename_)), ofs_(std::move(other.ofs_)) {
        other.filename_.clear(); // leave other in "empty" state
    }

    // Move assignment
    TempFile& operator=(TempFile&& other) noexcept {
        if (this != &other) {
            // Clean up current file
            if (ofs_.is_open()) ofs_.close();
            if (!filename_.empty()) std::filesystem::remove(filename_);

            filename_ = std::move(other.filename_);
            ofs_ = std::move(other.ofs_);
            other.filename_.clear();
        }
        return *this;
    }

    // Delete copy operations
    TempFile(const TempFile&) = delete;
    TempFile& operator=(const TempFile&) = delete;

    // Write text to file
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

int main() {
    try {
        TempFile tf1;
        tf1.write("First line in temp file");

        TempFile tf2 = std::move(tf1); // move ownership
        tf2.write("Second line after move");

        // tf1 is now empty; cannot write with tf1
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }

    // Upon exiting main, tf2 destructor will remove the temp file automatically
    return 0;
}
