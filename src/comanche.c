//-------------------------------------------------------------
//Comanche GLSL Demo
//
//Version:      1.0
//Author:       Andrew Pinion
//Date:         October 2012
//Website:      http://andrewpc.com/openGL
//
//This program is FOSS (free open source software). It may
//be edited and redistrubuted by anyone, provided that
//this header is included without any modification.
//
//This program is provided "AS IS", with no warranty
//of any kind, expressed or implied.
//-------------------------------------------------------------

//---------------
//Directives
//---------------
#include <stdio.h>                  //for file input & output operations
#include <stdlib.h>
#include <GL/glew.h>                //GL extension wrangler
#include <GL/glut.h>                //GL Utility toolkit
#include <GL/gl.h>                  //openGL main header
#include "image.h"                  //image module
#include "obj.h"                    //object module

#define ROTSPEED 5                  //rotational velocity speed setting
#define SPEED 0.1                   //translate velocity speed setting

//---------------
//Prototypes
//---------------
void tilt();
GLuint makeProgram(char vshaderName[100], char fshaderName[100]);
void kbSkyToggle ();

//---------------------
//global variables
//---------------------

//model orientation and position
GLdouble rotateX = 0;               //angle of rotation (x-axis)
GLdouble rotateY = 180;             //angle of rotation (y-axis)
GLdouble rotateZ = 0;               //angle of rotation (z-axis)
GLdouble translateX = 0;            //translation along X-axis
GLdouble translateY = 0;            //translation along Y-axis
GLdouble translateZ = 5;            //translation along Z-axis
GLdouble scale = 1.0;               //model scalar - 1.0 is normal

//keyboard control variables
int wireFrame = 0;                  //wireframe mode setting
int skyMode = 0;                    //sky color setting

//mouse control variables
int click_button;                   //the mouse button that was clicked
GLdouble click_rotation_x;          //mouse rotation about x
GLdouble click_rotation_y;          //mouse rotation about y
GLdouble click_scale;               //mouse right-click zooming
GLdouble click_nx;                  //mouse motion on x
GLdouble click_ny;                  //mouse motion on y

//uniform program-side variables
GLfloat waveHeight = 0.1;           //wave shader height uniform
GLfloat waveSpeed = 0.003;          //wave shader movement speed
GLfloat hSpinSpeed = 0.01;          //horizontal spin shader speed
GLfloat vSpinSpeed = 0.01;          //vertical spin shader speed

GLfloat brickSize[2] = {1, 1};
GLfloat brickFrac[2] = {0.8, 0.8};
GLfloat brickColor[3] = {1.0, 0.0, 0.0};
GLfloat mortarColor[3] = {1.0, 1.0, 1.0};

//each object's selected shader
int skyCurrentProg = 5;
int rotorCurrentProg = 2;
int chopperCurrentProg = 6;
int waterCurrentProg = 1;
int tailCurrentProg = 3;

//shader programs
GLuint waveProgram;
GLuint rotateProgram;
GLuint vRotateProgram;
GLuint brickProgram;
GLuint skyProgram;
GLuint fullBrightProgram;
GLuint metalProgram;
GLuint colorShiftProgram;

//shader uniform locators
GLuint uniform_time;
GLuint uniform_waveHeight;
GLuint uniform_waveSpeed;
GLuint uniform_hSpinSpeed;
GLuint uniform_vSpinSpeed;
GLuint uniform_mortarColor;
GLuint uniform_brickColor;
GLuint uniform_brickSize;
GLuint uniform_brickFrac;

//object pointers
obj *water;
obj *chopper;
obj *rotor;
obj *fighter;
obj *sky;
obj *tail;

char *load(const char *name)
{
    FILE *fp = 0;
    void *p = 0;
    size_t n = 0;
    if ((fp = fopen(name, "rb")))                   // Open the file.
    {
        if (fseek(fp, 0, SEEK_END) == 0)            // Seek the end.
            if ((n = (size_t) ftell(fp)))           // Tell the length.
                if (fseek(fp, 0, SEEK_SET) == 0)    // Seek the beginning.
                    if ((p = calloc(n + 1, 1)))     // Allocate a buffer.
                        fread(p, 1, n, fp);         // Read the data.
        fclose(fp);                                 // Close the file.
    }
    return p;
}
//-----------------------------------------
//Keyboard and Mouse Control Functions
//-----------------------------------------

