// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>

// Include GLEW
#include <GL/glew.h>

// Include GLM
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <GL/glut.h>

using namespace glm;
using Vec3 = glm::vec3;

#include "src/shader.hpp"
#include "src/objloader.hpp"

// question
int question = 5;
int const Q1_2 = 1;
int const Q1_3 = 2;
int const Q1_4 = 3;
int const Q3 = 4;
int const Q4 = 5;

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
glm::vec3 camera_position   = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 camera_target = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camera_up    = glm::vec3(0.0f, 1.0f,  0.0f);

// timing
float deltaTime = 0.1f;	// time between current frame and last frame
float lastFrame = 0.0f;

//rotation
float angle = 0.;
float zoom = 1.;

static GLint window;
static bool mouseRotatePressed = false;
static bool mouseMovePressed = false;
static bool mouseZoomPressed = false;
static int lastX=0, lastY=0, lastZoom=0;
static bool fullScreen = false;


GLuint programID;
GLuint VertexArrayID;
GLuint LightID;

//chaise
std::vector<unsigned short> chair_indices; //Triangles concaténés dans une liste
std::vector<std::vector<unsigned short> > chair_triangles;
std::vector<glm::vec3> chair_indexed_vertices;
GLuint chair_vertexbuffer;
GLuint chair_elementbuffer;
//suzanne
std::vector<unsigned short> suzanne_indices; //Triangles concaténés dans une liste
std::vector<std::vector<unsigned short> > suzanne_triangles;
std::vector<glm::vec3> suzanne_indexed_vertices;
GLuint suzanne_vertexbuffer;
GLuint suzanne_elementbuffer;
//sphere
std::vector<unsigned short> sphere_indices; //Triangles concaténés dans une liste
std::vector<std::vector<unsigned short> > sphere_triangles;
std::vector<glm::vec3> sphere_indexed_vertices;
GLuint sphere_vertexbuffer;
GLuint sphere_elementbuffer;


glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 matrice_identity(1.0f);

//scale
float shader_scale = 1;
GLfloat shader_scaleLocation;
//translate 
Vec3 shader_translate = Vec3(0,0,0);
GLfloat shader_translateLocation;

//rotation
bool rotation=false;

