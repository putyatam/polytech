#pragma once
#include <functional>
#include <deque>
#include <vector>
#include <array>
#include <string>

using namespace std;

class Effect {
private:
	function<void()> func;
public:
	Effect(function<void()>);
	void operator()();
};