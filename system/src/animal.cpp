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

bool par_num_for_dom[NUM_PAR_IN_CHR];
Chromosome chr1[6];
Chromosome ** chr2 = NULL;
bool first_chr = true;


//Priznak
Priznak::Priznak() :
	num(0),
	reg(false),
	cage(new float[5])
{
	cage[0] = cage[1] = cage[2] = cage[1] = cage[2] = 0;
}
Priznak::Priznak(int num1,bool reg1,float* age1):
	num(num1),
	reg(reg1),
	cage(new float[5])
{
	cage[0] = age1[0];
	cage[1] = age1[1];
	cage[2] = age1[2];
	cage[3] = age1[3];
	cage[4] = age1[4];
}
Priznak::Priznak(int num1,bool reg1,const float* age1):
	num(num1),
	reg(reg1),
	cage(new float[5])
{
	cage[0] = age1[0];
	cage[1] = age1[1];
	cage[2] = age1[2];
	cage[3] = age1[3];
	cage[4] = age1[4];
}
Priznak::Priznak(const Priznak& pr) :
	num(pr.getN()),
	reg(pr.getReg()),
	cage(new float[5])
{
	const float* cont = pr.getAgeCharac();
	cage[0] = cont[0];
	cage[1] = cont[1];
	cage[2] = cont[2];
	cage[3] = cont[3];
	cage[4] = cont[4];
	delete cont;
}
Priznak::~Priznak(){
	delete[5] cage;
}
// Gene
Gene::Gene() :
	mut(0),
	num(0),
	dom(false),
	pies(new unsigned int[5])
{
	pies[0] = 0;
	pies[1] = 0;
	pies[2] = 0;
	pies[3] = 0;
	pies[4] = 0;
}
Gene::Gene(double mut1,int num1, bool dom1,unsigned int pies1[5]) :
	dom(dom1),
	num(num1),
	mut(mut1),
	pies(new unsigned int[5])
{
	pies[0] = pies1[0];
	pies[1] = pies1[1];
	pies[2] = pies1[2];
	pies[3] = pies1[3];
	pies[4] = pies1[4];
}
Gene::Gene(const Gene& ge):
	dom(ge.getDom()),
	num(ge.getNum()),
	mut(ge.getMut())
{
	pies = new unsigned int[5];
	const unsigned int* pies1 = ge.getPies(); 
	pies[0] = pies1[0];
	pies[1] = pies1[1];
	pies[2] = pies1[2];
	pies[3] = pies1[3];
	pies[4] = pies1[4];
}
Gene& Gene::operator=(const Gene& gen){
	dom = gen.getDom();
	num = gen.getNum();
	mut = gen.getMut();
	if(pies == NULL)
		pies = new unsigned int[5];
	const unsigned int* pies1 = gen.getPies(); 
	pies[0] = pies1[0];
	pies[1] = pies1[1];
	pies[2] = pies1[2];
	pies[3] = pies1[3];
	pies[4] = pies1[4];

	return *this;
}
Gene::~Gene(){
	delete[5] pies;
}

void Gene::writeGene(FILE* F){
	if(F==NULL) 
		cout <<"ERROR file map";
	else{
		fputs(myto_string(dom).c_str(),F);
		for(int i=0;i<160;i++){
			fputs((myto_string((int)((pies[i/32]<<i) >> 31))).c_str(), F);
			if(i!=159)
				fputs(", ", F);
		}
	}
}
void Gene::mutation()
{
	int mutation = chance(MAX_RAND);
	if((mut > EPS && mutation%((int)(100.0/mut)) == 1))
	{
		int count = mutation%4;
		for(int i = 0;i<count;i++)
		{
			int plase = chance(MAX_RAND)%162;
			if(plase == 0)
				dom ^=dom;
			else 
			{
				plase--;
				pies[plase/32] ^= 1 << plase%32;
			}
		}
	}
	if(mut > EPS && mutation%((int)(100.0/mut)) == 0){ 
		mut = (double)mutation/(double)mut;
	}
}
Gene* Gene::cross(Gene* gen, int n)
{
	if(n>161)
		return NULL;
	/*if(n==1){
		bool cop = gen.getDom();
		*gen.getDom() = dom;
		dom = cop;
		return gen; 
	}*/
	if(n!=1){
		int t = (n-1)/32;
		int p = (n-1)%32;
		unsigned int cop = pies[t];

		pies[t] = (gen->getPies()[t] >>(32-p)) <<(32-p) | (pies[t] << p) >> p;
		gen->getPies()[t] = (gen->getPies()[t] << (p)) >>(p) | (cop>> (32-p)) << (32-p);
	}
	gen->mutation();
	mutation(); // сильно не уверен.
	return this;
}// проверить корректность возвращения такого значения
bool Gene::comparison(const Gene& gen1, bool type) const {
    if(!type){
        unsigned int different=0;
        for(int i = 0; i<4; i++){
				if(pies[i]>0 && gen1.getPies()[i]>0) 
				    different += (unsigned int)((double)(gen1.getPies()[i])/(double)(pies[i]));
				else
				    different += gen1.getPies()[i] + pies[i]; // т.к. одно значение равно = 0
        }
        if(different*0.2 > 2)
            return true;
        else
            return false;
    }
    else{
        int condition1 = 0;
        int condition2 = 0;
        for(int i = 0; i<4; i++){
            unsigned int pi1 = gen1.getPies()[i];
            for(int j = 0; j<32; j++)
            {
                condition1 += (pies[i] && (1 << j)) >> j;
                condition2 += (pi1 && (1 << j)) >> j;
            }
            condition1 = condition1/16 >= 1? 1 : 0;
            condition2 = condition2/16 >= 1? 1 : 0;
            if(condition1 != condition2)
                return true;
        }
        return false;
    } 
    return false;
}// not test

