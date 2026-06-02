#pragma once

#include <string>
#include <vector>
#include <deque>
#include <iostream>
#include <functional>

#include <map>


#include <string>
#include <cwctype> // для towlower

inline wchar_t tl(wchar_t ch) {
	return std::towlower(ch);
}

inline int currentRow = 1;
inline int currentPos = 1;
inline int currentLexem = 1;


template<typename Data>
	requires std::ranges::range<Data>
class FSM {
private:
	int currentState;

	using ElementType = std::ranges::range_value_t<Data>;
	using Condition = std::function<bool(ElementType)>;
	using Action = std::function<void(ElementType)>;
	using ActionSequence = std::vector<Action>;

	ElementType element;
	std::deque<ElementType> dataProc;

	std::vector<ActionSequence> matrixActions;
	std::vector<Condition> matrixConditions;
	std::vector<int> matrixStates;

	std::vector<int> offsets;

	std::function<void(bool)> errorProcessor;

	bool doStep() {
		int condStart = offsets[currentState];
		int condEnd = offsets[currentState + 1];

		if (!dataProc.empty()) {
			element = dataProc.front();

			for (int i = condStart; i < condEnd; i++) {
				if (matrixConditions[i](element)) {
					// Выполняем все действия последовательно
					for (auto& action : matrixActions[i]) {
						if (action) {
							action(element);
						}
					}
					currentState = matrixStates[i];
					return true;
				}
			}
			errorProcessor(false);
			return true;

		}
		errorProcessor(true);
		return false;


	};

public:
	FSM() = default;

	// Вспомогательные типы для TransitionMap
	using Transition = std::pair<int, std::pair<Condition, ActionSequence>>;
	using TransitionMap = std::unordered_map<int, std::vector<Transition>>;

	// Конструктор на основе карты переходов
	FSM(
		int stateCount,
		TransitionMap transitions,
		std::function<void(bool)> errorProcessor
	) : errorProcessor(errorProcessor)
	{
		offsets.resize(stateCount + 1);

		for (int state = 0; state < stateCount; ++state) {
			offsets[state] = static_cast<int>(matrixActions.size());

			auto it = transitions.find(state);
			if (it != transitions.end()) {
				for (const auto& [to, condAct] : it->second) {
					auto [condition, actions] = condAct;
					if (condition) {
						matrixActions.push_back(actions);
						matrixConditions.push_back(condition);
						matrixStates.push_back(to);
					}
				}
			}
		}
		offsets[stateCount] = static_cast<int>(matrixActions.size());
	}

	void assignTransitions(int stateCount, TransitionMap transitions, std::function<void(bool)> errProc) {
		// 1. Сбрасываем текущее состояние
		this->errorProcessor = std::move(errProc);
		this->currentState = 0;

		// 2. Очищаем старые таблицы
		matrixActions.clear();
		matrixConditions.clear();
		matrixStates.clear();
		offsets.clear();
		dataProc.clear(); // Безопаснее очистить входные данные при смене логики

		// 3. Заполняем заново (логика идентична конструктору)
		offsets.resize(stateCount + 1);

		for (int state = 0; state < stateCount; ++state) {
			offsets[state] = static_cast<int>(matrixActions.size());

			auto it = transitions.find(state);
			if (it != transitions.end()) {
				for (const auto& [to, condAct] : it->second) {
					const auto& [condition, actions] = condAct;
					if (condition) {
						matrixActions.push_back(actions);
						matrixConditions.push_back(condition);
						matrixStates.push_back(to);
					}
				}
			}
		}
		offsets[stateCount] = static_cast<int>(matrixActions.size());
	}

	void start(Data data, int startState = 0) {
		if (startState < (int)offsets.size() - 1) {
			dataProc.clear();
			for (const ElementType& item : data) {
				dataProc.push_back(item);
			}
			currentState = startState;
		}

		while (doStep()) {}
		currentPos = 1;
		currentRow = 1;
		currentLexem = 1;
	};

	void popFront() {
		if (!dataProc.empty()) {
			dataProc.pop_front();
			currentPos++;
		}
	};

	ElementType front() {
		if (!dataProc.empty()) {
			return dataProc.front();
		}
	}

	void setState(int state) {
		currentState = state;
	}
};


enum LexemType
{
	Div,
	Id,
	Plus,
	Diff,
	Mult,
	LBracket,
	RBracket,
	LCurly,
	RCurly,
	LSquare,
	RSquare,
	Assign,
	Semicolon,
	Comma,
	Var,
	ShortAnd,
	FullAnd,
	ShortOr,
	FullOr,
	Equal,
	NotEqual,
	Not,
	LessEqual,
	Less,
	GreaterEqual,
	Greater,
	Begin,
	Break,
	Bool,
	Case,
	Continue,
	Colon,
	Default,
	Do,
	End,
	EndCase,
	Else,
	For,
	False,
	Find,
	Fi,
	Int,
	If,
	Len,
	Od,
	Replace,
	Reverse,
	Resize,
	Read,
	Switch,
	Shimb,
	Sorted,
	True,
	Then,
	ToVector,
	Trans,
	While,
	Write,
	Comment,
	MultiComment,
	Fill,
	Copy
};


enum Group
{
	gNone,
	gComment,
	gMultiComment,
	gId,
	gVar,
	gWord
};



class Error {
private:
	int row;
	int pos;
	std::wstring text;
	std::wstring reason;
public:
	Error(int row, int pos, std::wstring text, std::wstring reason) : row(row), pos(pos), text(text), reason(reason) {}
	void print() {
		std::wcout << L"Error at Row: " << row << L", Position: " << pos << L", Text: \"" << text << L"\", Reason: " << reason << std::endl;
	}
	int getRow() const {
		return row;
	}
	int getPos() const {
		return pos;
	}
	std::wstring getText() const {
		return text;
	}
	std::wstring getReason() const {
		return reason;
	}
};

