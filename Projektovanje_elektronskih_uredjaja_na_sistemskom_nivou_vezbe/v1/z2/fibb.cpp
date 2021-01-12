#define SC_INCLUDE_FX
#define MAX_SIZE 64
#include <systemc>
#include <iostream>
#include <deque>
#include <vector>
#include <cmath>
#include <string>

int sc_main(int argc, char* argv[]){
	
	int n, f[MAX_SIZE];
	int i, out;
	f[0]=0;
	f[1]=1;
	n = std::stoi(argv[1]);
	if(n == 0){
		out = 0;
	}
	else if(n == 1){
		out = 1;
	}
	if(n > 1 && n < MAX_SIZE){
		
		for(i=2; i<=n; i++){
			f[i] = f[i-1] + f[i-2];	
		}
		out = f[n];
		
	}
	
	std::cout << out << std::endl;	
	return 0;
}

