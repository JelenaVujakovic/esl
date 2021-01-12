#include "Module.hpp"
#include <cstdlib>
#include <ctime>

using namespace sc_core;
using namespace std;

Module::Module(sc_core::sc_module_name name, sc_core::sc_mutex left_fork*, sc_core:sc_mutex right_fork*):left_fork(left_fork),right_fork(right_fork)
{
	srand(time(NULL));
 	SC_THREAD();
}

void Module::life()
{
	while(1)
	{
			int num = rand()%10;//razmislja na neodredjen period od 1-10 sec
			cout<<"filozof "<<name()<<"pocinje da razmislja"<<" @"<<sc_time_stamp()<<endl;
			wait(num,SC_SEC);		
			cout<<"filozof "<<name()<<"prestaje da razmislja"<<" @"<<sc_time_stamp()<<endl;

			if(!left_taken && !right_taken)//ako ni jedna nije uzeta, 50/50 koju ce pokusati uzeti 
				num=num%2;
			else if(!left_taken)//ako ljeva nije uzeta pokusaj da uzmes ljevu
				num=0;
			else if(!right_taken)//ako desna nije uzeta pokusaj da uzmes desnu
				num=1;	
			
			switch(num){
			case 0:
				cout<<"filozof "<<name()<<"pokusava da uzme ljevu viljusku"<<" @"<<sc_time_stamp()<<endl;
				if(left_fork->try_lock()!=-1)
					left_taken=true;
				if(left_taken)
					cout<<"filozof "<<name()<<"uspjeva da uzme ljevu viljusku"<<" @"<<sc_time_stamp()<<endl;
				else
					cout<<"filozof "<<name()<<"ne uspjeva da uzme ljevu viljusku"<<" @"<<sc_time_stamp()<<endl;
				break;	
			case 1:
				cout<<"filozof "<<name()<<"pokusava da uzme desnu viljusku"<<" @"<<sc_time_stamp()<<endl;
				if(right_fork->try_lock()!=-1)
					right_taken=true;
				if(right_taken)
					cout<<"filozof "<<name()<<"uspjeva da uzme desnu viljusku"<<" @"<<sc_time_stamp()<<endl;
				else
					cout<<"filozof "<<name()<<"ne uspjeva da uzme desnu viljusku"<<" @"<<sc_time_stamp()<<endl;
				break;	
			}
	
			if(right_taken && left_taken)//ako imas obje jedi na neodredjen period a zatim ih ostavi 
			{
				int eat=rand()%10;
				wait(eat,SC_SEC);
				left_fork->unlock();
				right_fork->unlock();
				left_taken=false;
				right_taken=false;
			}	
	
	}	
}

