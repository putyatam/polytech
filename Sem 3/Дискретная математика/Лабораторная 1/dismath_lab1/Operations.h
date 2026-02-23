#pragma once
#include "Header.h"
#include "Mult.h"

Mult Union(const Mult&, const Mult&);
Mult Intersect(const Mult&, const Mult&);
Mult Difference(const Mult&, const Mult&);
Mult Simmetric_Difference(const Mult&, const Mult&);
Mult Addition(const Mult&);
Mult Arithmetic_Sum(const Mult&, const Mult&);
Mult Arithmetic_Difference(const Mult&, const Mult&);
Mult Arithmetic_Product(const Mult&, const Mult&);
Mult Arithmetic_Division(const Mult&, const Mult&);
void All_operations(const Mult&, const Mult&);