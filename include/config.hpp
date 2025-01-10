#ifndef CONFIG_H
#define CONFIG_H

#include<string>

class Config {
public:
    Config() = default;
    bool load(const std::string& filepath);
    std::string getApiKey() const;

private:
    std::string apiKey; 
};

#endif
