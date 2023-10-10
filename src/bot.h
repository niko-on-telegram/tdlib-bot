#pragma once

#include <memory>

#include <td/telegram/Client.h>
#include <td/telegram/td_api.h>

using ClientId = td::ClientManager::ClientId;
namespace td_api = td::td_api;

class Bot {
public:
    Bot(std::string bot_token);
    void run_loop();

private:
    void process_update(td_api::object_ptr<td_api::Object> update);
    void on_authorization_state_update(td_api::object_ptr<td::td_api::AuthorizationState> &state);
    void send_message(std::string text, td_api::int53 chat_id);

    td::ClientManager *client_manager;
    ClientId client_id;
    std::uint64_t query_id{0};
    std::string bot_token;
    bool stop_requested{false};
};
