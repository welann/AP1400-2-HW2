#ifndef SERVER_H
#define SERVER_H
#include <iostream>
#include <sstream>

#include <map>
#include <memory>
#include <string>
#include <vector>

class Client;
void show_pending_transactions();
std::vector<std::string>& return_pending_trxs();
class Server {
public:
    Server();
    std::shared_ptr<Client> add_client(std::string id);
    std::shared_ptr<Client> get_client(std::string id) const;
    double get_wallet(std::string id) const;
    bool add_pending_trx(std::string trx, std::string signature) const;
    size_t mine();

    static bool parse_trx(std::string const& trx, std::string& sender, std::string& receiver, double& value)
    {
        std::stringstream sstr(trx);
        std::string temp;
        std::vector<std::string> out;
        while (std::getline(sstr, temp, '-')) {
            out.push_back(temp);
        }
        if (out.size() != 3) {
            // for(auto i:out){
            //     std::cout<<i<<std::endl;
            // }
            // std::cout<<"==="<<std::endl;
            throw std::runtime_error("the string's format is incorrect");
        }
        sender = out[0];
        receiver = out[1];
        value = stod(out[2]);
        return true;
    }

    friend void show_wallets(const Server& server);

private:
    std::map<std::shared_ptr<Client>, double> clients;
};





#endif // SERVER_H