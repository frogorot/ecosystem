
#ifndef __VECTOR_H
#define __VECTOR_H

const double eps_vector = 0.000000001;

class Vector
{
private:
	double x;
	double y;
public:
	Vector();
	Vector(double,double);
	Vector(const Vector&);
	~Vector();
	void SetX(double x1){x = x1;}
	void SetY(double y1){y = y1;}
	double GetX() const {return x;}
	double GetY() const {return y;}

	double length() const;
	double length2() const;

	Vector operator+(Vector& const right) const;
	void operator+=(Vector& const right);
	Vector operator-(Vector& const right) const;
	Vector operator-() const;
	Vector operator*(double scale) const;
	Vector operator/(double u) const;
	Vector operator/(Vector& const right) const;
	
	double operator&(Vector& const right) const;
	double operator^(Vector& const right) const;
	Vector operator>(Vector& const right) const;
	Vector& operator=(const Vector& val);

	bool operator<(double value) const;
	bool operator==(Vector& const right) const;
	bool operator!=(Vector& const right) const;
};
Vector Vector_norm(Vector& value);
class iVector
{
private:
	int x;
	int y;
public:
	iVector();
	iVector(int,int);
	iVector(const iVector&);
	~iVector();
	void SetX(double x1){x = x1;}
	void SetY(double y1){y = y1;}
	double GetX() const {return x;}
	double GetY() const {return y;}

	//double length() const;
	double length2() const;

	iVector operator+(iVector& const right) const;
	void operator+=(iVector& const right);
	iVector operator-(iVector& const right) const;
	iVector operator-() const;
	iVector operator*(int scale) const;
	//iVector operator/(int u) const;
	//iVector operator/(iVector& const right) const;
	
	double operator&(iVector& const right) const;
	//double operator^(iVector& const right) const;
	iVector operator>(iVector& const right) const;
	iVector& operator=(const iVector& val);

	bool operator<(int value) const;
	bool operator==(iVector& const right) const;
	bool operator!=(iVector& const right) const;
};

#endif;