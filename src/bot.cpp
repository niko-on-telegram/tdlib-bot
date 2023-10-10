#include "bot.h"

#include <spdlog/spdlog.h>

namespace {
    template<class... Ts>
    struct overloaded : Ts... { using Ts::operator()...; };

    template<class... Ts>
    overloaded(Ts...) -> overloaded<Ts...>;
}

Bot::Bot(std::string bot_token) :
        client_manager{td::ClientManager::get_manager_singleton()},
        client_id{client_manager->create_client_id()},
        bot_token{std::move(bot_token)}{

    spdlog::info("Client id: {} | Starting", client_id);
    client_manager->send(client_id, query_id++, td_api::make_object<td_api::getOption>("version"));
}

void Bot::run_loop() {
    while (!stop_requested) {
        spdlog::info("Checking updates...");
        auto response = client_manager->receive(10);
        if (!response.object) {
            spdlog::debug("Skipping empty obj");
            continue;
        }
        spdlog::info("request_id: {} | Object: {}", response.request_id, to_string(response.object));
        if (response.request_id == 0) {
            process_update(std::move(response.object));
        }
    }
}

void Bot::process_update(td_api::object_ptr<td_api::Object> update) {
    td_api::downcast_call(
            *update, overloaded(
                    [&](td_api::updateAuthorizationState &update_authorization_state) {
                        auto authorization_state = std::move(update_authorization_state.authorization_state_);
                        on_authorization_state_update(authorization_state);
                    },
                    [&](td_api::updateNewMessage &update_new_message) {
                        auto chat_id = update_new_message.message_->chat_id_;

                        std::string text;
                        if (update_new_message.message_->content_->get_id() == td_api::messageText::ID) {
                            text = static_cast<td_api::messageText &>(*update_new_message.message_->content_).text_->text_; // NOLINT(*-pro-type-static-cast-downcast)
                            if (text == "exit"){
                                stop_requested = true;
                                return;
                            }
                            send_message(text, chat_id);
                        }
                        spdlog::info("Received message: [chat_id:{}][text:{}]", chat_id, text);
                    },
                    [](auto&) {}));
}

void Bot::on_authorization_state_update(td_api::object_ptr<td::td_api::AuthorizationState> &state) {
    td_api::downcast_call(*state,
                          overloaded(
                                  [&](td_api::authorizationStateReady &) {
                                      spdlog::info("Authorization is completed");
                                  },
                                  [&](td_api::authorizationStateWaitPhoneNumber &) {
                                      auto bot_auth_request = td_api::make_object<td_api::checkAuthenticationBotToken>(
                                              bot_token);
                                      client_manager->send(client_id, query_id++, std::move(bot_auth_request));
                                  },
                                  [&](td_api::authorizationStateWaitTdlibParameters &) {
                                      auto request = td_api::make_object<td_api::setTdlibParameters>();
                                      request->database_directory_ = "tdlib";
                                      request->use_message_database_ = false;
                                      request->use_secret_chats_ = false;
                                      request->api_id_ = 94575;
                                      request->api_hash_ = "a3406de8d171bb422bb6ddf3bbd800e2";
                                      request->system_language_code_ = "en";
                                      request->device_model_ = "Desktop";
                                      request->application_version_ = "1.0";
                                      request->enable_storage_optimizer_ = true;
                                      client_manager->send(client_id, query_id++, std::move(request));
                                  },
                                  [](auto &){
                                      spdlog::warn("others");
                                  }));
}

void Bot::send_message(std::string text, td_api::int53 chat_id){
    auto send_message_request = td_api::make_object<td_api::sendMessage>();
    send_message_request->chat_id_ = chat_id;
    auto message_content = td_api::make_object<td_api::inputMessageText>();
    message_content->text_ = td_api::make_object<td_api::formattedText>();
    message_content->text_->text_ = std::move(text);
    send_message_request->input_message_content_ = std::move(message_content);

    client_manager->send(client_id, query_id++, std::move(send_message_request));
    spdlog::info("Message sent");
}