void scaleCheck()
{
    //keep the model's scale within reasonable bounds
    if (scale > 4) scale = 4;
    if (scale < 0.15) scale = 0.15;
}

//GLUT mouse motion function
void motion(int x, int y)
{
    //motion variable setup
    GLdouble nx = (GLdouble) x / glutGet(GLUT_WINDOW_WIDTH);
    GLdouble ny = (GLdouble) y / glutGet(GLUT_WINDOW_HEIGHT);
    GLdouble dx = nx - click_nx;
    GLdouble dy = ny - click_ny;

    //left mouse button rotation
    if (click_button == GLUT_LEFT_BUTTON)
    {
        //calculate the roation
        rotateX = click_rotation_x +  90.0 * dy;
        rotateY = click_rotation_y + 180.0 * dx;
		
        //rotational bounds checking
        if (rotateX >   90.0) rotateX  =  90.0;
        if (rotateX <  -90.0) rotateX  = -90.0;
        if (rotateY >  180.0) rotateY -= 360.0;
        if (rotateY < -180.0) rotateY += 360.0;
    }
    glutPostRedisplay();
}

//GLUT main mouse function
void mouse(int button, int state, int x, int y)
{
    //mouse wheel zooming
    if (button == 3) scale = scale + (scale * 0.05);
    if (button == 4) scale = scale - (scale * 0.05);

    //left mouse button rotations
    else
    {
        click_nx = (GLdouble) x / glutGet(GLUT_WINDOW_WIDTH);
        click_ny = (GLdouble) y / glutGet(GLUT_WINDOW_HEIGHT);

        click_button     = button;
        click_rotation_x = rotateX;
		click_rotation_y = rotateY;
		click_scale      = scale;
    }
    scaleCheck();
    glutPostRedisplay();
}

//number key 1 - wireframe toggle
void kbWireToggle ()
{
    if (wireFrame == 0)
    {
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        skyMode = 0;
        kbSkyToggle();
        wireFrame = 1;
    }
    else
    {
        glPolygonMode( GL_FRONT, GL_FILL );
        wireFrame = 0;
        skyMode = 5;
        kbSkyToggle();
    }
}

//number key 3 - sky color toggle
void kbSkyToggle ()
{
    if (skyMode == 0)
    {
        //space mode
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        skyMode++;
    }
    else if (skyMode == 1)
    {
        //gray mode
        glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
        skyMode++;
    }
    else if (skyMode == 2)
    {
        //fullbright mode
        glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
        skyMode++;
    }
    else
    {
        //normal sky
        glClearColor(0.29f, 0.31f, 0.36f, 0.0f);
        skyMode = 0;
    }
}

//number key 4 - orientation reset
void kbCenterObject()
{
    translateX = 0.0;
    translateY = 0.0;
    translateZ = 5.0;
    rotateX = 0;
    rotateY = 180;

    scale = 1.0;
    tilt();
}

//arrow key functions - rotation
void kbArrowLeft()  { rotateY += ROTSPEED; }
void kbArrowRight() { rotateY -= ROTSPEED; }
void kbArrowUp()    { rotateX += ROTSPEED; }
void kbArrowDown()  { rotateX -= ROTSPEED; }

//wasd key functions - translation
void kbW () { scale += 0.05; scaleCheck(); }
void kbS () { scale -= 0.05; scaleCheck(); }
void kbA () { translateX -= SPEED; }
void kbD () { translateX += SPEED; }

//GLUT main keyboard function
void keyboard(unsigned char key, int x, int y) 
{
    switch (key)
    {
        case 'w': kbW(); break;
        case 's': kbS(); break;
        case 'a': kbA(); break;
        case 'd': kbD(); break;
		
        case '1': kbWireToggle(); break;
        case '2': kbSkyToggle(); break;
        case '3': kbCenterObject(); break;
		
        //escape key
        case 27: exit(0);
    }
    glutPostRedisplay();
}

//GLUT arrow key function
void specialKeyboard(int key, int x, int y) 
{
    switch (key)
    {
        case GLUT_KEY_LEFT: kbArrowLeft(); break;
        case GLUT_KEY_RIGHT: kbArrowRight(); break;
        case GLUT_KEY_UP: kbArrowUp(); break;
        case GLUT_KEY_DOWN: kbArrowDown(); break;
    }
    glutPostRedisplay();
}

