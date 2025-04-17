#include "csv.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "board.h"

namespace Napoleon
{

void preprocess_csv(const std::string& input_path, const std::string& output_path, bool skip_header) {
  std::ifstream file(input_path);
  if (!file.is_open()) {
    std::cerr << "Error: could not open file '" << input_path << std::endl;
    return;
  }

  std::ofstream out(output_path);
  if (!out.is_open()) {
    std::cerr << "Error: could not open output file '" << output_path << std::endl;
    return;
  }
  std::string line;
  // Optionally skip the header line
  if (skip_header && std::getline(file, line)) {
    // Header skipped
  }

  // Read and parse each remaining line
  Board board;
  ulong count = 0;
  while (std::getline(file, line)) {
    std::istringstream ss(line);
    std::string fen, depth, score;

    // Parse exactly three comma‑separated fields
    if (std::getline(ss, fen, ',') &&
      std::getline(ss, depth, ',') &&
      std::getline(ss, score)) {
      board.LoadGame(fen);
      out << board.ToCsv() << "," << depth << "," << score << std::endl;
      if (count & 1024) {
        std::cout << "\rProcessed lines: " << count;
      }
      count++;
    }
    else {
      std::cerr << "Warning: malformed line: " << line << std::endl;
      exit(1);
    }
  }
}

}
