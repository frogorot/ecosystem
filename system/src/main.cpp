#include"iostream"
#include<string>
#include<vector>
#include<deque>
#include<list>
#include"stdio.h"

#include"../glut/glut.h"
#include"../Headers/animal.h"
#include"../Headers/fancs.h"
#include"../Headers/colors.h"

using namespace std;

int SCENE_W = 1002;//968;
int SCENE_H = 1002;//968;

Map* map1 = NULL;
deque <Animal> animal;
deque <int> hand_eat;
deque <int> hand_other;
int scale_bug = 10;
int demention_of_cell = 16;
int MAX_scale_bug = demention_of_cell*3;
int MAX_demention_of_cell = 15;


/*vector <Animal> species;
vector <int> population;
vector <Vector> gender;
vector <int> immature;
vector <int> mature;
vector <int> adult;
vector <int> aged;

vector <int> old;
vector <int> hunger;*/


//vector <int[4]> age; // видимо, так делать нельзя

FILE* F_popul = NULL;
FILE* F_spec = NULL;
//FILE* F_gend = NULL;
//FILE* F_age = NULL;
//FILE* F_death = NULL;

int dt = clock()/(CLOCKS_PER_SEC*0.001);
int olddt = clock()/(CLOCKS_PER_SEC*0.001);
int populdt = 0;//clock()/(CLOCKS_PER_SEC*0.001);
int regendt = 0;
int drowdt = clock()/(CLOCKS_PER_SEC*0.001);

float calcdt = 15;
float total_calcdt = 0;

int FPS; 

Vector pos_map = Vector(-301,-301);//-201,-201);//-6.668,-7.009);//-201,-201);//-49,-51);
double scale_map = 0.06108;//0.308 // 0.548; // <- TEST //0.098;//0.659;//0.179; //0.37;
bool stop = true;
bool Dragflag = false;
int oldx=0;
int oldy=0;

