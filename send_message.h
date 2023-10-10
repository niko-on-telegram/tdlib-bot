#pragma once

#include "CommonTdlib.h"
#include <spdlog/spdlog.h>

void send_message(std::string text, td_api::int53 chat_id, td::ClientManager *clientManager, std::int32_t client_id, std::uint64_t query_id){
    auto send_message_request = td_api::make_object<td_api::sendMessage>();
    send_message_request->chat_id_ = chat_id;
    auto message_content = td_api::make_object<td_api::inputMessageText>();
    message_content->text_ = td_api::make_object<td_api::formattedText>();
    message_content->text_->text_ = std::move(text);
    send_message_request->input_message_content_ = std::move(message_content);

    clientManager->send(client_id, query_id, std::move(send_message_request));
    spdlog::info("Message sent");
}