//----------------------------
//openGL Render Functions
//----------------------------

static void reshape(int w, int h)
{
    GLdouble x = 0.5 * (GLdouble) w / (GLdouble) h;
    GLdouble y = 0.5;

    //view setup
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-x, x, -y, y, 1.0, 900.0);

    glViewport(0, 0, w, h);
}

static void idle(void)
{
    glutPostRedisplay();
}

void chooseProgram(int shaderID)
{
    switch (shaderID)
    {
        case 0: { glUseProgram(0); break; }
        case 1:
        { 
            glUseProgram(waveProgram);
            
            uniform_time = glGetUniformLocation(waveProgram, "time");
            glUniform1f(uniform_time, glutGet(GLUT_ELAPSED_TIME));
            uniform_waveHeight = glGetUniformLocation(waveProgram, "height");
            glUniform1f(uniform_waveHeight, waveHeight);
            uniform_waveSpeed = glGetUniformLocation(waveProgram, "speed");
            glUniform1f(uniform_waveSpeed, waveSpeed);
            break;
        }
        case 2:
        { 
            glUseProgram(rotateProgram);
            
            uniform_time = glGetUniformLocation(rotateProgram, "time");
            glUniform1f(uniform_time, glutGet(GLUT_ELAPSED_TIME));
            uniform_hSpinSpeed = glGetUniformLocation(rotateProgram, "speed");
            glUniform1f(uniform_hSpinSpeed, hSpinSpeed);
            break; 
        }
        case 3: 
        { 
            glUseProgram(vRotateProgram); 
            
            uniform_time = glGetUniformLocation(vRotateProgram, "time");
            glUniform1f(uniform_time, glutGet(GLUT_ELAPSED_TIME));
            uniform_vSpinSpeed = glGetUniformLocation(vRotateProgram, "speed");
            glUniform1f(uniform_vSpinSpeed, vSpinSpeed);
            break;
        }
        case 4:
        { 
            glUseProgram(brickProgram);
            uniform_brickColor = glGetUniformLocation(brickProgram, "brick_color");
            glUniform3f(uniform_brickColor, brickColor[0],brickColor[1],brickColor[2]);
            
            uniform_mortarColor = glGetUniformLocation(brickProgram, "mortar_color");
            glUniform3f(uniform_mortarColor, mortarColor[0],mortarColor[1],mortarColor[2]);
            
            uniform_brickSize = glGetUniformLocation(brickProgram, "brick_size");
            glUniform2f(uniform_brickSize, brickSize[0],brickSize[1]);
            
            uniform_brickFrac = glGetUniformLocation(brickProgram, "brick_frac");
            glUniform2f(uniform_brickFrac, brickFrac[0], brickFrac[1]);
            break;
        }
        case 5: { glUseProgram(fullBrightProgram); break; }
        case 6: { glUseProgram(metalProgram); break; }
        case 7:
        { 
            glUseProgram(colorShiftProgram);
            uniform_time = glGetUniformLocation(colorShiftProgram, "time");
            glUniform1f(uniform_time, glutGet(GLUT_ELAPSED_TIME));
            break;
        }
    }
}

static void display(void)
{	
    //setup
    glMatrixMode(GL_MODELVIEW);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    //modify scene based on user controls
    glTranslatef(0.0, 0.0, -7);
    glTranslatef(translateX, translateY, 0.0);
    glRotated(rotateX, 1.0, 0.0, 0.0);
    glRotated(rotateY, 0.0, 1.0, 0.0);
    glScaled(scale,scale,scale);
    
    //draw the sky
    glPushMatrix();
    {
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        chooseProgram(skyCurrentProg);
        glTranslatef(0.0, 35.0, 0.0);
        glScaled(2.5,2.5,2.5);
        obj_render(sky);
        glUseProgram(0);
    }
    glPopMatrix();
    
    //draw rotor
    glPushMatrix();
    {
        chooseProgram(rotorCurrentProg);
        glScaled(0.25,0.25,0.25);
        glTranslatef(-1.0, -0.06, 0.0);
        obj_render(rotor);
        glUseProgram(0);
    }
    glPopMatrix();
    
    //draw tail rotor
    glPushMatrix();
    {
        chooseProgram(tailCurrentProg);
        glScaled(0.25,0.25,0.25);
        glTranslatef(5.45, 1.4, 0.0);
        obj_render(tail);
        glUseProgram(0);
    }
    glPopMatrix();
    
     //draw chopper
    glPushMatrix();
    {
        chooseProgram(chopperCurrentProg);
        glScaled(0.25,0.25,0.25);
        obj_render(chopper);
    }
    glPopMatrix();
    
    //draw water
    glPushMatrix();
    {
        chooseProgram(waterCurrentProg);
        glTranslatef(0.0, -6.0, 0.0);
        glScaled(3,3,3);
        obj_render(water);
        glUseProgram(0);
    }
    glPopMatrix();
    
    glutSwapBuffers();
}

