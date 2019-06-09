# ThreadTest

Here we have 2000 `BotSimulator`s running concurrently, each on its own thread. Each bot is waiting for a shared lock, then incrementing a global variable, then sleeping for some amount of time. Sleeping simu

In this case, The bot
