#include "game.h"
#include <fstream>

void Game::test_config(Config &config) const {
  std::for_each(
      config.begin(), config.end(),
      [](std::pair<std::string, std::map<std::string, std::string>> p) {
        std::cout << "Heading: " << p.first << '\n';
        std::for_each(p.second.begin(), p.second.end(),
                      [](std::pair<std::string, std::string> v) {
                        std::cout << v.first << ": " << v.second << '\n';
                      });
      });
  std::cout << "====Individual tests====\n";
  std::cout << "Window resolution and refresh rate: "
            << config["Window"]["width"] << "x" << config["Window"]["height"]
            << ", " << config["Window"]["refreshRate"] << '\n';
}

Config Game::parse_tokens(const std::vector<std::string> &tokenstream) const {
  bool nextHeading = true;
  std::string heading = "";
  Config config;
  for (std::string token : tokenstream) {

    std::string key, value;
    std::string::size_type begin = token.find_first_not_of(" ,[\f\t\v");

    if (nextHeading) {
      nextHeading = false;
      heading = token;
      config[heading] = {};
      // std::cout << "Heading: " << heading << "\n";
      continue;
    }

    if (token.find(']') != std::string::npos) {
      nextHeading = true;
      // std::cout << "next heading\n";
    }

    // Skip blank lines
    if (begin == std::string::npos) {
      continue;
    }

    // Extract the key value
    std::string::size_type end = token.find('=', begin);
    key = token.substr(begin, end - begin);

    // (No leading or trailing whitespace allowed)
    key.erase(key.find_last_not_of(" ,]\f\t\v") + 1);

    // No blank keys allowed
    if (key.empty()) {
      continue;
    }

    // Extract the value (no leading or trailing whitespace allowed)
    begin = token.find_first_not_of(" ,]\f\n\r\t\v", end + 1);
    end = token.find_last_not_of(" ,]\f\n\r\t\v") + 1;

    value = token.substr(begin, end - begin);

    // std::cout << "key: " << key << ", value: " << value << '\n';
    config[heading][key] = value;
  }
  return config;
}

const Config Game::read_file(const std::string &filename) const {
  // May throw, should be enclosed in try / catch expression
  std::vector<std::string> tokenstream{};
  std::string word;

  std::ifstream ifs(filename, std::ifstream::in);
  if (!ifs) {
    throw std::runtime_error("Could not read file");
  }
  while (ifs >> word) {
    tokenstream.push_back(word);
  }

  return parse_tokens(tokenstream);
}

const int Game::read_config_i(const std::string &header,
                              const std::string &value) {
  try {
    return std::stoi(m_config[header][value]);
  } catch (const std::invalid_argument &ia) {
    std::cerr << "Invalid argument: " << ia.what() << " | " << header << ":"
              << value << std::endl;
    return -1;
  }

  catch (const std::out_of_range &oor) {
    std::cerr << "Out of Range error: " << oor.what() << std::endl;
    return -2;
  }

  catch (const std::exception &e) {
    std::cerr << "Undefined error: " << e.what() << std::endl;
    return -3;
  }
}

const float Game::read_config_f(const std::string &header,
                                const std::string &value) {
  try {
    return std::stof(m_config[header][value]);
  } catch (const std::invalid_argument &ia) {
    std::cerr << "Invalid argument: " << ia.what() << " | " << header << ":"
              << value << std::endl;
    return -1;
  }

  catch (const std::out_of_range &oor) {
    std::cerr << "Out of Range error: " << oor.what() << std::endl;
    return -2;
  }

  catch (const std::exception &e) {
    std::cerr << "Undefined error: " << e.what() << std::endl;
    return -3;
  }
}

void Game::write_config(const std::string &header, const std::string &item,
                        const std::string &value) {
  if (m_config[header][item].size() == 0) {
    throw std::runtime_error("Wrong config item: " + header + ":" + item);
  } else {
    m_config[header][item] = value;
  }
}

const std::string Game::read_config_s(const std::string &header,
                                      const std::string &value) {
  try {
    return m_config[header][value];
  } catch (std::exception &e) {
    throw std::runtime_error(std::string(e.what(), sizeof(e.what())) +
                             " | Config string read error: " + header + value);
  }
}

void Game::export_config(Config &config, const std::string &filename) const {
  // May throw, should be enclosed in try / catch expression

  std::ofstream file;
  file.open(filename, std::ios::out);
  if (!file) {
    throw std::runtime_error("Could not create file");
  }

  // file << "something";

  std::for_each(
      config.begin(), config.end(),
      [&](std::pair<std::string, std::map<std::string, std::string>> p) {
        file << p.first << " [\n";
        std::for_each(p.second.begin(), p.second.end(),
                      [&](std::pair<std::string, std::string> v) {
                        file << '\t' << v.first << "=" << v.second << ",\n";
                      });
        file << "],\n";
      });

  file.close();
}
