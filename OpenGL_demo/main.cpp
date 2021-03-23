//
#include <GLUT/GLUT.h>
#define STB_IMAGE_IMPLEMENTATION


#include <iostream>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include "stb_image.h"
#include <list>
#include <map>
#include <unistd.h>

#define pie  3.1415
#define nfwidth  0.1;
using namespace std;
//®
GLfloat     step = 1.0;
GLfloat     r=1.0;
GLfloat     len=8.0;

GLboolean   bsrMode = false;
GLboolean   hasParticle=false;
GLfloat     bsrX1=-6,bsrX2=4;
GLfloat     topx=GLfloat(-len+2.0);
GLfloat     topy=GLfloat(r+0.5);
GLfloat     knifetop[3]={topx,topy,0.0};
GLfloat     knifeleft[3]={GLfloat(topx-0.5),GLfloat(topy+1.5),0.0};
GLfloat     kniferight[3]={GLfloat(topx+0.5),GLfloat(topy+1.5),0.0};


GLfloat     mouseX,mouseY;//鼠标当前屏幕位置
GLboolean   dragging=false;//是否被拖动
GLfloat     upP[1200][2]; //截面上部点集
map<int,bool> up;
GLint       ii=1;
unsigned int texD;//刀具
unsigned int texB;//棒子

struct particle{
    GLfloat x,y,z;
    GLfloat vx,vy,vz;
    GLfloat ax,ay,az;
    GLint lifetime;
    GLboolean alive;
    void init(GLfloat xx,GLfloat yy,GLfloat zz){
        x=xx;y=yy;z=zz;
        vx=rand()%61-30;vy=rand()%61-30;vz=rand()%61-30;
        ax=vx/100;ay=3.0;z=-vz/10;
        lifetime=rand()%31+10;
        alive=true;
    }
    void draw(){
        if(alive==true){
            glBegin(GL_TRIANGLE_STRIP);
            glNormal3f(0,0,1);
            glVertex3f(x+0.02, y, z);
            glVertex3f(x, y-0.02, z);
            glVertex3f(x, y+0.02, z);
            glVertex3f(x-0.02, y, z);
            glEnd();
        }
    }
    void update(){
        if(--lifetime==0){
            alive=false;
            return;
        }
        x+=vx/300;
        y+=vy/300;
        z+=vz/300;
        vx-=ax;
        vy-=ay;
        vz-=az;
    }
};
particle pa[100];

void updatetopx(){
    knifetop[0]=topx;
    knifeleft[0]=GLfloat(topx-0.5);
    kniferight[0]=GLfloat(topx+0.5);
}
void updatetopy(){
    knifetop[1]=topy;
    knifeleft[1]=GLfloat(topy+1.5);
    kniferight[1]=GLfloat(topy+1.5);
}
GLuint LoadGLTextures( string road)
{
    glGenTextures(1, &texB);
    glGenTextures(1, &texD);
    glBindTexture(GL_TEXTURE_2D, texB);
    int texwidth,texheight,nrChannels;//nrChannels表示通道数，R/G/B/A，一共4个通道，有些图片只有3个，A即为alpha
    stbi_set_flip_vertically_on_load(true);   //解决图像翻转问题，不需要像SOIL库中片段着色器的position设置为-y
    unsigned char *image = stbi_load(road.c_str(), &texwidth, &texheight, &nrChannels,STBI_rgb_alpha);//注意这里不是0
    if(image){
        if(nrChannels==3)//rgb 适用于jpg图像
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texwidth, texheight, 0,GL_RGBA, GL_UNSIGNED_BYTE, image);//后面一个是RGBA
        else if(nrChannels==4)//rgba 适用于png图像
            glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, texwidth, texheight, 0,GL_RGBA, GL_UNSIGNED_BYTE, image);//注意，两个都是RGBA
        glGenerateMipmap(GL_TEXTURE_2D);
    }
