#include "ITimer.hpp"
#include <condition_variable>
#include <thread>
#include <queue>
#include <mutex>

class Timer: public ITimer{
public:
	Timer();
	~Timer();
	void registerTimer(const Timepoint &tp, const TTimerCallback &cb);
	void registerTimer(const Millisecs &period, const TTimerCallback &cb);
	void registerTimer(const Timepoint &tp, const Millisecs & period, const TTimerCallback &cb);
	void registerTimer(const TPredicate &pred, const Millisecs & period, const TTimerCallback &cb);
private:

	void timerThread();
	enum timerType {Timer1, Timer2, Timer3, Timer4};
	
	struct Data
	{
		timerType type;
		Timepoint timePoint;
		TPredicate predicate;
		Millisecs period;
		TTimerCallback callBack;
		Timepoint timeLeft;
		Data(){}
		Data(timerType typ, Timepoint t, TPredicate pr, Millisecs p, TTimerCallback callb);
	};

	std::queue<Data> timerQueue;
	std::thread t;
	bool terminate = false;
	std::mutex mtx;
	std::condition_variable cv;
};