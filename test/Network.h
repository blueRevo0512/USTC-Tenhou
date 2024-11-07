#pragma once
#include <memory>
#include <map>
#include <boost/asio.hpp>
#include <string>

using boost::asio::ip::tcp;

class NetworkManager {
public:
    static NetworkManager& getInstance();
    void server(boost::asio::io_service& io_service, int port);
    void client(boost::asio::io_service& io_service, const std::string& host, const std::string& port, bool is_server);
    
    std::map<int, std::shared_ptr<tcp::socket>> clients;
    std::map<int, std::shared_ptr<tcp::socket>> clients_selection;

private:
    NetworkManager() = default;
    NetworkManager(const NetworkManager&) = delete;
    NetworkManager& operator=(const NetworkManager&) = delete;
}; 