GLuint makeProgram(char vshaderName[100], char fshaderName[100])
{
    GLchar *p;
    GLint s, n;
    GLuint program;
    GLuint frag_shader;
    GLuint vert_shader;

    //vert shader
    vert_shader = glCreateShader(GL_VERTEX_SHADER);
    GLchar *vert_text = load(vshaderName);
    glShaderSource (vert_shader, 1, (const GLchar **) &vert_text, 0);
    glCompileShader(vert_shader);
    free(vert_text);
		
    glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &s);
    glGetShaderiv(vert_shader, GL_INFO_LOG_LENGTH, &n);
    if ((s == 0) && (p = (GLchar *) calloc(n + 1, 1)))
    {
        glGetShaderInfoLog(vert_shader, n, NULL, p);
        fprintf(stderr, "OpenGL Vert Shader Error:\n%s", p);
        free(p);
    }
	
    //frag shader
    frag_shader = glCreateShader(GL_FRAGMENT_SHADER);	
    GLchar *frag_text = load(fshaderName);
    glShaderSource (frag_shader, 1, (const GLchar **) &frag_text, 0);
    glCompileShader(frag_shader);
    free(frag_text);
	
    glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &s);
    glGetShaderiv(frag_shader, GL_INFO_LOG_LENGTH, &n);
    if ((s == 0) && (p = (GLchar *) calloc(n + 1, 1)))
    {
        glGetShaderInfoLog(frag_shader, n, NULL, p);
        fprintf(stderr, "OpenGL Frag Shader Error:\n%s", p);
        free(p);
    }

    program = glCreateProgram();
    glAttachShader(program, vert_shader);
    glAttachShader(program, frag_shader);
    glLinkProgram(program);
	
    glGetProgramiv(program, GL_LINK_STATUS, &s);
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &n);
    if ((s == 0) && (p = (GLchar *) calloc(n + 1, 1)))
    {
        glGetProgramInfoLog(program, n, NULL, p);
        fprintf(stderr, "OpenGL Program Error:\n%s", p);
        free(p);
    }
    return program;
}

void tilt()
{
    skyMode = 2;
    kbSkyToggle();
    waveHeight = 0.1;
    waveSpeed = 0.003;
    hSpinSpeed = 0.01;
    vSpinSpeed = 0.01;
    
    skyCurrentProg = 5;
    rotorCurrentProg = 2;
    chopperCurrentProg = 6;
    waterCurrentProg = 1;
    brickSize[0] = 1;
    brickSize[1] = 1;
    brickFrac[0] = 0.8;
    brickFrac[1] = 0.8;
    brickColor[0] = 1.0;
    brickColor[1] = 0.0;
    brickColor[2] = 0.0;
    mortarColor[0] = 1.0;
    mortarColor[1] = 1.0;
    mortarColor[2] = 1.0;
}


//---------
//Main
//---------