//Chromosome
Chromosome::Chromosome():
	len(0),
	max_len(0),
	mutation(0)
{}
Chromosome::Chromosome(const Gene * gen1,int len1, double mutation1) :
	len(len1),
	max_len(0),
	mutation(mutation1)
{
	for(int i =0;i<len;i++)
	{
		gen.push_back(gen1[i]);
	}
}
Chromosome::Chromosome(const Chromosome& ch) :
	len(ch.getLen()),
	max_len(ch.getMax()),
	mutation(ch.getMutat())
{
	for(int i =0;i<len;i++)
	{
		gen.push_back(ch.getGen()[i]);
	}
}
Chromosome& Chromosome::operator=(const Chromosome& chr){
	len = chr.getLen();
	max_len = chr.getMax();
	mutation = chr.getMutat();
	gen.clear();
	for(int i =0;i<len;i++)
	{
		gen.push_back(chr.getGen()[i]);
	}
	return *this;
}
//Chromosome::~Chromosome(){
//	delete[len] gen;
//}
void Chromosome::mutat()
{
	for(int i =0;i<len;i++)
		gen[i].mutation();
}// not test
void Chromosome::crossing(Chromosome& ch) // так же и смена хромосом "местами", в качестве бонуса
{
    int con = chance(4); // fo tests change chanse to const
    int n;
    for(int i =0;i<=con;i++){
        n = chance(161*len-1);
        if(len == ch.len){
            int ge = n/161;
			int pos = n%161;
			chr1[4].getGen().insert(chr1[4].getGen().begin(), gen.begin(),gen.begin()+ge);
			chr1[4].getGen().insert(chr1[4].getGen().end(), ch.getGen().begin()+ge, ch.getGen().end());
			chr1[5].getGen().insert(chr1[5].getGen().begin(),ch.getGen().begin(), ch.getGen().begin() + ge);
			chr1[5].getGen().insert(chr1[5].getGen().end(),gen.begin()+ge, gen.end());
			chr1[5].setLen();
			gen.clear();
            for(int i =0;i<len;i++)
				gen.push_back(chr1[4].getGen()[i]);
            ch = chr1[5];

			chr1[4].getGen().clear();
			chr1[5].getGen().clear();
        }
    }
}// not test
bool Chromosome::operator!=(const Chromosome& right) const
{
    if(len!= right.len)
		return true;
    else{
        deque <Gene> ge1 = right.getGen();
        for(int i = 0;i<len;i++)
        {
			if(gen[i].comparison(ge1[i],TYP_GEN[i]))
                return true;
        }
    }
	return false;
} // not test

// Parametr

Parametr::Parametr():
	norm_par(0),
	par(0),
	//oldage(0),
	maxage(0),
	polage(new double[5]) // т.к. начальное значение (0,0)
{
	setN(0);
	setReg(false);
	float ca[] = {0.0,0.0,0.0,0.0,0.0}; 
	setAgeCharac(ca);
	polage[0] = polage[1] = polage[2] = 
	polage[3] = polage[4];
}
Parametr::Parametr(const Priznak& pr, double age):
	norm_par(0),
	par(pr.getAgeCharac()[0]),
	//oldage(0),
	maxage(age),
	polage(new double[5])
{
	setN(pr.getN());
	setReg(pr.getReg());
	setAgeCharac(pr.getAgeCharac());
	double pos = maxage*0.25;
	const float* cag = getAgeCharac();
	polage[0] = cag[0];
	polage[1] = cag[1]/pos;
	polage[2] = cag[2]/(pos*2);
	polage[3] = cag[3]/(maxage-pos); // 3/4 pos = 1/4
	polage[4] = cag[4]/(maxage);

	polage[2] = (polage[2]-polage[1])/pos;
	polage[3] = (polage[3]-polage[1])/(pos*2);
	polage[4] = (polage[4]-polage[1])/(maxage-pos); 

	polage[3] = (polage[3]-polage[2])/(pos);
	polage[4] = (polage[4]-polage[2])/(pos*2); 

	polage[4] = (polage[4]-polage[3])/(pos); // calkul pol coefs
}// not test
Parametr::Parametr(const Parametr* par):
	norm_par(par->getNorm()),
	par(par->getPar()),
	//oldage(par->getOldage()),
	maxage(par->getMaxage()),
	polage(new double[5])
{
	setN(par->getN());
	setReg(par->getReg());
	setAgeCharac(par->getAgeCharac());
	polage[0] = par->getPolage()[0];
	polage[1] = par->getPolage()[0];
	polage[2] = par->getPolage()[0];
	polage[3] = par->getPolage()[0];
	polage[4] = par->getPolage()[0];
}
Parametr& Parametr::operator=(const Parametr* param){
	norm_par = param->getNorm();
	par = param->getPar();
	//oldage = param->getOldage();
	maxage = param->getMaxage();
	setN(param->getN());
	setReg(param->getReg());
	setAgeCharac(param->getAgeCharac());
	polage = new double[5];
	polage[0] = param->getPolage()[0];
	polage[1] = param->getPolage()[0];
	polage[2] = param->getPolage()[0];
	polage[3] = param->getPolage()[0];
	polage[4] = param->getPolage()[0];

	return *this;
}
void Parametr::dam(double damage)
{
	par -= damage;
}
void Parametr::regen(double dt)
{
	if(getReg() && (int)par < norm_par)
		par += dt*0.1;
}
void Parametr::change_age(double ages)
{
	if(ages < maxage)
		norm_par = (float)(polage[0] + ages*(polage[1] + ages*(polage[2] + ages*(polage[3] + ages*polage[4]))));
	else
		par = 0.0;
	if(norm_par <0.0)
		norm_par = 0.0; 
}

//Dig_Par
Dig_Par::Dig_Par()
{
	norm_par = 0;
	par = 0;
	//oldage = 0;
	maxage = 0;
	polage = new double[5]; // т.к. начальное значение (0,0)
	setN(0);
	setReg(false);
	float ca[] = {0.0,0.0,0.0,0.0,0.0};  
	setAgeCharac(ca);
	polage[0] = polage[1] = polage[2] = 
	polage[3] = polage[4];
}
Dig_Par::Dig_Par(const Priznak& param,double age)
{	
	norm_par = 0;
	par = param.getAgeCharac()[0];
	//oldage = 0;
	maxage = age;
	if(!polage)
		polage = new double[5];
	setN(param.getN());
	setReg(param.getReg());
	setAgeCharac(param.getAgeCharac());
	double pos = maxage*0.25;
	const float* cag = getAgeCharac();
	polage[0] = cag[0];
	polage[1] = cag[1]/pos;
	polage[2] = cag[2]/(pos*2);
	polage[3] = cag[3]/(maxage-pos); // 3/4 pos = 1/4
	polage[4] = cag[4]/(maxage);

	polage[2] = (polage[2]-polage[1])/pos;
	polage[3] = (polage[3]-polage[1])/(pos*2);
	polage[4] = (polage[4]-polage[1])/(maxage-pos); 

	polage[3] = (polage[3]-polage[2])/(pos);
	polage[4] = (polage[4]-polage[2])/(pos*2); 

	polage[4] = (polage[4]-polage[3])/(pos); // calkul pol coefs
}// not test	
Dig_Par::Dig_Par(const Dig_Par* param)
{
	norm_par = param->getNorm();
	par = param->getPar();
	//oldage(par->getOldage()),
	maxage = param->getMaxage();
	if(!polage)
		polage = new double[5];
	setN(param->getN());
	setReg(param->getReg());
	setAgeCharac(param->getAgeCharac());
	polage[0] = param->getPolage()[0];
	polage[1] = param->getPolage()[0];
	polage[2] = param->getPolage()[0];
	polage[3] = param->getPolage()[0];
	polage[4] = param->getPolage()[0];
}
Dig_Par& Dig_Par::operator=(const Dig_Par* param)
{
	norm_par = param->getNorm();
	par = param->getPar();
	//oldage(par->getOldage()),
	maxage = param->getMaxage();
	if(!polage)
		polage = new double[5];
	setN(param->getN());
	setReg(param->getReg());
	setAgeCharac(param->getAgeCharac());
	polage[0] = param->getPolage()[0];
	polage[1] = param->getPolage()[0];
	polage[2] = param->getPolage()[0];
	polage[3] = param->getPolage()[0];
	polage[4] = param->getPolage()[0];

	return *this;
}
void Dig_Par::dam(double damage){
	par -= damage;
}
void Dig_Par::regen(double value){
	if(par< norm_par)
        par += value;
    if(par> norm_par)
        par -= 0.1; 
}// not test
void Dig_Par::change_age(double ages){

	if(ages< maxage){
		norm_par =polage[0] + ages * (polage[1] + (ages-maxage*0.25)*(polage[2] + (ages-maxage*0.5)*(polage[3] + (ages-maxage*0.75)*polage[4])));
	}
	else
		par = 0;
}// not test

