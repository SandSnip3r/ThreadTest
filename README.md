# ThreadTest

Here we have 2000 `BotSimulator`s running concurrently, each on its own thread.
They have some simulated contention on a couple shared resources in the form of a counter and printing. The amount of work they do before contention is configurable as `kWorkCycles`.
Between contention times, bot's sleep a random amount of time on a configurable interval (`kMinSleepTimeUSec`,`kMaxSleepTimeUSec`) to simulate a lack of packet data coming from the server.

### My results

If we take a conservative approach and assume that packets will never come any sooner than 100us(`kMinSleepTimeUSec=100`) apart or any longer than 1ms(`kMaxSleepTimeUSec=1000`) apart. Also, we'll assume there's a lot of data to be crunched before contention, so we'll set `kWorkCycles=10000`.

My CPU spikes and things get laggy, but the after 10 seconds, the console reports that the completed number of units of work was 7,030,003 meaning collaboratively between all 2000 bots, 703,000 units of work were done per second.

## Build

Build in linux with `g++ -o sim sim.cpp -std=c++14 -pthread -O3`
