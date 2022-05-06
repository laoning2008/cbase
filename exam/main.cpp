#include <base/memory/weak_ptr.h>
#include <string>
#include <iostream>
#include <future>
#include <thread>
#include <base/bind.h>
#include <base/callback_list.h>

using namespace base;

class Controller;
static std::thread t;

class Worker {
public:
    static base::RepeatingCallback<void(const std::string&)> callback_;
    static void StartNew(WeakPtr<Controller> controller);
    static void StartNew(base::RepeatingCallback<void(const std::string&)> callback);
private:
    Worker(WeakPtr<Controller> controller);
    void DidCompleteAsynchronousProcessing(const std::string& result);
    WeakPtr<Controller> controller_;
};

class Controller {
public:
    void SpawnWorker();
    void WorkComplete(const std::string& result);
private:
    // Member variables should appear before the WeakPtrFactory, to ensure
    // that any WeakPtrs to Controller are invalidated before its members
    // variable's destructors are executed, rendering them invalid.
    WeakPtrFactory<Controller> weak_factory_{ this };
};

base::RepeatingCallback<void(const std::string&)> Worker::callback_;

void Worker::StartNew(WeakPtr<Controller> controller) {
    Worker* worker = new Worker(std::move(controller));
    // Kick off asynchronous processing...
    t = std::thread([worker]() {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        worker->DidCompleteAsynchronousProcessing("task finished");
    });
}

void Worker::StartNew(base::RepeatingCallback<void(const std::string&)> callback)
{
    callback_ = callback;
    t = std::thread([]() {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        callback_.Run("hello weak callback");
    });
}

Worker::Worker(WeakPtr<Controller> controller)
    : controller_(std::move(controller)) 
{
}

void Worker::DidCompleteAsynchronousProcessing(const std::string& result) {
    if (controller_)
    {
        controller_->WorkComplete(result);
    }
}

void Controller::SpawnWorker()
{ 
    Worker::StartNew(base::BindRepeating(&Controller::WorkComplete, weak_factory_.GetWeakPtr()));
}

void Controller::WorkComplete(const std::string& result)
{
    std::cout << "result = " << result << std::endl;
}


class Ref : public base::RefCountedThreadSafe<Ref> {
public:
    int Foo() { return 3; }
};

//void callback(int* p)
//{
//    *p = 100;
//}

int main()
{
    //{
        Controller controller;
        controller.SpawnWorker();
    //}
    
    //{
    //    //scoped_refptr<Ref> ref = new Ref();
    //    int* p = new int;
    //  
    //    base::RepeatingCallback<void()> ref_cb = base::BindRepeating(callback, base::Owned(p));

    //    ref_cb.Run();
    //}

    t.join();
    return 0;
}