//Typ_Par
Typ_Par::Typ_Par()
{
	norm_par = 0;
	par = 0;
	//oldage = 0;
	maxage = 0;

	polage = new double[5]; // т.к. начальное значение (0,0)
	setN(0);
	setReg(false);
	float ca[] = {0.0,0.0,0.0,0.0,0.0};  
	setAgeCharac(ca);
	polage[0] = polage[1] = polage[2] = 
	polage[3] = polage[4];
}
Typ_Par::Typ_Par(const Priznak& pr,double age)
{	
	norm_par = 0;
	par = pr.getAgeCharac()[0];
	maxage = age;
	if(!polage)
		polage = new double[5];

	setN(pr.getN());
	setReg(pr.getReg());
	setAgeCharac(pr.getAgeCharac());

	double pos = maxage*0.25;
	const float* cag = getAgeCharac();
	polage[0] = cag[0];
	polage[1] = cag[1]/pos;
	polage[2] = cag[2]/(pos*2);
	polage[3] = cag[3]/(maxage-pos); // 3/4 pos = 1/4
	polage[4] = cag[4]/(maxage);

	polage[2] = (polage[2]-polage[1])/pos;
	polage[3] = (polage[3]-polage[1])/(pos*2);
	polage[4] = (polage[4]-polage[1])/(maxage-pos); 

	polage[3] = (polage[3]-polage[2])/(pos);
	polage[4] = (polage[4]-polage[2])/(pos*2); 

	polage[4] = (polage[4]-polage[3])/(pos); // calkul pol coefs
}// not test
Typ_Par::Typ_Par(const Typ_Par* param)
{
	norm_par = param->getNorm();
	par = param->getPar();
	maxage = param->getMaxage();
	if(!polage)
		polage = new double[5];
	setN(param->getN());
	setReg(param->getReg());
	setAgeCharac(param->getAgeCharac());

	polage[0] = param->getPolage()[0];
	polage[1] = param->getPolage()[0];
	polage[2] = param->getPolage()[0];
	polage[3] = param->getPolage()[0];
	polage[4] = param->getPolage()[0];
}
Typ_Par& Typ_Par::operator=(const Typ_Par* param)
{
	norm_par = param->getNorm();
	par = param->getPar();
	maxage = param->getMaxage();
	if(!polage)
		polage = new double[5];
	setN(param->getN());
	setReg(param->getReg());
	setAgeCharac(param->getAgeCharac());

	polage[0] = param->getPolage()[0];
	polage[1] = param->getPolage()[0];
	polage[2] = param->getPolage()[0];
	polage[3] = param->getPolage()[0];
	polage[4] = param->getPolage()[0];

	return *this;
}
void Typ_Par::dam(double damage)
{
}
void Typ_Par::regen(double dt)
{
}
void Typ_Par::change_age(double ages)
{
	if(ages< maxage){
		norm_par =polage[0] + ages * (polage[1] + (ages-maxage*0.25)*(polage[2] + (ages-maxage*0.5)*(polage[3] + (ages-maxage*0.75)*polage[4])));
		if(norm_par>VARIANTS_PAR[getN()])
			norm_par= VARIANTS_PAR[getN()];
		if(norm_par<0)
			norm_par= 0;
	}
	else
		par = 0;
}// not test

//Animal
Animal::Animal():
	num_chr(0),
	anim_aim(NULL),
	num_aim(0),
	prev_dt(0),
	way(false),
	maxage(0),

	param(NULL),
	age(0),
	position(Vector(0,0)),
	finish(Vector(0,1)),
	direction(Vector(0,0)),
	hunger(0), 
	targ(sav_life),
	aim(false)
{an_color[0] = 0.0; an_color[1] = 0.0; an_color[2] = 0.0;}
Animal::Animal(Chromosome* chr_val, int num_chr1, Vector pos1):
	num_chr(num_chr1),
	anim_aim(NULL),
	num_aim(0),
	prev_dt(0),
	way(false),
	maxage(0),
	position(pos1),
	finish(Vector(0,1)),
	direction(Vector(0,1)),
	param(NULL),
	age(0),
	hunger(0), 
	targ(sav_life),
	aim(false)
{
	an_color[0] = 0.0; an_color[1] = 0.0; an_color[2] = 0.0;
	for(int i=0;i<num_chr;i++)
		chr.push_back(chr_val[i]);
}
//Animal::Animal(Parametr** par1):
//	chr(NULL),
//	num_chr(0),
//	anim_aim(NULL),
//	num_aim(0),
//	prev_dt(0),
//	
//	param(NULL),
//	age(0),
//	position(Vector(0,0)),
//	finish(Vector(0,0)),
//	direction(Vector(0,0)),
//	hunger(0), 
//	targ(sav_life),
//	aim(false)
//{
//	param = new Parametr*[NUM_PAR_IN_CHR];
//	float r[3]; r[0]=r[1]=r[2] = 0;
//	int k = 1+NUM_PAR_IN_CHR/3;
//	for(int i=0;i<NUM_PAR_IN_CHR;i++){
//		param[i] = par1[i];
//		an_color[NUM_PAR_IN_CHR/(k)] += param[i].getAgeCharac()[0] + param[i].getAgeCharac()[1] + param[i].getAgeCharac()[2] + param[i].getAgeCharac()[3]+param[i].getAgeCharac()[4];
//	}
//	an_color[0] = an_color[0] - (int)(an_color[0]/128.0)*128;
//	an_color[1] = an_color[1] - (int)(an_color[1]/128.0)*128;
//	an_color[2] = an_color[2] - (int)(an_color[2]/128.0)*128;
//}
Animal::Animal(const Animal& anim):
	num_chr(anim.getNumChr()),
	anim_aim(anim.getAnimAim()),
	num_aim(anim.getNumAim()),
	prev_dt(anim.getDt()),
	way(anim.getWay()),
	maxage(anim.getMaxAge()),
	position(anim.position),
	finish(anim.finish),
	direction(anim.direction),
	age(anim.age),
	hunger(anim.hunger), 
	targ(anim.targ),
	aim(anim.aim)
{
	/*if(chr!=NULL)
		delete chr;*/
	for(int i=0;i<num_chr;i++)
		chr.push_back(anim.getChr(i));
	param = new Parametr*[NUM_PAR_IN_CHR];
	for(int i=0;i<NUM_PAR_IN_CHR && anim.param != NULL;i++)
		param[i] = anim.param[i];
	an_color[0] = anim.an_color[0];
	an_color[1] = anim.an_color[1];
	an_color[2] = anim.an_color[2];
}
Animal& Animal::operator= (const Animal& val)
{
    num_chr = val.num_chr;
	chr.clear();
    for(int i =0;i<num_chr;i++){
		chr.push_back(val.getChr(i));
    }
	if(param==NULL)
		param = new Parametr*[NUM_PAR_IN_CHR];
	for(int i =0;i<NUM_PAR_IN_CHR && val.param!=NULL && param!=NULL;i++){
        param[i] = val.param[i];
    }
	anim_aim = val.getAnimAim();
	num_aim = val.getNumAim();
	prev_dt = val.getDt();
	way = val.getWay();
	maxage = val.getMaxAge();
	age = val.age;
	position = val.position;
	finish = val.finish;
	direction = val.direction;
	hunger = val.hunger; 
	targ = val.targ;
	aim = val.aim;
	an_color[0] = val.an_color[0];
	an_color[1] = val.an_color[1];
	an_color[2] = val.an_color[2];

	return *this;
}

