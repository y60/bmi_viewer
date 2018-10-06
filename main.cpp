#include <fstream>
#include <string>
#include <vector>
#include <stdio.h>
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

int w,h;
GLfloat* csv_data;
GLint* pol_data;
GLuint* indices;

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
            csv_data[i+1]=y;
            csv_data[i+2]=(stof(buf)/10);
            x++;
        }else{
            csv_data[i]=x;
            csv_data[i+1]=y;
            csv_data[i+2]=(stof(buf.substr(0,br))/10);
            i+=3;
            if(br==buf.size()-1)return i;
            x=0;
            y++;
            csv_data[i]=x;
            csv_data[i+1]=y;
            csv_data[i+2]=(stof(buf.substr(br+1))/10);
        }
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
    int i,j;
    GLuint* p = indices;
    int temp=0;
    for(i=0;i<h-1;i++){
        for(j=0;j<w-1;j++){
            p[0]=temp;
            p[1]=temp+1;
            p[2]=temp+w+1;
            p[3]=temp+w;
            p+=4;
            temp++;
        }
        temp++;
    }
    printf("indices prepared.%d%d%d%d\n",indices[0],indices[1],indices[2],indices[3]);
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
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3,GL_FLOAT,0,csv_data);
    glDrawElements(GL_QUADS,(w-1)*(h-1)*4,GL_UNSIGNED_INT,indices);
    glDisableClientState(GL_VERTEX_ARRAY);
    printf("rendered.\n");
}