//    else
        stbi_image_free(image);
    
    glBindTexture(GL_TEXTURE_2D, texD);
    stbi_set_flip_vertically_on_load(true);   //解决图像翻转问题，不需要像SOIL库中片段着色器的position设置为-y
    unsigned char *Dimage = stbi_load("dao.png", &texwidth, &texheight, &nrChannels,STBI_rgb_alpha);//注意这里不是0
    if(Dimage){
        if(nrChannels==3)//rgb 适用于jpg图像
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texwidth, texheight, 0,GL_RGBA, GL_UNSIGNED_BYTE, Dimage);//后面一个是RGBA
        else if(nrChannels==4)//rgba 适用于png图像
            glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, texwidth, texheight, 0,GL_RGBA, GL_UNSIGNED_BYTE, Dimage);//注意，两个都是RGBA
        glGenerateMipmap(GL_TEXTURE_2D);
    }
//    else
        stbi_image_free(Dimage);
    
//    glBindTexture(GL_TEXTURE_2D,texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    return 0;
    
}


void beisaier(GLfloat t,GLfloat & rex,GLfloat & rey ,GLfloat & rez){
    GLfloat b1=3*t;
    GLfloat b2=t*t;
    GLfloat a4=t*t*t;
    GLfloat a3=-3*(b2-a4);
    GLfloat a2=3*(a4-2*b2+t);
    GLfloat a1=-a4+3*b2-b1+1;
    
    
    
    rex=a1*(bsrX1)+a2*(bsrX1)+a3*bsrX2+a4*bsrX2;
    rey=a1*rey+a4*rey;
    if(rez!=0) rez=a1*rez+a4*rez;
    
}
void initupP(){
    GLfloat xx,yy,zz;
    int i=0;
    for(GLfloat t=0;t<=1.0;t+=0.000833){
        yy=r;
        zz=0;
        beisaier(t,xx,yy,zz);
        upP[i][0]=xx;
        upP[i][1]=yy;
        i++;
    }
}
bool inknife(int x,int y){ //判断鼠标在道具上
    if(x<600) {
        GLfloat realrate=((600.0-x)/510.0);
        GLfloat tdrate=(-(knifetop[0]+kniferight[0])*0.5/len);
        GLfloat tdrate2=(-(knifetop[0]+knifeleft[0])*0.5/len);
        return (y>0&&y<570)&&realrate>tdrate&&realrate<tdrate2&&((y-390)/180.0>0.5) ;
        
    }
    else{
        GLfloat realrate=((x-600.0)/510.0);
        GLfloat tdrate2=((knifetop[0]+kniferight[0])*0.5/len);
        GLfloat tdrate=((knifetop[0]+knifeleft[0])*0.5/len);
        return (y>0&&y<570)&&realrate>tdrate&&realrate<tdrate2&&((y-390)/180.0>0.5) ;
    }
    
}


void convert2td(GLfloat & x,GLfloat & y){
    x=((GLfloat)x-600.0)/510.0*len;
    y=((GLfloat)y-390.0)/180.0*3.0;
}

