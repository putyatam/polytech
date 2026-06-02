#pragma once
#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <stack>
#include <string>
#include <map>
#include <iostream>
#include <stdexcept>
#include "codegen.h"
#include "scanner.h"

enum DataType {
    T_INT,
    T_BOOL,
    T_UNKNOWN
};

struct VarInfo {
    int      addr;
    DataType type;
    bool     isArray;
    bool     isMatrix;
    int      arraySize;
    int      arraySize2;

    VarInfo()
        : addr(0), type(T_UNKNOWN),
        isArray(false), isMatrix(false),
        arraySize(0), arraySize2(0) {}

    VarInfo(int a, DataType t,
        bool arr = false, bool mat = false,
        int sz1 = 0, int sz2 = 0)
        : addr(a), type(t),
        isArray(arr), isMatrix(mat),
        arraySize(sz1), arraySize2(sz2) {}
};

class Parser {
public:
    Parser(const std::vector<Lexem*>& lexems, CodeGen* cg)
        : lexems_(lexems), codegen_(cg), pos_(0),
        error_(false), nextAddr_(0) {}

    std::vector<std::pair<std::string, int>>* parse();

private:
    bool lastStmtWasCompound_ = false;
    struct LoopInfo {
        int              continueAddr;
        std::vector<int> breakJumps;
    };
    std::stack<LoopInfo> loops_;

    void program();
    void statementList();
    void statement();

    void declarationList();
    void declaration();
    DataType parseType();
    void identList(DataType type);
    void identInit(DataType type);

    void assignStatement(const std::wstring& name);
    void ifStatement();
    void whileStatement();
    void forStatement();
    void switchStatement();
    void writeStatement();
    void readStatement();

    void expression();
    void boolExpression();
    void boolTerm();
    void boolFactor();
    void boolPrimary();
    void arithmeticExpression();
    void term();
    void factor();
    void primary();

    void callFind();
    void callLen();
    void callReverse();
    void callResize();
    void callReplace();
    void callSorted();
    void callToVector();
    void callTrans();
    void callShimb();
    void callFill();
    void callCopy();
    void callSlice(const std::wstring& arrName, int fromVal, int toVal);
    bool tryArrayArithAssign(const VarInfo& dst);

    VarInfo callToVectorImpl();
    VarInfo callCopyImpl();
    VarInfo callReverseImpl();
    VarInfo callSortedImpl();
    VarInfo callTransImpl();
    VarInfo callShimbImpl();
    VarInfo callResizeImpl();
    VarInfo callReplaceImpl();
    VarInfo callFillImpl();

    VarInfo evalArrayExpr(DataType type = T_INT);

    VarInfo copyToTemp(const VarInfo& src);
    VarInfo matRowToTemp(const VarInfo& mat, int rowIdx);
    VarInfo sliceToTemp(const VarInfo& src, int from, int to);
    void    emitPrintArray(const VarInfo& vi);

    bool        isBlockTerminator() const;
    std::string closingTokenName(LexemType t) const;

    bool          see(LexemType t)     const;
    bool          seeNext(LexemType t) const;
    bool          match(LexemType t);
    void          mustBe(LexemType t, const std::string& errMsg = "");
    const Lexem& current()            const;
    void          next();
    void          reportError(const std::string& msg);

    std::map<std::wstring, VarInfo> symbolTable_;
    int nextAddr_ = 0;

    int      declareVar(const std::wstring& name, DataType type);
    int      declareArray(const std::wstring& name, DataType type, int size);
    int      declareMatrix(const std::wstring& name, DataType type, int rows, int cols);
    VarInfo& getVar(const std::wstring& name);
    bool     varExists(const std::wstring& name) const;

    std::wstring anonName(const std::wstring& prefix) {
        static int c = 0;
        return L"__" + prefix + L"_" + std::to_wstring(c++);
    }

    std::vector<Lexem*> lexems_;
    size_t              pos_;
    CodeGen* codegen_;
    bool                error_;

    std::vector<std::pair<std::string, int>> errors_;
};

#endif