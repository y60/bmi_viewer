#include <fstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <math.h>
#include <GL/glut.h>
using namespace std;

#define WINDOW_X (900)
#define WINDOW_Y (600)
#define WINDOW_NAME "Viewer"

int load_csv(char* file);
void load_gsx_csv(char* file);
void load_poll_csv(char* file);
void init_GL(int argc, char *argv[]);
void set_callback_functions();
void draw_map();
void glut_display();
void glut_keyboard(unsigned char key, int x, int y);
void prepare_polygon();
void closs(float* x1,float* x2,float* y1,float* y2, float* closs);//x2-x1とy2-y1の外積を計算して正規化してclossに入れる
void culculateColor(float* p,float* color);
void glut_motion(int x,int y);
void draw_sky();
void draw_gsx();
void draw_polls();
void draw_poll(float* x,float* color,string str);
void convertLatLon(float lat, float lon, float* xyz);
GLfloat green_dif[] ={88.0/255,181.0/255,64/255,1.0};
GLfloat green_amb[] ={30.0/255,33.0/255,19.0/255,1.0};
GLfloat red_line[] ={1,0,0,1.0};
GLfloat sea_color[]={0.0,0.0,1.0};
int gsx_size;
int poll_size;
double lat;
double lon;
double g_angle1 = 0.0;
double g_angle3 = 0.0;//視点の向き
double pos_x;
double pos_y;
double pos_z=0;
double v=3.0;
double g_angle2 = -3.141592 / 6;
int w,h;
GLfloat* gsx_data;
GLfloat* csv_data;
GLfloat* poll_data;
string* poll_name_data;
GLuint* indices;
GLfloat* normals;
GLfloat* colors;

int main(int argc, char* argv[]){
    //読み込み
    h=stoi(argv[2]);
    w=stoi(argv[3]);
    int size = load_csv(argv[1]);
    load_gsx_csv(argv[4]);
    load_poll_csv(argv[5]);
    pos_x=w/2;
    pos_y=h/2;
    printf("%d*%d map %s loaded.\n",w,h,argv[1]);

    init_GL(argc,argv);

    prepare_polygon();
    set_callback_functions();
    glNormalPointer(GL_FLOAT,0,normals);
    glVertexPointer(3,GL_FLOAT,0,csv_data);
    glutMainLoop();
}

int load_csv(char* file){
    csv_data = (float *)malloc(sizeof(float)*w*h*3);

    ifstream ifs(file);
    string buf;
    getline(ifs, buf,',');
    lon=stof(buf);
    getline(ifs, buf,'\n');
    lat=stof(buf);
    printf("%f,%f\n",lat,lon);
    int i=0;
    int br;
    int x=0;
    int y=0;
    while (getline(ifs, buf,',')) {
        br=buf.find('\n');
        if(br==string::npos){
            if(x<w&&y<h){
            csv_data[i]=x;
            csv_data[i+1]=h-y;
            csv_data[i+2]=(stof(buf)/10);
                i+=3;
            }
        }else{
            if(x<w&&y<h){
            csv_data[i]=x;
            csv_data[i+1]=h-y;
            csv_data[i+2]=(stof(buf.substr(0,br))/10);
            i+=3;
            }
            if(br==buf.size()-1)return i;
            x=0;
            y++;
            csv_data[i]=x;
            csv_data[i+1]=h-y;
            csv_data[i+2]=(stof(buf.substr(br+1))/10);
            i+=3;
        }
        x++;
    }
    printf("csv loaded.");
    return i-1;
}

void load_gsx_csv(char* file){
    ifstream ifs(file);
    string buf;
    getline(ifs, buf);
    gsx_size=stoi(buf);
    printf("gsx count:%d\n",gsx_size);
    gsx_data = (float *)malloc(sizeof(float)*gsx_size*3);
    float *p=gsx_data;
    int count=0;
    while (getline(ifs, buf,',')&&count<gsx_size) {
        float mlat=stof(buf);
        getline(ifs, buf,',');
        float mlon=stof(buf);
        getline(ifs, buf,'\n');
        convertLatLon(mlat,mlon,p);
        p+=3;
        count++;
    }
    pos_x=gsx_data[0];
    pos_y=gsx_data[1];
    pos_z=gsx_data[2]+100;
    printf("gsx loaded.\n");
}

void load_poll_csv(char* file){
    ifstream ifs(file);
    string buf;
    getline(ifs, buf);
    poll_size=stoi(buf);
    printf("poll count:%d\n",poll_size);
    poll_data = (float *)malloc(sizeof(float)*poll_size*3);
    poll_name_data = (string *)malloc(sizeof(string)*poll_size);
    float *p=poll_data;
    int count=0;
    while (getline(ifs, buf,',')&&count<poll_size) {
        float mlat=stof(buf);
        getline(ifs, buf,',');
        float mlon=stof(buf);
        getline(ifs, buf,'\n');
        printf("%s\n",buf);
        buf.copy(poll_name_data[count],buf.size());
        convertLatLon(mlat,mlon,p);
        p+=3;
        count++;
    }
    printf("poll loaded.\n");
}

