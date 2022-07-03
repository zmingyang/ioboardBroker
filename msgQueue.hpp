

#ifndef __MSG_QUEUE_H__
#define __MSG_QUEUE_H__

#include <iostream>           // std::cout
#include <thread>             // std::thread
#include <mutex>              // std::mutex, std::unique_lock
#include <condition_variable> // std::condition_variable
#include <queue>
#include <string>

class msgQueue
{
  public:
  void getMsg ( std::string &msg );

  void putMsg( std::string msg );

  private:
  std::mutex mtx;
  std::condition_variable cv;
  int ready = 0;

  std::queue<std::string> myqueue;
};

extern msgQueue ToIOBoardQueue;
extern msgQueue FromIOBoardQueue;

#endif
