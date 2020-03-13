#include<math.h>
#include<time.h>
#include"../Headers/map.h"
#include"../Headers/fancs.h"
#include"../Headers/colors.h"


//Particle
Particle::Particle():
	volume(1),
	height(0),
	pos(0,0)
{}
Particle::Particle(double vo1, float he1, Vector pos1):
	volume(vo1),
	height(he1),
	pos(pos1)
{}
Particle::Particle(const Particle& pain):
	volume(pain.volume),
	height(pain.height),
	pos(pain.pos)
{}
Particle& Particle::operator=(const Particle& part){
	volume = part.volume;
	height = part.height;
	pos = part.pos;
	return *this;
}
Particle::~Particle(){
	pos.~Vector();
}

//Point
Point::Point():
	ty(ground)
{ 
	setVol(0.0); setHeigh(0.0); setPos(Vector(0.0,0.0));
}
Point::Point(double vo1,type ty1, float he1,Vector pos1):
	ty(ty1)
{ setVol(vo1); setHeigh(he1); setPos(pos1);}
Point::Point(const Point& poin):
	ty(poin.getType())
{setVol(poin.volume); setHeigh(poin.height); setPos(poin.pos);}
Point& Point::operator=(const Point& poin){
	volume = poin.volume;
	ty = poin.getType();
	height = poin.height;
	pos = poin.pos;
	return *this;
}
Point::~Point(){}

void Point::Drowpoint()
{
	if(ty == ground)
		glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,brown);
	if(ty == sand)
		glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,yellow);
	if(ty == rock)
		glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,grey);
	if(ty == vater)
		glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,blue);
	/*GLfloat noise[] = {height,height,height};
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,noise);*/ // Это для теста в градиенте
	glPushMatrix();
	//glScaled(0.3,0.3,1);
	glTranslated(pos.GetX(),pos.GetY(),0);
	glBegin(GL_QUADS);
		glVertex2d(0,0);
		glVertex2d(0,1);
		glVertex2d(1,1);
		glVertex2d(1,0);
	glEnd();

	glLineWidth(2);

	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,black);

	glBegin(GL_LINE_LOOP);
		glVertex2d(0,0);
		glVertex2d(0,1);
		glVertex2d(1,1);
		glVertex2d(1,0);
	glEnd();
	glPopMatrix();
}
void Point::writePoint(FILE * F)
{
	if(F==NULL) 
	{
		cout <<"ERROR file map";
	}
	else
	{
		string vol = "(";
		vol += myto_string((int)volume);
		vol+= ")";
		string t = "{";
		t += myto_string((int)ty);
		string he = ",";
		he += myto_string((int)height);
		string poX = ",";
		poX+= myto_string((int)pos.GetX());
		string poY = ",";
		poY += myto_string((int)pos.GetY());
		poY += "}";
		fputs(vol.c_str(),F);
		fputs(t.c_str(),F);
		fputs(he.c_str(),F);
		fputs(poX.c_str(),F);
		fputs(poY.c_str(),F);
	}
}
void Point::writePoint_whisout_vol(FILE* F)
{
	if(F==NULL) 
	{
		cout <<"ERROR file map";
	}
	else
	{
		string t = "{";
		t += myto_string((int)ty);
		string he = ",";
		he += myto_string((int)height);
		string poX = ",";
		poX+= myto_string((int)pos.GetX());
		string poY = ",";
		poY += myto_string((int)pos.GetY());
		poY += "}";
		fputs(t.c_str(),F);
		fputs(he.c_str(),F);
		fputs(poX.c_str(),F);
		fputs(poY.c_str(),F);
	}
}
void Point::writePoint_vol(FILE* F)
{
	if(F==NULL) 
	{
		cout <<"ERROR file map";
	}
	else
	{
		string vol = "(";
		vol += myto_string((int)volume);
		vol+= ")";
		fputs(vol.c_str(),F);
	}
}