void convertLatLon(float mlat, float mlon, float* xyz){
    xyz[0]=(mlon-lon)*9000;
    xyz[1]=-(mlat-lat)*9000;
    xyz[2]=csv_data[(w*((int)xyz[1])+(int)xyz[0])*3+2];
    xyz[1]=h-xyz[1];
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
    color[0]=0;
    color[1]=1;
    color[2]=0;
}

void set_callback_functions(){
    glutDisplayFunc(glut_display);
    glutKeyboardFunc(glut_keyboard);
    glutMotionFunc(glut_motion);
}

float angle=0;

void glut_keyboard(unsigned char key, int x, int y){
    switch(key){
        case 'q':
            exit(0);
        break;
        case 'g':
            angle+=0.1;
        break;
        case 'a'://左
            g_angle3-=0.03;
        break;
        case 'f'://右
            g_angle3+=0.03;
        break;
        case 'e'://全進
            pos_x+= v*sin(g_angle3); 
	        pos_y+= v*cos(g_angle3);
        break;
        case 'x'://後退
            pos_x-= v*sin(g_angle3); 
	        pos_y-= v*cos(g_angle3);
        break;
    }
    glutPostRedisplay();
}

void glut_motion(int x, int y){
    static int px = -1, py = -1;
    if(px >= 0 && py >= 0){
        g_angle1 += (double)-(x - px)/20;
        g_angle2 += (double)(y - py)/20;
        pos_z += (double)(y - py);
    }
    px = x;
    py = y;
    glutPostRedisplay();
}

void glut_display(){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30.0, 1.5, 0.1, 10000);


    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(pos_x,pos_y,pos_z,
            pos_x+h * sin(g_angle3), 
	        pos_y+h * cos(g_angle3),
	        0, 
            0.0, 0.0, 1.0);
    // gluLookAt(w/2,h/2,h*3,
    //             w/2,h/2,0,
    //             0,1,0);
    
    GLfloat lightpos[] = {((float)w/2)*(1-cos(angle)),((float)h)*(1-sin(angle)),1000,1.0};

    GLfloat diffuse[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat ambient[] = {1.0, 1.0, 1.0, 1.0};
    glClearColor(63.0/255,166.0/255,250.0/255,1.0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    glLightfv(GL_LIGHT0,GL_POSITION,lightpos);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuse);
    glLightfv(GL_LIGHT0,GL_AMBIENT,ambient);
    draw_map();
    draw_gsx();
    draw_polls();
    glFlush();
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    
    glutSwapBuffers();
}

void draw_map(){
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glMaterialfv(GL_FRONT,GL_DIFFUSE,green_dif);
    glMaterialfv(GL_FRONT,GL_AMBIENT,green_amb);
    glDrawElements(GL_QUADS,(w-1)*(h-1)*4,GL_UNSIGNED_INT,indices);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    
    glMaterialfv(GL_FRONT,GL_DIFFUSE,sea_color);
    glBegin(GL_QUADS);
    int sea_level=0;
    glNormal3d(0,0,1);
    glVertex3f(0,0,sea_level);
    glVertex3f(w,0,sea_level);
    glVertex3f(w,h,sea_level);
    glVertex3f(0,h,sea_level);
    glEnd();
}
void draw_gsx(){
    glMaterialfv(GL_FRONT,GL_DIFFUSE,red_line);
    glBegin(GL_LINE_STRIP);
    float *p=gsx_data;
    for(int i=0;i<gsx_size;i++){
        glVertex3f(p[0],p[1],p[2]+1.0);
        p+=3;
    }
    glEnd();
}

void draw_polls(){
    draw_poll(gsx_data,red_line,"Start");
    draw_poll(gsx_data+(gsx_size-1)*3,red_line,"Goal");
    for (int i = 0;i<poll_size;i++){
      draw_poll(poll_data+i*3,red_line,poll_name_data[i]);
    }
}
void draw_poll(float* x,float* color,string str){
    glMaterialfv(GL_FRONT,GL_DIFFUSE,color);
    glBegin(GL_LINE_STRIP);
    glVertex3f(x[0],x[1],x[2]);
    glVertex3f(x[0],x[1],pos_z+1);
    glEnd();
    glRasterPos3f(x[0],x[1],pos_z);
    for(int i = 0;i<str.size();i++){
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15,str[i]);
    }
} 