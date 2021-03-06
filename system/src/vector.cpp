#include<cmath>
#include"../Headers/vector.h"
#include<iostream>
Vector::Vector():
	x(0),
	y(0)
{}
Vector::Vector(double x1,double y1):
	x(x1),
	y(y1)
{}
Vector::Vector(const Vector& right):
	x(right.GetX()),
	y(right.GetY())
{}
Vector::~Vector(){}
double Vector::length() const
{
	return sqrt( x*x + y*y);
}
double Vector::length2() const
{
	return x*x + y*y;
}

Vector Vector::operator+(Vector& const right) const
{
	return Vector(x+right.GetX(), y+right.GetY());
}
void Vector::operator+=(Vector& const right)
{
	x+= right.GetX();
	y+= right.GetY();
}
Vector Vector::operator-(Vector& const right) const
{
	return Vector(x - right.GetX(), y - right.GetY());
}
Vector Vector::operator-() const
{
	return Vector(-x, -y); 
}
Vector Vector::operator*(double scale) const
{
	return Vector(x * scale, y * scale);
}
Vector Vector::operator/(double u) const
{
	if(abs(u) > eps_vector)
		return Vector(x,y)*(1/u);
	std::cout << "ERROR_/_of_0_IN_VECTOR:OPERATOR/(double)" << "\n";
	return Vector(x,y)*(1/eps_vector);
}
Vector Vector::operator/(Vector& const right) const
{
	if(abs(right.GetX()) > eps_vector && abs(right.GetY()) > eps_vector)
		return Vector(this->GetX()/right.GetX(),
					  this->GetY()/right.GetY());

	std::cout << "ERROR_/_of_0_IN_VECTOR:OPERATOR/(Vector& const)" << "\n";

	if(abs(right.GetX()) > eps_vector)
		return Vector(this->GetX()/right.GetX(),
					  this->GetY()/eps_vector);

	if(abs(right.GetY()) > eps_vector)
		return Vector(this->GetX()/eps_vector,
					  this->GetY()/right.GetY());

	return Vector(this->GetX()/eps_vector,
				  this->GetY()/eps_vector);
}

double Vector::operator&(Vector& const right) const
{
	//scalar multiply
	return x * right.GetX() + y * right.GetY(); 
}
double Vector::operator^(Vector& const right) const
{//angle
	double len = sqrt( (x * x + y * y)) * right.length2();
	if((len) > eps_vector)
		return( (x * right.GetX() + y * right.GetY()) / (len) );
	return 0.0;
}
Vector Vector::operator>(Vector& const right) const
{
	return Vector(
		x * right.x,
		y * right.y);
}
Vector& Vector::operator=(const Vector& val)
{
	x = val.GetX();
	y = val.GetY();
	return *this;
}

bool Vector::operator<(double value) const
{	
	return value <= 0 || this->length2() < value*value;
}
bool Vector::operator==(Vector& const right) const
{
	return x == right.GetX() && y == right.GetY();
}
bool Vector::operator!=(Vector& const right) const
{
	return x != right.GetX() || y != right.GetY();	
}
Vector Vector_norm(Vector& value)
{
	if(value != Vector(0,0))
	{
	double longg  = sqrt(value.GetX()* value.GetX() + value.GetY() * value.GetY()); 
	return Vector(
		value.GetX() / longg,
		value.GetY() / longg);
	}
	return value;
}

iVector::iVector():
	x(0),
	y(0)
{}
iVector::iVector(int x1, int y1):
	x(x1),
	y(y1)
{}
iVector::iVector(const iVector& right):
	x(right.GetX()),
	y(right.GetY())
{}
iVector::~iVector(){}
/*double iVector::length() const
{
	return sqrt( x*x + y*y);
}*/
double iVector::length2() const
{
	return x*x + y*y;
}

iVector iVector::operator+(iVector& const right) const
{
	return iVector(x+right.GetX(), y+right.GetY());
}
void iVector::operator+=(iVector& const right)
{
	x+= right.GetX();
	y+= right.GetY();
}
iVector iVector::operator-(iVector& const right) const
{
	return iVector(x - right.GetX(), y - right.GetY());
}
iVector iVector::operator-() const
{
	return iVector(-x, -y); 
}
iVector iVector::operator*(int scale) const
{
	return iVector(x * scale, y * scale);
}
//iVector iVector::operator/(int u) const
//{
//	if(abs(u) > eps_vector)
//		return iVector(x,y)*(1/u);
//	std::cout << "ERROR_/_of_0_IN_VECTOR:OPERATOR/(double)" << "\n";
//	return iVector(x,y)*(1/eps_vector);
//}
//iVector iVector::operator/(iVector& const right) const
//{
//	if(abs(right.GetX()) > eps_vector && abs(right.GetY()) > eps_vector)
//		return iVector(this->GetX()/right.GetX(),
//					  this->GetY()/right.GetY());
//
//	std::cout << "ERROR_/_of_0_IN_VECTOR:OPERATOR/(iVector& const)" << "\n";
//
//	if(abs(right.GetX()) > eps_vector)
//		return iVector(this->GetX()/right.GetX(),
//					  this->GetY()/eps_vector);
//
//	if(abs(right.GetY()) > eps_vector)
//		return iVector(this->GetX()/eps_vector,
//					  this->GetY()/right.GetY());
//
//	return iVector(this->GetX()/eps_vector,
//				  this->GetY()/eps_vector);
//}

double iVector::operator&(iVector& const right) const
{
	//scalar multiply
	return x * right.GetX() + y * right.GetY(); 
}
//double iVector::operator^(iVector& const right) const
//{//angle
//	double len = sqrt( (x * x + y * y)) * right.length2();
//	if((len) > eps_vector)
//		return( (x * right.GetX() + y * right.GetY()) / (len) );
//	return 0.0;
//}
iVector iVector::operator>(iVector& const right) const
{
	return iVector(
		x * right.x,
		y * right.y);
}
iVector& iVector::operator=(const iVector& val)
{
	x = val.GetX();
	y = val.GetY();
	return *this;
}

bool iVector::operator<(int value) const
{	
	return value <= 0 || this->length2() < value*value;
}
bool iVector::operator==(iVector& const right) const
{
	return x == right.GetX() && y == right.GetY();
}
bool iVector::operator==(const iVector& right) const
{
	return x == right.GetX() && y == right.GetY();
}
bool iVector::operator!=(iVector& const right) const
{
	return x != right.GetX() || y != right.GetY();	
}