void menuEvents(int menuOption)
{
    switch (menuOption)
    {
        case 1: { waveHeight += 0.1; break; }
        case 2: { if (waveHeight > 0) waveHeight -= 0.1; break; }
        case 3: { waveSpeed += 0.003; break; }
        case 4: { if (waveSpeed > 0) waveSpeed -= 0.003; break; }
        
        case 11: { hSpinSpeed += 0.0025; break; }
        case 12: { hSpinSpeed -= 0.0025; break; }   
        case 13: { hSpinSpeed = 0; break; }

        case 21: { vSpinSpeed += 0.0025; break; }
        case 22: { vSpinSpeed -= 0.0025; break; }   
        case 23: { vSpinSpeed = 0; break; }

        case 31: { mortarColor[0] = 1.0; mortarColor[1] = 1.0; mortarColor[2] = 1.0; break; }
        case 32: { mortarColor[0] = 0.0; mortarColor[1] = 0.0; mortarColor[2] = 0.0; break; }
        case 33: { brickColor[0] = 1.0; brickColor[1] = 0.0; brickColor[2] = 0.0; break; }
        case 34: { brickColor[0] = 0.0; brickColor[1] = 1.0; brickColor[2] = 0.0; break; }
        case 35: { brickColor[0] = 0.0; brickColor[1] = 0.0; brickColor[2] = 1.0; break; }
        case 36: { brickFrac[0] += 0.1; brickFrac[1] += 0.1; break; }
        case 37: { brickFrac[0] -= 0.1; brickFrac[1] -= 0.1; break; }

        case 51: { waterCurrentProg = 1; break; }
        case 52: { waterCurrentProg = 2; break; }
        case 53: { waterCurrentProg = 3; break; }
        case 54: { waterCurrentProg = 4; break; }
        case 55: { waterCurrentProg = 5; break; }
        case 56: { waterCurrentProg = 6; break; }
        case 57: { waterCurrentProg = 7; break; }
        case 58: { waterCurrentProg = 0; break; }
        
        case 61: { rotorCurrentProg = 1; break; }
        case 62: { rotorCurrentProg = 2; break; }
        case 63: { rotorCurrentProg = 3; break; }
        case 64: { rotorCurrentProg = 4; break; }
        case 65: { rotorCurrentProg = 5; break; }
        case 66: { rotorCurrentProg = 6; break; }
        case 67: { rotorCurrentProg = 7; break; }
        case 68: { rotorCurrentProg = 0; break; }

        case 71: { chopperCurrentProg = 1; break; }
        case 72: { chopperCurrentProg = 2; break; }
        case 73: { chopperCurrentProg = 3; break; }
        case 74: { chopperCurrentProg = 4; break; }
        case 75: { chopperCurrentProg = 5; break; }
        case 76: { chopperCurrentProg = 6; break; }
        case 77: { chopperCurrentProg = 7; break; }
        case 78: { chopperCurrentProg = 0; break; }
        
        case 81: { skyCurrentProg = 1; break; }
        case 82: { skyCurrentProg = 2; break; }
        case 83: { skyCurrentProg = 3; break; }
        case 84: { skyCurrentProg = 4; break; }
        case 85: { skyCurrentProg = 5; break; }
        case 86: { skyCurrentProg = 6; break; }
        case 87: { skyCurrentProg = 7; break; }
        case 88: { skyCurrentProg = 0; break; }
    }
}

