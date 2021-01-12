#include "lotto.hpp"
#include <iostream>

using namespace sc_core;
using namespace std;

lotto::lotto(sc_core::sc_module_name name)
{
	SC_THREAD(lotto_combination);
	/*SC_THREAD(first_player); 
	SC_THREAD(second_player); 
	SC_THREAD(third_player); 
	SC_THREAD(print_winner);*/ 
}

void lotto::lotto_combination()
{
	while(1)
    {
        a.push_back(rand() % 39 + 1);
 
                while(1)
                {
                        a.push_back(rand() % 39 + 1);
 
                        for(int i = 0; i < a.size(); ++i)
                        {
                            if((a[i] == a[a.size()-1]) && (i != a.size()-1))
                            {
                                a.pop_back();
                                break;
                            }
                        }
 
                        if(a.size() == 7)
                            break;
                }
                
        sort(a.begin(), a.end());
 
        for(int i = 0; i < 7; ++i)
            cout<<a[i]<<" ";
        cout<<endl;    
        
        
         while(a.size() != 0)
            a.pop_back(); 
            
        wait(1, SC_SEC);   
    }
}

