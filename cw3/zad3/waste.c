#include<stdio.h>
#include<stdlib.h>


int main(int args, char * argv[]){
    if(args<2){
        printf("Wrong arguments. Specify m - memory or c - cpu.");
        exit(1);
    }

    if(argv[1][0]=='m'){
        if (args != 3) { //argv[2] stores how much memory in MiB to waste.
            printf("Specify in second argument how much memory schould be wasted!");
            exit(1);
        }
        long int wastesize = ((long int) atoi(argv[2])) << 20;
        char *waste = malloc(wastesize * sizeof(char));
	waste[0]='#';
    } else if(argv[1][0]=='c') {
        for (int i=0; 1; i++);
    }
}
