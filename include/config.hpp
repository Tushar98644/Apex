#ifndef CONFIG_H
#define CONFIG_H

#include<string>

class Config {
public:
    Config() = default;
    bool load(const std::string& filepath);
    std::string getClientSecret() const;
    std::string getClientId() const;

private:
    std::string clientSecret; 
    std::string clientId;
};

#endif
