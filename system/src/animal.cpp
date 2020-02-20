#include<cmath>
#include<time.h>
#include<random>
#include<vector>
#include <stdint.h>
#include"../Headers/animal.h"
#include"../Headers/colors.h"

const double SQRT2 = sqrt(2.0);
const double PI = 3.1415926536;
const double RAD = 180.0/PI;


// Animal parametrs
const double decrease_enrg_ms = 0.00006614;//0.002857;
const double to_next_trop_lev = 0.1; 
const int time_recalcul_maj = 2000;



//Priznak

//Animal
Animal::Animal():
	prev_dt(0),
	energy (0),
	kinetic_energy(0),
	for_live(0),
	en_div(0),
	age(0),
	position(Vector(0,0)),
	direction(Vector(0,0)),
	targ(Null)
{an_color[0] = 0.0; an_color[1] = 0.0; an_color[2] = 0.0;}
Animal::Animal(double en, double ki_en, double for_li, double en_d, GLfloat color[3], Vector pos, Vector dir):
	prev_dt(0),
	energy (en),
	kinetic_energy(ki_en),
	for_live(for_li),
	en_div(en_d),
	age(0),
	position(pos),
	direction(dir),
	targ(Null)
{an_color[0] = color[0]; an_color[1] = color[1]; an_color[2] = color[2];}
Animal::Animal(const Animal& anim):
	energy(anim.getEnergy()),
	kinetic_energy(anim.getKinEn()),
	for_live(anim.getForLive()),
	en_div(anim.getEnDiv()),
	prev_dt(anim.getDt()),
	position(anim.position),
	direction(anim.direction),
	age(anim.age),
	targ(anim.targ)
{
	an_color[0] = anim.an_color[0];
	an_color[1] = anim.an_color[1];
	an_color[2] = anim.an_color[2];
}
Animal& Animal::operator= (const Animal& val)
{
	energy = val.getEnergy();
	kinetic_energy = val.getKinEn();
	for_live = val.getForLive();
	en_div = val.getEnDiv();
	prev_dt = val.getDt();
	age = val.age;
	position = val.position;
	direction = val.direction;
	targ = val.targ;
	an_color[0] = val.an_color[0];
	an_color[1] = val.an_color[1];
	an_color[2] = val.an_color[2];

	return *this;
}

void Animal::drowAnimal(Vector centre){
	//GLfloat red[] = {10.0,0.0,0.0};
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,an_color);

	glPushMatrix();
	glTranslated(centre.GetX() + position.GetX(),centre.GetY() + position.GetY(),0.5);
	glRotated(RAD*atan2((direction).GetY(),(direction).GetX())-90.0,0.0,0.0,1.0);
	//glScaled(0.7,0.7,0.7);
	glBegin(GL_POLYGON);
		glVertex2d(-0.35,-0.35);
		glVertex2d(-0.35,0.175);
		glVertex2d(0,0.525);
		glVertex2d(0.35,0.175);
		glVertex2d(0.35,-0.35);
	glEnd();

	glPopMatrix();

}
void Animal::new_maj(deque <Animal>& anim, const Map& map)
{
    if(energy < en_div){
		if(map.getSub((int)(position.GetX())*map.getDemen() + (int)(position.GetY()))->getKeep()>0 && 
			map.getSub((int)(position.GetX())*map.getDemen() + (int)(position.GetY()))->getKeep() == 
			map.getSub((int)(position.GetX())*map.getDemen() + (int)(position.GetY()))->getHeigh())
				targ = eat;
		else{
			int pos = (int)(position.GetX())*map.getDemen() + (int)(position.GetY());
			int x = (int)direction.GetX();
			int y = (int)direction.GetY();
			if(map.getSub(pos + x *map.getDemen() + y)->getKeep()>0){
				targ = motion;
			}
			else if(map.getSub(pos - y *map.getDemen() + x)->getKeep()>0){// left side
				targ = motion;
				direction = Vector(-y, x);
			}
			else if(map.getSub(pos + y *map.getDemen() - x)->getKeep()>0){// right side
				targ = motion;
				direction = Vector(y, -x);
			}
			else if(map.getSub(pos - x *map.getDemen() - y)->getKeep()>0){// back side
				targ = motion;
				direction = Vector(-x, -y);
			}
			else
				targ = motion; // find or not  bug go through
		}
	}
	else
		targ = reprod;
}

void Animal::move(double dt, Map& map){ // т.к. считаем dt в ms
    energy -= kinetic_energy + for_live; // PAR_CHANGE param[7] на энергию
	map.getSub((int)(position.GetX())*map.getDemen() + (int)(position.GetY()))->eraseBug();
	position = position + direction;
	map.getSub((int)(position.GetX())*map.getDemen() + (int)(position.GetY()))->addBug();

	targ = Null;
}
void Animal::eating(double dt,Map& map){
	Substratum* sub = map.getSub((int)(position.GetX())*map.getDemen() + (int)(position.GetY()));
	if(sub->getKeep() > 0)
		energy += sub->decay(Stock/sub->getNumBug()) - for_live;

	targ  = Null;
}
void Animal::devide(Map& map, deque <Animal>& anim){
	targ = Null;
	if(energy >= en_div){
		energy -= 2* for_live;
		this->devEnergy();
		Vector di = direction;
		direction = Vector(-direction.GetY(), direction.GetX());
		anim.push_back(*this);
		direction = di;
		anim.at(anim.size()-1).new_maj(anim, map);
		map.getSub((int)(position.GetX())*map.getDemen() + (int)(position.GetY()))->addBug();
	}
}
bool Animal::live(Map& map,deque <Animal>& anim, double dt, double absdt, int n){
    age+= dt;
	bool death = false;
	double residue_dt = dt;
	if(targ == eat)
	    eating(residue_dt,map);
	else if(targ == reprod)
	    devide(map, anim);
	else if(targ == motion)
		move(dt, map);
	if(energy<=0)
		death = true;
	if(targ == Null)
		new_maj(anim, map);
	return death; // смерть от голода - 1, от старости - 2.
}