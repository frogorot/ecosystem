#include<iostream>
#include<fstream>
#include<vector>
#include"../glut/glut.h"
#include"../Headers/vector.h"
#include"../Headers/fancs.h"

using namespace std;
#ifndef __MAP_H
#define __MAP_H

const double VOLUM = 4.0;
const double EPS = 0.0001;

const double Stock = 6;
enum type	// тип поверхности
{
	ground = 1,
	sand,
	rock,
	vater
};
enum food{
	meat = 1,
	solute,
};

class Particle
{
protected:
	double volume; // объЄм точки 
	float height; // высота/глубина если земл€/вода.
	Vector pos; // координаты пиксел€
public:
	Particle();
	Particle(double,float,Vector);
	Particle(const Particle& part);
	Particle& operator=(const Particle& part);
	~Particle();
	inline const double getVol()const { return volume;}
	inline const float getHeigh()const { return height;}
	inline const Vector getPos()const { return pos;}
	inline void setVol(double vol) { volume = vol;}
	inline void setHeigh(float he) { height = he;}
	inline void setPos(const Vector& po) { pos = po;}
};


class Point: public Particle
{
private:
	type ty;      // тип точки. 
public:
	Point();
	Point(double,type,float,Vector);
	Point(const Point& poin);
	Point& operator=(const Point& poin);
	~Point();
	inline const type getType()const { return ty; }
	void Drowpoint();
	void writePoint(FILE*);
	void writePoint_wthisout_vol(FILE*);
	void writePoint_vol(FILE*);
};

class Substratum: public Particle
{
private:
	food fo;      // тип точки. 
	bool regen;
	float keep; // нынешняя питательность (кол-во энергии)
	float volume_reg;
	int num_bug;
public:
	Substratum();
	Substratum(double,food,float,Vector,bool,float,float);
	Substratum(const Substratum* sub);
	Substratum& operator=(const Substratum* sub);
	~Substratum();

	inline const float getKeep()const {return keep;}
	inline const float getVol_reg()const {return volume_reg;}
	inline const bool getRegen()const {return regen;}
	inline const int getNumBug()const {return num_bug;}
	const food getFood()const{ return fo;}

	inline void setKeep(float ke) {keep = ke;}
	inline void setVol_reg(float vol_r) {volume_reg = vol_r;}
	inline void setRegen(bool re) {regen = re;}
	void setFood(food f){ fo = f;}
	inline void addBug() { ++num_bug; }
	inline void eraseBug() { --num_bug; }
	inline void SetNumBug(const int n) { num_bug = n;} 

	void drowSub();

	float decay(float vol);
	inline void regKeep(float dt) {keep += (regen && keep<height) ? dt*volume_reg : 0;}
	inline void increaseKeep(float energy) { keep += energy>0.0 ? energy : 0; }
};

class Map
{
private:
	vector <Point> po; // все пиксели карты
	int num_point; // количество пикселей
	vector <Substratum> substratum; // набор питательных точек, вообще должен наследоватьс€ от Point, сейчас ty - тип пищи, height - запас.
	int demension;
	//vector <Point> vat;// вода, зачем хранитьс€ отдельно не знаю. ¬озможно, порще проверка на утопление.
public:
	Map();
	Map(const vector <Point>& poin,const vector <Substratum>& sub);
	Map(const Map& map);
	Map& operator=(const Map& map);
	~Map();
	void readMap(const char* fail); // считывание из файла
	void drawMap(Vector centre); // отрисовка карты
	void writeMap(vector <Point>,const char*);
	void genMap(int N, double vol, float p, const char* fail);
	void regSub(float dt);

	inline const vector <Point>& getPoin()const {return po;}
	inline const vector <Substratum>& getSub()const {return substratum;}

	inline const Substratum* getSub(int n) const {return n<substratum.size() ? &substratum.at(n): NULL;}
	inline Substratum* getSub(int n) {return n<substratum.size() ? &substratum.at(n): NULL;}
	inline vector <Substratum>& getSub() {return substratum;}
	inline const Point getPoint(int n) const { return po.at(n);}

	inline const int getNumPoint() const {return num_point;}
	inline int getDemen() const {return demension;} 
};
#endif;