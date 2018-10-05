#include <fstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <GL/glut.h>
using namespace std;

#define WINDOW_X (500)
#define WINDOW_Y (500)
#define WINDOW_NAME "Viewer"

int load_csv(char* file,int* array);
void init_GL(int argc, char *argv[]);
void set_callback_functions();
void draw_map();
void glut_display();

int w,h;
int* data;

int main(int argc, char* argv[]){
    //読み込み
    w=stoi(argv[2]);
    h=stoi(argv[3]);
    data = (int *)malloc(sizeof(int)*w*h);
    load_csv(argv[1],data);
    printf("%d*%d map %s loaded.\n",w,h,argv[1]);

    init_GL(argc,argv);
    set_callback_functions();
    glutMainLoop();
}

int load_csv(char* file,int* array){
    ifstream ifs(file);
    string buf;
    int i=0;
    int br;
    while (getline(ifs, buf,',')) {
        br=buf.find('\n');
        if(br==string::npos){
        array[i]=(int)(stof(buf)*10);
        }else{
            array[i]=(int)(stof(buf.substr(0,br))*10);
        i++;
            if(br==buf.size()-1)return i;
            array[i]=(int)(stof(buf.substr(br+1))*10);
    }
        i++;
    }
    return i-1;
}

void init_GL(int argc, char *argv[]){
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize(WINDOW_X,WINDOW_Y);
  glutCreateWindow(WINDOW_NAME);
  glEnableClientState(GL_VERTEX_ARRAY);
}

void set_callback_functions(){
    glutDisplayFunc(glut_display);
}

void glut_display(){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30.0, 1.0, 0.1, 100);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0,0,20,0.0,0.0,0.0,0.0,1,0);

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    draw_map();
    glFlush();
    glDisable(GL_DEPTH_TEST);
    
    glutSwapBuffers();
}

void draw_map(){
    glutSolidSphere(0.2, 50, 50);

}
