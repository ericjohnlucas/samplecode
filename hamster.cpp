#include<iostream>
#include<string>
#include<vector>
#include<functional>
#include<pthread.h>
#include<fstream>
#include<atomic>

using namespace std;
//using namespace std::tr1;

typedef vector<string> stringlist;


stringlist split(string str, string arg) {
       stringlist ret;

       int strlen = arg.length();
       int begin = 0;
       int end = 0;
       int e = 0;

       while((end = str.find(arg, begin + 1)) != string::npos) {
               ret.push_back(str.substr(begin + e, end - (begin + e)));
               begin = end + strlen - 1;
               e = strlen;
       }

       ret.push_back(str.substr(begin + 1));

       return ret;
}

class thread {
public:
       thread(function<void ()> cbk) {
               threadfunc = cbk;
               pthread_create(&thr, NULL, run, this);
       }

protected:
       static void* run(void* data) {
               thread* inst = (thread*)data;
               inst->threadfunc();
               return NULL;
       }

       function< void ()> threadfunc;
       pthread_t thr;
};

class mutex {
public:
       mutex() {
               pthread_mutex_init(&mut, NULL);
       }

       void lock() {
               pthread_mutex_lock(&mut);
       }

       void unlock() {
               pthread_mutex_unlock(&mut);
       }

       bool try_lock() {
               return pthread_mutex_trylock(&mut) == 0;
       }

protected:
       mutex(const mutex& other) {}
       pthread_mutex_t mut;
};

//this is provided to sync output to cout so it does not
//get printed to the terminal all jumbled
mutex coutlock;

template <class T> class unique_lock {
public:
       unique_lock(T& _mut) : mut(_mut) {
               mut.lock();
       }

       ~unique_lock() {
               mut.unlock();
       }

protected:
       T& mut;
};

class Bowl {
public:
       Bowl() {
               food = 0;
       }


       bool eat() {
               unique_lock<mutex> locker(lock);
               if(food > 0) {
                       food--;
                       return true;
               }

               return false;
       }

       void refill(int amount) {
               unique_lock<mutex> locker(lock);
               food += amount;
       }


protected:
       int food;
       mutex lock;
};

class BabyHamester {
public:
       BabyHamester(const BabyHamester& other) : bowl(other.bowl) {
               id = other.id;
               think_time = other.think_time;
               eat_time = other .eat_time;
       }

       BabyHamester(Bowl& _bowl, int _id, int _tt, int _et) : bowl(_bowl) {
               id = _id;
               think_time = _tt;
               eat_time = _et;
       }

       BabyHamester& operator=(const BabyHamester& other) {
               id = other.id;
               think_time = other.think_time;
               eat_time = other.eat_time;
               bowl = other.bowl;
       }

       void start() {
               thr = new thread(bind(&BabyHamester::exec, this));
       }

       void lock() {
               babylock.lock();
       }

       void unlock() {
               babylock.unlock();
       }

protected:
       void exec() {
               while(true) {
                       if(bowl.eat()) {
                               unique_lock<mutex> locker(babylock);
                               coutlock.lock();
                               cout<<"Baby (id=" <<id <<") eating, process_time=" <<eat_time <<endl;
                               coutlock.unlock();
                               sleep(eat_time);
                       }
                       coutlock.lock();
                       cout<<"Baby (id=" <<id <<") thinking, process_time=" <<think_time <<endl;
                       coutlock.unlock();
                       sleep(think_time);
               }
       }

       int id;
       int think_time;
       int eat_time;
       mutex babylock;
       thread* thr;

       Bowl& bowl;
};

class ParentHamester {
public:
       ParentHamester(const ParentHamester& other) : babies(other.babies), bowl(other.bowl) {
               id = other.id;
               think_time = other.think_time;
               refill_time = other.refill_time;
       }

       ParentHamester(Bowl& _bowl, vector<BabyHamester>& bab, int _id, int _tt, int _rt) : babies(bab), bowl(_bowl) {
               id = _id;
               think_time = _tt;
               refill_time = _rt;
       }

       ParentHamester& operator=(const ParentHamester& other) {
               id = other.id;
               think_time = other.think_time;
               refill_time = other.refill_time;
               bowl = other.bowl;
       }

       void start() {
               thr = new thread(bind(&ParentHamester::exec, this));
       }


protected:
       void exec() {
               while(true) {
                       //acquire all the locks for the babies
                       for(int i = 0; i < babies.size(); i++) {
                               babies[i].lock();
                       }

                       coutlock.lock();
                       cout<<"Parent (id=" <<id <<"), refilling, process_time=" <<refill_time <<endl;
                       coutlock.unlock();
                       bowl.refill(babies.size());
                       sleep(refill_time);

                       //done refilling the bowl release all the locks for the babies
                       for(int i = 0; i < babies.size(); i++) {
                               babies[i].unlock();
                       }

                       coutlock.lock();
                       cout<<"Parent (id=" <<id <<"), thinking, process_time=" <<think_time <<endl;
                       coutlock.unlock();
                       sleep(think_time);
               }
       }

       int id;
       int think_time;
       int refill_time;
       vector<BabyHamester>& babies;
       Bowl& bowl;
       thread* thr;
};

int main(int argc, char *argv[]) {
       Bowl bowl;
       vector<ParentHamester> parents;
       vector<BabyHamester> babies;

       //open the file and load the data
       ifstream data("assignment1_data/input1.txt");
       if(!data.is_open()) {
               cerr<<"could not open the file" <<endl;
               return 0;
       }

       //load the configuration file
       string line;
       while(getline(data, line), !data.eof()) {
               if(line.empty()) continue;

               stringlist args = split(line, " ");

               int id = atoi(args[0].c_str());
               bool parent = args[1][0] == 'w' ? 1 : 0;
               int arga = atoi(args[2].c_str());
               int argb = atoi(args[3].c_str());

               if(parent) {
                       parents.push_back(ParentHamester(bowl, babies, id, arga, argb));
               } else {
                       babies.push_back(BabyHamester(bowl, id, arga, argb));
               }
       }

       //bowl starts empty so start the parents first
       for(unsigned i = 0; i < parents.size(); i++) {
               parents[i].start();
       }

       //now start the babies
       for(unsigned i = 0; i < babies.size(); i++) {
               babies[i].start();
       }

       //hit enter to end the program
       cin.get();
       return 0;
}
