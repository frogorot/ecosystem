#include"iostream"
#include<string>
#include<vector>
#include<deque>
#include"stdio.h"

#include"../glut/glut.h"
#include"../Headers/animal.h"
#include"../Headers/fancs.h"
#include"../Headers/colors.h"

using namespace std;

int SCENE_W = 968;
int SCENE_H = 968;

Map* map1 = NULL;
deque <Animal> animal;
vector <Animal> species;
vector <int> population;
vector <Vector> gender;
vector <int> immature;
vector <int> mature;
vector <int> adult;
vector <int> aged;

vector <int> old;
vector <int> hunger;


//vector <int[4]> age; // видимо, так делать нельзя

FILE* F_popul = NULL;
FILE* F_spec = NULL;
FILE* F_gend = NULL;
FILE* F_age = NULL;
FILE* F_death = NULL;

int dt = clock()/(CLOCKS_PER_SEC*0.001);
int olddt = clock()/(CLOCKS_PER_SEC*0.001);
int populdt = 0;//clock()/(CLOCKS_PER_SEC*0.001);
int regendt = 0;
int drowdt = clock()/(CLOCKS_PER_SEC*0.001);

float calcdt = 15;
float total_calcdt = 0;

int FPS; 

Vector pos_map = Vector(-101,-101);//-49,-51);
double scale_map = 0.659;//0.179; //0.37;
bool stop = false;
bool Dragflag = false;
int oldx=0;
int oldy=0;

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);

	glEnable(GL_LIGHT0);
	GLfloat light_diffuse[] = {1.0,1.0,1.0,1.0};
	GLfloat light_ambient[] = {0,0,0,1.0};
	GLfloat light_position[] = {0.0,0.0,1.0,0};
	glLightfv(GL_LIGHT0,GL_POSITION, light_position);
	glLightModelfv(GL_DIFFUSE,light_diffuse);
	glLightModelfv(GL_AMBIENT,light_ambient); // настройки света

	glNormal3f(0.0, 0.0, -1.0);
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,red);

	/*glBegin(GL_POLYGON);
		glVertex2d(-2.5,-1);
		glVertex2d(-2.5,1);
		glVertex2d(2.5,1);
		glVertex2d(2.5,-1);
	glEnd();

	glPushMatrix();

	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,white);
	glTranslated(-1.5,0,0);
		glutSolidSphere(0.5,8,8);
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,lgrey);
		glTranslated(1,0,0);
			glutSolidSphere(0.5,8,8);
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,dgrey);
	glTranslated(1,0,0);
		glutSolidSphere(0.5,8,8);
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,black);
	glTranslated(1,0,0);
		glutSolidSphere(0.5,8,8);*/
	glPushMatrix();
	
	glTranslated(20,20,0);
	if(!stop)
		glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,green);
	else
		glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,red);
	glutSolidSphere(0.5,8,8);

	glPopMatrix();
	
	glPushMatrix();
		glScaled(scale_map, scale_map, scale_map);
		map1->drawMap(pos_map - Vector(0.5,0.5));
		for(unsigned int i=0;i<animal.size();i++)
			animal[i].drowAnimal(pos_map);
	glPopMatrix();
	
	FPS++;
	glDisable(GL_LIGHT0);
	glFlush();
	glutSwapBuffers();
}
void idle()
{
	int dt=clock()/(CLOCKS_PER_SEC*0.001);
	if(dt - olddt > 1000)
	{
		//cout << "FPS. " << FPS << "\n";
		FPS = 0;
		olddt = dt;
	}
	if(!stop){
		F_popul = fopen("population.log","a"); 
		if((dt - drowdt > 15)){
			//int tim = dt - drowdt;
			total_calcdt += calcdt;
			int siz = animal.size();
			for(unsigned int i =0; i<siz; i++){
				bool die = animal[i].live(*map1,animal,calcdt, total_calcdt,i);
				cout << animal[i].getEnergy() << "\n";
				if(die){
					//map1->getSub((int)(animal[i].position.GetX())*map1->getDemen() + (int)(animal[i].position.GetY()))->eraseBug();
					animal.erase(animal.begin() + i);
					--i;
					--siz;
				}
			}
			cout << "______" << "\n";
		}
		stop = !stop;
		fputs(myto_string((int)animal.size()).c_str(), F_popul);
		fputs("\n",F_popul);
		fclose(F_popul);
	}
	drowdt = dt;
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
	switch(key)

	{
		case 'd':
			pos_map.SetX(pos_map.GetX() -2);
			break;
		case 'a':
			pos_map.SetX(pos_map.GetX() +2);
			break;
		case 'w':
			pos_map.SetY(pos_map.GetY() -2);
			break;
		case 's':
			pos_map.SetY(pos_map.GetY() +2);
			break;
		case 'в':
			pos_map.SetX(pos_map.GetX() -2);
			break;
		case 'ф':
			pos_map.SetX(pos_map.GetX() +2);
			break;
		case 'ц':
			pos_map.SetY(pos_map.GetY() -2);
			break;
		case 'ы':
			pos_map.SetY(pos_map.GetY() +2);
			break;
		case 'p':
			stop = !stop;
			break;
		//case 'r':
		//	map1->genMap(100,4,0.5, "test1.txt");
		//	break;
		case '[':
			scale_map+=0.03;
			break;
		case ']':
			scale_map-=0.03;
			if(scale_map < 0.001)
				scale_map = 0.001;
			break;
		case 'х':
			scale_map+=0.03;
			break;
		case 'ъ':
			scale_map-=0.03;
			if(scale_map < 0.001)
				scale_map = 0.001;
			break;
		case '9':
			calcdt *= 0.3; //speed_life
			if(calcdt < 0.01)
				calcdt = 0.01;
			break;
		case '0':
			calcdt *=1.3;
			break;
	}
	glutPostRedisplay();
}
void mouseClick(int button, int state, int x, int y)
{
	if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		Dragflag = true;
		oldx= x;
		oldy= y;
	}
	if(button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
	{
		Dragflag = false;
	}
}
void mouseMotion(int x, int y)
{
	if(Dragflag)
	{
		pos_map.SetX(pos_map.GetX() - (x - oldx)*0.25);
		pos_map.SetY(pos_map.GetY() + (y - oldy)*0.25);
		oldx = x;
		oldy = y;
	}
}
void reshape(int width, int height)
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-20,20,-20,20);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	SCENE_W = width;
	SCENE_H = height;
}