//Substratum
Substratum::Substratum():
	fo(solute),
	regen(false),
	keep(0.0),
	volume_reg(0.0),
	num_bug(0)
{ 
	setVol(0.0); 
	setHeigh(0.0); 
	setPos(Vector());
}
Substratum::Substratum(double vo1, food fo1, float he1,Vector pos1,bool reg1, float keep1, float vol_reg1):
	fo(fo1),
	regen(reg1),
	keep(keep1),
	volume_reg(vol_reg1),
	num_bug(0)
{ 
	setVol(vo1); 
	setHeigh(he1); 
	setPos(pos1);
}
Substratum::Substratum(const Substratum* sub):
	fo(sub->getFood()),
	regen(sub->getRegen()),
	keep(sub->getKeep()),
	volume_reg(sub->getVol_reg()),
	num_bug(sub->getNumBug())
{	
	setVol(sub->getVol()); 
	setHeigh(sub->getHeigh()); 
	setPos(sub->getPos()); 
}
Substratum& Substratum::operator=(const Substratum* sub){
	setVol(sub->getVol()); 
	setHeigh(sub->getHeigh()); 
	setPos(sub->getPos());
	setRegen(sub->getRegen());
	setKeep(sub->getKeep());
	setFood(sub->getFood());
	setVol_reg(sub->getVol_reg());
	SetNumBug(sub->getNumBug());

	return *this;
}
Substratum::~Substratum(){}
void Substratum::drowSub(){
	if(fo == solute){
		if(keep<EPS)
			glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,blue);
		else //if(fabs(keep - Stock)<EPS )
			glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,yellow);
		//else
			//glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,grey);
	}
	if(fo == meat){
		glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,red);
	}
	glPushMatrix();
	glTranslated(pos.GetX(),pos.GetY(),0);
	glBegin(GL_QUADS);
		glVertex2d(0,0);
		glVertex2d(0,1);
		glVertex2d(1,1);
		glVertex2d(1,0);
	glEnd();

	/*if(num_bug >0){
		/*GLfloat col[3];
		col[0] = num_bug*0.75;
		col[1] =8/num_bug;
		col[2] = 0.6;*/
		/*if(num_bug < 8)
			glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,col[num_bug]);
		else
			glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,black);
	}*/
	/*glLineWidth(2);

	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,black);

	glBegin(GL_LINE_LOOP);
		glVertex2d(0,0);
		glVertex2d(0,1);
		glVertex2d(1,1);
		glVertex2d(1,0);
	glEnd();*/
	glPopMatrix();
}
float Substratum::decay(float vol){
    float cop = vol<keep ? vol : keep;
    keep -= cop;

	//keep+= 1.0<=cop ? 1.0 : cop; 
    return cop;
}

