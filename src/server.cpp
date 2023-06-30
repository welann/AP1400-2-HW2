#include "server.h"
#include "client.h"
#include <memory>
#include <random>
#include <string>
#include <vcruntime.h>

std::vector<std::string> pending_trxs;

std::vector<std::string>& return_pending_trxs()
{
    return pending_trxs;
}

void show_pending_transactions()
{
    std::cout << std::string(20, '*') << std::endl;
    for (const auto& trx : pending_trxs)
        std::cout << trx << std::endl;
    std::cout << std::string(20, '*') << std::endl;
}

void show_wallets(const Server& server)
{
    std::cout << std::string(20, '*') << std::endl;
    for (const auto& client : server.clients)
        std::cout << client.first->get_id() << " : " << client.second << std::endl;

    std::cout << std::string(20, '*') << std::endl;
}

Server::Server() { pending_trxs.clear(); }//每次初始化要记得清理，否则会影响其他的测试样例

std::shared_ptr<Client> Server::add_client(std::string id)
{

    for (auto& client : clients) {
        if (client.first->get_id() == id) {
            std::random_device dev;
            std::uniform_int_distribution<int> dis(1000, 9999);
            std::mt19937 rng(dev());
            id += std::to_string(dis(rng));
        }
    }

    Client result(id, *this);
    auto pclient = std::make_shared<Client>(result);
    clients.insert({ pclient, 5 });
    return pclient;
}

std::shared_ptr<Client> Server::get_client(std::string id) const
{
    for (auto& client : clients) {
        if (client.first->get_id() == id) {
            return client.first;
        }
    }
    return nullptr;
}

double Server::get_wallet(std::string id) const
{
    for (auto& client : clients) {
        if (client.first->get_id() == id) {
            return client.second;
        }
    }
    return -1;
}

bool Server::add_pending_trx(std::string trx, std::string signature) const
{
    // std::cout << "trx1: " << trx << std::endl;
    // std::cout<<id << " " << receiver<< " " << std::to_string(value)<< std::endl;
    std::string sender {}, receiver {};
    double value;
    parse_trx(trx, sender, receiver, value);
    // std::cout << "trx2: " << trx << std::endl;

    std::shared_ptr<Client> sender_client = get_client(sender);
    std::shared_ptr<Client> receiver_client = get_client(receiver);

    // std::cout<<sender_client<<" "<<receiver_client<<" "<<value<<std::endl;
    if (sender_client == nullptr || receiver_client == nullptr || sender_client == receiver_client) {
        return false;
    }

    auto public_key = sender_client->get_publickey();
    bool authentic = crypto::verifySignature(public_key, trx, signature);

    if (authentic) {
        pending_trxs.push_back(trx);
        return true;
    }

    return false;
}

size_t Server::mine()
{
    std::string mempool {};
    for (const auto trx : pending_trxs) {
        mempool += trx;
    }
    bool mined = false;
    std::shared_ptr<Client> miner;
    size_t nonce = 0;
    while (!mined) {
        for (auto client : clients) {
            nonce = client.first->generate_nonce();
            auto tempstr = mempool + std::to_string(nonce);
            std::string hash { crypto::sha256(tempstr) };

            if (hash.substr(0, 10).find("000") != std::string::npos) {
                // std::cout << "expected: " << tempstr << std::endl;

                // std::cout << "expected: " << hash << std::endl;
                // std::cout << "expected1: " << nonce << std::endl;
                mined = true;
                miner = client.first;
                break;
            }
        }
    }
    // std::cout << "expected2: " << nonce << std::endl;

    clients[miner] += 6.25;
    for (auto trx : pending_trxs) {
        // std::cout << trx << std::endl;
        std::string sender {}, receiver {};
        double value;
        parse_trx(trx, sender, receiver, value);
        // std::cout << "sender: " << sender << " receiver: " << receiver << " value: " << value << std::endl;

        auto client1 = get_client(sender);
        auto client2 = get_client(receiver);
        clients[client1] -= value;
        clients[client2] += value;
    }
    return nonce;
}