void Animal::norm_finish(int demen){
	if(finish.GetX()< 0.0 || finish.GetY()< 0.0 || finish.GetX()> demen || finish.GetY()> demen){
		double len = -1.0;
		Vector h = Vector();
		double fin = finish.length();
		double cl = 0;
		if(finish.GetX()< 0.0){
			len = position.GetX();
			h = Vector(len,0);
			cl = sqrt(fin*fin - len*len);
			if(cl+ position.GetY()>demen)
				cl = demen-position.GetY();
			if(cl+ position.GetY()<0)
				cl= position.GetY();
			
			if(finish.GetY() > position.GetY())
				finish = h + Vector(0,cl);
			else
				finish = h - Vector(0,cl);
		}
		if(finish.GetX() > demen){
			len = demen - position.GetX();
			h = Vector(len,0);
			cl = sqrt(fin*fin - len*len);
			if(cl+ position.GetY()>demen)
				cl = demen-position.GetY();
			if(cl+ position.GetY()<0)
				cl= position.GetY();
			
			if(finish.GetY() > position.GetY())
				finish = h + Vector(0,cl);
			else
				finish = h - Vector(0,cl);
		}
		if(finish.GetY()< 0.0){
			len = position.GetY();
			h = Vector(0,len);
			cl = sqrt(fin*fin - len*len);
			if(cl+ position.GetX()>demen)
				cl = demen-position.GetX();
			if(cl+ position.GetX()<0)
				cl= position.GetX();
			
			if(finish.GetX() > position.GetX())
				finish = h + Vector(0,cl);
			else
				finish = h - Vector(0,cl);
		}
		if(finish.GetY() > demen){
			len = demen - position.GetY();
			h = Vector(0,len);
			cl = sqrt(fin*fin - len*len);
			if(cl+ position.GetX()>demen)
				cl = demen-position.GetX();
			if(cl+ position.GetX()<0)
				cl= position.GetX();
			
			if(finish.GetX() > position.GetX())
				finish = h + Vector(0,cl);
			else
				finish = h - Vector(0,cl);
		}	
	}
}