void drawline(int step){
    GLfloat fi;
    
    glLineWidth(1.0);
    
    
    if(!bsrMode){
        GLfloat xstart=-len;
        GLfloat ystart=r*cos(step/180.0*pie);
        GLfloat zstart=r*sin(step/180.0*pie);
        for(fi=step;fi<step+180;fi+=0.5){
            glBegin(GL_LINES);
            glVertex3f(xstart, ystart, zstart);
            glVertex3f(xstart, -ystart, -zstart);
            
            glNormal3f(0, -ystart, -zstart);glTexCoord2f(0.0f, 0.5-(fi-step)/360);
            glVertex3f(xstart, -ystart, -zstart);
            glTexCoord2f(0.125f, 0.5-(fi-step)/360);glNormal3f(0, -ystart, -zstart);
            glVertex3f(xstart+2, -ystart, -zstart);
            glTexCoord2f(0.875f, 0.5-(fi-step)/360);glNormal3f(0, -ystart, -zstart);
            glVertex3f(-xstart-2, -ystart, -zstart);
            glTexCoord2f(1.0f, 0.5-(fi-step)/360);glNormal3f(0, -ystart, -zstart);
            glVertex3f(-xstart, -ystart, -zstart);
            
            glVertex3f(-xstart, -ystart, -zstart);
            glVertex3f(-xstart, ystart, zstart);
            
            
            
            glTexCoord2f(1.0f, 1.0f-(fi-step)/360);
            glNormal3f(0, ystart, zstart);
            glVertex3f(-xstart, ystart, zstart);
            glTexCoord2f(0.875f, 1.0f-(fi-step)/360);
            glNormal3f(0, ystart, zstart);
            glVertex3f(-xstart-2, ystart, zstart);
            
            
            
            glTexCoord2f(0.125f, 1.0f-(fi-step)/360);
            glNormal3f(0, ystart, zstart);
            glVertex3f(xstart+2, ystart, zstart);
            glTexCoord2f(0.0f, 1.0f-(fi-step)/360);
            glNormal3f(0, ystart, zstart);
            glVertex3f(xstart, ystart, zstart);
            
            
            glEnd();
            
            map<int,bool>::iterator iter=up.begin();
            map<int,bool>::iterator prev=iter;
            if(iter==up.end()){
                glBegin(GL_LINE_STRIP);
                
                glTexCoord2f((upP[0][0]+8)/16, 1.0-(fi-step)/360);
                glNormal3f(0, cos(fi/180.0*pie), sin(fi/180.0*pie));
                glVertex3f(upP[0][0], upP[0][1]*cos(fi/180.0*pie), upP[0][1]*sin(fi/180.0*pie));
                
                glTexCoord2f((upP[1199][0]+8)/16, 1.0-(fi-step)/360);
                glNormal3f(0, cos(fi/180.0*pie), sin(fi/180.0*pie));
                glVertex3f(upP[1199][0], upP[1199][1]*cos(fi/180.0*pie), upP[1199][1]*sin(fi/180.0*pie));
                
                glTexCoord2f((upP[1199][0]+8)/16, 0.5-(fi-step)/360);
                glNormal3f(0,-cos(fi/180.0*pie), -sin(fi/180.0*pie));
                glVertex3f(upP[1199][0], -upP[1199][1]*cos(fi/180.0*pie), -upP[1199][1]*sin(fi/180.0*pie));
                
                glTexCoord2f((upP[0][0]+8)/16, 0.5-(fi-step)/360);
                glNormal3f(0,-cos(fi/180.0*pie), -sin(fi/180.0*pie));
                glVertex3f(upP[0][0], -upP[0][1]*cos(fi/180.0*pie), -upP[0][1]*sin(fi/180.0*pie));
                glEnd();
            }
            else{
                glBegin(GL_LINE_STRIP);
                glTexCoord2f((upP[0][0]+8)/16, 0.5-(fi-step)/360);
                glNormal3f(0,-cos(fi/180.0*pie), -sin(fi/180.0*pie));
                glVertex3f(upP[0][0], -1.0*cos(fi/180.0*pie), -1.0*sin(fi/180.0*pie));
                while(iter!=up.end()){
                    if(iter==up.begin()){
                        glTexCoord2f((upP[iter->first][0]+8)/16, 0.5-(fi-step)/360);
                        glNormal3f(0,-cos(fi/180.0*pie), -sin(fi/180.0*pie));
                        glVertex3f(upP[iter->first][0],  -1.0*cos(fi/180.0*pie), -1.0*sin(fi/180.0*pie));
                        
                    }
                    else{
                        glTexCoord2f(0,1);
                        glVertex3f(upP[iter->first][0], -upP[iter->first][1]*cos(fi/180.0*pie), -upP[iter->first][1]*sin(fi/180.0*pie));
                    }
                    prev=iter;
                    iter++;
                }
                glTexCoord2f((upP[prev->first][0]+8)/16, 0.5-(fi-step)/360);
                glNormal3f(0,-cos(fi/180.0*pie), -sin(fi/180.0*pie));
                glVertex3f(upP[prev->first][0],  -1.0*cos(fi/180.0*pie), -1.0*sin(fi/180.0*pie));
                glTexCoord2f((upP[1199][0]+8)/16, 0.5-(fi-step)/360);
                glNormal3f(0,-cos(fi/180.0*pie), -sin(fi/180.0*pie));
                glVertex3f(upP[1199][0], -1.0*cos(fi/180.0*pie), -1.0*sin(fi/180.0*pie));
                
                glTexCoord2f((upP[1199][0]+8)/16, 1.0-(fi-step)/360);
                glNormal3f(0,cos(fi/180.0*pie), sin(fi/180.0*pie));
                glVertex3f(upP[1199][0], 1.0*cos(fi/180.0*pie), 1.0*sin(fi/180.0*pie));    ///d对称面
                
                map<int,bool>::reverse_iterator riter=up.rbegin();
                while(riter!=up.rend()){
                    if(riter==up.rbegin()){
                        glTexCoord2f((upP[riter->first][0]+8)/16, 1.0-(fi-step)/360);
                        glNormal3f(0,cos(fi/180.0*pie), sin(fi/180.0*pie));
                        glVertex3f(upP[riter->first][0],  1.0*cos(fi/180.0*pie), 1.0*sin(fi/180.0*pie));
                    }else{
                        glTexCoord2f(0,1);
                        glVertex3f(upP[riter->first][0], upP[riter->first][1]*cos(fi/180.0*pie), upP[riter->first][1]*sin(fi/180.0*pie));
                    }
                    
                    riter++;
                }
                glTexCoord2f((upP[up.begin()->first][0]+8)/16, 1.0-(fi-step)/360);
                glNormal3f(0,cos(fi/180.0*pie), sin(fi/180.0*pie));
                glVertex3f(upP[up.begin()->first][0],  1.0*cos(fi/180.0*pie), 1.0*sin(fi/180.0*pie));
                glTexCoord2f((upP[0][0]+8)/16, 1.0-(fi-step)/360);
                glNormal3f(0,cos(fi/180.0*pie), sin(fi/180.0*pie));
                glVertex3f(upP[0][0], 1.0*cos(fi/180.0*pie), 1.0*sin(fi/180.0*pie));
                
                glEnd();
                
            }
            
            
            
            ystart=r*cos(fi/180.0*pie);
            zstart=r*sin(fi/180.0*pie);
        }
        
    }
    else{
        GLfloat xstart=-len;
        GLfloat ystart=r*cos(step/180.0*pie);
        GLfloat zstart=r*sin(step/180.0*pie);
        for(fi=step;fi<step+180;fi+=0.9){
            
            glBegin(GL_LINES);
            glTexCoord2f(0.5-(-bsrX1/len)*0.5f, 1.0f-(fi-step)/360.0);
            glNormal3f(0, ystart, zstart);
            glVertex3f(bsrX1, ystart, zstart);
            glTexCoord2f(0.0f, 1.0f-(fi-step)/360.0);
            glNormal3f(0, ystart, zstart);
            glVertex3f(xstart,ystart, zstart);
            
            glVertex3f(xstart,ystart, zstart);
            glVertex3f(xstart, -ystart, -zstart);
            
            glTexCoord2f(0.0f,0.5f-(fi-step)/360.0);
            glNormal3f(0,-ystart, -zstart);
            glVertex3f(xstart, -ystart, -zstart);
            glTexCoord2f(0.5-(-bsrX1/len)*0.5f,0.5f-(fi-step)/360.0);
            glNormal3f(0, -ystart, -zstart);
            glVertex3f(bsrX1, -ystart, -zstart);
            glEnd();
            
            glBegin(GL_LINES);
            glTexCoord2f((bsrX2/len/2)+0.5f, 1.0f-(fi-step)/360.0);
            glNormal3f(0, ystart, zstart);
            glVertex3f(bsrX2, ystart, zstart);
            glTexCoord2f(1.0f, 1.0f-(fi-step)/360.0);
            glNormal3f(0, ystart, zstart);
            glVertex3f(-xstart,ystart, zstart);
            
            glVertex3f(-xstart,ystart, zstart);
            glVertex3f(-xstart, -ystart, -zstart);
            
            glTexCoord2f(1.0f,0.5f-(fi-step)/360.0);
            glNormal3f(0, -ystart, -zstart);
            glVertex3f(-xstart, -ystart, -zstart);
            glTexCoord2f((bsrX2/len/2)+0.5f,0.5f-(fi-step)/360.0);
            glNormal3f(0, -ystart, -zstart);
            glVertex3f(bsrX2, -ystart, -zstart);
            glEnd();
            
            glBegin(GL_POINTS);
            for(int i=0;i<ii;i++){
                glTexCoord2f(0,0);
                glNormal3f(0, cos(fi/180.0*pie), sin(fi/180.0*pie));
                glVertex3f(upP[i][0], upP[i][1]*cos(fi/180.0*pie), upP[i][1]*sin(fi/180.0*pie));
                glNormal3f(0, -cos(fi/180.0*pie), -sin(fi/180.0*pie));
                glVertex3f(upP[i][0], -upP[i][1]*cos(fi/180.0*pie), -upP[i][1]*sin(fi/180.0*pie));
            }
            glEnd();
            glBegin(GL_LINES);
            glTexCoord2f(0,0);
            glNormal3f(-1.0, 0, 0);
            glVertex3f(upP[ii-1][0], upP[ii-1][1]*cos(fi/180.0*pie), upP[ii-1][1]*sin(fi/180.0*pie));
            glNormal3f(-1.0, 0, 0);
            glVertex3f(upP[ii-1][0], cos(fi/180.0*pie), sin(fi/180.0*pie));
            glTexCoord2f(0,0);
            glNormal3f(1.0, 0, 0);
            glVertex3f(upP[ii-1][0], -upP[ii-1][1]*cos(fi/180.0*pie), -upP[ii-1][1]*sin(fi/180.0*pie));
            glNormal3f(1.0, 0, 0);
            glVertex3f(upP[ii-1][0], -cos(fi/180.0*pie), -sin(fi/180.0*pie));
            
            glNormal3f(0, cos(fi/180.0*pie), sin(fi/180.0*pie));
            glTexCoord2f((upP[ii-1][0]+8)/16,1.0f-(fi-step)/360.0);
            glVertex3f(upP[ii-1][0], cos(fi/180.0*pie), sin(fi/180.0*pie));
            glNormal3f(0, cos(fi/180.0*pie), sin(fi/180.0*pie));
            glTexCoord2f((bsrX2+0.1+8)/16,1.0f-(fi-step)/360.0);
            glVertex3f(bsrX2+0.1, cos(fi/180.0*pie), sin(fi/180.0*pie));
            
            glNormal3f(0, -cos(fi/180.0*pie), -sin(fi/180.0*pie));
            glTexCoord2f((upP[ii-1][0]+8)/16,0.5f-(fi-step)/360.0);
            glVertex3f(upP[ii-1][0], -cos(fi/180.0*pie), -sin(fi/180.0*pie));
            glNormal3f(0, -cos(fi/180.0*pie), -sin(fi/180.0*pie));
            glTexCoord2f((bsrX2+0.1+8)/16,0.5f-(fi-step)/360.0);
            glVertex3f(bsrX2+0.1, -cos(fi/180.0*pie), -sin(fi/180.0*pie));
            glEnd();
            ystart=r*cos(fi/180.0*pie);
            zstart=r*sin(fi/180.0*pie);
        }
        
    }
    
    
}






