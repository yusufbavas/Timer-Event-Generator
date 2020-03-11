#include "Timer.hpp"

#include <iostream>

Timer::Timer(){
	t = std::thread(&Timer::timerThread,this); 
}

Timer::~Timer(){
	terminate = true;
	if (t.joinable())
		t.join();
}
void Timer::timerThread(){

	std::unique_lock<std::mutex> lck(mtx);
	Data task;
	double delay = 0;

	while(true){
		while(timerQueue.empty()){
			cv.wait_for(lck,std::chrono::seconds(1));
			if(terminate)
				return;
		}
		
		task = timerQueue.front();
		timerQueue.pop();
		lck.unlock();
		
		if(task.type == Timer1){
			delay = std::chrono::duration_cast<std::chrono::duration<double,std::milli>>(task.timePoint - CLOCK::now()).count();
			if (delay <= 0)
				task.callBack();
			
			else
				timerQueue.push(task);	
		}
		else if(task.type == Timer2){
			delay = std::chrono::duration_cast<std::chrono::duration<double,std::milli>>(task.timeLeft - CLOCK::now()).count();
			if (delay <= 0)
			{
				task.timeLeft = CLOCK::now() + task.period;
				task.callBack();
			}	
			timerQueue.push(task);
		}
		else if(task.type == Timer3){
			if (std::chrono::duration_cast<std::chrono::duration<double>>(task.timePoint - CLOCK::now()).count() > 0)
			{
				delay = std::chrono::duration_cast<std::chrono::duration<double,std::milli>>(task.timeLeft - CLOCK::now()).count();
				if (delay <= 0)
				{
					task.timeLeft= CLOCK::now() + task.period;
					task.callBack();
				}	
				timerQueue.push(task);
			}
		}
		else{
			delay = std::chrono::duration_cast<std::chrono::duration<double,std::milli>>(task.timeLeft - CLOCK::now()).count();
			if (delay <= 0 && task.predicate())
			{
				task.timeLeft =  CLOCK::now() + task.period;
				task.callBack();
			}
			if(delay > 0 || task.predicate())
				timerQueue.push(task);
		}
		if (delay <= -5){
			std::cout<<"Deadline Error!!"<<std::endl;
			delay = 0;
		}
		lck.lock();
	}

	
}

void Timer::registerTimer(const Timepoint &tp, const TTimerCallback &cb){
	std::unique_lock<std::mutex> lck(mtx);
	Timer::Data data(timerType::Timer1,tp,[]() -> bool {return false;} ,Millisecs(-1),cb);
	timerQueue.push(data);
	cv.notify_one();
}

void Timer::registerTimer(const Millisecs &period, const TTimerCallback &cb){
	std::unique_lock<std::mutex> lck(mtx);
	Timer::Data data(timerType::Timer2,CLOCK::now(),[]() -> bool {return false;} ,period,cb);
	timerQueue.push(data);
	cv.notify_one();
}

void Timer::registerTimer(const Timepoint &tp, const Millisecs & period, const TTimerCallback &cb){
	std::unique_lock<std::mutex> lck(mtx);
	Timer::Data data(timerType::Timer3,tp,[]() -> bool {return false;} ,period,cb);
	timerQueue.push(data);
	cv.notify_one();
}

void Timer::registerTimer(const TPredicate &pred, const Millisecs & period, const TTimerCallback &cb){
	std::unique_lock<std::mutex> lck(mtx);
	Timer::Data data(timerType::Timer4,CLOCK::now(),pred ,period,cb);
	timerQueue.push(data);
	cv.notify_one();
}
Timer::Data::Data(timerType typ, Timepoint t, TPredicate pr, Millisecs p, TTimerCallback callb){
	type = typ;
	timePoint = t;
	predicate = pr;
	period = p;
	callBack = callb;
	timeLeft = CLOCK::now();
}