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

class Priznak // �������� ������ � ���������
{
private:
	int num;// ����� ��������, ������� ��������
	bool reg;  // ����������� �����������
	float* cage; //���������� ������������� (5 ���� ���, ��� �������� �� 5, ������ = 1)
public:
	Priznak();
	Priznak(int,bool,float*);
	Priznak(int,bool,const float*);
	Priznak(const Priznak& pr);
	~Priznak();

	inline const int getN() const {return num;}
	inline const bool getReg() const {return reg;}
	inline const float* getAgeCharac() const {return cage;}
	
	inline void setN(int n) {num = n;}
	inline void setReg(bool re) {reg = re;}
	inline  void setAgeCharac(const float ca[5]) {cage[0] = ca[0]; cage[1] = ca[1]; cage[2] = ca[2]; cage[3] = ca[3]; cage[4] = ca[4];}
}; // ����������

class Gene // ������ ������������ �� ���� �������� ������� �� �������: dom,pri.
{
private:
	double mut; // ����������� �������;  ~ dev* 4
	int num;
	bool dom;// �������������
	unsigned int* pies;
public:
	Gene();
	Gene(double,int, bool, unsigned int[5]);
	Gene(const Gene& gen);
	Gene& operator=(const Gene& gen);
	~Gene();
	
	void writeGene(FILE*); 
	void mutation(); // ������� ����, (��������,���������������� � ������������ �����)

	Gene* cross(Gene* gen, int n); // �������������� ����� ��� �������������, ����� �������� �� ������������� ��������

	inline const double getMut() const {return mut;}
	inline const int getNum() const {return num;}
	inline const bool getDom() const {return dom;}
	inline const unsigned int* getPies() const {return pies;}

	inline double getMut() {return mut;}
	inline bool getDom() {return dom;}
	inline unsigned int* getPies() {return pies;}

	bool comparison(const Gene& gen1, bool type) const; // !!!!!
}; // �� ����������

class Chromosome
{
private:
	deque <Gene> gen;//���� ���������
	int len;// ���������� �����
	int max_len; // ����� ������ ���������
	double mutation; //����������� ������� ������ ��������� �/��� ������� �����
	
public:
	Chromosome();
	Chromosome(const Gene* gen1, int len1,double mutation1);
	Chromosome(const Chromosome& ch);
	Chromosome& operator=(const Chromosome& chr);
	//~Chromosome();
	void mutat();
	void setLen() { len = len==gen.size() ? len : gen.size();}

	void crossing(Chromosome& ch);
	bool operator!=(const Chromosome& right) const; // ���������� �� ���? ����: ������� �������� � ���� �����.

	inline const deque<Gene>& getGen() const {return gen;}
	inline const int getLen() const {return len;}
	inline const double getMutat() const {return mutation;}
	inline const int getMax() const {return max_len;}

	inline deque<Gene>& getGen() {return gen;}
};

class Parametr : public Priznak
{
protected:
	int norm_par; // �������� ��������� ��� ������� ��������
	float par; // ������� ��������, ��������, �����������
	//double oldage; // �������
	double maxage; // need for calculate polage
	double* polage;
public:
	Parametr();
	Parametr(const Priznak&,double);
	Parametr(const Parametr* par);
	Parametr& operator=(const Parametr*);
	virtual void dam(double damage);
	virtual void regen(double);
	virtual void change_age(double ages);
	void live(double damage,double dt,double ages)
	{
		dam(damage);
		regen(dt);
		change_age(ages);
	}
	inline const int getNorm() const { return norm_par;}
	inline const float getPar() const { return par; }
	//inline const double getOldage() const { return oldage; }
	inline const double getMaxage() const { return maxage; }
	inline const double* getPolage() const { return polage; }
};

class Dig_Par : public Parametr
{
public:
	Dig_Par();
	Dig_Par(const Priznak&,double);
	Dig_Par(const Dig_Par* par);
	Dig_Par& operator=(const Dig_Par*);
	void dam(double damage);
	void regen(double dt);
	void change_age(double ages);
};

class Typ_Par : public Parametr
{
public:
	Typ_Par();
	Typ_Par(const Priznak&,double);
	Typ_Par(const Typ_Par* par);
	Typ_Par& operator=(const Typ_Par*);

	void dam(double damage);
	void regen(double dt);
	void change_age(double ages);
};

enum target
{
	sav_life = 1,
	attack,
	eat,
	reprod // �����������
};
using namespace std;
class Animal
{
private:
	deque <Chromosome> chr;
	int num_chr; // ��� �������� ������
	Animal* anim_aim;
	int num_aim;
	double prev_dt;
	void norm_finish(int demen);
	bool way;
	int maxage;
public:
	// !@#$  convert to privat
	GLfloat an_color[3];
	
	Parametr** param; // vosmojno nado ** t.k. virtual
	double age;
	Vector position;
	Vector finish; // � ���������� �������� �� �������
	Vector direction; // ������� ����������� �������� (������, ����� finish)
	double hunger; // �����
	//double thirst; // �����
	target targ;   // �������� ����
	// !@#$
	bool aim; // ���� �������� ��� ������� �����. 1 - ���� ��������, 0 - �������
	// !@#$
	
	Animal();
	Animal(Chromosome* chr_val, int num_chr, Vector pos1);
	//Animal(Parametr** par1);
	Animal(const Animal& anim); // �����������
	Animal& operator= (const Animal& val); // fix
	void writeAnimal(FILE*);
	const int getNumChr()const{return num_chr;}
	const Chromosome& getChr(int n) const{//return n< num_chr ? chr[n] : chr[num_chr-1];}
		if(n<num_chr)
			return chr[n];
		else
			return chr[num_chr-1];
		return chr[num_chr-1];
	}
	Animal* getAnimAim() const {return anim_aim;}
	const int getNumAim() const {return num_aim;}
	const double getDt() const {return prev_dt; }
	void setMaxAge(int mage) {maxage = mage; }
	const int getMaxAge() const{return maxage; }
	const bool getWay() const {return way;}
	
	bool operator==(const Animal&) const; // good
	void setPar();

	void drowAnimal(Vector centre);
	
	//void getPar();
	bool cross(deque <Animal>& anim);

	void new_maj(deque <Animal>&,const Map& map, int n); // �����
	double move(double dt); // ����������
	void relaxing(double dt); // ����������
	bool atak(deque <Animal>&); // ����������
	//void drink(double dt); // ����������
	bool eating(double dt, Map&); // ����������
	int live(Map&, deque <Animal>&, double dt, double absdt, int n);
};
#endif;