class Lexem {
private:
	std::wstring str;
	LexemType type;
	int id;
	inline static int idsCounter = 0;
	inline static std::unordered_map<std::wstring, int> idMap;
public:
	Lexem(std::wstring str, LexemType type) : str(str), type(type) {
		if (type == Id) {
			if (idMap.find(str) != idMap.end()) {
				id = idMap[str];
			}
			else {
				id = idsCounter++;
				idMap[str] = id;
			}
		}
		else {
			id = 0;
		}
	}

	LexemType getType() const {
		return type;
	}

	std::wstring getStr() const {
		return str;
	}
};

inline Lexem lDiv = Lexem(L"/", Div);
inline Lexem lPlus = Lexem(L"+", Plus);
inline Lexem lDiff = Lexem(L"-", Diff);
inline Lexem lMult = Lexem(L"*", Mult);
inline Lexem lLBracket = Lexem(L"(", LBracket);
inline Lexem lRBracket = Lexem(L")", RBracket);
inline Lexem lLCurly = Lexem(L"{", LCurly);
inline Lexem lRCurly = Lexem(L"}", RCurly);
inline Lexem lLSquare = Lexem(L"[", LSquare);
inline Lexem lRSquare = Lexem(L"]", RSquare);
inline Lexem lAssign = Lexem(L":=", Assign);
inline Lexem lSemicolon = Lexem(L";", Semicolon);
inline Lexem lComma = Lexem(L",", Comma);
inline Lexem lShortAnd = Lexem(L"&&", ShortAnd);
inline Lexem lFullAnd = Lexem(L"&", FullAnd);
inline Lexem lShortOr = Lexem(L"||", ShortOr);
inline Lexem lFullOr = Lexem(L"|", FullOr);
inline Lexem lEqual = Lexem(L"=", Equal);
inline Lexem lNotEqual = Lexem(L"!=", NotEqual);
inline Lexem lNot = Lexem(L"!", Not);
inline Lexem lLessEqual = Lexem(L"<=", LessEqual);
inline Lexem lLess = Lexem(L"<", Less);
inline Lexem lGreaterEqual = Lexem(L">=", GreaterEqual);
inline Lexem lGreater = Lexem(L">", Greater);
inline Lexem lBegin = Lexem(L"begin", Begin);
inline Lexem lBreak = Lexem(L"break", Break);
inline Lexem lBool = Lexem(L"bool", Bool);
inline Lexem lCase = Lexem(L"case", Case);
inline Lexem lContinue = Lexem(L"continue", Continue);
inline Lexem lDo = Lexem(L"do", Do);
inline Lexem lEnd = Lexem(L"end", End);
inline Lexem lElse = Lexem(L"else", Else);
inline Lexem lFor = Lexem(L"for", For);
inline Lexem lFalse = Lexem(L"false", False);
inline Lexem lFind = Lexem(L"find", Find);
inline Lexem lFi = Lexem(L"fi", Fi);
inline Lexem lInt = Lexem(L"int", Int);
inline Lexem lIf = Lexem(L"if", If);
inline Lexem lLen = Lexem(L"len", Len);
inline Lexem lOd = Lexem(L"od", Od);
inline Lexem lReplace = Lexem(L"replace", Replace);
inline Lexem lReverse = Lexem(L"reverse", Reverse);
inline Lexem lResize = Lexem(L"resize", Resize);
inline Lexem lRead = Lexem(L"read", Read);
inline Lexem lSwitch = Lexem(L"switch", Switch);
inline Lexem lShimb = Lexem(L"shimb", Shimb);
inline Lexem lTrue = Lexem(L"true", True);
inline Lexem lThen = Lexem(L"then", Then);
inline Lexem lWhile = Lexem(L"while", While);
inline Lexem lWrite = Lexem(L"write", Write);
inline Lexem lColon = Lexem(L":", Colon);
inline Lexem lDefault = Lexem(L"default", Default);
inline Lexem lToVector = Lexem(L"toVector", ToVector);
inline Lexem lSorted = Lexem(L"sorted", Sorted);
inline Lexem lTrans = Lexem(L"trans", Trans);
inline Lexem lFill = Lexem(L"fill", Fill);
inline Lexem lCopy = Lexem(L"copy", Copy);
inline Lexem lEndCase = Lexem(L"endcase", EndCase);



inline bool is_ws(wchar_t c) {
	if (c == L'\n') {
		currentRow += 1;
		currentPos = 1;
	}
	return c == L' ' || c == L'\t' || c == L'\n';
};

inline bool is_digit(wchar_t c) {
	return c >= L'0' && c <= L'9';
};

inline bool is_letter(wchar_t c) {
	return (c >= L'a' && c <= L'z');
};

inline bool ret_true(wchar_t c) {
	return true;
};


