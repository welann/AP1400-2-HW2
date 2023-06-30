#include "client.h"
#include "server.h"
#include <random>
#include <string>

Client::Client(std::string id, const Server& server)
    : server(&server)
    , id(id)
{
    crypto::generate_key(public_key, private_key);
}

std::string Client::get_id() const
{
    return id;
}

std::string Client::get_publickey() const
{
    return public_key;
}

double Client::get_wallet() const
{
    return server->get_wallet(id);
}

std::string Client::sign(std::string txt) const
{
    std::string signature = crypto::signMessage(private_key, txt);
    return signature;
}

bool Client::transfer_money(std::string receiver, double value)
{
    if (value > get_wallet()) {
        return false;
    }

    std::string trx = id + "-" + receiver + "-" + std::to_string(value);
    // std::cout<<"trx: "<<trx << std::endl;
    // std::cout<<id << " " << receiver<< " " << std::to_string(value)<< std::endl;

    std::string signature = crypto::signMessage(private_key, trx);

    return server->add_pending_trx(trx, signature);
}
size_t Client::generate_nonce()
{
    std::random_device dev;
    std::uniform_int_distribution<int> dis(0, INT_MAX);
    std::mt19937 rng(dev());
    return dis(rng);
}