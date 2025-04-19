#include "./utils.h"

#include <string>
#include <iostream>
#include <fstream>

std::string Utils::load_file_source(const char* p_filepath)
{
  std::fstream file_stream { p_filepath, std::ios::in };
  std::string content, line = "";

  if (!file_stream.is_open())
  {
    std::cout << "FILE::READ::FAILED " << p_filepath << std::endl;
    exit(EXIT_FAILURE);
  }

  while (!file_stream.eof())
  {
    getline(file_stream, line);
    content.append(line + "\n");
  }

  file_stream.close();
  return content;
}