void Animal::writeAnimal(FILE* F){
	if(F==NULL) 
		cout <<"ERROR file map";
	else{
		string chrom = "chr[";
		Chromosome ch = Chromosome();
		Gene gen = Gene();
		for(int i =0;i<num_chr;i++){
			fputs(chrom.c_str(),F);
			fputs(myto_string(i).c_str(),F);
			fputs("]", F);
			ch = chr[i];
			for(int j =0;j<ch.getLen();j++){
				gen = ch.getGen()[j];
				fputs("[",F);
				gen.writeGene(F);
				fputs("], ",F);
			}
		}
		fputs("\n",F);
	}
}
bool Animal::operator==(const Animal& anim1) const
{
    //bool test = true;
	if(anim1.num_chr != num_chr)
		return false;
	else{
		for(int i = 0;i<num_chr;i++)
		{
			if(chr[i] != anim1.chr[i])
				return false;
		}
		return true;
	}
	return false;
} // not test
void Animal::setPar()
{
	param = new Parametr*[NUM_PAR_IN_CHR];
	int test = 0;
	float k = 3.0/NUM_PAR_IN_CHR;//1+NUM_PAR_IN_CHR/3;
	double _maage = 0;
	for(int i=0;i<NUM_PAR_IN_CHR;i++)
		par_num_for_dom[i] = false;

	if(getChr(num_chr-1).getLen() != 1)
		cout << "ERROR_MAX_AGE\n";
	else
		_maage = getChr(num_chr-1).getGen()[0].getPies()[0];
	setMaxAge((int)_maage);
	float pies[5];
	Chromosome ch = Chromosome();
	Gene ge = Gene();
	for(int i=0;i<num_chr;i++){
		ch = getChr(i);
		for(int j =0;j<ch.getLen();j++){
			ge = ch.getGen()[j];
			if( !par_num_for_dom[ge.getNum()]){
				if(TYP_GEN[ge.getNum()]){
					pies[0] = pies[1] = pies[2] = pies[3] = pies[4] = 0;
					    pies[0] =(readBinStr(ge.getPies()[0],0,3) + readBinStr(ge.getPies()[0],4,7)*2 + readBinStr(ge.getPies()[0],8,11)*4 + 
									readBinStr(ge.getPies()[0],12,15)*8 + readBinStr(ge.getPies()[0],16,19)*16 + readBinStr(ge.getPies()[0],20,23)*32 +
									readBinStr(ge.getPies()[0],24,27)*64 + readBinStr(ge.getPies()[0],28,31)*128 ) * SHIFT_PAR[ge.getNum()];
					    pies[1] =(readBinStr(ge.getPies()[1],0,3) + readBinStr(ge.getPies()[1],4,7)*2 + readBinStr(ge.getPies()[1],8,11)*4 + 
									readBinStr(ge.getPies()[1],12,15)*8 + readBinStr(ge.getPies()[1],16,19)*16 + readBinStr(ge.getPies()[1],20,23)*32 +
									readBinStr(ge.getPies()[1],24,27)*64 + readBinStr(ge.getPies()[1],28,31)*128 ) * SHIFT_PAR[ge.getNum()];
					    pies[2] =(readBinStr(ge.getPies()[2],0,3) + readBinStr(ge.getPies()[2],4,7)*2 + readBinStr(ge.getPies()[2],8,11)*4 + 
									readBinStr(ge.getPies()[2],12,15)*8 + readBinStr(ge.getPies()[2],16,19)*16 + readBinStr(ge.getPies()[2],20,23)*32 +
									readBinStr(ge.getPies()[2],24,27)*64 + readBinStr(ge.getPies()[2],28,31)*128 ) * SHIFT_PAR[ge.getNum()];
					    pies[3] =(readBinStr(ge.getPies()[3],0,3) + readBinStr(ge.getPies()[3],4,7)*2 + readBinStr(ge.getPies()[3],8,11)*4 + 
									readBinStr(ge.getPies()[3],12,15)*8 + readBinStr(ge.getPies()[3],16,19)*16 + readBinStr(ge.getPies()[3],20,23)*32 +
									readBinStr(ge.getPies()[3],24,27)*64 + readBinStr(ge.getPies()[3],28,31)*128 ) * SHIFT_PAR[ge.getNum()];
					    pies[4] =(readBinStr(ge.getPies()[4],0,3) + readBinStr(ge.getPies()[4],4,7)*2 + readBinStr(ge.getPies()[4],8,11)*4 + 
									readBinStr(ge.getPies()[4],12,15)*8 + readBinStr(ge.getPies()[4],16,19)*16 + readBinStr(ge.getPies()[4],20,23)*32 +
									readBinStr(ge.getPies()[4],24,27)*64 + readBinStr(ge.getPies()[4],28,31)*128 ) * SHIFT_PAR[ge.getNum()];
					param[ge.getNum()] = new Dig_Par;
					*param[ge.getNum()] = Dig_Par(Priznak(ge.getNum(),true,pies), _maage);
					an_color[(int)(k*i)] +=pies[0]+pies[1]+pies[2]+pies[3]+pies[4];// param[i].getAgeCharac()[0] + param[i].getAgeCharac()[1] + param[i].getAgeCharac()[2] + param[i].getAgeCharac()[3]+param[i].getAgeCharac()[4];
					test++;
				}
				else{
					pies[0] = pies[1] = pies[2] = pies[3] = pies[4] = 0;

					pies[0] = readBinStr(ge.getPies()[0],0,7) + readBinStr(ge.getPies()[0],8,15)*2 +  
							  readBinStr(ge.getPies()[0],16,23)*4 + readBinStr(ge.getPies()[0],24,31)*8;
					pies[1] = readBinStr(ge.getPies()[1],0,7) + readBinStr(ge.getPies()[1],8,15)*2 +  
							  readBinStr(ge.getPies()[1],16,23)*4 + readBinStr(ge.getPies()[1],24,31)*8;
					pies[2] = readBinStr(ge.getPies()[2],0,7) + readBinStr(ge.getPies()[2],8,15)*2 +  
							  readBinStr(ge.getPies()[2],16,23)*4 + readBinStr(ge.getPies()[2],24,31)*8;
					pies[3] = readBinStr(ge.getPies()[3],0,7) + readBinStr(ge.getPies()[3],8,15)*2 +  
							  readBinStr(ge.getPies()[3],16,23)*4 + readBinStr(ge.getPies()[3],24,31)*8;
					pies[4] = readBinStr(ge.getPies()[4],0,7) + readBinStr(ge.getPies()[4],8,15)*2 +  
							  readBinStr(ge.getPies()[4],16,23)*4 + readBinStr(ge.getPies()[4],24,31)*8;

					param[ge.getNum()] = new Typ_Par;
					*param[ge.getNum()] = Typ_Par(Priznak(ge.getNum(),true,pies),_maage);
					an_color[(int)(k*i)] +=pies[0]+pies[1]+pies[2]+pies[3]+pies[4];
					test++;
				}
				par_num_for_dom[ge.getNum()] = ge.getDom();
			}
		}
	}
	pies[0] = pies[1] = pies[2] = pies[3] = pies[4] = to_gen_typ(chance(1));
	*param[14] = Typ_Par(Priznak(14,true,pies),_maage);
	param[9]->dam(param[9]->getPar()-param[2]->getPar());

	an_color[0] = (an_color[0] - ((int)(an_color[0]/128.0))*128.0)*0.07;
	an_color[1] = (an_color[1] - ((int)(an_color[1]/128.0))*128.0)*0.07;
	an_color[2] = (an_color[2] - ((int)(an_color[2]/128.0))*128.0)*0.07;

	if(test<NUM_PAR_IN_CHR)
		std::cout<<"!!!!!!!!!!___IDEOLOGICAL__ERROR__IN__setPar__Not__all__parametr___!!!!!!!!";
}

void Animal::drowAnimal(Vector centre){
	//GLfloat red[] = {10.0,0.0,0.0};
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,an_color);

	glPushMatrix();
	glTranslated(centre.GetX() + position.GetX(),centre.GetY() + position.GetY(),0.5);
	glRotated(RAD*atan2((finish-position).GetY(),(finish-position).GetX())-90.0,0.0,0.0,1.0);
	//glScaled(0.7,0.7,0.7);
	glBegin(GL_POLYGON);
		glVertex2d(-0.35,-0.35);
		glVertex2d(-0.35,0.175);
		glVertex2d(0,0.525);
		glVertex2d(0.35,0.175);
		glVertex2d(0.35,-0.35);
	glEnd();

	glTranslated(0.0,0.0,0.1);
	if(targ == sav_life)
		glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,white);
	if(targ == attack)
		glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,lgrey);
	if(targ == eat)
		glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,dgrey);
	if(targ == reprod)
		glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,green);

	glutSolidSphere(0.25,5,5);

	glPopMatrix();

}

