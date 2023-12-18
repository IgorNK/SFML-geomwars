namespace Configuration {
typedef std::map<std::string, std::map<std::string, std::string>> Config;

static Config config;

void test_config(Config & config) {
    std::for_each(config.begin(), config.end(),
    [](std::pair<std::string, std::map<std::string, std::string>> p){
        std::cout << "Heading: " << p.first << '\n';
        std::for_each(p.second.begin(), p.second.end(),
        [](std::pair<std::string, std::string> v){
            std::cout << v.first << ": " << v.second << '\n';
        });
    });
    std::cout << "====Individual tests====\n";
    std::cout << "Window resolution and refresh rate: " << config["Window"]["width"] << "x" << config["Window"]["height"] << ", " << config["Window"]["refreshRate"] << '\n';
}

Config parse_tokens(const std::vector<std::string> tokenstream) {
    bool nextHeading = true;
    std::string heading = "";
    Config config;
    for (std::string token : tokenstream) {
        
        std::string key, value;
        std::string::size_type begin = token.find_first_not_of( " ,[\f\t\v" );

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
        std::string::size_type end = token.find( '=', begin );
        key = token.substr( begin, end - begin );

        // (No leading or trailing whitespace allowed)
        key.erase( key.find_last_not_of( " ,]\f\t\v" ) + 1 );

        // No blank keys allowed
        if (key.empty()) {
            continue;
        }

        // Extract the value (no leading or trailing whitespace allowed)
        begin = token.find_first_not_of( " ,]\f\n\r\t\v", end + 1 );
        end   = token.find_last_not_of(  " ,]\f\n\r\t\v" ) + 1;

        value = token.substr( begin, end - begin );

        // std::cout << "key: " << key << ", value: " << value << '\n';
        config[heading][key] = value;     
    }
    return config;
}

Config read_file(const std::string filename) {
    std::vector<std::string> tokenstream {};
    std::string word;
    
    std::ifstream ifs(filename, std::ifstream::in);
    while (ifs >> word) {
        tokenstream.push_back(word);
    }

    return parse_tokens(tokenstream);
}
}