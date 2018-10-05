#include <fstream>
#include <string>
#include <vector>
#include <stdio.h>
using namespace std;

int load_csv(char* file,int* array);

int main(int argc, char* argv[]){
    //読み込み
    int w,h;
    w=stoi(argv[2]);
    h=stoi(argv[3]);
    int* data = (int *)malloc(sizeof(int)*w*h);
    load_csv(argv[1],data);
    printf("%d*%d map %s loaded.\n",w,h,argv[1]);
}

int load_csv(char* file,int* array){
    ifstream ifs(file);
    string buf;
    int i=0;
    while (getline(ifs, buf,',')) {
        array[i]=(int)(stof(buf)*10);
        i++;
    }
    return i-1;
}