Animal Bug1(int x, int y, int d);

int main(int argc, char** argv)
{
	setlocale(LC_ALL,"RUS");
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(SCENE_W, SCENE_H);
	glutCreateWindow("OpenGL экосистема");

	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouseClick);
	glutMotionFunc(mouseMotion);
	glutReshapeFunc(reshape);
	glClearColor(0.95, 0.95, 0.95, 1.0);


	F_popul = fopen("population.log","w"); 
	F_spec = fopen("species.log","w"); 
	F_gend = fopen("gender.log","w");
	F_age = fopen("age.log","w");
	F_death = fopen("death.log","w");

	if(F_popul == NULL || F_spec == NULL){
		cout << "ERROR_FILE_POPUL_||_SPEC\n";
		system("Pause");
		return 0;
	}

	map1 = new Map;
	//string mapf;
	cout << "Hellow! \n";
	/*cin  >> mapf;
	cout << mapf << endl;*/ //map1->genMap(100,4,0.4, "test11.txt");
	map1->genMap(200,4,4, "uniform.txt"); // "test.txt"->mapf
	//map1->readMap("test11.txt"); // "test.txt"
	int demen = map1->getDemen();
	//for(int i =0;i<4;i++){
		animal.push_back(Bug1(100,100,0));;//Vector(chance(demen-1), chance(demen-1))
		//animal.push_back(Bug1(25,23,1));
		//animal.push_back(Bug1(23,25,3));
		//animal.push_back(Bug1(25,25,2));
		map1->getSub(100*map1->getDemen() + 100)->addBug();
		//map1->getSub(25*map1->getDemen() + 23)->addBug();
		//map1->getSub(23*map1->getDemen() + 25)->addBug();
		//map1->getSub(25*map1->getDemen() + 25)->addBug();
	//}

	glutMainLoop();
	
	delete map1;
	animal.clear();
	fclose(F_popul);
	fclose(F_spec);
	fclose(F_gend);
	fclose(F_age);
	fclose(F_death);

	return 0;
}

Animal Bug1(int x, int y, int d){
	GLfloat color[3] = {2.55, 1.60, 0.51};
	Vector dir;
	switch(d%4){
	case 0:
		dir = Vector(0,1);
		break;
	case 1:
		dir = Vector(-1,0);
		break;
	case 2:
		dir = Vector(0,-1);
		break;
	case 3:
		dir = Vector(1,0);
		break;
	}
	/*double en, ki, for_li, di; 
	cout << "Write energy: ";
	cin >> en;
	cout << "Write price of motion: ";
	cin >> ki;
	cout << "Write price of live: ";
	cin >> for_li;
	cout << "Write diverge limit: ";
	cin >> di;*/
	return Animal(2.1, 2.7, 0.5, 10.3, color, Vector(x, y), dir); //Animal(en, ki, for_li, di, color, Vector(x, y), dir); 
		//Animal(2.0, 2.8, 0.5, 8.55, color, Vector(x, y), dir); 


		//Animal(1.0, 1.0, 1.6, 4.0, color, Vector(x, y), dir); 
		//Animal(1.0, 1.0, 1.5, 4.0, color, Vector(x, y), dir);
		//Animal(1.0, 1.0, 1.4999999999, 4.0, color, Vector(x, y), dir);
		//Animal(1.0, 1.0, 1.55, 4.0, color, Vector(x, y), dir);
	//Animal(1.0, 1.0, 1.6, 4.0, color, Vector(x, y), dir);
	//Animal(1.0, 1.0, 1.5, 4.0, color, Vector(x, y), dir);
	//Animal(1.0, 2.0, 1.0, 4.0, color, Vector(x, y), dir);
}// energy->kinetic->for_live->div

