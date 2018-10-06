#include <fstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <math.h>
#include <GL/glut.h>
using namespace std;

#define WINDOW_X (500)
#define WINDOW_Y (500)
#define WINDOW_NAME "Viewer"

int load_csv(char* file);
void init_GL(int argc, char *argv[]);
void set_callback_functions();
void draw_map();
void glut_display();
void prepare_polygon();
void closs(float* x1,float* x2,float* y1,float* y2, float* closs);//x2-x1とy2-y1の外積を計算して正規化してclossに入れる
void culculateColor(float* p,float* color);

int w,h;
GLfloat* csv_data;
GLuint* indices;
GLfloat* normals;
GLfloat* colors;

int main(int argc, char* argv[]){
    //読み込み
    h=stoi(argv[2]);
    w=stoi(argv[3]);
    int size = load_csv(argv[1]);
    printf("%d*%d map %s loaded.\n",w,h,argv[1]);

    init_GL(argc,argv);

    prepare_polygon();
    set_callback_functions();
    glutMainLoop();
}

int load_csv(char* file){
    csv_data = (float *)malloc(sizeof(float)*w*h*3);

    ifstream ifs(file);
    string buf;
    int i=0;
    int br;
    int x=0;
    int y=0;
    while (getline(ifs, buf,',')) {
        br=buf.find('\n');
        if(br==string::npos){
            csv_data[i]=x;
            csv_data[i+1]=h-y;
            csv_data[i+2]=(stof(buf)/10);
            
        }else{
            csv_data[i]=x;
            csv_data[i+1]=h-y;
            csv_data[i+2]=(stof(buf.substr(0,br))/10);
            i+=3;
            if(br==buf.size()-1)return i;
            x=0;
            y++;
            csv_data[i]=x;
            csv_data[i+1]=h-y;
            csv_data[i+2]=(stof(buf.substr(br+1))/10);
        }
        x++;
        i+=3;
    }
    return i-1;
}

void init_GL(int argc, char *argv[]){
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize(WINDOW_X,WINDOW_Y);
  glutCreateWindow(WINDOW_NAME);
}

void prepare_polygon(){
    indices = (GLuint *)malloc(sizeof(GLuint)*(w-1)*(h-1)*4);
    normals = (GLfloat *)malloc(sizeof(GLfloat)*w*h*3);
    colors = (GLfloat *)malloc(sizeof(GLfloat)*w*h*3);
    int i,j;
    GLuint* p = indices;
    int temp=0;
    for(i=0;i<h-1;i++){
        for(j=0;j<w-1;j++){
            p[0]=temp;
            p[1]=temp+1;
            p[2]=temp+w+1;
            p[3]=temp+w;
            temp++;
            p+=4;
        }
        temp++;
    }
    GLfloat* p0 = csv_data;
    GLfloat* q = normals;
    GLfloat* c = colors;
    for(i=0;i<h;i++){
        for(j=0;j<w;j++){
            culculateColor(p0,c);
            closs(p0, j<w-1?p0+3:p0-3, p0, i<h-1?p0+w*3:p0-w*3,q);
            p0+=3;
            c+=3;
            q+=3;
        }
    }
    printf("indices prepared.\n");
}

void substract(float* x,float* y,float* result){//x-y
    for(int i = 0;i<3;i++){
        result[i]=x[i]-y[i];
    }
}

void closs(float* x1,float* x2,float* y1,float* y2, float* closs){
    static float* x =  (float *)malloc(sizeof(float)*3);
    static float* y =  (float *)malloc(sizeof(float)*3);
    static float r;
   substract(x2,x1,x);
   substract(y2,y1,y);
   closs[0]=x[1]*y[2]-x[2]*y[1];
   closs[1]=x[2]*y[0]-x[0]*y[2];
   closs[2]=x[0]*y[1]-x[1]*y[0];
    r=sqrt(closs[0]*closs[0]+closs[1]*closs[1]+closs[2]*closs[2]);
    if(closs[2]<0)r=-r;
    closs[0]/=r;
    closs[1]/=r;
    closs[2]/=r;
}

void culculateColor(float* p,float* color){
    color[0]=1;
    color[1]=1;
    color[2]=1;
}

void set_callback_functions(){
    glutDisplayFunc(glut_display);
}

void glut_display(){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30.0, 1.0, 0.1, 10000);


    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(w/2,h/2,4000,
                w/2,h/2,0,
                0,1,0);
    
    GLfloat lightpos[] = {(float)w,(float)h,400,1.0};

    GLfloat diffuse[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat ambient[] = {1.0, 1.0, 1.0, 1.0};
   
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glLightfv(GL_LIGHT0,GL_POSITION,lightpos);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuse);
    glLightfv(GL_LIGHT0,GL_AMBIENT,ambient);


    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    draw_map();
    glFlush();
    glDisable(GL_DEPTH_TEST);
    
    glutSwapBuffers();
}

void draw_map(){
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
     glEnableClientState(GL_COLOR_ARRAY);
    glVertexPointer(3,GL_FLOAT,0,csv_data);
     glColorPointer(3,GL_FLOAT,0,colors);
    glNormalPointer(GL_FLOAT,0,normals);
    glDrawElements(GL_QUADS,(w-1)*(h-1)*4,GL_UNSIGNED_INT,indices);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    printf("rendered.\n");
}
