#include <iostream>
#include <mutex>
#include <algorithm>
#include <thread>
#include <unistd.h>
#include <random>

using namespace std;

mt19937 createRandomEngine() {
  random_device rd;
  array<int, mt19937::state_size> seed_data;
  generate_n(seed_data.data(), seed_data.size(), ref(rd));
  seed_seq seq(begin(seed_data), end(seed_data));
  return mt19937(seq);
}

class BotSimulator {
private:
  int minSleep_{0}, maxSleep_{0};
  uniform_int_distribution<int> dist{minSleep_,maxSleep_};
  static int count;
  static mutex countMutex;
  static mt19937 eng;
  static mutex engMutex;
  static mutex printMutex;
  template<int MinSleepUSec=0, int MaxSleepUSec=0>
  void sleep() {
    int sleep;
    {
      unique_lock<mutex> engLockGuard(engMutex);
      sleep = dist(eng);
    }
    usleep(sleep);
  }
public:
  BotSimulator() = default;
  BotSimulator(int minSleep, int maxSleep) : minSleep_(minSleep), maxSleep_(maxSleep) {}
  template<int WorkCycles>
  void work() {
    volatile int n=0;
    for (int i=0; i<WorkCycles; ++i) {
      ++n;
    }
    n=1;
  }
  template<int WorkCycles=100, int MinSleepUSec=0, int MaxSleepUSec=0>
  void run() {
    while (true) {
      {
        // Do some work here
        work<WorkCycles>();
        // Work task is to increment `count`
        unique_lock<mutex> countLockGuard(countMutex);
        ++count;
      }
      // Every 1000000, print an update
      if (count % 10000 == 0) {
        unique_lock<mutex> printLockGuard(printMutex);
        cout << MinSleepUSec << ',' << MaxSleepUSec << ',' << count << '\n';
      }
      sleep<MinSleepUSec,MaxSleepUSec>();
    }
  }
};

template<>
void BotSimulator::sleep<0,0>() {
  // No sleep
}

int BotSimulator::count{0};
mt19937 BotSimulator::eng{createRandomEngine()};
mutex BotSimulator::countMutex;
mutex BotSimulator::engMutex;
mutex BotSimulator::printMutex;

int main() {
  /// kSimCount             Number of bots to simulate
  /// kMinSleepTimeUSec     Minimum bot "sleep" time in microseconds
  /// kMaxSleepTimeUSec     Maximum bot "sleep" time in microseconds
  /// botSims               Collection of bot simulators
  /// botThreads            Collection of threads for each bot simulator's work
  const int kSimCount{2000};
  constexpr int kWorkCycles{10000};
  constexpr int kMinSleepTimeUSec{100};
  constexpr int kMaxSleepTimeUSec{1000};
  vector<BotSimulator> botSims(kSimCount);
  vector<thread> botThreads;
  botThreads.reserve(kSimCount);
  for (int i=0; i<kSimCount; ++i) {
    botThreads.emplace_back(&BotSimulator::run<kWorkCycles, kMinSleepTimeUSec, kMaxSleepTimeUSec>, botSims[i]);
  }
  for (auto &thr : botThreads) {
    thr.join();
  }
  return 0;
}