#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <zconf.h>
#include <fcntl.h>
#include "timeMeasure.h"
//test
struct RecArr{
    int arraySize;
    int recordSize;
    char ** val;
};

struct RecArr createRecords(int arraySize,int recordSize){
    struct RecArr arr;
    arr.arraySize = arraySize;
    arr.recordSize = recordSize;
    char ** arrPtr = malloc(sizeof(*arrPtr)*arraySize);
    if(arrPtr){
        for(int i=0;i<arraySize;i++){
            arrPtr[i]= malloc(sizeof *arrPtr[i] * recordSize);
        }
    }
    arr.val=arrPtr;
    return arr;
}

void fillRecords(struct RecArr * arr){
    srand(time(NULL));
    for(int i=0;i<arr->arraySize;i++){
        for(int j=0;j<arr->recordSize;j++){
            arr->val[i][j]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"[rand() % 52];
        }
    }
}

void removeRecords(struct RecArr * a){
    for(int i=0;i<a->arraySize;i++){
        free(a->val[i]);
        a->val[i]=NULL;
    }
    free(a->val);
    a->val=NULL;
}

void printRecords(struct RecArr arr){
    for(int i=0;i<arr.arraySize;i++){
        for(int j=0;j<arr.recordSize;j++){
            printf("%c\t",arr.val[i][j]);
        }
        printf("\n");
    }
}

void swapRecords(struct RecArr * arr, int a, int b){
    char * hld = arr->val[a];
    arr->val[a] = arr->val[b];
    arr->val[b] = hld;
}

void sortRecords(struct RecArr * arr){
    for(int i=0;i<arr->arraySize;i++){
        for(int j=1;j<arr->arraySize;j++){
            if(*arr->val[j-1]>*arr->val[j]){
                swapRecords(arr,j-1,j);
            }
        }
    }
}

void saveFile(char* fname,struct RecArr * a){
    FILE *fp;
    fp = fopen(fname,"w");
    for(int i=0;i<a->arraySize;i++) {
        fwrite(a->val[i], 1, a->arraySize, fp);
    }
    fclose(fp);
}

void generate(char* fname,int as, int rs){
    struct RecArr a;
    a = createRecords(as,rs);
    fillRecords(&a);
    saveFile(fname,&a);
    removeRecords(&a);
}

void sortLib(char* fname,int as, int rs){
    FILE *fp = NULL;
    char b1[rs], b2[rs];
    fp = fopen(fname, "r+");
    for(int i=0;i<as;i++) {
        for(int j=1;j<as;j++){
            fseek(fp, (j-1)*rs, 0);
            fread(b1, sizeof(char), rs,fp);
            fseek(fp, j*rs, 0);
            fread(b2, sizeof(char), rs,fp);
            if(b1[0]>b2[0]){
                fseek(fp, (j-1)*rs, 0);
                fwrite(b2, sizeof(char),rs,fp);
                fseek(fp,j*rs,0);
                fwrite(b1, sizeof(char),rs,fp);
            }
        }
    }
    fclose(fp);
}

void sortSys(char* fname,int as, int rs){
    char b1[rs], b2[rs];
    int fp = open(fname, O_RDWR, S_IRUSR | S_IWUSR);
    for(int i=0;i<as;i++) {
        for(int j=1;j<as;j++){
            lseek(fp, (j-1)*rs, SEEK_SET);
            read(fp, b1, rs);
            lseek(fp, j*rs, SEEK_SET);
            read(fp, b2, rs);
            if(b1[0]>b2[0]){
                lseek(fp, (j-1)*rs, SEEK_SET);
                write(fp, b2, rs);
                lseek(fp,j*rs,SEEK_SET);
                write(fp, b1, rs);
            }
        }
    }
    close(fp);
}

void copyLib(char* from, char* to, int as, int rs){
    FILE *fp1 = NULL;
    FILE *fp2 = NULL;
    char b[rs];
    fp1 = fopen(from, "r");
    fp2 = fopen(to,"w");
    for(int i=0;i<as;i++) {
        fread(b, sizeof(char), rs,fp1);
        fwrite(b,sizeof(char), rs,fp2);
    }
    fclose(fp1);
    fclose(fp2);
}

void copySys(char* from, char* to, int as, int rs){
    char b[rs];
    int fp1 = open(from, O_RDONLY);
    int fp2 = open(to,O_WRONLY | O_CREAT | O_TRUNC , S_IRUSR | S_IWUSR);
    for(int i=0;i<as;i++) {
        read(fp1, b, rs);
        write(fp2, b, rs);
    }
    close(fp1);
    close(fp2);
}

int a2i(char *s)
{
    int sign=1;
    if(*s == '-') {
        sign = -1;
        s++;
    }
    int num=0;
    while(*s)
    {
        num=((*s)-'0')+num*10;
        s++;
    }
    return num*sign;
}
int sc(char* s,char* s2){
    while (*s && *s2){
        if(*s == *s2){
            s++, s2++;
        }
        else
            return 0;
    }
    return 1;
}



int main(int argc,char ** argv){
    struct TimeMes time;
	printf("\n%s\t",argv[1]);
    if(sc(argv[1],"generate")){
        generate(argv[2], a2i(argv[3]), a2i(argv[4]));
    }
    else if(sc(argv[1],"sort")){
	printf("%s\t", argv[5]);
	printf("Record size: %s\n", argv[4]);
	if(sc(argv[5],"sys")) {
            time = startTime();
                sortSys(argv[2], a2i(argv[3]), a2i(argv[4]));
            endTime(&time);
        }
        else {
            time = startTime();
                sortLib(argv[2], a2i(argv[3]), a2i(argv[4]));
            endTime(&time);
        }
    }
    else if(sc(argv[1],"copy")){
        printf("%s\t", argv[6]);
        printf("Record size: %s\n", argv[5]);
	if(sc(argv[6],"sys")) {
            time = startTime();
                copySys(argv[2], argv[3], a2i(argv[4]), a2i(argv[5]));
            endTime(&time);
        }
        else {
            time = startTime();
                copyLib(argv[2], argv[3], a2i(argv[4]), a2i(argv[5]));
            endTime(&time);
        }
    }
    return 0;
}
