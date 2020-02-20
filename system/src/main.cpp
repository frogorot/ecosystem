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


//vector <int[4]> age; // видимо, так делать нельз€

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

Vector pos_map = Vector(-25,-25);//-49,-51);
double scale_map = 0.76; //0.37;
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
		cout << "FPS. " << FPS << "\n";
		//cout << "color " << animalan_color[0] << ", " << anim->an_color[1] << ", " << anim->an_color[2] << "\n";
		FPS = 0;
		olddt = dt;
	}
	if(!stop){
		if(total_calcdt - regendt > 90){ // 105 выбрано с потолка.  ратно calcdt
			map1->regSub(total_calcdt - regendt);
			regendt = total_calcdt; 
		}// HARD_CODE
		if(total_calcdt - populdt> 45){//dt-populdt > 1500){ // 2000

			F_popul = fopen("population.log","a"); 
			F_spec = fopen("species.log","a");
			F_gend = fopen("gender.log","a");
			F_age = fopen("age.log","a");
			F_death = fopen("death.log","a");

			int siz = animal.size();
			bool find = false;
			for(int i =0;i<siz;i++){
				find = false;
				for(unsigned int j=0;j<species.size();j++){ // т.к. размер может увеличитьс€ по ходу выполнени€
					if(animal[i] == species[j]){
						find = true;
						++population[j];
						if(animal[i].param[14]->getPar() == 0) // M | ∆
							gender[j].SetX(gender[j].GetX() + 1.0);
						else
							gender[j].SetY(gender[j].GetY() + 1.0);
						int age = (int)(animal[i].age/animal[i].getMaxAge() * 4);
						switch(age){
							case 0:
								++immature[j];
								break;
							case 1:
								++mature[j];
								break;
							case 2:
								++adult[j];
								break;
							case 3:
								++aged[j];
								break;
						}
						break;
					}
				}
				if(!find){ // если по€вилс€ новый вид, добавл€ем в список видов и повтор€ем проверку.
					species.push_back(animal[i]);
					population.push_back(1);
					if(animal[i].param[14] == 0) // M | ∆
						gender.push_back(Vector(1,0));
					else
						gender.push_back(Vector(0,1));
					int pos = (int)(animal[i].age/animal[i].getMaxAge() * 4);
						switch(pos){
							case 0:
								immature.push_back(1);
								mature.push_back(0);
								adult.push_back(0);
								aged.push_back(0);
								break;
							case 1:
								immature.push_back(0);
								mature.push_back(1);
								adult.push_back(0);
								aged.push_back(0);
								break;
							case 2:
								immature.push_back(0);
								mature.push_back(0);
								adult.push_back(1);
								aged.push_back(0);
								break;
							case 3:
								immature.push_back(0);
								mature.push_back(0);
								adult.push_back(0);
								aged.push_back(1);
								break;
						}
						old.push_back(0);
						hunger.push_back(0);
					species[species.size()-1].writeAnimal(F_spec);
				}
			}

			for(unsigned int i=0;i<population.size();i++){
				fputs(myto_string(population[i]).c_str(), F_popul);
				fputs("\t", F_popul);
				fputs((myto_string((int)gender[i].GetX()) + "\t" + myto_string((int)gender[i].GetY()) + " ").c_str(), F_gend);
				fputs("\t", F_gend);
				fputs((myto_string(immature[i])+"\t"+myto_string(mature[i])+"\t"+myto_string(adult[i])+"\t"+myto_string(aged[i])+" ").c_str(), F_age);
				fputs("\t", F_age);
				fputs((myto_string(hunger[i])+"\t"+myto_string(old[i])+" ").c_str(), F_death);
				fputs("\t", F_death);
				population[i] = 0;
				gender[i] = Vector(0,0);
				immature[i] = 0;
				mature[i] = 0;
				adult[i] = 0;
				aged[i] = 0;
				old[i] = 0;
				hunger[i] = 0;
			}
			fputs("\n",F_popul);
			fputs("\n",F_gend);
			fputs("\n",F_age);
			fputs("\n",F_death);
			populdt = total_calcdt;

			fclose(F_popul);
			fclose(F_spec);
			fclose(F_gend);
			fclose(F_age);
			fclose(F_death);
		}// HARD_CODE

		if((dt - drowdt > 15)){
			//int tim = dt - drowdt;
			total_calcdt += calcdt;
			for(unsigned int i =0; i<animal.size(); i++){
				if(animal[i].live(*map1,animal,calcdt, total_calcdt,i)==1){
				//if(animal[i].live(*map1,animal,tim*speed_life, dt + (dt - drowdt)*speed_life,i))
					bool find = false;
					for(unsigned int j=0;j<species.size();j++){ // т.к. размер может увеличитьс€ по ходу выполнени€
						if(animal[i] == species[j]){
							find = true;
							++hunger[j];
							break;
						}
					}
					if(!find){ // если по€вилс€ новый вид, добавл€ем в список видов и повтор€ем проверку.
						species.push_back(animal[i]);
						population.push_back(1);
						int age = (int)(animal[i].age/animal[i].getMaxAge() * 4);
						switch(age){
							case 0:
								immature.push_back(1);
								mature.push_back(0);
								adult.push_back(0);
								aged.push_back(0);
								break;
							case 1:
								immature.push_back(0);
								mature.push_back(1);
								adult.push_back(0);
								aged.push_back(0);
								break;
							case 2:
								immature.push_back(0);
								mature.push_back(0);
								adult.push_back(1);
								aged.push_back(0);
								break;
							case 3:
								immature.push_back(0);
								mature.push_back(0);
								adult.push_back(0);
								aged.push_back(1);
								break;
						}
						old.push_back(0);
						hunger.push_back(1);
						if(animal[i].param[14] == 0) // M | ∆
							gender.push_back(Vector(1,0));
						else
							gender.push_back(Vector(0,1));
						species[species.size()-1].writeAnimal(F_spec);
					}
					animal.erase(animal.begin() + i);
				}	
				if(animal[i].live(*map1,animal,calcdt, total_calcdt,i)==2){
				//if(animal[i].live(*map1,animal,tim*speed_life, dt + (dt - drowdt)*speed_life,i))
					bool find = false;
					for(unsigned int j=0;j<species.size();j++){ // т.к. размер может увеличитьс€ по ходу выполнени€
						if(animal[i] == species[j]){
							find = true;
							++old[j];
							break;
						}
					}
					if(!find){ // если по€вилс€ новый вид, добавл€ем в список видов и повтор€ем проверку.
						species.push_back(animal[i]);
						population.push_back(1);

						immature.push_back(0);
						mature.push_back(0);
						adult.push_back(0);
						aged.push_back(1);

						old.push_back(1);
						hunger.push_back(0);
						if(animal[i].param[14] == 0) // M | ∆
							gender.push_back(Vector(1,0));
						else
							gender.push_back(Vector(0,1));
						species[species.size()-1].writeAnimal(F_spec);
					}
					animal.erase(animal.begin() + i);
				}	
			}// HARD_CODE
		}
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

Animal Bostrichidae(Vector pos);

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
	map1->genMap(50,4,4, "uniform.txt"); // "test.txt"->mapf
	//map1->readMap("test11.txt"); // "test.txt"
	int demen = map1->getDemen();
	//for(int i =0;i<4;i++){
		animal.push_back(Bostrichidae(Vector(23,23)));//Vector(chance(demen-1), chance(demen-1))
		animal.push_back(Bostrichidae(Vector(25,23)));
		animal.push_back(Bostrichidae(Vector(23,25)));
		animal.push_back(Bostrichidae(Vector(25,25)));
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

//¬ажно!!! Ќужно задавать двойной набор хромосом, алгоритм рассчитан на двойной набор.

Animal Bostrichidae(Vector pos){ // какой-то жучок, конкретнее капюшонник или лжекороед, ещЄ конкретнее «ерновой точильщик Rhyzopertha dominica F.
	//Parametr* parametr = new Parametr[NUM_PAR_IN_CHR];
	unsigned int charact [5];
	double maxage = 280.0*2.0*1000.0;// примерно 280 дней жизни, развитие от 29 до 111, имаго (собственно жук) 60 - 365
	//^ т.к. считаем по 12 часов, а не 24, и ведЄм подсчЄт мс.
	Gene* gen = new Gene[4];
	Chromosome* chr = new Chromosome[12];
	// 3 140
	charact[0] = to_gen(0); charact[1] = to_gen(140); charact[2] = to_gen(140); charact[3] = to_gen(140); charact[4] = to_gen(47);// 3.140.140.140.47
	gen[0] = Gene(0.001,0,false,charact);
	charact[0] = to_gen(5); charact[1] = to_gen(10); charact[2] = to_gen(10); charact[3] = to_gen(10); charact[4] = to_gen(10);
	gen[1] = Gene(0.001,1,false,charact);
	charact[0] = to_gen(20); charact[1] = to_gen(20); charact[2] = to_gen(20); charact[3] = to_gen(20); charact[4] = to_gen(20);
	gen[2] = Gene(0.001,2,false,charact);//25 50 75 75 50
	charact[0] = to_gen(1); charact[1] = to_gen(1); charact[2] = to_gen(1); charact[3] = to_gen(1); charact[4] = to_gen(1);
	gen[3] = Gene(0.001,3,false,charact);
	
	chr[0] = Chromosome(gen,4,0.001);
	chr[1] = Chromosome(gen,4,0.001);


	charact[0] = to_gen(1); charact[1] = to_gen(1); charact[2] = to_gen(1); charact[3] = to_gen(1); charact[4] = to_gen(1);
	gen[0] = Gene(0.001,4,false,charact);
	charact[0] = to_gen_typ(2); charact[1] = to_gen_typ(2); charact[2] = to_gen_typ(2); charact[3] = to_gen_typ(2); charact[4] = to_gen_typ(2);
	gen[1] = Gene(0.001,5,false,charact);
	charact[0] = to_gen_typ(1); charact[1] = to_gen_typ(1); charact[2] = to_gen_typ(1); charact[3] = to_gen_typ(1); charact[4] = to_gen_typ(1);
	gen[2] = Gene(0.001,6,false,charact);

	chr[2] = Chromosome(gen,3,0.001);
	chr[3] = Chromosome(gen,3,0.001);

	charact[0] = to_gen_typ(0); charact[1] = to_gen_typ(0); charact[2] = to_gen_typ(0); charact[3] = to_gen_typ(0); charact[4] = to_gen_typ(0);
	gen[0] = Gene(0.001,7,false,charact);
	charact[0] = to_gen(1); charact[1] = to_gen(3); charact[2] = to_gen(3); charact[3] = to_gen(3); charact[4] = to_gen(3);
	gen[1] = Gene(0.001,8,false,charact);
	charact[0] = to_gen(40); charact[1] = to_gen(40); charact[2] = to_gen(40); charact[3] = to_gen(40); charact[4] = to_gen(40);
	gen[2] = Gene(0.001,9,false,charact); // 50 100 150 150 100
	// ~1600 ƒж/жук за жизнь, за 12 часов (1 сек модельного времени) тер€ет 1.732, за просчЄт, который 0.015 сек модельного времени, 0.02886

	chr[4] = Chromosome(gen,3,0.001);
	chr[5] = Chromosome(gen,3,0.001);

	charact[0] = 0; charact[1] = 0; charact[2] = 0; charact[3] = 0; charact[4] = 0;
	gen[0] = Gene(0.001,10,false,charact);
	charact[0] = to_gen(10); charact[1] = to_gen(10); charact[2] = to_gen(10); charact[3] = to_gen(10); charact[4] = to_gen(10);
	gen[1] = Gene(0.001,11,false,charact);

	chr[6] = Chromosome(gen,2,0.001);
	chr[7] = Chromosome(gen,2,0.001);


	charact[0] = to_gen(20); charact[1] = to_gen(50); charact[2] = to_gen(100); charact[3] = to_gen(100); charact[4] = 0;
	gen[0] = Gene(0.001,12,false,charact);
	charact[0] = 0; charact[1] = 0; charact[2] = to_gen(2); charact[3] = to_gen(2); charact[4] = to_gen(2);
	gen[1] = Gene(0.001,13,false,charact);
	int gender = chance(1);
	charact[0] = to_gen_typ(gender); charact[1] = to_gen_typ(gender); charact[2] = to_gen_typ(gender); charact[3] = to_gen_typ(gender); charact[4] = to_gen_typ(gender);
	gen[2] = Gene(0.001,14,false,charact);

	chr[8] = Chromosome(gen,3,0.001);
	chr[9] = Chromosome(gen,3,0.001);

	charact[0] = maxage; charact[1] = maxage; charact[2] = maxage; charact[3] = maxage; charact[4] = maxage;
	gen[0] = Gene(0.001,15,false,charact);

	chr[10] = Chromosome(gen,1,0.001);
	chr[11] = Chromosome(gen,1,0.001);

	Animal an = Animal(chr,12,pos);
	an.setPar();

	delete[] gen;
	delete[] chr;

	return an;
}