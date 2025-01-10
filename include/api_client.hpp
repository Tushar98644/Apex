#ifndef API_CLIENT_H
#define API_CLIENT_H

#include <string>

class APIClient {
public:
    APIClient(const std::string& baseUrl);
    ~APIClient() = default;

    std::string getMarketData(const std::string& instrument);

private:
    std::string baseUrl_;
};

#endif
