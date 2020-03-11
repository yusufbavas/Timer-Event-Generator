#include <iostream>
#include <string>
#include "Timer.hpp"

void Test(){
	std::cout<<"Test"<<std::endl;
}
void Test2(){
	std::cout<<"Test2"<<std::endl;
}
void Test3(){
	std::cout<<"Test3"<<std::endl;
}

int main(int argc, char const *argv[]){
	
	Timepoint tp = CLOCK::now() + std::chrono::seconds(2);
	Timepoint tp2 = CLOCK::now() + std::chrono::seconds(1);
	Timepoint final = CLOCK::now() + std::chrono::seconds(5);
	
	Millisecs ms = Millisecs (1000);
	Millisecs ms2 = Millisecs (2000);

	Timer t;
	
	std::function<void()> f= Test;
	std::function<void()> f2 = Test2;
	std::function<void()> f3 = Test3;

	t.registerTimer(tp,f);
	t.registerTimer(final,ms,f3);

	t.registerTimer(tp2,f2);
	t.registerTimer(tp2,f);
	t.registerTimer(tp,f2);

	t.registerTimer(ms,f);
	t.registerTimer(ms2,f2);
	
	//while (1);
	return 0;
}