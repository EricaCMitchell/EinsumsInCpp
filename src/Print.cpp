#include "EinsumsInCpp/Print.hpp"

#include <algorithm>
#include <cstdarg>
#include <cstdio>
#include <iomanip>
#include <mutex>
#include <regex>
#include <sstream>
#include <string>
#include <thread>
#include <unistd.h>

namespace EinsumsInCpp {

namespace Print {

namespace {

std::mutex lock;

int indent_level{0};
std::string indent_string{};

bool print_master_thread_id{false};
std::thread::id main_thread_id = std::this_thread::get_id();

bool suppress{false};

auto printing_to_screen() -> bool { return isatty(fileno(stdout)); }

} // namespace

void update_indent_string() {
    indent_string = "";
    indent_string.insert(0, indent_level, ' ');
}

void indent() {
    indent_level += 4;
    update_indent_string();
}

void deindent() {
    indent_level -= 4;
    if (indent_level < 0)
        indent_level = 0;
    update_indent_string();
}

auto current_indent_level() -> int { return indent_level; }

void always_print_thread_id(bool onoff) { print_master_thread_id = onoff; }

void suppress_output(bool onoff) { suppress = onoff; }

} // namespace Print

namespace {

void print_line(const std::string &line) {
    std::string line_header;
    if (Print::print_master_thread_id && std::this_thread::get_id() == Print::main_thread_id) {
        std::ostringstream oss;
        oss << "[ main #" << std::setw(6) << std::this_thread::get_id() << " ] ";
        line_header = oss.str();
    } else if (std::this_thread::get_id() != Print::main_thread_id) {
        std::ostringstream oss;
        oss << "[ tid  #" << std::setw(6) << std::this_thread::get_id() << " ] ";
        line_header = oss.str();
    }
    line_header.append(Print::indent_string);

    std::lock_guard<std::mutex> guard(Print::lock);
    std::printf("%s", line_header.c_str());
    std::printf("%s", line.c_str());
    std::printf("\n");
}

} // namespace

namespace Detail {
void println(const std::string &str) {
    if (Print::suppress == false) [[likely]] {
        std::istringstream iss(str);

        for (std::string line; std::getline(iss, line);) {
            print_line(line);
        }
    }
}
} // namespace Detail

} // namespace EinsumsInCpp