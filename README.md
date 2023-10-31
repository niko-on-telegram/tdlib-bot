## Example of a simple telegram echo-bot written using C++ and TDLib

### Deps(Ubuntu 22.04):
```bash
apt update && apt install git cmake g++ gperf zlib1g-dev libssl-dev
```

### Build:
```
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release && cmake --build build -j$(nproc)
```

### Run prerequisites:
Initialize `BOT_TOKEN` env variable with your Telegram bot token.
After that you can run tdlib_bot.