void buildMenus()
{
    GLuint mainMenu, waveMenu, hSpinMenu, vSpinMenu, brickMenu;
    GLuint waterShaderSelectMenu, rotorShaderSelectMenu, chopperShaderSelectMenu, skyShaderSelectMenu;

    waveMenu = glutCreateMenu(menuEvents);
    glutAddMenuEntry("Bigger Waves", 1);
    glutAddMenuEntry("Smaller Waves", 2);
    glutAddMenuEntry("Faster Waves", 3);
    glutAddMenuEntry("Slower Waves", 4);
    
    hSpinMenu = glutCreateMenu(menuEvents);
    glutAddMenuEntry("Spin Up", 11);
    glutAddMenuEntry("Spin Down", 12);
    glutAddMenuEntry("Stop Spinning", 13);
    
    vSpinMenu = glutCreateMenu(menuEvents);
    glutAddMenuEntry("Spin Up", 21);
    glutAddMenuEntry("Spin Down", 22);
    glutAddMenuEntry("Stop Spinning", 23);
    
    brickMenu = glutCreateMenu(menuEvents);
    glutAddMenuEntry("White Mortar", 31);
    glutAddMenuEntry("Black Mortar", 32);
    glutAddMenuEntry("Red Bricks", 33);
    glutAddMenuEntry("Green Bricks", 34);
    glutAddMenuEntry("Blue Bricks", 35);
    glutAddMenuEntry("Less Mortar", 36);
    glutAddMenuEntry("More Mortar", 37);
    
    waterShaderSelectMenu = glutCreateMenu(menuEvents);
    glutAddMenuEntry("Waves", 51);
    glutAddMenuEntry("Horizontal Spin", 52);
    glutAddMenuEntry("Vertical Spin", 53);
    glutAddMenuEntry("Bricks", 54);
    glutAddMenuEntry("Fullbright", 55);
    glutAddMenuEntry("Polished Metal", 56);
    glutAddMenuEntry("Color Shift", 57);
    glutAddMenuEntry("Fixed Function", 58);
    
    rotorShaderSelectMenu = glutCreateMenu(menuEvents);
    glutAddMenuEntry("Waves", 61);
    glutAddMenuEntry("Horizontal Spin", 62);
    glutAddMenuEntry("Vertical Spin", 63);
    glutAddMenuEntry("Bricks", 64);
    glutAddMenuEntry("Fullbright", 65);
    glutAddMenuEntry("Polished Metal", 66);
    glutAddMenuEntry("Color Shift", 67);
    glutAddMenuEntry("Fixed Function", 68);
    
    chopperShaderSelectMenu = glutCreateMenu(menuEvents);
    glutAddMenuEntry("Waves", 71);
    glutAddMenuEntry("Horizontal Spin", 72);
    glutAddMenuEntry("Vertical Spin", 73);
    glutAddMenuEntry("Bricks", 74);
    glutAddMenuEntry("Fullbright", 75);
    glutAddMenuEntry("Polished Metal", 76);
    glutAddMenuEntry("Color Shift", 77);
    glutAddMenuEntry("Fixed Function", 78);
    
    skyShaderSelectMenu = glutCreateMenu(menuEvents);
    glutAddMenuEntry("Waves", 81);
    glutAddMenuEntry("Horizontal Spin", 82);
    glutAddMenuEntry("Vertical Spin", 83);
    glutAddMenuEntry("Bricks", 84);
    glutAddMenuEntry("Fullbright", 85);
    glutAddMenuEntry("Polished Metal", 86);
    glutAddMenuEntry("Color Shift", 87);
    glutAddMenuEntry("Fixed Function", 88);
    
    mainMenu = glutCreateMenu(menuEvents);
    glutAddSubMenu("Wave Shader Settings", waveMenu);
    glutAddSubMenu("H-Spin Shader Settings", hSpinMenu);
    glutAddSubMenu("V-Spin Shader Settings", vSpinMenu);
    glutAddSubMenu("Brick Shader Settings", brickMenu);
    glutAddMenuEntry(" ", 99);    
    glutAddSubMenu("Change Water's Shader", waterShaderSelectMenu);
    glutAddSubMenu("Change Rotor's Shader", rotorShaderSelectMenu);
    glutAddSubMenu("Change Chopper's Shader", chopperShaderSelectMenu);
    glutAddSubMenu("Change The Sky's Shader", skyShaderSelectMenu);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

int main(int argc, char *argv[])
{
    //initialize GLUT and GLEW
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE);
    glutInit(&argc, argv);
    glutInitWindowSize(800, 600);
    glutCreateWindow(argv[0]);
    glutSetWindowTitle("Object Viewer");
    glewInit();
	
    //GLUT function connections
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeyboard);
    glutMotionFunc(motion);
    glutMouseFunc(mouse);
	
    //object allocations
    chopper = obj_create("chopper.obj");
    fighter = obj_create("fighter.obj");
    water = obj_create("water.obj");
    rotor = obj_create("rotor.obj");
    tail = obj_create("tail.obj");
    sky = obj_create("skyball.obj");
    
    //wave shader setup
    waveProgram = makeProgram("water.vert", "fullbright.frag");
    rotateProgram = makeProgram("hrot.vert", "basic.frag");
    vRotateProgram = makeProgram("vrot.vert", "basic.frag");
    brickProgram = makeProgram("brick.vert", "brick.frag");
    fullBrightProgram = makeProgram("basic.vert", "fullbright.frag");
    metalProgram = makeProgram("metal.vert", "metal.frag");
    colorShiftProgram = makeProgram("colorshift.vert", "colorshift.frag");
    
    buildMenus();
    
    //default settings
    tilt();
    glPolygonMode( GL_FRONT, GL_FILL );
    
    //openGL Toggles and Initializations
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
    
    //lighting configuration
    GLfloat LightPosition[] = {0.0, 1.0, 1.0, 0.0};
    glShadeModel(GL_SMOOTH);
    glLightfv (GL_LIGHT0, GL_POSITION, LightPosition);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
	
    //render the scene via glutMainLoop.
    if (glewInit() == GLEW_OK)
        glutMainLoop();
    return 0;
}

