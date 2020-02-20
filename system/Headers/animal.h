#include<time.h>
#include<vector>
#include<deque>
#include"../Headers/map.h"


#ifndef __ANIMAL_H
#define __ANIMAL_H 

const int NUM_PAR_IN_CHR = 16; // ����� ������������� ~ �����
const bool TYP_GEN[] = {1,1,1,1,1,0,0,0,1,1,1,1,1,1,0,1};  // 1 - ��������� ��������, 0 - ��� 
// 0) ����� ��������
// 1) ������ ����������� (��� ���, ��� � ��������� � ��������) 
// 2) ����� �������, ��� ������� ��������� �����
// 3) ����������
// 4) ������ (� ��������� ��������, �� �� �����)
// 5) ��� ����
// 6) ��� ����, ������� ��� ��� �������
// 7) ������������� ������� ��� ����������� (�����������, ������� � �.�.)
// 8) ��������
// 9) �������
// 10) �����
// 11) �������� �������
// 12) ��������
// 13) ������ ��������
// 14) ��� (�������)
// 15) ����������������� ����� (��������->������) 
const int VARIANTS_PAR[] = {0,0,0,0,0,1,1,1,0,0,0,0,0,1,0}; // ��� ������� ���������� ������������ ����� ���������
const float SHIFT_PAR[] = {1,1,10,0.1,0.1,1,1,1,1,10,1,1,1,1,1,1}; // ����� �� const, �������, �� �������.


// ������� �������� ����������, �������� // PAR_CHANGE ����� �� ��������������� ��������
enum target
{
	Null = 1,
	eat,
	motion,
	reprod // �����������
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
	Vector direction; // ������� ����������� �������� (������, ����� finish)

	target targ;   // �������� ����
	
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

	void new_maj(deque <Animal>&, const Map& map); // �����
	void move(double dt, Map&); // ����������
	void eating(double dt, Map&); // ����������
	bool live(Map&, deque <Animal>&, double dt, double absdt, int n);
};
#endif;

