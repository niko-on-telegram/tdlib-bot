#pragma once

#include "CommonTdlib.h"
#include <iostream>


bool on_authorization_state_update(td_api::object_ptr<td::td_api::AuthorizationState> &state,
                                   td::ClientManager *clientManager, std::int32_t client_id, std::uint64_t query_id) {
    bool auth = false;
    td_api::downcast_call(*state,
                          overloaded(
                                  [&auth](td_api::authorizationStateReady &) {
                                      std::cout << "Authorization is completed" << std::endl;
                                      auth = true;
                                  },
                                  [](td_api::authorizationStateLoggingOut &) {
//                                      are_authorized_ = false;
                                      std::cout << "Logging out" << std::endl;
                                  },
                                  [](td_api::authorizationStateClosing &) { std::cout << "Closing" << std::endl; },
                                  [](td_api::authorizationStateClosed &) {
//                                      are_authorized_ = false;
//                                      need_restart_ = true;
                                      std::cout << "Terminated" << std::endl;
                                  },
                                  [&](td_api::authorizationStateWaitPhoneNumber &) {
//                                      TODO: checkAuthenticationBotToken
//                                      send_query(
//                                              td_api::make_object<td_api::setAuthenticationPhoneNumber>(phone_number, nullptr),
//                                              create_authentication_query_handler());
                                      auto bot_auth_request = td_api::make_object<td_api::checkAuthenticationBotToken>(
                                              "1199280588:AAEgfNYqicjQiT6jDhKNIqdp-2ujiRcTzzI");
                                      clientManager->send(client_id, query_id, std::move(bot_auth_request));
                                  },
                                  [](td_api::authorizationStateWaitOtherDeviceConfirmation &state) {
                                      std::cout << "Confirm this login link on another device: " << state.link_ << std::endl;
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
                                      clientManager->send(client_id, query_id, std::move(request));
                                  },
                                  [](auto &){
                                      std::cout << "others";
                                  }));
    return auth;
}