glm::mat4 getViewMatrix(){
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix(){
	return ProjectionMatrix;
}


// Initial position : on +Z
glm::vec3 position = glm::vec3( 0, 0, 0 );
// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// Initial vertical angle : none
float verticalAngle = 0.0f;
// Initial Field of View
float initialFoV = 45.0f;

float speed = 3.0f; // 3 units / second
float mouseSpeed = 0.005f;


	// Right vector
glm::vec3 rightVector() {
    return glm::vec3(
		sin(horizontalAngle - 3.14f/2.0f),
		0,
		cos(horizontalAngle - 3.14f/2.0f)
	);
}

// Direction : Spherical coordinates to Cartesian coordinates conversion
glm::vec3 directionVector() {
    return glm::vec3(
        cos(verticalAngle) * sin(horizontalAngle),
        sin(verticalAngle),
        cos(verticalAngle) * cos(horizontalAngle)
    );
}

void computeMatricesFromInputs(float moveX, float moveY);
void initLight ();
void init ();
void draw ();
void drawChairModel();
void drawSuzanneModel();
void drawSphereModel();
void drawOneChair();
void drawMultipleChair();
void drawSuzanne();
void drawWithCam();
void drawSolarSystem();
void display ();
void idle ();
void key (unsigned char keyPressed, int x, int y);
void mouse (int button, int state, int x, int y);
void motion (int x, int y);
void reshape(int w, int h);
int main (int argc, char ** argv);
void printMatrix(const glm::mat4& mat);
void aide();
void changeQuestion();

// ------------------------------------

void aide(){
    std::cout << "- - - - - - - - - -" << std::endl;
    std::cout << "Touche :" << std::endl;
    std::cout << "q : quitter la fenetre" << std::endl;
    std::cout << "h : donne les touches a utiliser" << std::endl;
    std::cout << "r : enclenche la rotation automatique" << std::endl;
    std::cout << "/ : montre le visuel de la prochaine question\n/!/ selon les questions les touchesS ne seront pas forcement les mêmes" << std::endl;
    switch (question)
    {
    case Q1_2:
        std::cout << "- : reduit la chaise" << std::endl;
        std::cout << "+ : grossis la chaise" << std::endl;
        std::cout << "6 : decale la chaise vers la droite" << std::endl;
        std::cout << "4 : decale la chaise vers la gauche" << std::endl;
        std::cout << "8 : decale la chaise vers le haut" << std::endl;
        std::cout << "2 : decale la chaise vers le bas" << std::endl;
        break;
    case Q1_3:
        std::cout << "6 : fait tourner la chaise dans le sens anti-horaire" << std::endl;
        std::cout << "4 : fait tourner la chaise dans le sens horaire" << std::endl;
        break;
    case Q4:
    case Q3:
        std::cout << "- : dezoom" << std::endl;
        std::cout << "+ : zoom" << std::endl;
        std::cout << "/!/ il y a possibilité de bouger la camera avec la souris" << std::endl;
    case Q1_4:
        std::cout << "6 : realise une rotation sur x de l'entite" << std::endl;
        std::cout << "4 : realise une rotation sur x de l'entite" << std::endl;
        break;
    default:
        break;
    }

}

void changeQuestion(){
    shader_scale=1;
    shader_translate=Vec3(0,0,0);
    angle=0;
    if (question<5){question++;}
    else {question=1;}
    switch (question)
    {
    case Q1_2:
        glutSetWindowTitle("TP3-HAI719I-Q1.2");
        break;
    case Q1_3:
        glutSetWindowTitle("TP3-HAI719I-Q1.3");
        break;
    case Q1_4:
        glutSetWindowTitle("TP3-HAI719I-Q1.4");
        break;
    case Q3:
        glutSetWindowTitle("TP3-HAI719I-Q3");
        break;
    case Q4:
        glutSetWindowTitle("TP3-HAI719I-Q4");
        break;
    default:
        break;
    }
}

void printMatrix(const glm::mat4& mat) {
    std::cout << mat[0][0] << " " << mat[1][0] << " " << mat[2][0] << " " << mat[3][0] << "\n" << mat[0][1] << " " << mat[1][1] << " " << mat[2][1] << " " << mat[3][1] << "\n" << mat[0][2] << " " << mat[1][2] << " " << mat[2][2] << " " << mat[3][2] << "\n" << mat[0][3] << " " << mat[1][3] << " " << mat[2][3] << " " << mat[3][3] << std::endl;
}

void initLight () {
    GLfloat light_position1[4] = {22.0f, 16.0f, 50.0f, 0.0f};
    GLfloat direction1[3] = {-52.0f,-16.0f,-50.0f};
    GLfloat color1[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat ambient[4] = {0.3f, 0.3f, 0.3f, 0.5f};

    glLightfv (GL_LIGHT1, GL_POSITION, light_position1);
    glLightfv (GL_LIGHT1, GL_SPOT_DIRECTION, direction1);
    glLightfv (GL_LIGHT1, GL_DIFFUSE, color1);
    glLightfv (GL_LIGHT1, GL_SPECULAR, color1);
    glLightModelfv (GL_LIGHT_MODEL_AMBIENT, ambient);
    glEnable (GL_LIGHT1);
    glEnable (GL_LIGHTING);
}

void init () {
    // camera.resize (SCREENWIDTH, SCREENHEIGHT);
    initLight ();
    // glCullFace (GL_BACK);
    // glEnable (GL_CULL_FACE);
    glDepthFunc (GL_LESS);
    glEnable (GL_DEPTH_TEST);
    glClearColor (0.2f, 0.2f, 0.3f, 1.0f);
    glEnable(GL_COLOR_MATERIAL);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return;
    }

}




// ------------------------------------
// rendering.
// ------------------------------------

void draw () {
    glUseProgram(programID);
    glEnableVertexAttribArray(0);
    if(rotation){angle++;}
    switch (question)
    {
    case Q1_2:
        drawOneChair();
        break;
    case Q1_3:
        drawMultipleChair();
        break;
    case Q1_4:
        drawSuzanne();
        break;
    case Q3:
        drawWithCam();
        break;
    case Q4:
        drawSolarSystem();
        break;
    default:
        break;
    }
    glDisableVertexAttribArray(0);
}

void drawChairModel(){
    // 1rst attribute buffer : vertices
    glBindBuffer(GL_ARRAY_BUFFER, chair_vertexbuffer);
    glVertexAttribPointer(
                0,                  // attribute
                3,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalized?
                0,                  // stride
                (void*)0            // array buffer offset
                );

    // Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chair_elementbuffer);

    // Draw the triangles !
    glDrawElements(
                GL_TRIANGLES,      // mode
                chair_indices.size(),    // count
                GL_UNSIGNED_SHORT,   // type
                (void*)0           // element array buffer offset
    );
}
void drawSuzanneModel(){
    // 1rst attribute buffer : vertices
    glBindBuffer(GL_ARRAY_BUFFER, suzanne_vertexbuffer);
    glVertexAttribPointer(
                0,                  // attribute
                3,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalized?
                0,                  // stride
                (void*)0            // array buffer offset
                );

    // Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, suzanne_elementbuffer);

    // Draw the triangles !
    glDrawElements(
                GL_TRIANGLES,      // mode
                suzanne_indices.size(),    // count
                GL_UNSIGNED_SHORT,   // type
                (void*)0           // element array buffer offset
    );
}
void drawSphereModel(){
    // 1rst attribute buffer : vertices
    glBindBuffer(GL_ARRAY_BUFFER, sphere_vertexbuffer);
    glVertexAttribPointer(
                0,                  // attribute
                3,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalized?
                0,                  // stride
                (void*)0            // array buffer offset
                );

    // Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere_elementbuffer);

    // Draw the triangles !
    glDrawElements(
                GL_TRIANGLES,      // mode
                sphere_indices.size(),    // count
                GL_UNSIGNED_SHORT,   // type
                (void*)0           // element array buffer offset
    );
}
void drawOneChair(){
    GLfloat MVPlocation = glGetUniformLocation(programID, "MVP");
    shader_scaleLocation = glGetUniformLocation(programID, "scale");
    shader_translateLocation = glGetUniformLocation(programID, "translate");
    glUniform3fv(shader_translateLocation,1,&shader_translate[0]);
    glUniform1f(shader_scaleLocation,shader_scale);
    glUniformMatrix4fv(MVPlocation,1,GL_FALSE,&matrice_identity[0][0]);
    drawChairModel();
}

void drawMultipleChair(){
    // Afficher une premiere chaise
    GLfloat MVPlocation = glGetUniformLocation(programID, "MVP");
    shader_scaleLocation = glGetUniformLocation(programID, "scale");
    shader_translateLocation = glGetUniformLocation(programID, "translate");
    mat4 static_MVP1=matrice_identity;
    Vec3 translate_vector=Vec3(-1,-2,0);
    float chair_scale=2;
    glUniformMatrix4fv(MVPlocation,1,GL_FALSE,&static_MVP1[0][0]);
    glUniform3fv(shader_translateLocation,1,&translate_vector[0]);
    glUniform1f(shader_scaleLocation,chair_scale);
    drawChairModel();

    // Afficher une seconde chaise
    mat4 static_MVP2=matrice_identity;
    static_MVP2[0][0]=-1;
    glUniformMatrix4fv(MVPlocation,1,GL_FALSE,&static_MVP2[0][0]);
    drawChairModel();


    // Afficher une troisieme chaise!
    Vec3 centre_gravite=Vec3(0,0.5,0);
    mat4 MVP=translate(matrice_identity, centre_gravite);
    MVP=rotate(MVP, glm::radians(angle), Vec3(0,0,1));
    MVP=translate(MVP, -centre_gravite);
    Vec3 position_initiale=Vec3(0,0,0);
    glUniform3fv(shader_translateLocation,1,&position_initiale[0]);
    glUniform1f(shader_scaleLocation,1);
    glUniformMatrix4fv(MVPlocation,1,GL_FALSE,&MVP[0][0]);
    drawChairModel();
}
void drawSuzanne(){
    GLfloat MVPlocation = glGetUniformLocation(programID, "MVP");
    shader_scaleLocation = glGetUniformLocation(programID, "scale");
    shader_translateLocation = glGetUniformLocation(programID, "translate");

    Vec3 axe_rotation=normalize(cross(Vec3(0,1,0),Vec3(1,1,1)));
    float cosAngle = dot(Vec3(0,1,0),Vec3(1,1,1));
    Vec3 centre_gravite=Vec3(0,0.5,0);
    mat4 MVP=rotate(matrice_identity, atan(cosAngle), axe_rotation);
    MVP=translate(MVP, centre_gravite);
    MVP=rotate(MVP, glm::radians(angle), Vec3(0,1,0));
    MVP=translate(MVP, -centre_gravite);
    Vec3 position_initiale=Vec3(0,0,0);
    glUniform3fv(shader_translateLocation,1,&position_initiale[0]);
    glUniform1f(shader_scaleLocation,2);
    glUniformMatrix4fv(MVPlocation,1,GL_FALSE,&MVP[0][0]);
    drawSuzanneModel();
}
void drawWithCam(){
    GLfloat MVPlocation = glGetUniformLocation(programID, "MVP");
    shader_scaleLocation = glGetUniformLocation(programID, "scale");
    shader_translateLocation = glGetUniformLocation(programID, "translate");
    Vec3 axe_rotation=normalize(cross(Vec3(0,1,0),Vec3(1,1,1)));
    float cosAngle = dot(Vec3(0,1,0),Vec3(1,1,1));
    Vec3 centre_gravite=Vec3(0,0.5,0);
    mat4 MVP=mat4(1);
    MVP=MVP*ProjectionMatrix*ViewMatrix;
    MVP=rotate(MVP, atan(cosAngle), axe_rotation);
    MVP=translate(MVP, centre_gravite);
    MVP=rotate(MVP, glm::radians(angle), Vec3(0,1,0));
    MVP=translate(MVP, -centre_gravite);
    Vec3 position_initiale=Vec3(0,0,0);
    glUniform3fv(shader_translateLocation,1,&position_initiale[0]);
    glUniform1f(shader_scaleLocation,2);
    glUniformMatrix4fv(MVPlocation,1,GL_FALSE,&MVP[0][0]);
    drawSuzanneModel();
}
void drawSolarSystem(){
    GLfloat MVPlocation = glGetUniformLocation(programID, "MVP");
    shader_scaleLocation = glGetUniformLocation(programID, "scale");
    shader_translateLocation = glGetUniformLocation(programID, "translate");
    // soleil
    Vec3 centre_gravite=Vec3(0,0.5,0);
    mat4 MVPsoleil=mat4(1);
    MVPsoleil=MVPsoleil*ProjectionMatrix*ViewMatrix;
    MVPsoleil=translate(MVPsoleil, centre_gravite);
    MVPsoleil=rotate(MVPsoleil, glm::radians(angle), Vec3(0,1,0));
    MVPsoleil=translate(MVPsoleil, -centre_gravite);
    Vec3 position_initiale=Vec3(0,0,0);
    glUniform3fv(shader_translateLocation,1,&position_initiale[0]);
    glUniform1f(shader_scaleLocation,1);
    glUniformMatrix4fv(MVPlocation,1,GL_FALSE,&MVPsoleil[0][0]);
    drawSphereModel();
    
    //terre
    mat4 MVPterre=mat4(1);

    MVPterre=MVPterre*ProjectionMatrix*ViewMatrix;
    MVPterre=rotate(MVPterre, glm::radians(angle), Vec3(0,1,0));
    Vec3 position_terre=Vec3(2,0,0);
    MVPterre=translate(MVPterre, position_terre);
    MVPterre=rotate(MVPterre, glm::radians(23.44f), Vec3(1,0,0));
    glUniform3fv(shader_translateLocation,1,&position_initiale[0]);
    glUniform1f(shader_scaleLocation,10);
    glUniformMatrix4fv(MVPlocation,1,GL_FALSE,&MVPterre[0][0]);
    drawSphereModel();

    //lune

    mat4 MVPlune=mat4(1);
    MVPlune=MVPlune*ProjectionMatrix*ViewMatrix;
    MVPlune=rotate(MVPlune, glm::radians(angle), Vec3(0,1,0));
    MVPlune=translate(MVPlune, position_terre);
    Vec3 position_lune=Vec3(0.5,0,0);
    MVPlune=rotate(MVPlune, glm::radians(angle), Vec3(0,1,0));
    MVPlune=translate(MVPlune, position_lune);
    MVPterre=rotate(MVPterre, glm::radians(6.68f), Vec3(1,0,0));
    /*
    //MVPlune=translate(MVPlune, centre_gravite);
    //MVPlune=rotate(MVPlune, glm::radians(23.44f), Vec3(1,1,1));
    //MVPlune=translate(MVPlune, -centre_gravite);

    MVPlune=rotate(MVPlune, glm::radians(rotation_dynamique), Vec3(0,1,0));

    Vec3 position_terre=Vec3(4,0,0);
    */
    glUniform3fv(shader_translateLocation,1,&position_initiale[0]);
    glUniform1f(shader_scaleLocation,50);
    glUniformMatrix4fv(MVPlocation,1,GL_FALSE,&MVPlune[0][0]);
   drawSphereModel();
}


void display () {
    glLoadIdentity ();
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // camera.apply ();
    draw ();
    glFlush ();
    glutSwapBuffers ();
}

void idle () {
    glutPostRedisplay ();
    float time = glutGet(GLUT_ELAPSED_TIME) / 1000.f;
    deltaTime = time - lastFrame;
    lastFrame = time;
}

void key (unsigned char keyPressed, int x, int y) {
    switch (keyPressed) {
    case 'f':
        if (fullScreen == true) {
            glutReshapeWindow (SCR_WIDTH, SCR_HEIGHT);
            fullScreen = false;
        } else {
            glutFullScreen ();
            fullScreen = true;
        }
        break;
    case 'q':
        exit (0);
        break;
    case 'h':
        aide();
        break;
    case '/':
        changeQuestion();
        break;
    case '+': //Press + key to increase scale
        shader_scale-=0.1;
        camera_position+=0.1f*camera_target;
        computeMatricesFromInputs(0.f,0.f);
        break;
    case '-': //Press - key to decrease scale
        shader_scale+=0.1;
        camera_position-=0.1f*camera_target;
        computeMatricesFromInputs(0.f,0.f);
        break;
    case '8': 
        shader_translate[1]+=0.1;
        break;
    case '2': 
        shader_translate[1]-=0.1;
        break;
    case '4': 
        shader_translate[0]-=0.1;
        angle--;
        break;
    case '6': 
        shader_translate[0]+=0.1;
        angle++;
        break;
    case 'r': 
        rotation = !rotation;
        break;
    default:
        break;
    }
    idle ();
}

void specialKeys(int key, int x, int y) {
    if(key == GLUT_KEY_LEFT)
		position -= rightVector() * deltaTime * speed;
    else if(key == GLUT_KEY_RIGHT)
		position += rightVector() * deltaTime * speed;
    else if(key == GLUT_KEY_DOWN)
		position -= directionVector() * deltaTime * speed;
    else if(key == GLUT_KEY_UP)
        position += directionVector() * deltaTime * speed;
}

void mouse (int button, int state, int x, int y) {
    if (state == GLUT_UP) {
        mouseMovePressed = false;
        mouseRotatePressed = false;
        mouseZoomPressed = false;
    } else {
        if (button == GLUT_LEFT_BUTTON) {
            //camera.beginRotate (x, y);
            mouseMovePressed = false;
            mouseRotatePressed = true;
            mouseZoomPressed = false;
            lastX = x;
            lastY = y;
        } else if (button == GLUT_RIGHT_BUTTON) {
            lastX = x;
            lastY = y;
            mouseMovePressed = true;
            mouseRotatePressed = false;
            mouseZoomPressed = false;
        } else if (button == GLUT_MIDDLE_BUTTON) {
            if (mouseZoomPressed == false) {
                lastZoom = y;
                mouseMovePressed = false;
                mouseRotatePressed = false;
                mouseZoomPressed = true;
            }
        }
    }
    idle ();
}

void motion (int x, int y) {
    if (mouseRotatePressed == true) {
        computeMatricesFromInputs(x - lastX, y - lastY);
        lastX = x;
        lastY = y;
    }
    else if (mouseMovePressed == true) {
    }
    else if (mouseZoomPressed == true) {
    }
}

void computeMatricesFromInputs(float moveX, float moveY){
    //std::cout << moveX << " " << moveY << std::endl;
	// Compute new orientation
	horizontalAngle += mouseSpeed * moveX / 10.f;
	verticalAngle   += mouseSpeed * moveY / 10.f;
	// Up vector
	glm::vec3 up = glm::cross( rightVector(), directionVector() );

	float FoV = initialFoV;

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);

	// Camera matrix
	ViewMatrix       = glm::lookAt(
								camera_position,           // Camera is here
								camera_position + directionVector(), // and looks here : at the same position, plus "direction"
								up                  // Head is up (set to 0,-1,0 to look upside-down)
						   );
}


