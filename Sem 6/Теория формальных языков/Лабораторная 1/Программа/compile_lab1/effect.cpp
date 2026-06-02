#include "effect.h"


Effect::Effect(function<void()> func) {
	this->func = func;
}

void Effect::operator()() {
	this->func();
}

	