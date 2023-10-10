#include <iostream>


#include <td/telegram/Client.h>
#include <td/telegram/td_api.h>
#include <functional>
#include <sstream>
#include "auth_flow.h"
#include "send_message.h"
#include <spdlog/spdlog.h>

bool process_update(td_api::object_ptr<td_api::Object> update, td::ClientManager *clientManager, std::int32_t client_id,
                    std::uint64_t query_id) {
    bool auth = false;
    td_api::downcast_call(
            *update, overloaded(
                    [&](td_api::updateAuthorizationState &update_authorization_state) {
                        auto authorization_state_ = std::move(update_authorization_state.authorization_state_);
                        auth = on_authorization_state_update(authorization_state_, clientManager, client_id, query_id);
                    },
                    [&](td_api::updateNewMessage &update_new_message) {
                        auto chat_id = update_new_message.message_->chat_id_;

                        std::string text;
                        if (update_new_message.message_->content_->get_id() == td_api::messageText::ID) {
                            text = static_cast<td_api::messageText &>(*update_new_message.message_->content_).text_->text_; // NOLINT(*-pro-type-static-cast-downcast)
                        }
//                        std::cout << "Receive message: [chat_id:" << chat_id << "] ["
//                                  << text << "]" << std::endl;
                        send_message(text, chat_id, clientManager, client_id, query_id);
                    },
                    [](auto &update) {}));
    return auth;
}

int main() {
    td::ClientManager::execute(td_api::make_object<td_api::setLogVerbosityLevel>(1));

    std::unique_ptr<td::ClientManager> client_manager = std::make_unique<td::ClientManager>();
    std::int32_t client_id = client_manager->create_client_id();
    std::uint64_t query_id{0};

    td_api::object_ptr<td_api::AuthorizationState> authorization_state_;
    bool are_authorized{false};
    client_manager->send(client_id, query_id++, td_api::make_object<td_api::getOption>("version"));

    while (true) {
        if (!are_authorized) {
            auto response = client_manager->receive(10);
            if (!response.object) {
                std::cout << "Skipping empty obj";
                continue;
            }
            std::cout << response.request_id << " " << to_string(response.object) << std::endl;
            if (response.request_id == 0) {
                are_authorized = process_update(std::move(response.object), client_manager.get(), client_id, ++query_id);
            }
        } else {
//            std::cout << "Enter action [q] quit [u] check for updates and request results [c] show chats [m <chat_id> "
//                         "<text>] send message [me] show self [l] logout: "
//                      << std::endl;
//            std::string line;
//            std::getline(std::cin, line);
//            std::istringstream ss(line);
//            std::string action;
//            if (!(ss >> action)) {
//                continue;
//            }
//            if (action == "q") {
//                break;
//            }
//            if (action == "u") {
//                std::cout << "Checking for updates..." << std::endl;
            while (true) {
                spdlog::info("Checking updates...");

                auto response = client_manager->receive(10);

                if (response.object) {
                    process_update(std::move(response.object), client_manager.get(), client_id, ++query_id);
                } else {
                    break;
                }
            }
//            }
//            } else if (action == "close") {
//                std::cout << "Closing..." << std::endl;
//                send_query(td_api::make_object<td_api::close>(), {});
//            } else if (action == "me") {
//                send_query(td_api::make_object<td_api::getMe>(),
//                           [this](Object object) { std::cout << to_string(object) << std::endl; });
//            } else if (action == "l") {
//                std::cout << "Logging out..." << std::endl;
//                send_query(td_api::make_object<td_api::logOut>(), {});
//            } else if (action == "m") {
//                std::int64_t chat_id;
//                ss >> chat_id;
//                ss.get();
//                std::string text;
//                std::getline(ss, text);
//
//                std::cout << "Sending message to chat " << chat_id << "..." << std::endl;
//                auto send_message = td_api::make_object<td_api::sendMessage>();
//                send_message->chat_id_ = chat_id;
//                auto message_content = td_api::make_object<td_api::inputMessageText>();
//                message_content->text_ = td_api::make_object<td_api::formattedText>();
//                message_content->text_->text_ = std::move(text);
//                send_message->input_message_content_ = std::move(message_content);
//
//                send_query(std::move(send_message), {});
//            } else if (action == "c") {
//                std::cout << "Loading chat list..." << std::endl;
//                send_query(td_api::make_object<td_api::getChats>(nullptr, 20), [this](Object object) {
//                    if (object->get_id() == td_api::error::ID) {
//                        return;
//                    }
//                    auto chats = td::move_tl_object_as<td_api::chats>(object);
//                    for (auto chat_id : chats->chat_ids_) {
//                        std::cout << "[chat_id:" << chat_id << "] [title:" << chat_title_[chat_id] << "]" << std::endl;
//                    }
//                });
//            }
        }
    }


    return 0;
}
