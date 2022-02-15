#include <ProducerConsumer.h>
#include <pthread.h>
#include <iostream>
#include <fstream>
#include <ctime>
using namespace std;

// TODO: add BoundedBuffer, locks and any global variables here
BoundedBuffer b(10);
int psleeps, csleeps, totalItems;
int itemCount = 0;
pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m3 = PTHREAD_MUTEX_INITIALIZER;
ofstream myFile;

void InitProducerConsumer(int p, int c, int psleep, int csleep, int items) {
  // TODO: constructor to initialize variables declared
  //       also see instructions on the implementation
  psleeps = psleep;
  csleeps = csleep;
  totalItems = items;
  myFile.open("output.txt");
  pthread_t pThreads[p];
  pthread_t cThreads[c];
  int maximum = max(p, c);
  int nums[maximum];
  int i;
  for (i = 0; i < maximum; i++) {
    nums[i] = i;
  }
  for (i = 0; i < maximum; i++) {
    if (i < p)
      pthread_create(&pThreads[i], NULL, producer, &nums[i]);
    if (i < c)
      pthread_create(&cThreads[i], NULL, consumer, &nums[i]);
  }
  for (i = 0; i < maximum; i++) {
    if (i < p)
      pthread_join(pThreads[i], NULL);
    if (i < c)
      pthread_join(cThreads[i], NULL);
  }
  myFile.close();
  pthread_mutex_destroy(&m1);
  pthread_mutex_destroy(&m2);
  pthread_mutex_destroy(&m3);
}

void* producer(void* threadID) {
  // TODO: producer thread, see instruction for implementation
  // producer thread sits in a loop and in each iteration,
  // it first sleeps for a specified duration, then
  // produces a data item (the data item in this case is a integer with a random value),
  // and calls the add method of the bounded buffer to add the data item to the buffer.
  pthread_mutex_lock(&m2);
  while(itemCount < totalItems) {
    usleep(psleeps);
    int *id= (int*) threadID;
    int item = rand();
    b.append(item);
    itemCount++;
    pthread_mutex_unlock(&m2);
    pthread_mutex_lock(&m1);
    chrono::system_clock::time_point now = chrono::system_clock::now();
    time_t t = chrono::system_clock::to_time_t(now);
    myFile << "Producer #" << *id << ", time = " << ctime(&t) << ", producing data item #" << itemCount << ", item value=" << item << endl;
    pthread_mutex_unlock(&m1);
    pthread_mutex_lock(&m2);
  }
  pthread_mutex_unlock(&m2);
  pthread_exit(NULL);
}

void* consumer(void* threadID) {
  // TODO: consumer thread, see instruction for implementation
  // a consumer thread sits in a loop, and
  // first sleeps for a specified duration and then
  // calls the remove method of the bounded buffer to remove an item from the circular buffer.
  pthread_mutex_lock(&m3);
  while(itemCount < totalItems || !b.isEmpty()) {
    usleep(csleeps);
    int *id = (int*) threadID;
    int item = b.remove();
    pthread_mutex_unlock(&m3);
    pthread_mutex_lock(&m1);
    chrono::system_clock::time_point now = chrono::system_clock:: now();
    time_t t = chrono::system_clock::to_time_t(now);
    myFile << "Consumer #" << *id << ", time = " << ctime(&t) << ", consuming data item with value=" << item << endl;
    pthread_mutex_unlock(&m1);
    pthread_mutex_lock(&m3);
  }
  pthread_mutex_unlock(&m3);
  pthread_exit(NULL);
}