//Map
Map::Map():
num_point(0),
demension(0)
{}
Map::Map(const vector <Point>& poin, const vector <Substratum>& sub):
	num_point(po.size()),
	demension(sqrt((double)num_point))
{
	po = poin;
	substratum = sub;
}
Map::Map(const Map& map):
	num_point(map.getNumPoint()),
	demension(map.getDemen())
{
	po = map.getPoin();
	substratum = map.getSub();
}
Map& Map::operator=(const Map& map)
{
	num_point = map.getNumPoint();
	demension = map.getDemen();
	po = map.getPoin();
	substratum = map.getSub();
	return *this;
}
Map::~Map()
{
	// так как ничего содержательного в Point & Substraturm нет
	po.~vector();
	substratum.~vector();
}
// fail means file, it's not a pessimizm)
void Map::readMap(const char* fail)
{
	FILE * F;
	fopen_s(&F, fail,"r"); 
	char s;
	string st;
	if(F==NULL) 
	{
		cout <<"ERROR file map";
	}
	else
	{
		double volr = 1;
		type tr = ground;
		int hr = 0;
		Vector posr = Vector();
		while(!feof(F))
		{
			s = fgetc(F);
			if(s =='(')
				volr = readFdigit(F);

			if(s =='{')
			{
				tr = (type)readFdigit(F);
				hr = readFdigit(F);
				posr.SetX(readFdigit(F));
				posr.SetY(readFdigit(F));
				po.push_back(Point(volr,tr,hr,posr));
				substratum.push_back(Substratum(volr,solute,hr,posr,true,hr,0.1));
				num_point +=1;
			}
		}
		fclose(F);
		demension = sqrt((double)num_point);
	}
}
void Map::drawMap(Vector centre) // координата точки, которая выводится в левый нижний угол.
{
	glPushMatrix();
	glTranslated(centre.GetX(),centre.GetY(),0);
	for(int i =0;i<num_point;i++)
	{
		//po[i].Drowpoint(); - хотя это тоже можно отображать
		substratum[i].drowSub();
	}
	glPopMatrix();
}
void Map::writeMap(vector <Point> po, const char* fail)
{
	int len = po.size();
	FILE * F;
	fopen_s(&F, fail,"w"); 
	if(F==NULL) 
	{
		cout <<"ERROR file map";
	}
	else
	{
		po[0].writePoint_vol(F);
		int sys = 0;
		for(int i=0;i<len;i++)
		{
			if(po[i].getVol() != po[sys].getVol() )
			{
				po[i].writePoint_vol(F);
				sys = i;
			}
			po[i].writePoint_whisout_vol(F);
		}
		fclose(F);
	}
}
void Map::genMap(int N, double vol, float p, const char* file)// ошибка в передаче ссылки
{
	po.clear();
	num_point = 0;
	demension = N;
	//float x = rand();
	//float y = rand();
	float h = 0;
	Point poin = Point();
	FILE * F;
	fopen_s(&F, file,"w");
	if(F==NULL) 
	{
		cout <<"ERROR file map";
	}
	else
	{
		poin = Point(vol,ground,0,Vector(0,0));
		poin.writePoint_vol(F);
		fputc(10,F);
		for(int i = 0;i<N;i++)
		{
			for(int e = 0;e<N;e++)
			{
				//if( N*0.25<i && i<N*0.75 && N*0.25<e && e<N*0.75)
					h = Stock;//(PerlinNoise(x+i,y+e,p,3)-0.1)*500;//(PerlinNoise(x+i,y+e,p,3)+ 0.2)*20;
				//else
					//h = 0;
				if(h<=0)
				{
					poin = Point(vol,vater,h,Vector(i,e));
				}
				if(h>0 && h<=1200)
					poin = Point(vol,sand,h,Vector(i,e));
				if(h>1200 && h<=3000)
					poin = Point(vol,ground,h,Vector(i,e));
				if(h>3000)
					poin = Point(vol,rock,h,Vector(i,e));
				poin.writePoint_whisout_vol(F);
				po.push_back(poin);
				substratum.push_back(Substratum(vol,solute,h,Vector(i,e),true,h,0.001));
				num_point++;
			}
			fputc(10,F);
		}
		fclose(F);
	}
}
void Map::genMapTEST(int m, int k, int N, double vol, float p, const char* file)// ошибка в передаче ссылки
{
	po.clear();
	num_point = 0;
	demension = N; // all dementions k-1
	//float x = rand();
	//float y = rand();
	float h = (6.*k+5.)*m;//3./(float)k+3.*k+5.;
	//Stock = h;
	Point poin = Point();
	FILE * F;
	fopen_s(&F, file,"w");
	if(F==NULL) 
	{
		cout <<"ERROR file map";
	}
	else
	{
		poin = Point(vol,ground,0,Vector(0,0));
		poin.writePoint_vol(F);
		fputc(10,F);
		for(int i = 0;i<N;i++)
		{
			for(int e = 0;e<N;e++)
			{
				poin = Point(vol,sand,h,Vector(i,e));
				poin.writePoint_whisout_vol(F);
				po.push_back(poin);
				substratum.push_back(Substratum(vol,solute,h,Vector(i,e),true,h,0.001));
				num_point++;
			}
			fputc(10,F);
		}

		/*for(int i = 0;i<demension;i++)
		{
			if(i<3||i>k+1){
				for(int e = 0;e<demension;e++)
				{
					poin = Point(vol,sand,0,Vector(i,e));

					poin.writePoint_whisout_vol(F);
					po.push_back(poin);
					substratum.push_back(Substratum(vol,solute,0,Vector(i,e),true,0,0.001));
					num_point++;
				}
			}
			else{
				poin = Point(vol,sand,0,Vector(i,0));

				poin.writePoint_whisout_vol(F);
				po.push_back(poin);
				substratum.push_back(Substratum(vol,solute,0,Vector(i,0),true,0,0.001));
				num_point++;

				poin = Point(vol,sand,0,Vector(i,1));

				poin.writePoint_whisout_vol(F);
				po.push_back(poin);
				substratum.push_back(Substratum(vol,solute,0,Vector(i,1),true,0,0.001));
				num_point++;

				poin = Point(vol,sand,0,Vector(i,2));

				poin.writePoint_whisout_vol(F);
				po.push_back(poin);
				substratum.push_back(Substratum(vol,solute,0,Vector(i,2),true,0,0.001));
				num_point++;
				for(int e = 3;e<k+2;e++)
				{
					poin = Point(vol,sand,h,Vector(i,e));

					poin.writePoint_whisout_vol(F);
					po.push_back(poin);
					substratum.push_back(Substratum(vol,solute,h,Vector(i,e),true,h,0.001));
					num_point++;
				}
				poin = Point(vol,sand,0,Vector(i,k+2));

				poin.writePoint_whisout_vol(F);
				po.push_back(poin);
				substratum.push_back(Substratum(vol,solute,0,Vector(i,k+2),true,0,0.001));
				num_point++;

				poin = Point(vol,sand,0,Vector(i,k+3));

				poin.writePoint_whisout_vol(F);
				po.push_back(poin);
				substratum.push_back(Substratum(vol,solute,0,Vector(i,k+3),true,0,0.001));
				num_point++;

				poin = Point(vol,sand,0,Vector(i,k+4));

				poin.writePoint_whisout_vol(F);
				po.push_back(poin);
				substratum.push_back(Substratum(vol,solute,0,Vector(i,k+4),true,0,0.001));
				num_point++;

				fputc(10,F);
			}
		}*/



		fclose(F);
	}
}
void Map::regSub(float dt){
	unsigned int s = substratum.size();
	for(unsigned int i =0;i<s;i++)
		substratum[i].regKeep(dt);
}