bool Animal::cross(deque <Animal>& anim)
{
	if(anim.size() <= num_aim)
		return true;
	if(anim_aim == &anim[num_aim] && getNumChr() == anim_aim->getNumChr())
	{
		Animal an[4];
		int num = (param[0]->getPar() + anim_aim->param[0]->getPar())* 0.5; // PAR_CHANGE par + anim.par 
		if(first_chr){
			chr2 = new Chromosome*[4];
			chr2[0] = new Chromosome[NUM_PAR_IN_CHR*2]; 
			chr2[1] = new Chromosome[NUM_PAR_IN_CHR*2];
			chr2[2] = new Chromosome[NUM_PAR_IN_CHR*2];
			chr2[3] = new Chromosome[NUM_PAR_IN_CHR*2]; // так как набор двойной

			for(int i =0;i<4;i++) // на всякий случай, занулим
				for(int j =0;j<NUM_PAR_IN_CHR*2; j++){
					chr2[i][j] = Chromosome();
				}

			first_chr = false;
		} // Очень коряво, но не представляю, что делать. Однажды выделяем память. 
		//Считается, что миниум по одному гену в хромосоме и все другие случаи просто отметаем, 
		//такие животные не смогут размножаться.
		
		Vector pos = (position + anim_aim->position) * 0.5; // всё равно разбегутся, а проверки на столкновение нет
		int number = (num+3)/4;
		int counter = num%4;

		for(int i = 0; i < number; i++)
		{
			for(int e = 0;e<num_chr;e+=2) // этот алгоритм расчитан на двойной набор хромосом,
			{
				chr1[0] = getChr(e);
				chr1[0].mutat(); // считаем, что ошибка вылезает при репродуктировании, одни дети могут быть больными, другие - здоровыми
				chr1[1] = getChr(e+1);
				chr1[1].mutat();
				chr1[1].crossing(chr1[0]);

				chr1[2] = anim_aim->getChr(e);
				chr1[2].mutat();
				chr1[3] = anim_aim->getChr(e+1);
				chr1[3].mutat();
				chr1[3].crossing(chr1[2]);
				if(i+1 == number)
				{
					if(counter >= 1)
					{
						chr2[0][e] = chr1[0];
						chr2[0][e+1] = chr1[2];
					}
					if(counter >= 2)
					{
						chr2[1][e] = chr1[0];
						chr2[1][e+1] = chr1[3];
					}
					if(counter >= 3)
					{
						chr2[2][e] = chr1[1];
						chr2[2][e+1] = chr1[2];
					}
				}
				else
				{
						chr2[0][e] = chr1[0];
						chr2[0][e+1] = chr1[2];
						chr2[1][e] = chr1[0];
						chr2[1][e+1] = chr1[3];
						chr2[2][e] = chr1[1];
						chr2[2][e+1] = chr1[2];
						chr2[3][e] = chr1[1];
						chr2[3][e+1] = chr1[3];
				}
			}
			if(i+1 == number) // возможна проблема при number%4 == 0 (в ранней версии это отдельно обрабатывалось)
			{
				if(counter >= 1)
				{
					an[0] = Animal(chr2[0],num_chr,pos);
					an[0].setPar();
					anim.push_back(an[0]);

					param[9]->dam(an[0].param[2]->getPar()*0.5);
					anim_aim->param[9]->dam(an[0].param[2]->getPar()*0.5);
				}
				else
				if(counter >= 2)
				{
					an[1] = Animal(chr2[1],num_chr,pos);
					an[1].setPar();
					anim.push_back(an[1]);

					param[9]->dam(an[1].param[2]->getPar()*0.5);
					anim_aim->param[9]->dam(an[1].param[2]->getPar()*0.5);
				}
				if(counter >= 3)
				{
					an[2] = Animal(chr2[2],num_chr,pos);
					an[2].setPar();
					anim.push_back(an[2]);

					param[9]->dam(an[2].param[2]->getPar()*0.5);
					anim_aim->param[9]->dam(an[2].param[2]->getPar()*0.5);
				}
			}
			else
			{
				an[0] = Animal(chr2[0],num_chr,pos);
				an[0].setPar();
				an[1] = Animal(chr2[1],num_chr,pos);
				an[1].setPar();
				an[2] = Animal(chr2[2],num_chr,pos);
				an[2].setPar();
				an[3] = Animal(chr2[3],num_chr,pos);
				an[3].setPar();
				anim.push_back(an[0]);
				anim.push_back(an[1]);
				anim.push_back(an[2]);
				anim.push_back(an[3]);

				param[9]->dam(an[0].param[2]->getPar()*0.5);
				anim_aim->param[9]->dam(an[0].param[2]->getPar()*0.5);
				param[9]->dam(an[1].param[2]->getPar()*0.5);
				anim_aim->param[9]->dam(an[1].param[2]->getPar()*0.5);
				param[9]->dam(an[2].param[2]->getPar()*0.5);
				anim_aim->param[9]->dam(an[2].param[2]->getPar()*0.5);
				param[9]->dam(an[3].param[2]->getPar()*0.5);
				anim_aim->param[9]->dam(an[3].param[2]->getPar()*0.5);
			}
		}
		return true;
	}
	return false;
} // not test