void reshape(int w, int h) {
    // camera.resize (w, h);
}

int main (int argc, char ** argv) {
    aide();
    if (argc > 2) {
        exit (EXIT_FAILURE);
    }
    glutInit (&argc, argv);
    glutInitDisplayMode (GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize (SCR_WIDTH, SCR_HEIGHT);
    window = glutCreateWindow ("TP3-HAI719I");

    init ();
    glutIdleFunc (idle);
    glutDisplayFunc (display);
    glutKeyboardFunc (key);
    glutReshapeFunc (reshape);
    glutMotionFunc (motion);
    glutMouseFunc (mouse);
    glutSpecialFunc(specialKeys);
    key ('?', 0, 0);

    computeMatricesFromInputs(0.f, 0.f);

    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Create and compile our GLSL program from the shaders
    programID = LoadShaders( "vertex_shader.glsl", "fragment_shader.glsl" );

    //Chargement de la chaise

    //Chargement du fichier de maillage
    std::string chair_file("data/chair.off");
    loadOFF(chair_file, chair_indexed_vertices, chair_indices, chair_triangles );

    // Load it into a VBO

    glGenBuffers(1, &chair_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, chair_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, chair_indexed_vertices.size() * sizeof(glm::vec3), &chair_indexed_vertices[0], GL_STATIC_DRAW);
    
    // Generate a buffer for the indices as well
    glGenBuffers(1, &chair_elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chair_elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, chair_indices.size() * sizeof(unsigned short), &chair_indices[0] , GL_STATIC_DRAW);
    
    //Chargement de Suzanne
    
    //Chargement du fichier de maillage
    std::string suzanne_file("data/suzanne.off");
    loadOFF(suzanne_file, suzanne_indexed_vertices, suzanne_indices, suzanne_triangles );

    // Load it into a VBO

    glGenBuffers(1, &suzanne_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, suzanne_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, suzanne_indexed_vertices.size() * sizeof(glm::vec3), &suzanne_indexed_vertices[0], GL_STATIC_DRAW);
    
    // Generate a buffer for the indices as well
    glGenBuffers(1, &suzanne_elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, suzanne_elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, suzanne_indices.size() * sizeof(unsigned short), &suzanne_indices[0] , GL_STATIC_DRAW);

    //Chargement de la sphere

    //Chargement du fichier de maillage
    std::string filename("data/sphere.off");
    loadOFF(filename, sphere_indexed_vertices, sphere_indices, sphere_triangles );

    // Load it into a VBO

    glGenBuffers(1, &sphere_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, sphere_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sphere_indexed_vertices.size() * sizeof(glm::vec3), &sphere_indexed_vertices[0], GL_STATIC_DRAW);

    // Generate a buffer for the indices as well
    glGenBuffers(1, &sphere_elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere_elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphere_indices.size() * sizeof(unsigned short), &sphere_indices[0] , GL_STATIC_DRAW);

    // Get a handle for our "LightPosition" uniform
    glUseProgram(programID);
    LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glutMainLoop ();

    // Cleanup VBO and shader
    glDeleteBuffers(1, &chair_vertexbuffer);
    glDeleteBuffers(1, &chair_elementbuffer);
    glDeleteBuffers(1, &suzanne_vertexbuffer);
    glDeleteBuffers(1, &suzanne_elementbuffer);
    glDeleteBuffers(1, &sphere_vertexbuffer);
    glDeleteBuffers(1, &sphere_elementbuffer);
    glDeleteProgram(programID);
    glDeleteVertexArrays(1, &VertexArrayID);


    return EXIT_SUCCESS;
}
