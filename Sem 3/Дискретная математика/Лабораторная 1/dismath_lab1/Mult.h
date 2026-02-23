#pragma once
#include "Header.h"
class Mult {
private:
	const Mult* Parent_mult;
	vector<string> Codes;
	vector<int> Values;
	unsigned int Amount;
	unsigned int Power;
public:
	Mult(int, bool = true);
	Mult(const Mult*, bool = true, unsigned int = 0);
	Mult(const Mult*, vector<int>&);
	void Pr_mult(string = "") const;
	void Pr_elem(int) const;

	const vector<string>& Get_Codes() const;
	const vector<int>& Get_Values() const;
	unsigned int Get_Amount() const;
	const Mult* Get_Parent() const;
	unsigned int Get_Power() const;
	int operator[](int) const;
};