Animal Bug1(int x, int y, int d);
Animal Bug2(int x, int y, int k);
Animal Bug3TEST(int x, int y, int k);

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
		unsigned int size = hand_eat.size() + hand_other.size();

		if (size == 0) {
			fopen_s(&F_spec, "species.log", "a");
			fputs((myto_string(scale_bug) + "\t" + myto_string(demention_of_cell) + "dead\n").c_str(), F_spec);
			fclose(F_spec);

			++scale_bug;
			if (scale_bug > MAX_scale_bug) {
				scale_bug = 2;
				++demention_of_cell;
				if (demention_of_cell > MAX_demention_of_cell) {
					stop = true;
				}
			}
			

			hand_eat.clear();
			hand_other.clear();
			delete map1;
			animal.clear();

			map1 = new Map;

			cout << "Hellow! \n";
			map1->genMapTEST(scale_bug, demention_of_cell, 600, 4, 4, "uniform.txt");

			animal.push_back(Bug3TEST(300, 300, demention_of_cell));
			animal.back().new_maj(animal, *map1);
			if (animal.back().targ == eat)
				hand_eat.push_back(animal.size() - 1);
			else
				hand_other.push_back(animal.size() - 1);

			map1->getSub(300 * map1->getDemen() + 300)->addBug();
		}
		else {
			vector <int> check;
			unsigned int size_of_chek = 0;
			for (unsigned int i = 0; i < size; i++) {
				if (i == 0) {
					check.push_back(i);
					++size_of_chek;
				}
				else {
					bool finde = false;
					for (unsigned int j = 0; j < size_of_chek; j++) {
						if (animal.at(check.at(j)) == animal.at(i)) {
							finde = true;
							break;
						}
					}
					if (!finde) {
						check.push_back(i);
						++size_of_chek;
					}
				}
			}
			if (size > 0 && size_of_chek * scale_bug == size) {
				fopen_s(&F_spec, "species.log", "a");
				fputs((myto_string(scale_bug) + "\t" + myto_string(demention_of_cell) + "life\n").c_str(), F_spec);
				fclose(F_spec);
				++scale_bug;
				if (scale_bug > MAX_scale_bug) {
					scale_bug = 2;
					++demention_of_cell;
					if (demention_of_cell > MAX_demention_of_cell) {
						stop = true;
					}
				}
				


			}
			check.clear();
		}
	}
	if(!stop){
		fopen_s(&F_popul, "population.log","a");
		if((dt - drowdt > 15)){
			//int tim = dt - drowdt;
			total_calcdt += calcdt;
			int siz_eat = hand_eat.size();
			//list <int>::iterator it = hand_eat.begin();
			//auto end_eat = hand_eat.end();
			int siz_oth = hand_other.size();
			//auto end_oth = hand_other.end();
			bool die;
			for(int i = 0; i< siz_eat; i++){
				die = animal[hand_eat[i]].live(*map1,animal);
				//cout << animal[*it].getEnergy() << "\n";
				if(die){
					map1->getSub((animal[hand_eat[i]].position.GetX())*map1->getDemen() + (animal[hand_eat[i]].position.GetY()))->eraseBug();
					animal.erase(animal.begin() + hand_eat[i]);

					int de = hand_eat[i];
					double siz = hand_other.size();
					for(int j =0;j<siz;j++)
						if(hand_other[j] > de)
							--hand_other[j];
					siz = hand_eat.size();
					for(int j =0;j<siz;j++)
						if(hand_eat[j] > de)
							--hand_eat[j];
				}
				else {
					if (animal[hand_eat[i]].targ == eat)
						hand_eat.push_back(hand_eat[i]);
					else
						hand_other.push_back(hand_eat[i]);
				}
			}
			hand_eat.erase(hand_eat.begin(), hand_eat.begin()+siz_eat);

			for(unsigned int i =0; i<siz_oth; i++){
				if(animal[hand_other[i]].targ == reprod){
					die = animal[hand_other[i]].live(*map1,animal);
					if(animal.back().targ == eat)
						hand_eat.push_back(animal.size()-1);
					else
						hand_other.push_back(animal.size()-1);
				}
				else
					die = animal[hand_other[i]].live(*map1,animal);
				
				//cout << animal[hand_other[i]].getEnergy() << "\n";
				if(die){
					map1->getSub((int)(animal[hand_other[i]].position.GetX())*map1->getDemen() + (int)(animal[hand_other[i]].position.GetY()))->eraseBug();
					animal.erase(animal.begin() + hand_other[i]);

					int de = hand_other[i];
					double siz = hand_other.size();
					for(int j =0;j<siz;j++)
						if(hand_other[j] > de)
							--hand_other[j];
					siz = hand_eat.size();
					for(int j =0;j<siz;j++)
						if(hand_eat[j] > de)
							--hand_eat[j];
				}
				else {
					if (animal[hand_other[i]].targ == eat)
						hand_eat.push_back(hand_other[i]);
					else
						hand_other.push_back(hand_other[i]);
				}
			}
			hand_other.erase(hand_other.begin(), hand_other.begin()+siz_oth);
			
			siz_eat = hand_eat.size();
			siz_oth = hand_other.size();
			for(unsigned int i =0; i<siz_eat; i++){
				if(animal[hand_eat[i]].targ == Null)
					animal[hand_eat[i]].new_maj(animal,*map1);

				if (animal[hand_eat[i]].targ == eat)
					hand_eat.push_back(hand_eat[i]);
				else
					hand_other.push_back(hand_eat[i]);
			}
			for(unsigned int i =0; i<siz_oth; i++){
				if(animal[hand_other[i]].targ == Null)
					animal[hand_other[i]].new_maj(animal,*map1);

				if (animal[hand_other[i]].targ == eat)
					hand_eat.push_back(hand_other[i]);
				else
					hand_other.push_back(hand_other[i]);
			}
			hand_eat.erase(hand_eat.begin(), hand_eat.begin() + siz_eat);
			hand_other.erase(hand_other.begin(), hand_other.begin() + siz_oth);
			//cout << "______" << "\n";
		}
		//stop = !stop;
		//cout << animal.size() << "\n";
		fputs(myto_string((int)animal.size()).c_str(), F_popul);
		fputs("\n",F_popul);
		fclose(F_popul);
		//stop = !stop;
	}
	drowdt = dt;
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
	switch(key)

	{
		case 'd':
			pos_map.SetX(pos_map.GetX() -2*(1/scale_map));
			break;
		case 'a':
			pos_map.SetX(pos_map.GetX() +2*(1/(scale_map)));
			break;
		case 'w':
			pos_map.SetY(pos_map.GetY() -2*(1/(scale_map)));
			break;						  
		case 's':						  
			pos_map.SetY(pos_map.GetY() +2*(1/(scale_map)));
			break;						  
		case 'в':						  
			pos_map.SetX(pos_map.GetX() -2*(1/(scale_map)));
			break;						  
		case 'ф':						  
			pos_map.SetX(pos_map.GetX() +2*(1/(scale_map)));
			break;						  
		case 'ц':						  
			pos_map.SetY(pos_map.GetY() -2*(1/(scale_map)));
			break;						  
		case 'ы':						  
			pos_map.SetY(pos_map.GetY() +2*(1/(scale_map)));
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

//Animal Bug1(int x, int y, int d);
//Animal Bug2(int x, int y, int k);
//Animal Bug3TEST(int x, int y, int k);

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


	/*fopen_s(&F_popul, "population.log", "a");
	fopen_s(&F_spec, "species.log", "w");*/
	/*fopen_s(&F_gend, "gender.log", "w");
	fopen_s(&F_age, "age.log", "w");
	fopen_s(&F_death, "death.log","w");*/

	/*if(F_popul == NULL || F_spec == NULL){
		cout << "ERROR_FILE_POPUL_||_SPEC\n";
		system("Pause");
		return 0;
	}*/

	/*int k = 2;
	cout<< "Write demention of sqare: ";
	cin >> k;
	while(k<2){
		cout<< "Wrong demention, please try again (demention>=2): ";
		cin >> k;
	}*/

	stop = !stop;

	map1 = new Map;
	//string mapf;
	cout << "Hellow! \n";
	/*cin  >> mapf;
	cout << mapf << endl;*/ //map1->genMap(100,4,0.4, "test11.txt");

	//map1->genMap(400,4,4, "uniform.txt"); // "test.txt"->mapf
	//map1->genMapTEST(k,k+5, 4,4, "uniform.txt");
	map1->genMapTEST(scale_bug,demention_of_cell,600, 4,4, "uniform.txt");
	//map1->readMap("uniform.txt"); // "test11.txt" // "test.txt"

	int demen = map1->getDemen();
	//for(int i =0;i<4;i++){
		//animal.push_back(Bug2(200,200,8));//Vector(chance(demen-1), chance(demen-1))
		animal.push_back(Bug3TEST(300,300,demention_of_cell));
		animal.back().new_maj(animal, *map1);
		if(animal.back().targ == eat)
			hand_eat.push_back(animal.size()-1);
		else
			hand_other.push_back(animal.size()-1);
		//animal.push_back(Bug1(25,23,1));
		//animal.push_back(Bug1(23,25,3));
		//animal.push_back(Bug1(25,25,2));
		map1->getSub(300*demen + 300)->addBug();
		//map1->getSub(25*map1->getDemen() + 23)->addBug();
		//map1->getSub(23*map1->getDemen() + 25)->addBug();
		//map1->getSub(25*map1->getDemen() + 25)->addBug();
	//}

	glutMainLoop();
	
	delete map1;
	animal.clear();
	//fclose(F_popul);
	//fclose(F_spec);
	//fclose(F_gend);
	//fclose(F_age);
	//fclose(F_death);

	return 0;
}