inline FSM<std::wstring>* initTm(
	std::wstring& buffer,
	Group& currentGroup,
	std::vector<Lexem*>& lexemTable,
	std::vector<Error*>& errorTable,
	std::map<int, int>& lexemsR
) {
	FSM<std::wstring>::TransitionMap tM;
	FSM<std::wstring>* fsm = new FSM<std::wstring>();


	auto addToLexemsR = [&lexemsR]() {
		lexemsR.insert({ currentRow, currentLexem });
		currentLexem++;
		};


	auto y1 = [&fsm](wchar_t c) {fsm->popFront(); };
	auto y2 = [&buffer](wchar_t c) { buffer += c; };
	auto y3 = [&buffer](wchar_t c) { buffer.clear(); };
	auto y4 = [&buffer](wchar_t c) { currentRow++; currentPos = 1; };
	auto y_Div = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lDiv); addToLexemsR(); };
	auto y_Id = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(new Lexem(buffer, Id)); addToLexemsR(); };
	auto y_Plus = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lPlus); addToLexemsR(); };
	auto y_Diff = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lDiff); addToLexemsR(); };
	auto y_Mult = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lMult); addToLexemsR(); };
	auto y_LBracket = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lLBracket); addToLexemsR(); };
	auto y_RBracket = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lRBracket); addToLexemsR(); };
	auto y_LCurly = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lLCurly); addToLexemsR(); };
	auto y_RCurly = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lRCurly); addToLexemsR(); };
	auto y_LSquare = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lLSquare); addToLexemsR(); };
	auto y_RSquare = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lRSquare); addToLexemsR(); };
	auto y_Assign = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lAssign); addToLexemsR(); };
	auto y_Semicolon = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lSemicolon); addToLexemsR(); };
	auto y_Comma = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lComma); addToLexemsR(); };
	auto y_Var = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(new Lexem(buffer, Var)); addToLexemsR(); };
	auto y_ShortAnd = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lShortAnd); addToLexemsR(); };
	auto y_FullAnd = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lFullAnd); addToLexemsR(); };
	auto y_ShortOr = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lShortOr); addToLexemsR(); };
	auto y_FullOr = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lFullOr); addToLexemsR(); };
	auto y_Equal = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lEqual); addToLexemsR(); };
	auto y_NotEqual = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lNotEqual); addToLexemsR(); };
	auto y_Not = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lNot); addToLexemsR(); };
	auto y_LessEqual = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lLessEqual); addToLexemsR(); };
	auto y_Less = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lLess); addToLexemsR(); };
	auto y_GreaterEqual = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lGreaterEqual); addToLexemsR(); };
	auto y_Greater = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lGreater); addToLexemsR(); };
	auto y_Begin = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lBegin); addToLexemsR(); };
	auto y_Break = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lBreak); addToLexemsR(); };
	auto y_Bool = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lBool); addToLexemsR(); };
	auto y_Case = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lCase); addToLexemsR(); };
	auto y_Continue = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lContinue); addToLexemsR(); };
	auto y_Do = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lDo); addToLexemsR(); };
	auto y_End = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lEnd); addToLexemsR(); };
	auto y_Else = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lElse); addToLexemsR(); };
	auto y_For = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lFor); addToLexemsR(); };
	auto y_False = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lFalse); addToLexemsR(); };
	auto y_Find = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lFind); addToLexemsR(); };
	auto y_Fi = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lFi); addToLexemsR(); };
	auto y_Int = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lInt); addToLexemsR(); };
	auto y_If = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lIf); addToLexemsR(); };
	auto y_Len = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lLen); addToLexemsR(); };
	auto y_Od = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lOd); addToLexemsR(); };
	auto y_Replace = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lReplace); addToLexemsR(); };
	auto y_Reverse = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lReverse); addToLexemsR(); };
	auto y_Resize = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lResize); addToLexemsR(); };
	auto y_Read = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lRead); addToLexemsR(); };
	auto y_Switch = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lSwitch); addToLexemsR(); };
	auto y_Shimb = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lShimb); addToLexemsR(); };
	auto y_True = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lTrue); addToLexemsR(); };
	auto y_Then = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lThen); addToLexemsR(); };
	auto y_While = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lWhile); addToLexemsR(); };
	auto y_Write = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lWrite); addToLexemsR(); };
	auto y_Trans = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lTrans); addToLexemsR(); };

	auto y_Colon = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lColon); addToLexemsR(); };
	auto y_ToVector = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lToVector); addToLexemsR(); };
	auto y_Default = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lDefault); addToLexemsR(); };
	auto y_Sorted = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lSorted); addToLexemsR(); };
	auto y_Comment = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(new Lexem(buffer, Comment)); };
	auto y_MultiComment = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(new Lexem(buffer, MultiComment)); };

	auto y_Fill = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lFill); addToLexemsR(); };
	auto y_Copy = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lCopy); addToLexemsR(); };
	auto y_EndCase = [&lexemTable, &buffer, &addToLexemsR](wchar_t c) { lexemTable.push_back(&lEndCase); addToLexemsR(); };

	auto y_GroupNone = [&currentGroup](wchar_t c) { currentGroup = gNone; };
	auto y_GroupComment = [&currentGroup](wchar_t c) { currentGroup = gComment; };
	auto y_GroupMultiComment = [&currentGroup](wchar_t c) { currentGroup = gMultiComment; };
	auto y_GroupId = [&currentGroup](wchar_t c) { currentGroup = gId; };
	auto y_GroupVar = [&currentGroup](wchar_t c) { currentGroup = gVar; };
	auto y_GroupWord = [&currentGroup](wchar_t c) { currentGroup = gWord; };





	tM[0] = {
		{1, {is_ws, {y1}}},
		{2, {[](wchar_t c) {return c == L'/'; } , {y2, y1}}},
		{15, {[](wchar_t c) {return c == L'+'; } , {y_Plus, y1}}},
		{16, {[](wchar_t c) {return c == L'-'; } , {y_Diff, y1}}},
		{17, {[](wchar_t c) {return c == L'*'; } , {y_Mult, y1}}},
		{18, {[](wchar_t c) {return c == L'('; } , {y_LBracket, y1}}},
		{19, {[](wchar_t c) {return c == L')'; } , {y_RBracket, y1}}},
		{20, {[](wchar_t c) {return c == L'{'; } , {y_LCurly, y1}}},
		{21, {[](wchar_t c) {return c == L'}'; } , {y_RCurly, y1}}},
		{22, {[](wchar_t c) {return c == L'['; } , {y_LSquare, y1}}},
		{23, {[](wchar_t c) {return c == L']'; } , {y_RSquare, y1}}},
		{24, {[](wchar_t c) {return c == L':'; } , {y2, y1}}},
		{26, {[](wchar_t c) {return c == L';'; } , {y_Semicolon, y1}}},
		{27, {[](wchar_t c) {return c == L','; } , {y_Comma, y1}}},
		{28, {is_digit, {y_GroupVar, y2, y1}}},
		{36, {[](wchar_t c) {return c == L'&'; } , {y2, y1}}},
		{39, {[](wchar_t c) {return c == L'|'; } , {y2, y1}}},
		{42, {[](wchar_t c) {return c == L'='; } , {y_Equal, y1}}},
		{43, {[](wchar_t c) {return c == L'!'; } , {y2, y1}}},
		{45, {[](wchar_t c) {return c == L'<'; } , {y2, y1}}},
		{48, {[](wchar_t c) {return c == L'>'; } , {y2, y1}}},
		{51, {[](wchar_t c) {return tl(c) == L'b'; } , {y_GroupWord, y2, y1}}},
		{60, {[](wchar_t c) {return tl(c) == L'c'; } , {y_GroupWord, y2, y1}}},
		{71, {[](wchar_t c) {return tl(c) == L'd'; } , {y_GroupWord, y2, y1}}},
		{73, {[](wchar_t c) {return tl(c) == L'e'; } , {y_GroupWord, y2, y1}}},
		{79, {[](wchar_t c) {return tl(c) == L'f'; } , {y_GroupWord, y2, y1}}},
		{94, {[](wchar_t c) {return tl(c) == L'i'; } , {y_GroupWord, y2, y1}}},
		{98, {[](wchar_t c) {return tl(c) == L'l'; } , {y_GroupWord, y2, y1}}},
		{101, {[](wchar_t c) {return tl(c) == L'o'; } , {y_GroupWord, y2, y1}}},
		{103, {[](wchar_t c) {return tl(c) == L'r'; } , {y_GroupWord, y2, y1}}},
		{125, {[](wchar_t c) {return tl(c) == L's'; } , {y_GroupWord, y2, y1}}},
		{140, {[](wchar_t c) {return tl(c) == L't'; } , {y_GroupWord, y2, y1}}},
		{147, {[](wchar_t c) {return tl(c) == L'w'; } , {y_GroupWord, y2, y1}}},
		{12, {[](wchar_t c) {return is_letter(tl(c)); }, {y_GroupId, y2, y1}}},
	};

	tM[1] = {
		{0, {ret_true, {y_GroupNone}}}
	};
	tM[2] = {
		{3, {[](wchar_t c) {return c == L'/'; } , {y_GroupComment, y2, y1}}},
		{6, {[](wchar_t c) {return c != L'/' && c != L'*'; } , {y_Div, y3}}},
		{7, {[](wchar_t c) {return c == L'*'; } , {y_GroupMultiComment, y2, y1}}}
	};
	tM[3] = {
		{4, {[](wchar_t c) {return c != L'\n'; } , {y2, y1}}},
		{5, {[](wchar_t c) {return c == L'\n'; } , {y4, y_Comment, y3, y1}}}
	};
	tM[4] = {
		{4, {[](wchar_t c) {return c != L'\n'; } , {y2, y1}}},
		{5, {[](wchar_t c) {return c == L'\n'; } , {y4, y_Comment, y3, y1}}}
	};
	tM[5] = {
		{0, {ret_true, {y_GroupNone}}}
	};
	tM[6] = {
		{0, {ret_true, {y_GroupNone}}}
	};
	tM[7] = {
		{8, {[](wchar_t c) {return c == L'\n'; } , {y4, y2, y1}}},
		{8, {[](wchar_t c) {return c != L'*'; } , {y2, y1}}},
		{9, {[](wchar_t c) {return c == L'*'; } , {y2, y1}}}
	};
	tM[8] = {
		{8, {[](wchar_t c) {return c == L'\n'; } , {y4, y2, y1}}},
		{8, {[](wchar_t c) {return c != L'*'; } , {y2, y1}}},
		{9, {[](wchar_t c) {return c == L'*'; } , {y2, y1}}}
	};
	tM[9] = {
		{10, {[](wchar_t c) {return c == L'/'; } , {y2, y_MultiComment, y3, y1}}},
		{11, {[](wchar_t c) {return c == L'\n'; } , {y4, y2, y1}}},
		{11, {[](wchar_t c) {return is_ws(c) or c != L'/'; } , {y2, y1}}}
	};
	tM[10] = {
		{0, {ret_true, {y_GroupNone}}}
	};
	tM[11] = {
		{8, {[](wchar_t c) {return c == L'\n'; } , {y4, y2, y1}}},
		{8, {[](wchar_t c) {return c != L'*'; } , {y2, y1}}},
		{9, {[](wchar_t c) {return c == L'*'; } , {y2, y1}}}
	};
	tM[12] = {
		{13, {[](wchar_t c) {return not (is_digit(c) or is_letter(tl(c))); } , {y_Id, y3}}},
		{14, {[](wchar_t c) {return is_digit(c) or is_letter(tl(c)); } , {y2, y1}}}
	};
	tM[13] = {
		{0, {ret_true, {y_GroupNone}}}
	};
	tM[14] = {
		{13, {[](wchar_t c) {return not (is_digit(c) or is_letter(tl(c))); } , {y_Id, y3}}},
		{14, {[](wchar_t c) {return is_digit(c) or is_letter(tl(c)); } , {y2, y1}}}
	};
	tM[15] = {
		{0, {ret_true, {y_GroupNone}}}
	};
	tM[16] = {
		{0, {ret_true, {y_GroupNone}}}
	};
	tM[17] = {
		{0, {ret_true, {y_GroupNone}}}
	};
	tM[18] = {
		{0, {ret_true, {y_GroupNone}}}
	};
	tM[19] = {
		{0, {ret_true, {y_GroupNone}}}
	};
	tM[20] = {
		{0, {ret_true, {y_GroupNone}}}
	};
	tM[21] = {
		{0, {ret_true, {y_GroupNone}}}
	};
	tM[22] = {
		{0, {ret_true, {y_GroupNone}}}
	};
	tM[23] = {
		{0, {ret_true, {y_GroupNone}}}
	};
	tM[24] = {
		{25, {[](wchar_t c) {return c == L'='; }, {y_Assign, y3, y1}}},
		{157, {[](wchar_t c) {return c != L'='; }, {y_Colon, y3}}}
	};
	tM[25] = {
		{0, {ret_true, {y_GroupNone}}}
	};
	tM[26] = {
		{0, {ret_true, {y_GroupNone}}}
	};
	tM[27] = {
		{0, {ret_true, {y_GroupNone}}}
	};
	tM[28] = {
		{28, {is_digit, {y2, y1}}},
		{29, {[](wchar_t c) {return not is_digit(c) and not is_letter(tl(c)); }, {y_Var, y3}}}
	};
	tM[29] = {
		{0, {ret_true, {y_GroupNone}}}
	};
	tM[36] = {
		{37, {[](wchar_t c) {return c == L'&'; }, {y_ShortAnd, y3, y1}}},
		{38, {[](wchar_t c) {return c != L'&'; }, {y_FullAnd, y3}}}
	};
	tM[37] = {
		{0, {ret_true, {y_GroupNone}}}
	};
	tM[38] = {
		{0, {ret_true, {y_GroupNone}}}
	};
	tM[39] = {
		{40, {[](wchar_t c) {return c == L'|'; }, {y_ShortOr, y3, y1}}},
		{41, {[](wchar_t c) {return c != L'|'; }, {y_FullOr, y3}}}
	};
	tM[40] = {
		{0, {ret_true, {y_GroupNone}}}
	};
	tM[41] = {
		{0, {ret_true, {y_GroupNone}}}
	};
	tM[42] = {
		{0, {ret_true, {y_GroupNone}}}
	};
	tM[43] = {
		{44, {[](wchar_t c) {return c == L'='; }, {y_NotEqual, y3, y1}}},
		{156, {[](wchar_t c) {return c != L'='; }, {y_Not, y3}}}
	};
	tM[44] = {
		{0, {ret_true, {y_GroupNone}}}
	};
	tM[156] = {
		{0, {ret_true, {y_GroupNone}}}
	};
	tM[45] = {
		{46, {[](wchar_t c) {return c == L'='; }, {y_LessEqual, y3, y1}}},
		{47, {[](wchar_t c) {return c != L'='; }, {y_Less, y3}}}
	};
	tM[46] = {
		{0, {ret_true, {y_GroupNone}}}
	};
	tM[47] = {
		{0, {ret_true, {y_GroupNone}}}
	};
	tM[48] = {
		{49, {[](wchar_t c) {return c == L'='; }, {y_GreaterEqual, y3, y1}}},
		{50, {[](wchar_t c) {return c != L'='; }, {y_Greater, y3}}}
	};
	tM[49] = {
		{0, {ret_true, {y_GroupNone}}}
	};
	tM[50] = {
		{0, {ret_true, {y_GroupNone}}}
	};
	tM[51] = {
		{52, {[](wchar_t c) {return tl(c) == L'e'; }, {y2, y1}} },
		{56, {[](wchar_t c) {return tl(c) == L'r'; }, {y2, y1}} },
		{82, {[](wchar_t c) {return tl(c) == L'o'; }, {y2, y1}} }
	};
	tM[52] = {
		{53, {[](wchar_t c) {return tl(c) == L'g'; }, {y2, y1}} }
	};
	tM[53] = {
		{54, {[](wchar_t c) {return tl(c) == L'i'; }, {y2, y1}} }
	};
	tM[54] = {
		{55, {[](wchar_t c) {return tl(c) == L'n'; }, {y_Begin, y3, y1}} }
	};
	tM[55] = {
		{0, {ret_true, {}} }
	};
	tM[56] = {
		{57, {[](wchar_t c) {return tl(c) == L'e'; }, {y2, y1}} }
	};
	tM[57] = {
		{58, {[](wchar_t c) {return tl(c) == L'a'; }, {y2, y1}} }
	};
	tM[58] = {
		{59, {[](wchar_t c) {return tl(c) == L'k'; }, {y_Break, y3, y1}} }
	};
	tM[59] = {
		{0, {ret_true, {}} }
	};
	tM[60] = {
		{61, {[](wchar_t c) {return tl(c) == L'a'; }, {y2, y1}} },
		{64, {[](wchar_t c) {return tl(c) == L'o'; }, {y2, y1}} }
	};
	tM[61] = {
		{62, {[](wchar_t c) {return tl(c) == L's'; }, {y2, y1}} }
	};
	tM[62] = {
		{63, {[](wchar_t c) {return tl(c) == L'e'; }, {y_Case, y3, y1}} }
	};
	tM[63] = {
		{0, {ret_true, {}} }
	};
	tM[64] = {
		{65, {[](wchar_t c) {return tl(c) == L'n'; }, {y2, y1}} },
		{32, {[](wchar_t c) {return tl(c) == L'p'; }, {y2, y1}}}
	};
	tM[32] = {
		{33, {[](wchar_t c) {return tl(c) == L'y'; }, {y_Copy, y3, y1}}}
	};
	tM[33] = {
		{0, {ret_true, {}} }
	};
	tM[65] = {
		{66, {[](wchar_t c) {return tl(c) == L't'; }, {y2, y1}} }
	};
	tM[66] = {
		{67, {[](wchar_t c) {return tl(c) == L'i'; }, {y2, y1}} }
	};
	tM[67] = {
		{68, {[](wchar_t c) {return tl(c) == L'n'; }, {y2, y1}} }
	};
	tM[68] = {
		{69, {[](wchar_t c) {return tl(c) == L'u'; }, {y2, y1}} }
	};
	tM[69] = {
		{70, {[](wchar_t c) {return tl(c) == L'e'; }, {y_Continue, y3, y1}} }
	};
	tM[70] = {
		{0, {ret_true, {}} }
	};
	tM[71] = {
		{72, {[](wchar_t c) {return tl(c) == L'o'; }, {y_Do, y3, y1}} },
		{158, {[](wchar_t c) {return tl(c) == L'e'; }, {y2, y1}}}
	};
	tM[72] = {
		{0, {ret_true, {}} }
	};
	tM[73] = {
		{74, {[](wchar_t c) {return tl(c) == L'n'; }, {y2, y1}} },
		{76, {[](wchar_t c) {return tl(c) == L'l'; }, {y2, y1}} }
	};
	tM[74] = {
		{75, {[](wchar_t c) {return tl(c) == L'd'; }, {y1}} }
	};
	tM[75] = {
		{179, {[](wchar_t c) {return tl(c) == L'c'; }, {y2, y1}} },
		{0, {ret_true, {y_End, y3}} }
	};
	tM[179] = {
		{180, {[](wchar_t c) {return tl(c) == L'a'; }, {y2, y1}} },
	};
	tM[180] = {
		{181, {[](wchar_t c) {return tl(c) == L's'; }, {y2, y1}} },
	};
	tM[181] = {
		{182, {[](wchar_t c) {return tl(c) == L'e'; }, {y_EndCase, y3, y1}} },
	};
	tM[182] = {
		{0, {ret_true, {}} }
	};
	tM[76] = {
		{77, {[](wchar_t c) {return tl(c) == L's'; }, {y2, y1}} }
	};
	tM[77] = {
		{78, {[](wchar_t c) {return tl(c) == L'e'; }, {y_Else, y3, y1}} }
	};
	tM[78] = {
		{0, {ret_true, {}} }
	};
	tM[79] = {
		{80, {[](wchar_t c) {return tl(c) == L'o'; }, {y2, y1}} },
		{86, {[](wchar_t c) {return tl(c) == L'a'; }, {y2, y1}} },
		{90, {[](wchar_t c) {return tl(c) == L'i'; }, {y2, y1}} }
	};
	tM[80] = {
		{81, {[](wchar_t c) {return tl(c) == L'r'; }, {y_For, y3, y1}} }
	};
	tM[81] = {
		{0, {ret_true, {}} }
	};
	tM[86] = {
		{87, {[](wchar_t c) {return tl(c) == L'l'; }, {y2, y1}} }
	};
	tM[87] = {
		{88, {[](wchar_t c) {return tl(c) == L's'; }, {y2, y1}} }
	};
	tM[88] = {
		{89, {[](wchar_t c) {return tl(c) == L'e'; }, {y_False, y3, y1}} }
	};
	tM[89] = {
		{0, {ret_true, {}} }
	};
	tM[90] = {
		{91, {[](wchar_t c) {return tl(c) == L'n'; }, {y2, y1}} },
		{93, {[](wchar_t c) {return not(is_digit(c) or is_letter(tl(c))); }, {y_Fi, y3}} },
		{30, {[](wchar_t c) {return tl(c) == L'l'; }, {y2, y1}}}
	};
	tM[30] = {
		{31, {[](wchar_t c) {return tl(c) == L'l'; }, {y_Fill, y3, y1}}}
	};
	tM[31] = {
		{0, {ret_true, {}} }
	};
	tM[91] = {
		{92, {[](wchar_t c) {return tl(c) == L'd'; }, {y_Find, y3, y1}} }
	};
	tM[92] = {
		{0, {ret_true, {}} }
	};
	tM[93] = {
		{0, {ret_true, {}} }
	};
	tM[94] = {
		{95, {[](wchar_t c) {return tl(c) == L'n'; }, {y2, y1}} },
		{97, {[](wchar_t c) {return tl(c) == L'f'; }, {y_If, y3, y1}} }
	};
	tM[95] = {
		{96, {[](wchar_t c) {return tl(c) == L't'; }, {y_Int, y3, y1}} }
	};
	tM[96] = {
		{0, {ret_true, {}} }
	};
	tM[97] = {
		{0, {ret_true, {}} }
	};
	tM[98] = {
		{99, {[](wchar_t c) {return tl(c) == L'e'; }, {y2, y1}} },
	};
	tM[99] = {
		{100, {[](wchar_t c) {return tl(c) == L'n'; }, {y_Len, y3, y1}} }
	};
	tM[100] = {
		{0, {ret_true, {}} }
	};
	tM[101] = {
		{102, {[](wchar_t c) {return tl(c) == L'd'; }, {y_Od, y3, y1}} },
	};
	tM[102] = {
		{0, {ret_true, {}} }
	};
	tM[103] = {
		{104, {[](wchar_t c) {return tl(c) == L'e'; }, {y2, y1}} },
	};
	tM[104] = {
		{105, {[](wchar_t c) {return tl(c) == L'p'; }, {y2, y1}} },
		{110, {[](wchar_t c) {return tl(c) == L'v'; }, {y2, y1}} },
		{115, {[](wchar_t c) {return tl(c) == L's'; }, {y2, y1}} },
		{119, {[](wchar_t c) {return tl(c) == L'a'; }, {y2, y1}} }
	};
	tM[105] = {
		{106, {[](wchar_t c) {return tl(c) == L'l'; }, {y2, y1}} }
	};
	tM[106] = {
		{107, {[](wchar_t c) {return tl(c) == L'a'; }, {y2, y1}} }
	};
	tM[107] = {
		{108, {[](wchar_t c) {return tl(c) == L'c'; }, {y2, y1}} }
	};
	tM[108] = {
		{109, {[](wchar_t c) {return tl(c) == L'e'; }, {y_Replace, y3, y1}} }
	};
	tM[109] = {
		{0, {ret_true, {}} }
	};
	tM[110] = {
		{111, {[](wchar_t c) {return tl(c) == L'e'; }, {y2, y1}} }
	};
	tM[111] = {
		{112, {[](wchar_t c) {return tl(c) == L'r'; }, {y2, y1}} }
	};
	tM[112] = {
		{113, {[](wchar_t c) {return tl(c) == L's'; }, {y2, y1}} }
	};
	tM[113] = {
		{114, {[](wchar_t c) {return tl(c) == L'e'; }, {y_Reverse, y3, y1}} }
	};
	tM[114] = {
		{0, {ret_true, {}} }
	};
	tM[115] = {
		{116, {[](wchar_t c) {return tl(c) == L'i'; }, {y2, y1}} }
	};
	tM[116] = {
		{117, {[](wchar_t c) {return tl(c) == L'z'; }, {y2, y1}} }
	};
	tM[117] = {
		{118, {[](wchar_t c) {return tl(c) == L'e'; }, {y_Resize, y3, y1}} }
	};
	tM[118] = {
		{0, {ret_true, {}} }
	};
	tM[119] = {
		{120, {[](wchar_t c) {return tl(c) == L'd'; }, {y_Read, y3, y1}} }
	};
	tM[120] = {
		{0, {ret_true, {}} }
	};
	tM[125] = {
		{131, {[](wchar_t c) {return tl(c) == L'w'; }, {y2, y1}} },
		{136, {[](wchar_t c) {return tl(c) == L'h'; }, {y2, y1}} },
		{164, {[](wchar_t c) {return tl(c) == L'o'; }, {y2, y1}} }
	};
	tM[131] = {
		{132, {[](wchar_t c) {return tl(c) == L'i'; }, {y2, y1}} }
	};
	tM[132] = {
		{133, {[](wchar_t c) {return tl(c) == L't'; }, {y2, y1}} }
	};
	tM[133] = {
		{134, {[](wchar_t c) {return tl(c) == L'c'; }, {y2, y1}} }
	};
	tM[134] = {
		{135, {[](wchar_t c) {return tl(c) == L'h'; }, {y_Switch, y3, y1}} }
	};
	tM[135] = {
		{0, {ret_true, {}} }
	};
	tM[136] = {
		{137, {[](wchar_t c) {return tl(c) == L'i'; }, {y2, y1}} }
	};
	tM[137] = {
		{138, {[](wchar_t c) {return tl(c) == L'm'; }, {y2, y1}} }
	};
	tM[138] = {
		{139, {[](wchar_t c) {return tl(c) == L'b'; }, {y_Shimb, y3, y1}} }
	};
	true;
	tM[139] = {
		{0, {ret_true, {}} }
	};
	tM[140] = {
		{141, {[](wchar_t c) {return tl(c) == L'r'; }, {y2, y1}} },
		{144, {[](wchar_t c) {return tl(c) == L'h'; }, {y2, y1}} },
		{169, {[](wchar_t c) {return tl(c) == L'o'; }, {y2, y1}} }
	};
	tM[141] = {
		{142, {[](wchar_t c) {return tl(c) == L'u'; }, {y2, y1}} },
		{176, {[](wchar_t c) {return tl(c) == L'a'; }, {y2, y1}} }
	};
	tM[142] = {
		{143, {[](wchar_t c) {return tl(c) == L'e'; }, {y_True, y3, y1}} }
	};
	tM[143] = {
		{0, {ret_true, {}} }
	};
	tM[144] = {
		{145, {[](wchar_t c) {return tl(c) == L'e'; }, {y2, y1}} }
	};
	tM[145] = {
		{146, {[](wchar_t c) {return tl(c) == L'n'; }, {y_Then, y3, y1}} }
	};
	tM[146] = {
		{0, {ret_true, {}} }
	};
	tM[147] = {
		{148, {[](wchar_t c) {return tl(c) == L'h'; }, {y2, y1}} },
		{152, {[](wchar_t c) {return tl(c) == L'r'; }, {y2, y1}} }
	};
	tM[148] = {
		{149, {[](wchar_t c) {return tl(c) == L'i'; }, {y2, y1}} }
	};
	tM[149] = {
		{150, {[](wchar_t c) {return tl(c) == L'l'; }, {y2, y1}} }
	};
	tM[150] = {
		{151, {[](wchar_t c) {return tl(c) == L'e'; }, {y_While, y3, y1}} }
	};
	tM[151] = {
		{0, {ret_true, {}} }
	};
	tM[152] = {
		{153, {[](wchar_t c) {return tl(c) == L'i'; }, {y2, y1}} }
	};
	tM[153] = {
		{154, {[](wchar_t c) {return tl(c) == L't'; }, {y2, y1}} }
	};
	tM[154] = {
		{155, {[](wchar_t c) {return tl(c) == L'e'; }, {y_Write, y3, y1}} }
	};
	tM[155] = {
		{0, {ret_true, {}} }
	};
	tM[156] = {
		{0, {ret_true, {y_GroupNone}}}
	};
	tM[157] = {
		{0, {ret_true, {y_GroupNone}}}
	};
	tM[158] = {
		{159, {[](wchar_t c) {return tl(c) == L'f'; }, {y2, y1}}}
	};
	tM[159] = {
		{160, {[](wchar_t c) {return tl(c) == L'a'; }, {y2, y1}}}
	};
	tM[160] = {
		{161, {[](wchar_t c) {return tl(c) == L'u'; }, {y2, y1}}}
	};
	tM[161] = {
		{162, {[](wchar_t c) {return tl(c) == L'l'; }, {y2, y1}}}
	};
	tM[162] = {
		{163, {[](wchar_t c) {return tl(c) == L't'; }, {y_Default, y3, y1}}}
	};
	tM[163] = {
		{0, {ret_true, {y_GroupNone}}}
	};
	tM[164] = {
		{165, {[](wchar_t c) {return tl(c) == L'r'; }, {y2, y1}}}
	};
	tM[165] = {
		{166, {[](wchar_t c) {return tl(c) == L't'; }, {y2, y1}}}
	};
	tM[166] = {
		{167, {[](wchar_t c) {return tl(c) == L'e'; }, {y2, y1}}}
	};
	tM[167] = {
		{168, {[](wchar_t c) {return tl(c) == L'd'; }, {y_Sorted, y3, y1}}}
	};
	tM[168] = {
		{0, {ret_true, {y_GroupNone}}}
	};
	tM[169] = {
		{170, {[](wchar_t c) {return tl(c) == L'v'; }, {y2, y1}}}
	};
	tM[170] = {
		{171, {[](wchar_t c) {return tl(c) == L'e'; }, {y2, y1}}}
	};
	tM[171] = {
		{172, {[](wchar_t c) {return tl(c) == L'c'; }, {y2, y1}}}
	};
	tM[172] = {
		{173, {[](wchar_t c) {return tl(c) == L't'; }, {y2, y1}}}
	};
	tM[173] = {
		{174, {[](wchar_t c) {return tl(c) == L'o'; }, {y2, y1}}}
	};
	tM[174] = {
		{175, {[](wchar_t c) {return tl(c) == L'r'; }, {y_ToVector, y3, y1}}}
	};
	tM[175] = {
		{0, {ret_true, {y_GroupNone}}}
	};
	tM[176] = {
		{177, {[](wchar_t c) {return tl(c) == L'n'; }, {y2, y1}}}
	};
	tM[177] = {
		{178, {[](wchar_t c) {return tl(c) == L'n'; }, {y_Trans, y3, y1}}}
	};
	tM[178] = {
		{0, {ret_true, {}} }
	};
	tM[82] = {
		{83, {[](wchar_t c) {return tl(c) == L'o'; }, {y2, y1}} }
	};
	tM[83] = {
		{84, {[](wchar_t c) {return tl(c) == L'l'; }, {y_Bool, y3, y1}} }
	};
	tM[84] = {
		{0, {ret_true, {}} }
	};

	auto errorProcessor = [&buffer, &currentGroup, &fsm, &errorTable, &lexemTable, &y_Comment, &addToLexemsR](bool isEnd) {
		int pos_delta = 0;
		int buffer_len = buffer.size();
		if (isEnd) {
			if (buffer_len != 0) {
				switch (currentGroup) {
				case gComment:
					lexemTable.push_back(new Lexem(buffer, Comment));
					buffer.clear();
					break;
				case gMultiComment:
					errorTable.push_back(new Error(currentRow, currentPos, buffer, L"Multirow comment is not finished"));
					buffer.clear();
					break;
				case gVar:
					errorTable.push_back(new Error(currentRow, currentPos, buffer, L"Value of Var is not finished"));
					buffer.clear();
					break;
				}
			}
			else {
				switch (currentGroup) {
				case gComment:
					y_Comment(L' ');
					buffer.clear();
					break;
				case gMultiComment:
					errorTable.push_back(new Error(currentRow, currentPos, buffer, L"Multirow comment is not finished"));
					buffer.clear();
					break;
				}
			}
		}
		else {
			switch (currentGroup) {
			case gNone:
				errorTable.push_back(new Error(currentRow, currentPos - 1, buffer, L"Unexpected symbol"));
				fsm->popFront();

				buffer.clear();
				fsm->setState(0);
				break;
			case gComment:
				errorTable.push_back(new Error(currentRow, currentPos, L"", L"Comment is not finished"));
				fsm->setState(0);
				break;
			case gId:
				errorTable.push_back(new Error(currentRow, currentPos, buffer, L"Unexpected symbol in identificator"));
				buffer.clear();
				fsm->setState(0);
				break;
			case gVar:
				errorTable.push_back(new Error(currentRow, currentPos, buffer, L"Unexpected symbol in variable"));
				while (is_digit(fsm->front()) or is_letter(fsm->front())) {
					buffer.push_back(fsm->front());
					fsm->popFront();
					pos_delta++;
				}
				buffer.clear();


				fsm->setState(0);
				break;
			case gWord:
				currentGroup = gId;
				fsm->setState(12);
				break;
			};
		}
		};

	fsm->assignTransitions(200, tM, errorProcessor);
	return fsm;
}


struct ScanResult {
	std::vector<Lexem*> lexems;
	std::vector<Error*> errors;
	std::map<int, int> lexemsRows;

	void clear() {
		std::vector<Lexem*> lexems_to_del;
		for (auto lexem : lexems) {
			if (lexem->getType() == Id or lexem->getType() == Var) {
				delete lexem;
			}
		}
		for (auto error : errors) {
			delete error;
		}
		lexems.clear();
		errors.clear();
		lexemsRows.clear();

	}
};


ScanResult tokenize(FSM<std::wstring>* fsm, const std::string& filePath);
ScanResult tokenize_str(FSM<std::wstring>* fsm, std::wstring data);