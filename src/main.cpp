#include "bot.h"

#include <spdlog/spdlog.h>

#include <cstdlib>
#include <optional>

static std::optional<std::string> getEnv(std::string_view var)
{
    if (const char* val = std::getenv(var.data()); val != nullptr)
    {
        return {val};
    }
    return {};
}

int main() {
    auto opt_token = getEnv("BOT_TOKEN");
    if (!opt_token){
        spdlog::error("BOT_TOKEN env var required!");
        return -1;
    }

    td::ClientManager::execute(td_api::make_object<td_api::setLogVerbosityLevel>(1));
    Bot bot{*opt_token};
    bot.run_loop();
    return 0;
}