Animal Bug1(int x, int y, int d){
	GLfloat color[3] = {2.55, 1.60, 0.51};
	iVector dir;
	switch(d%4){
	case 0:
		dir = iVector(0,1);
		break;
	case 1:
		dir = iVector(-1,0);
		break;
	case 2:
		dir = iVector(0,-1);
		break;
	case 3:
		dir = iVector(1,0);
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
	return Animal(0,0,1,6, color, iVector(x, y), dir); //3

		//Ниже находятся примеры для вещественных значений параметров.
		//Animal(3.0,3.0,1.0,8.0, color, Vector(x, y), dir); //14
		//Animal(1.0,3.0,1.0,11.0, color, Vector(x, y), dir); //12
		//Animal(1.0,8.0,4.0,60.0, color, Vector(x, y), dir); //37
		//Animal(1.0,8.0,4.0,60.0, color, Vector(x, y), dir); //37
		//Animal(1.0,3.0,1.0,9.0, color, Vector(x, y), dir); //5.5 при каком-то S порождает выживание первого жука, хотя D/2<V+2L
		//Animal(1.0,3.0,1.0,11.0, color, Vector(x, y), dir); //12 // забавный старт, потом опять квадраты
		//Animal(1.0, 2.0, 1.0, 11, color, Vector(x, y), dir); //6
		//Animal(2.1, 2.7, 0.5, 10.4, color, Vector(x, y), dir); //6
		 //Animal(3.0, 2.0, 1.0, 18.0, color, Vector(x, y), dir); //6
		//Animal(2.0, 1.0, 0.35, 12.1, color, Vector(x, y), dir); //6
		//Animal(1.5, 1.0, 0.5, 6.5, color, Vector(x, y), dir); //5
		//Animal(1.0, 1.0, 1.4, 7, color, Vector(x, y), dir); //4
		//Animal(2.0, 1.0, 0.35, 12.1, color, Vector(x, y), dir); 6
		//Animal(2.1, 2.7, 0.5, 10.3, color, Vector(x, y), dir); 6//Animal(en, ki, for_li, di, color, Vector(x, y), dir); 
		//Animal(2.0, 2.8, 0.5, 8.55, color, Vector(x, y), dir); 6


		//Animal(1.0, 1.0, 1.6, 4.0, color, Vector(x, y), dir); 
		//Animal(1.0, 1.0, 1.5, 4.0, color, Vector(x, y), dir);
		//Animal(1.0, 1.0, 1.4999999999, 4.0, color, Vector(x, y), dir);
		//Animal(1.0, 1.0, 1.55, 4.0, color, Vector(x, y), dir);
	//Animal(1.0, 1.0, 1.6, 4.0, color, Vector(x, y), dir);
	//Animal(1.0, 1.0, 1.5, 4.0, color, Vector(x, y), dir);
	//Animal(1.0, 2.0, 1.0, 4.0, color, Vector(x, y), dir);
}// energy->kinetic->for_live->div

Animal Bug2(int x, int y, int k){
	GLfloat color[3] = {2.55, 1.60, 0.51};
	iVector dir;

	dir = iVector(0,1);
	/*E,V,L,D*/
	double V = 3.0*k;
	double L = 1.0;
	double D = 6.0*k-3.0/k+2;
	return Animal(0,V,L,D, color, iVector(x, y), dir); 
}// energy->kinetic->for_live->div

Animal Bug3TEST(int x, int y, int k){
	GLfloat color[3] = {2.55, 1.60, 0.51};
	iVector dir;

	dir = iVector(0,1);
	/*E,V,L,D*/
	double V = 6.0*k-3.0;
	double L = 1.0;
	double D = 12.0*k-7.5;//12.0*k-7.0;
	return Animal(0/*6.0*(k-1)*//*3.0*k-3.0/k*/,V,L,D, color, iVector(x, y), dir); 
}// energy->kinetic->for_live->div