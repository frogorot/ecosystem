#include<time.h>
#include<vector>
#include<deque>
#include"../Headers/map.h"


#ifndef __ANIMAL_H
#define __ANIMAL_H 

const int NUM_PAR_IN_CHR = 16; // число характеристик ~ генов
const bool TYP_GEN[] = {1,1,1,1,1,0,0,0,1,1,1,1,1,1,0,1};  // 1 - численный параметр, 0 - тип 
// 0) Число потомков
// 1) Радиус обнаружения (как еды, так и опасности и партнёров) 
// 2) Число энергии, при котором наступает голод
// 3) Маскировка
// 4) Размер (в некоторых расчётах, он же масса)
// 5) Тип пищи
// 6) Тип пищи, которую даёт при кончине
// 7) Необходимость партнёра для размножения (гермофродит, деление и т.д.)
// 8) Скорость
// 9) Энергия
// 10) Атака
// 11) Скорость питания
// 12) Здоровье
// 13) Радиус опыления
// 14) Пол (кажется)
// 15) Продолжительность жизни (старость->смерть) 
const int VARIANTS_PAR[] = {0,0,0,0,0,1,1,1,0,0,0,0,0,1,0}; // для типовых параметров максимальный номер параметра
const float SHIFT_PAR[] = {1,1,10,0.1,0.1,1,1,1,1,10,1,1,1,1,1,1}; // сдвиг на const, кажется, не работал.


// создать описание параметров, заменить // PAR_CHANGE везде на соответствующий параметр
enum target
{
	Null = 1,
	eat,
	motion,
	reprod // размножение
};
using namespace std;
class Animal
{
private:
	double energy;
	double kinetic_energy;
	double for_live;
	double en_div;

	double prev_dt;
public:
	// !@#$  convert to privat
	GLfloat an_color[3];
	
	double age;
	Vector position;
	Vector direction; // текущее направление движения (сейчас, норма finish)

	target targ;   // нынешняя цель
	
	Animal();
	Animal(double en, double ki_en, double for_li, double en_d, GLfloat color[3], Vector pos, Vector dir);
	Animal(const Animal& anim);
	Animal& operator= (const Animal& val); // fix

	const double getDt() const {return prev_dt; }
	const double getEnergy() const {return energy; }
	const double getKinEn() const {return kinetic_energy; }
	const double getForLive() const {return for_live; }
	const double getEnDiv() const { return en_div; }
	void drowAnimal(Vector centre);
	inline void devEnergy() { energy *= 0.5; }
	void devide(Map& , deque <Animal>& );

	void new_maj(deque <Animal>&, const Map& map); // додел
	void move(double dt, Map&); // определить
	void eating(double dt, Map&); // определить
	bool live(Map&, deque <Animal>&, double dt, double absdt, int n);
};
#endif;