void Animal::new_maj(deque <Animal>& anim, const Map& map,int n)
{
    int num_animal = anim.size();
	int rad = param[1]->getPar() * param[1]->getPar(); // PAR_CHANGE 10 заменить на радиус обнаружения
    bool need_eat = false;
    bool sav_life_b = false;
    bool eat_b = false;
	bool eat_attack = false;
	bool reprod_b = false;

    bool first_eat = true;
    int min_len_eat = -1;
    Vector fin_eat = Vector();
	int num_anim_eat = -1;

    bool first_rep = true;
    int min_len_rep = -1;
    Vector fin_rep = Vector();
	int num_anim_reprod = -1;

	double min_angle_eat = -2;
	double min_angle_rep = -2;

    double koef = 0;

    if(param[9]->getPar() < param[2]->getPar()) // PAR_CHANGE 20 на необход время питания(человек ~ 0.9 от максимальной энергии, верблюд ~ 0.3 от маск энергии )
        need_eat = true;
    for(int i =0;i<num_animal;i++)
	{
		if(i!=n){
			Vector test = anim[i].position;// если радиус будет тяжёл заменить на прямоугольник
			double length = (test-position).length2();
			if(length*anim[i].param[3]->getPar() < rad){ // PAR_CHANGE 0.5 заменить на масктровку

			    // проверка sav_life

			    int other = anim[i].param[4]->getPar(); // PAR_CHANGE 2 заменить на размер
			    if(param[4]->getPar() * 10 <= other || other * 10 <= param[4]->getPar()){// PAR_CHANGE 1 заменить на размер
			        //если животное замечает другое(муравей и кит или корова и лошадь)
			        if((length <rad*0.1 && !(targ ==eat && aim && ((test - finish).length2() < param[4]->getPar()*4)))
			            || ( anim[i].targ == eat && anim[i].aim && (anim[i].finish - position).length2() < param[4]->getPar()*4)){ // Если животное очень близко (мы за ним не гонимся) или хочет что-то съесть, причём нас
			            if(param[4]->getPar() >= other) // PAR_CHANGE 1 заменить на размер                               // ^- PAR_CHANGE 1 заменить на размер
							koef = -(param[4]->getPar()/other) + 31;// расстояние пропорционально размеру // PAR_CHANGE 1 заменить на размер
			            else
			                koef = -(other/param[4]->getPar()) + 31; // PAR_CHANGE 1 заменить на размер
			            Vector way_direc = test - position;
			            if(way_direc.length2() < koef * koef && koef > EPS ) // > EPS на всякий случай
			            {
			                if(targ != sav_life){
			                    finish = Vector();
								direction = Vector();
							}
			                finish += Vector_norm(way_direc) * (way_direc.length()/ koef);
			                targ = sav_life;
			                sav_life_b = true;
			            }
			        }
			    }

			    // конец проверки sav_life
            // проверка eat

			    if(!sav_life_b && need_eat){
					if(!first_eat && anim[i].param[6]->getPar() == param[5]->getPar()){ // т.к.если нашли еду на карте(eat_map в любом случае false) first_eat == false, если нашли что-то
			                    // PAR_CHANGE 3 заменить на тип пищи, котору даёт anim[i](мышь - мясо, дерево - древесина)
			                    // PAR_CHANGE 2 заменить на тип, которым питается данное животное
						targ = attack;
			            if(first_eat){
			                min_len_eat = length;
			                fin_eat = test;
			                first_eat = false;
							num_anim_eat = i;
			            }
			            if(length + EPS < min_len_eat){
							min_len_eat = length;
			                fin_eat = test;
							num_anim_eat = i;
							min_angle_eat = -2; // обнуляем минимальный угол
			            }
						if(fabs(length - min_len_eat) <=EPS){
							if(length > EPS && ( (direction&(test - position))/length ) > min_angle_eat){
								min_angle_eat = (direction&(test - position))/length;
								fin_eat = test;
								num_anim_eat = i;
							}
							if(length <= EPS && ( direction&(test - position) ) > min_angle_eat){
								min_angle_eat = (direction&(test - position));
								fin_eat = test;
								num_anim_eat = i;
							}
						}
			            eat_attack = true;
			        }
					eat_b = true;
			    }

			    // конец проверки eat
            // проверка reprod

			    if(!sav_life_b && !eat_b){ 
			        targ = reprod;
					if(anim[i].targ == reprod && (((*this)==(anim[i]) && anim[i].param[14]->getPar() != param[14]->getPar()) || param[7]->getPar()==0)){// PAR_CHANGE false заменить на необод партнёра для размножения(бактерии | мыши)
			            if(first_rep){
			                min_len_rep = length;
							fin_rep = (test + position)*0.5; // идёт половину пути, вторую половину идёт партнёр
							num_anim_reprod = i;
			                first_rep = false;
			            }
						 if(length + EPS < min_len_rep){
							min_len_rep = length;
			                fin_rep = (test + position)*0.5;
							num_anim_reprod = i;
							min_angle_rep = -2; // обнуляем минимальный угол
			            }
						if(fabs(length - min_len_rep) <=EPS){
							if(length > EPS && ( (direction&(test - position))/length ) > min_angle_rep){
								min_angle_rep = (direction&(test - position))/length;
								fin_rep = (test + position)*0.5;
								num_anim_reprod = i;
							}
							if(length <= EPS && ( direction&(test - position) ) > min_angle_rep){
								min_angle_rep = (direction&(test - position));
								fin_rep = (test + position)*0.5;
								num_anim_reprod = i;
							}
						}
			        }
					reprod_b = true;
			    }
			    // конец проверки reprod
			}
		}
    }
	int demen = map.getDemen();
	if(sav_life_b && (finish.GetX()< 0.0 || finish.GetY()< 0.0 || finish.GetX()> demen || finish.GetY()> demen)){
		double len = -1.0;
		Vector h = Vector();
		double fin = finish.length();
		double cl = 0;
		if(finish.GetX()< 0.0){
			len = position.GetX();
			h = Vector(len,0);
			cl = sqrt(fin*fin - len*len);
			if(cl+ position.GetY()>demen)
				cl = demen-position.GetY();
			if(cl+ position.GetY()<0)
				cl= position.GetY();
			
			if(finish.GetY() > position.GetY())
				finish = h + Vector(0,cl);
			else
				finish = h - Vector(0,cl);
		}
		if(finish.GetX() > demen){
			len = demen - position.GetX();
			h = Vector(len,0);
			cl = sqrt(fin*fin - len*len);
			if(cl+ position.GetY()>demen)
				cl = demen-position.GetY();
			if(cl+ position.GetY()<0)
				cl= position.GetY();
			
			if(finish.GetY() > position.GetY())
				finish = h + Vector(0,cl);
			else
				finish = h - Vector(0,cl);
		}
		if(finish.GetY()< 0.0){
			len = position.GetY();
			h = Vector(0,len);
			cl = sqrt(fin*fin - len*len);
			if(cl+ position.GetX()>demen)
				cl = demen-position.GetX();
			if(cl+ position.GetX()<0)
				cl= position.GetX();
			
			if(finish.GetX() > position.GetX())
				finish = h + Vector(0,cl);
			else
				finish = h - Vector(0,cl);
		}
		if(finish.GetY() > demen){
			len = demen - position.GetY();
			h = Vector(0,len);
			cl = sqrt(fin*fin - len*len);
			if(cl+ position.GetX()>demen)
				cl = demen-position.GetX();
			if(cl+ position.GetX()<0)
				cl= position.GetX();
			
			if(finish.GetX() > position.GetX())
				finish = h + Vector(0,cl);
			else
				finish = h - Vector(0,cl);
		}	
	}
	// просто проверка на !выход за границу карты и устранение этой проблемы

	if(need_eat && !sav_life_b){// рассматриваем еду на карте
		//int side = SQRT2 * param[1].getPar(); // PAR_CHANGE 10 заменить на радиус обнаружения
		targ = eat;
		Substratum sub = Substratum();
		double sub_len = 0;
		Vector sub_fin = Vector();
		for(int p = 0 ; p<map.getDemen(); p++)
		    for(int q = 0; q<map.getDemen();q++){
				sub = map.getSub((p * map.getDemen()) + q);
				if((sub.getPos() - position).length2() < rad && sub.getFood() == param[5]->getPar() && sub.getKeep() > 0 ){ // PAR_CHANGE 2 на тип пищи
		            sub_fin = sub.getPos();
		            sub_len = (sub_fin - position).length2();
		            if(first_eat){
		                min_len_eat = sub_len;
		                fin_eat = sub_fin;
		                first_eat = false;
		            }
					if(fabs(sub_len - min_len_eat) <=EPS){
						if(sub_len > EPS && ( (direction&(sub_fin - position))/sub_len ) > min_angle_eat){
							min_angle_eat = (direction&(sub_fin - position))/sub_len;
							fin_eat = sub_fin;
						}
						if(sub_len <= EPS && ( direction&(sub_fin - position) ) > min_angle_eat){
							min_angle_eat = (direction&(sub_fin - position));
							fin_eat = sub_fin;
						}
					}
					if(sub_len + EPS< min_len_eat){
		                min_len_eat = sub_len;
		                fin_eat = sub_fin;
						if(sub_len > EPS)
							min_angle_eat = (direction&(sub_fin - position))/sub_len; // обнуляем угол
						else
							min_angle_eat = (direction&(sub_fin - position));
		            }
		        }
		    }
	} // Если что-то уже нашли, то корректируем результат, если ничего не было, просто находим-^

	if(!need_eat && !sav_life_b)
		targ = reprod;

	if(eat_attack || targ == attack){
			if(min_len_eat < 0){
				if(!way){
					finish = Vector(chance(demen-1), chance(demen-1)); // идём искать по свету, где ...
					way = true;
				}
			}
			else{
			    finish = fin_eat;
				*anim_aim = anim[num_anim_eat];
				num_aim = num_anim_eat;
			}
    }
	if((eat_b || targ == eat) || (need_eat && !eat_attack && !eat_b)){
		if(min_len_eat < 0){
			if(!way){
				finish = Vector(chance(demen-1), chance(demen-1)); // идём искать по свету, где ...
				way = true;
			}
		}
		else
		    finish = fin_eat;
    }
	if(reprod_b || targ == reprod){
		if(min_len_rep < 0){
			if(!way){
				finish = Vector(chance(demen-1), chance(demen-1)); // идём искать по свету, где ...
				way = true;
			}
		}
		else{
			if((anim[num_anim_reprod].position - position).length2() > param[13]->getPar()*param[13]->getPar()) // PAR_CHANGE 11 на радиус опыления
				finish = fin_rep;
			else
				finish = position;
			anim_aim = &anim[num_anim_reprod];
			num_aim = num_anim_reprod;
		}
	}
	direction = Vector_norm(finish-position);
}

