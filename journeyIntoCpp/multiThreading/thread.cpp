// Bo Qian mutlithreading 

#include <thread>
#include <string>
#include <mutex>
#include <iostream>
#include <fstream>
#include <future>
using namespace std;

/*
std::deque<int> q;
std::mutex mu;
std::condition_variable cond;

void function_1 () {
  int count = 10;
  while (count > 0) {
    std::unique_lock<mutex> locker(mu);
    q.push_front(count);
    locker.unlock();
    cond.notify_one(); //wakes up waiting threads
    std::this_thread::sleep_for(chrono::seconds(1));
    count--;
  }
}

void function_2() {
  int data = 0;
  while (data != 1) {
    std::unique_lock<mutex> locker(mu);
    cond.wait(locker, [](){return !q.empty();}); //spurious wake
    data = q.back();
    q.pop_back();
    locker.unlock();
    cout << "t2 got a value from t1: " << data << endl;
  }
}
*/

/*
int factorial(int N) {
  int res = 1;
  for (int i=N; i>1; i--){
    res *= i;
  }
  cout << "Result is: " << res << endl;
  return res;
}

int main () {
  // thread
  std::thread t1(factorial, 6); // create a thread object
  std::this_thread::sleep_for(chrono::milliseconds(3));
  chrono::steady_clock::time_point tp = chrono::stead_clock::now() + chrono::microseconds(4);
  std::this_thread::sleep_until(tp);

  // mutex
  std::mutex mu; // create a mutex obj to sync data access
  // don't use mu.lock() or unlock() directly
  std::lock_guard<mutex> locker(mu); // instead use this 
  std::unique_lock<mutex> ulocker(mu); //extra flexibility
  // ulock can lock and unlock a mutex multiple times and can transfer ownership of mutex from one ulock to another
  ulocker.try_lock();
  ulocker.try_lock_for(chrono::nanseconds(500));
  ulocker.try_lock_until(tp);

  // condition var
  std::condition_variable cond; // to sync the execution order of threads
  cond.wiat_for(chrono::microseconds(2));
  cond.wait_until(ulocker, tp);

  // Future and promise
  std::promise<int> p; // 
  std::future<int> f = p.get_future(); // f can be used to fetch the var
  // sent over by p
  f.get();
  f.wait();
  f.wait_for(chrono::milliseconds(2));
  f.wait_until(tp);

  // async
  std::future<int> fu = async(factorial, 6); // another way to spawn a
  //thread, returns a future
  // can either a child thread to run the factorial fcn or run it in the
  // same thread

  // packaged task
  std::packaged_task<int(int)> t(factorial); // class template
  // that is parameterized with the fucntion signature
  std::future<int> fu2 = t.get_future();
  t(6);
}
Avoiding Data Race:
1. Use mutex to synchronize data access
2. Never leak a handle of data to outside
3. Design you interface appropriately
*/

/* Avoiding deadlock
1. Prefer locking single mutex
2. avoid locking a mutex and then calling a user provided function
3. USe std::lock() to lock more than one mutex
4. Lock the mutex in the same order

Locking Granularity:
- Fine-grained lock: protects small amount of data
     too fine -> programming tricky more exposed to deadlock
- Coarse-grained lock: protects big amount of data
     too coarse lose out of parallel computing because things competing for resources.

Ways of getting a future:
- promise::get_future()
- packaged_task::get_future()
- async() returns a future
*/
