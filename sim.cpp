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
public:
  BotSimulator() = default;
  BotSimulator(int minSleep, int maxSleep) : minSleep_(minSleep), maxSleep_(maxSleep) {}
  template<int MinSleepUSec=0, int MaxSleepUSec=0>
  void run() {
    while (true) {
      {
        unique_lock<mutex> countLockGuard(countMutex);
        ++count;
      }
      if (count % 1000 == 0) {
        unique_lock<mutex> printLockGuard(printMutex);
        cout << MinSleepUSec << ',' << MaxSleepUSec << ',' << count << '\n';
      }
      int sleep;
      {
        unique_lock<mutex> engLockGuard(engMutex);
        sleep = dist(eng);
      }
      usleep(sleep);
    }
  }
};

template<>
void BotSimulator::run<0,0>() {
  while (true) {
    {
      unique_lock<mutex> countLockGuard(countMutex);
      ++count;
    }
    if (count % 1000000 == 0) {
      unique_lock<mutex> printLockGuard(printMutex);
      cout << "0,0," << count << '\n';
    }
  }
}

int BotSimulator::count{0};
mt19937 BotSimulator::eng{createRandomEngine()};
mutex BotSimulator::countMutex;
mutex BotSimulator::engMutex;
mutex BotSimulator::printMutex;

int main() {
  /// kSimCount     number of bots to simulate
  const int kSimCount{2000};
  vector<BotSimulator> botSims(kSimCount);
  vector<thread> botThreads;
  botThreads.reserve(kSimCount);
  for (int i=0; i<kSimCount; ++i) {
    botThreads.emplace_back(&BotSimulator::run<1000, 10000>, botSims[i]);
  }
  for (auto &thr : botThreads) {
    thr.join();
  }
  return 0;
}