double Animal::move(double dt){ // т.к. считаем dt в ms
	if(dt* dt*param[8]->getPar()*param[8]->getPar()*0.000001 < (position - finish).length2()){// PAR_CHANGE 4 на скорость (можно модифицировать под среду)
        position += Vector_norm(finish-position) * dt * param[8]->getPar()*0.001; // PAR_CHANGE 4 на скорость
        param[9]->dam(dt*param[8]->getPar()*param[4]->getPar()*0.001); // PAR_CHANGE param[9] на энергию
		return dt;
    }// PAR_CHANGE 5 на размер
    else{
		way = false;
		double dist = (position - finish).length();
        param[9]->dam(dist*param[4]->getPar() * 0.001); // PAR_CHANGE param[7] на энергию
		position = finish;
		if(param[4]->getPar()> EPS)
			return dist/(param[4]->getPar() * 0.001);
		return 0;

    }  // PAR_CHANGE 5 на размер

	return 0;
}
void Animal::relaxing(double dt){// считаем, что энергия тратится меньше
	param[9]->regen(dt); // всё равно в live уменьшается
	// PAR_CHANGE param[7] на энергию
}
bool Animal::atak(deque <Animal>& animal){
	if(animal.size()-1 < num_aim) // do true pointer
		return true;
	if(animal.at(num_aim) == *anim_aim && (position- anim_aim->position).length2() < (anim_aim->param[4]->getPar() + param[4]->getPar())*(anim_aim->param[4]->getPar() + param[4]->getPar())*0.4*0.4){ // PAR_CHANGE 5,6 размер this и anim
		anim_aim->param[12]->dam(param[10]->getPar()); // PAR_CHANGE param[4] на здоровье anim
		 // PAR_CHANGE 7 на атаку this
		anim_aim->param[9]->dam(param[10]->getPar()); // PAR_CHANGE param[7] на энергию,
		// PAR_CHANGE 7 на урон this
		param[9]->dam(param[10]->getPar()*param[4]->getPar() * 0.01);// PAR_CHANGE 7 на урон this
		// PAR_CHANGE 8 на размер/массу this 
		// Или PAR_CHANGE 7*8 на энергию для удара

		//? Либо перенести 0.01 в SHIFT_PAR, либо задать как const!!!
		return false;
	}
	return true;
}
bool Animal::eating(double dt,Map& map){
	Substratum* sub = map.getSub((int)(finish.GetX())*map.getDemen() + (int)(finish.GetY()));
    if(sub->getFood() == param[5]->getPar()){ // PAR_CHANGE 2 на тип пищи животного
		if(sub->getKeep() > 0)
			param[9]->regen(sub->decay(param[11]->getPar()*dt*0.001)); // PAR_CHANGE 3 на cкорость питания
		else
			return true;
		return false;
    }
	return false;
}
int Animal::live(Map& map,deque <Animal>& anim, double dt, double absdt, int n){
    age+= dt;
	bool test = false;
	int death = 0;
	double residue_dt = dt;

    if((position - finish).length2() > 0)
        residue_dt = dt - move(dt);
    else{
		if(residue_dt > 0){
			way = false;
			if(targ == eat)
			    test = eating(residue_dt,map);
			if(targ == attack)
			    test = atak(anim);
			if(targ == reprod)
			    test = cross(anim);
		}
	}
	param[9]->dam(dt*decrease_enrg_ms); // PAR_CHANGE так и оставить 
	// ~1600 Дж/жук за жизнь, за 12 часов (1 сек модельного времени) теряет 2.857

	if(param[9]->getPar() <0 || param[12]->getPar() < 0){// PAR_CHANGE param[1] - здоровье 
	// PAR_CHANGE param[9] - энергия
		map.getSub().push_back(Substratum(VOLUM, meat, param[9]->getNorm()*to_next_trop_lev, 
								Vector((int)position.GetX(),(int)position.GetY()), false, 
								param[9]->getNorm()*to_next_trop_lev, 0.0));
		//вроде энергия, only to_next_trop_lev is transmitted to next trophic level
		death = 1; 
	}		
	else
		if(age>=maxage)
			return 2;
		if(test || (absdt-prev_dt)> time_recalcul_maj || prev_dt == 0){ 
			for(int i =0;i<NUM_PAR_IN_CHR;i++){
				param[i]->change_age(dt);
			}
			new_maj(anim, map, n);
			//cout << "Aniaml[" << n <<"] fin: " << finish.GetX() << ", " << finish.GetY() << "|\t|  " << param[9].getPar() << ", " <<dt* 0.001  <<"\n";
			prev_dt = absdt;
		}
	return death; // смерть от голода - 1, от старости - 2.
}