/**
        Opengl 函数部分
 */

void init(){
    srand((int)time(NULL));
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_DST_ALPHA);
    
    glEnable(GL_POINT_SMOOTH);           //启用点反走样
    glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);
    
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT,GL_DONT_CARE);
    glEnable(GL_BLEND);
    
    glEnable(GL_DEPTH_TEST);  //深度测试
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glEnable(GL_MULTISAMPLE);  //多重采用
    
    
    glShadeModel(GL_SMOOTH);    // 启用阴影平滑
    
    
    {
        GLfloat sun_light_position[] = {10.0f, 5.0f, 0.0f, 1.0f}; //光源的位置在世界坐标系圆心，齐次坐标形式
        GLfloat sun_light_ambient[]   = {0.4f, 0.4f, 0.4f, 1.0f}; //RGBA模式的环境光，为0
        GLfloat sun_light_diffuse[]   = {1.0f, 1.0f, 1.0f, 1.0f}; //RGBA模式的漫反射光，全白光
        GLfloat sun_light_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};  //RGBA模式下的镜面光 ，全白光
        glLightfv(GL_LIGHT0, GL_POSITION, sun_light_position);
        glLightfv(GL_LIGHT0, GL_AMBIENT,   sun_light_ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE,   sun_light_diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, sun_light_specular);
        
        //开启灯光
        glEnable(GL_LIGHT0);
        glEnable(GL_LIGHTING);
    }
    
    for(int i=0;i<1200;++i){
        upP[i][0]=i/100.0-6;
        upP[i][1]=1.0;
    }
    

    LoadGLTextures("metal.jpg");
}
void display(void)
{
    clock_t c1=clock();
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(0, 0 ,15 , 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glBindTexture(GL_TEXTURE_2D, texD);
    glBegin(GL_TRIANGLES);
    ////        glColor3f(1, 0, 0);
    
    glTexCoord2f(0, 0);
    glNormal3f(0, 0, 1);
    glVertex3f(knifetop[0]-0.5, knifetop[1], knifetop[2]);
    glTexCoord2f(0, 1);
    glNormal3f(0, 0, 1);
    glVertex3f(knifeleft[0], knifeleft[1], knifeleft[2]);
    glTexCoord2f(1, 1);
    glNormal3f(0, 0, 1);
    glVertex3f(kniferight[0]-0.5, kniferight[1], kniferight[2]);
    
    glTexCoord2f(0, 0);
    glNormal3f(0, 0, 1);
    glVertex3f(knifetop[0]-0.5, knifetop[1], knifetop[2]);
    glTexCoord2f(1, 1);
    glNormal3f(0, 0, 1);
    glVertex3f(kniferight[0]-0.5, kniferight[1], kniferight[2]);
    glTexCoord2f(1, 0);
    glNormal3f(0, 0, 1);
    glVertex3f(knifetop[0], knifetop[1], knifetop[2]);
    
    glEnd();
    if(bsrMode&&ii<1200){
        topx=upP[ii][0];
        topy=upP[ii][1];
        updatetopx();
        updatetopy();
        hasParticle=true;
    }
    glBindTexture(GL_TEXTURE_2D, texB);
    drawline(int(step)%360);
    if(bsrMode&&ii<200) ii+=3;
    else if(bsrMode&&ii<1200) ii++;
    
    glDisable(GL_TEXTURE_2D);
    
    
    if(hasParticle){
        for(int i=0;i<100;i++){
            pa[i].update();
            if(!pa[i].alive) pa[i].init(topx, topy, 0.0);
            pa[i].draw();
        }
        hasParticle=false;
    }else{
        for(int i=0;i<100;i++){
            pa[i].update();
            pa[i].draw();
        }
    }
    clock_t c2=clock();
    if(c2-c1<40000) usleep((useconds_t)(40000-c2+c1));
    glutSwapBuffers();
    step += 8;
    
}

void loca(int w, int h)
{
    
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    gluPerspective(45.0, (GLfloat)w / (GLfloat)h, 1.0, 25.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
}

void keyboardFunc(unsigned char c, int x, int y)
{
    switch (c)
    {
        case 'c':
        case 'C':
            
            bsrMode=!bsrMode;
            if(bsrMode){
                initupP();
            }
            else{
                ii=1;
                if(up.begin()!=up.end()) {
                    up.clear();
                    
                }
                topx=GLfloat(-len+2.0);
                topy=GLfloat(r+0.5);
                updatetopx();
                updatetopy();
                upP[0][0]=-6.0;
                upP[0][1]=1.0;
                for(int i=0;i<1200;++i){
                    upP[i][0]=i/100.0-6;
                    upP[i][1]=1.0;
                }
            }
            break;
            
        case 'w':
            LoadGLTextures("wood.jpg");break;
        case 'm':
            LoadGLTextures("metal.jpg");break;
        case 'q':
        case 'Q':
            exit(0);
            break;
    }
}



void mouse(int button, int state, int x, int y)
{
    GLint viewport[4];
    GLdouble mvmatrix[16], projmatrix[16];
    GLint realy;  /*  OpenGL y coordinate position  */
    GLdouble wx, wy, wz;  /*  returned world x, y, z coords  */
    
    switch (button) {
        case GLUT_LEFT_BUTTON:
            if (state == GLUT_DOWN) {
                
                glGetIntegerv (GL_VIEWPORT, viewport);
                glGetDoublev (GL_MODELVIEW_MATRIX, mvmatrix);
                glGetDoublev (GL_PROJECTION_MATRIX, projmatrix);
                /*  note viewport[3] is height of window in pixels  */
                realy = viewport[3] - (GLint) y - 1;
                gluUnProject ((GLdouble) x, (GLdouble) realy, 0.0,
                              mvmatrix, projmatrix, viewport, &wx, &wy, &wz);
                gluUnProject ((GLdouble) x, (GLdouble) realy, 1.0,
                              mvmatrix, projmatrix, viewport, &wx, &wy, &wz);
                if(inknife(x, realy)) dragging=true;
            } else{
                dragging=false;
            }
            break;
        case GLUT_RIGHT_BUTTON:
            if (state == GLUT_DOWN){
                GLfloat X=x;
                GLfloat Y=y;
                convert2td(X, Y);
                cout<<X<<" "<<Y<<endl;
                if(-6<X&&X<0) bsrX1=X;
                if(0<X&&X<6.0)  bsrX2=X;
                cout<<bsrX1<<"   "<<bsrX2<<endl;
            }
               
            break;
        default:
            break;
    }
}

void mousemove(int x,int y){
    GLfloat xx=x;
    
    GLfloat yy=800-y-1;
    convert2td(xx, yy);//屏幕坐标转化opengl坐标 x->xx ,y->yy
    //    if(xx)
    if(dragging){
        if(xx>topx&&xx<GLfloat(len-2.0)) {
            topx=xx;
            updatetopx();
        }
        if(yy<topy+0.75&&yy>0.95){
            topy=yy-0.75;
            updatetopy();
        }
        if(yy>topy+0.75&&yy<2.25){
            topy=yy-0.75;
            updatetopy();
        }
        if(topy>r+0.2&&xx>GLfloat(-len+2.0)&&xx<GLfloat(len-2.0)){
            topx=xx;
            updatetopx();
        }
        
        int i=(xx+6)*100;
        if(topy<upP[i][1]){
            upP[i][1]=topy;
            if(i==0||upP[i-1][1]==1.0){
                up[i]=1;
            }
            else up[i]=0;
            hasParticle=true;
        }
    }
    
}

void OnTimer(int value)
{
    glutPostRedisplay();
    glutTimerFunc(64, OnTimer, value);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
    
    glutInitWindowSize(1200, 800);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);
    
    init();
    
    glutMouseFunc(mouse);
    glutMotionFunc(mousemove);
    glutDisplayFunc(display);
    glutReshapeFunc(loca);
    glutIdleFunc(display);
    glutKeyboardFunc(keyboardFunc);
    glutMainLoop();
    return 0;
}




