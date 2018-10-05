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
void prepare_polygon();

int w,h;
int* csv_data;
GLint* pol_data;

int main(int argc, char* argv[]){
    //読み込み
    h=stoi(argv[2]);
    w=stoi(argv[3]);
    csv_data = (int *)malloc(sizeof(int)*w*h);
    int size = load_csv(argv[1],csv_data);
    printf("%d*%d map %s loaded.\n",w,h,argv[1]);

    init_GL(argc,argv);

    prepare_polygon();
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
}

void prepare_polygon(){
    glEnableClientState(GL_VERTEX_ARRAY);
    
    pol_data = (GLint *)malloc(sizeof(GLint)*(w-1)*(h-1)*4*3);
    int i,j;
    GLint* p = pol_data;
    int* q = csv_data;
    for(i=0;i<h-1;i++){
        for(j=0;j<w-1;j++){
            p[0]=j;
            p[1]=h-i;
            p[2]=q[0];
            p[3]=j+1;
            p[4]=h-i;
            p[5]=q[1];
            p[6]=j+1;
            p[7]=h-(i+1);
            p[8]=q[w+1];
            p[9]=j;
            p[10]=h-(i+1);
            p[11]=q[w];
            p+=12;
            q+=1;
        }
        q+=1;
    }
    printf("pol_data prepared.\n");
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
    
    GLfloat lightpos[] = {w,h,1000,1.0};

    GLfloat diffuse[] = {1.0, 1.0, 1.0, 1.0};

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glLightfv(GL_LIGHT0,GL_POSITION,lightpos);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuse);


    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    draw_map();
    glFlush();
    glDisable(GL_DEPTH_TEST);
    
    glutSwapBuffers();
}

void draw_map(){
     glVertexPointer(3,GL_INT,0,pol_data);
    for(int i = 0;i<(w-1)*(h-1);i++){
         glDrawArrays(GL_POLYGON,i*4,4);
     }
    glBegin(GL_POLYGON);
    double p0[]={1,1,30};
    double p1[]={1,0,30};
    double p2[]={0,0,30};
    glVertex3dv(p0);
    glVertex3dv(p1);
    glVertex3dv(p2);
    glEnd();
    printf("rendered.\n");
}
