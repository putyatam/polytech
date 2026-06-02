#include "parser.h"
#include <sstream>

std::vector<std::pair<std::string,int>>* Parser::parse()
{
    program();
    if(!error_) {
        codegen_->flush();
    }
    return &errors_;
}

void Parser::program()
{
    mustBe(Begin, "Программа должна начинаться с 'begin'");
    declarationList();     statementList();
    mustBe(End, "Программа должна заканчиваться на 'end'");
    codegen_->emit(STOP);
}

void Parser::declarationList()
{
    while(see(Int) || see(Bool)) {
        declaration();
        mustBe(Semicolon, "Ожидалась ';' после объявления");
    }
}

void Parser::declaration()
{
    DataType dt = parseType();
    identList(dt);
}

DataType Parser::parseType()
{
    if(match(Int))  return T_INT;
    if(match(Bool)) return T_BOOL;
    reportError("Ожидалось ключевое слово типа ('int' или 'bool')");
    next();
    return T_INT;
}

void Parser::identList(DataType type)
{
    identInit(type);
    while(match(Comma))
        identInit(type);
}

void Parser::identInit(DataType type)
{
    if(!see(Id)) {
        reportError("Ожидался идентификатор в объявлении");
        return;
    }
    std::wstring name = current().getStr();
    next();

    if(match(LSquare)) {
                if(!see(Var)) { reportError("Ожидался целочисленный размер массива"); return; }
        int sz1 = std::stoi(current().getStr());
        if (sz1 <= 0) reportError("Размер массива должен быть положительным");
        next();
        mustBe(RSquare, "Ожидалась ']' после размера массива");

        if (match(LSquare)) {
                        if (!see(Var)) { reportError("Ожидался целочисленный размер второго измерения"); return; }
            int sz2 = std::stoi(current().getStr());
            if (sz2 <= 0) reportError("Размер второго измерения должен быть положительным");
            next();
            mustBe(RSquare, "Ожидалась ']' после размера второго измерения");
            int base = declareMatrix(name, type, sz1, sz2);
            if (see(Equal) || see(Assign)) {
                next();
                VarInfo src = evalArrayExpr(type);
                int srcTotal = src.isMatrix ? src.arraySize * src.arraySize2 : src.arraySize;
                int dstTotal = sz1 * sz2;
                if (srcTotal != dstTotal)
                    reportError("Несоответствие размеров инициализатора матрицы: ожидалось "
                        + std::to_string(dstTotal) + " элементов, получено " + std::to_string(srcTotal));
                for (int i = 0; i < dstTotal && i < srcTotal; ++i) {
                    codegen_->emit(LOAD, src.addr + i);
                    codegen_->emit(STORE, base + i);
                }
            }
        }
        else {
                        int base = declareArray(name, type, sz1);
            if (see(Equal) || see(Assign)) {
                next();
                VarInfo src = evalArrayExpr(type);
                int srcSize = src.isMatrix ? src.arraySize * src.arraySize2 : src.arraySize;
                if (srcSize != sz1)
                    reportError("Несоответствие размеров инициализатора массива: ожидалось "
                        + std::to_string(sz1) + " элементов, получено " + std::to_string(srcSize));
                for (int i = 0; i < sz1 && i < srcSize; ++i) {
                    codegen_->emit(LOAD, src.addr + i);
                    codegen_->emit(STORE, base + i);
                }
            }
        }
    }
    else {
                int addr = declareVar(name, type);
        if (match(Assign)) {
            expression();
            codegen_->emit(STORE, addr);
        }
    }
}

void Parser::statementList()
{
    if (isBlockTerminator()) {
        return;
    }

    while (true) {
                if (see(Int) || see(Bool)) {
            declaration();
            mustBe(Semicolon, "Ожидалась ';' после объявления");
            if (isBlockTerminator()) break;
            continue;
        }

                bool isCompoundStatement = false;

                if (see(If) || see(While) || see(For) || see(Switch)) {
            isCompoundStatement = true;
        }

        statement();

                        if (isCompoundStatement) {
                        if (see(Semicolon)) {
                reportError("Точка с запятой не допускается после составного оператора");
                next();             }
        }

        if (isBlockTerminator()) {
            break;
        }
        else {
                        if (!isCompoundStatement) {
                mustBe(Semicolon, "Ожидалась ';' после оператора");
            }
        }
    }
}

bool Parser::isBlockTerminator() const
{
    return see(End) || see(Od) || see(Fi) || see(EndCase)
        || see(Else) || see(Default) || see(Case) || see(Break);
}

std::string Parser::closingTokenName(LexemType t) const
{
    switch(t) {
    case End:     return "end";
    case Od:      return "od";
    case Fi:      return "fi";
    case EndCase: return "endcase";
    case Else:    return "else";
    case Default: return "default";
    case Case:    return "case";
    default:      return "токен("+std::to_string((int)t)+")";
    }
}

void Parser::statement()
{
                    if(see(Id)) {
        std::wstring name = current().getStr();
        next();
        assignStatement(name);
    }

                else if(match(If)) {
        ifStatement();
    }

        else if(match(While)) {
        whileStatement();
    }

        else if(match(For)) {
        forStatement();
    }

        else if(match(Switch)) {
        switchStatement();
    }

        else if(match(Break)) {
        if(!loops_.empty()) {
            int a = codegen_->reserve();
            loops_.top().breakJumps.push_back(a);
        }
        else {
            reportError("'break' использован вне цикла или switch");
        }
    }

        else if(match(Continue)) {
        if(!loops_.empty()) {
            std::stack<LoopInfo> tmp = loops_;
            int ca = -1;
            while(!tmp.empty()) {
                if(tmp.top().continueAddr != -1) {
                    ca = tmp.top().continueAddr;
                    break;
                }
                tmp.pop();
            }
            if(ca == -1) reportError("'continue' использован вне цикла");
            else         codegen_->emit(JUMP, ca);
        }
        else {
            reportError("'continue' использован вне цикла");
        }
    }

            else if(match(Write)) {
        writeStatement();
    }

        else if(match(Read)) {
        readStatement();
    }

            else if(see(Find)    || see(Len)    || see(Reverse) || see(Resize) ||
            see(Replace) || see(Sorted) || see(ToVector)|| see(Trans)  ||
            see(Shimb)   || see(Fill)   || see(Copy))
    {
        primary();
        codegen_->emit(POP);
    }

    else {
        reportError("Неожиданный токен в операторе");
        next();
    }
}

void Parser::assignStatement(const std::wstring& name)
{
    if(!varExists(name)) {
        reportError("Присваивание необъявленной переменной");
        while(!see(Semicolon) && !isBlockTerminator()
              && pos_ < lexems_.size())
            next();
        return;
    }

    VarInfo& vi = getVar(name);

    if(match(LSquare)) {
                if(vi.isMatrix && see(Var) && seeNext(RSquare)) {
                        int rowIdx = std::stoi(current().getStr()); next();
            mustBe(RSquare, "Ожидалась ']'");
            int cols = vi.arraySize2;
            if(rowIdx < 0 || rowIdx >= vi.arraySize)
                reportError("Индекс строки выходит за границы матрицы");
            if(match(LSquare)) {
                                expression();
                mustBe(RSquare, "Ожидалась ']'");
                mustBe(Assign,  "Ожидалось ':='");
                expression();
                codegen_->emit(BSTORE, vi.addr + rowIdx * cols);
            }
            else {
                mustBe(Assign, "Ожидалось ':='");
                VarInfo src = evalArrayExpr(vi.type);
                int total = src.isMatrix
                    ? src.arraySize * src.arraySize2
                    : src.arraySize;
                for(int c = 0; c < cols && c < total; ++c) {
                    codegen_->emit(LOAD,  src.addr + c);
                    codegen_->emit(STORE, vi.addr + rowIdx * cols + c);
                }
            }
        }
        else {
                        expression();
            mustBe(RSquare, "Ожидалась ']'");
            if(match(LSquare)) {
                int cols = vi.arraySize2;
                codegen_->emit(PUSH, cols);
                codegen_->emit(MULT);
                expression();
                mustBe(RSquare, "Ожидалась ']'");
                codegen_->emit(ADD);
                mustBe(Assign, "Ожидалось ':='");
                expression();
                codegen_->emit(BSTORE, vi.addr);
            }
            else {
                mustBe(Assign, "Ожидалось ':='");
                expression();
                codegen_->emit(BSTORE, vi.addr);
            }
        }
    }
    else if((vi.isArray || vi.isMatrix) && see(Assign)) {
                next();                 if(pos_ + 1 < lexems_.size()) {
            LexemType nxt = lexems_[pos_ + 1]->getType();
            if(see(Id) && varExists(current().getStr())
               && (nxt == Plus || nxt == Diff || nxt == Mult))
            {
                if(tryArrayArithAssign(vi)) return;
            }
        }
                VarInfo src = evalArrayExpr(vi.type);
        int srcTotal = src.isMatrix
            ? src.arraySize * src.arraySize2
            : src.arraySize;
        int dstTotal = vi.isMatrix
            ? vi.arraySize * vi.arraySize2
            : vi.arraySize;
        for(int i = 0; i < dstTotal && i < srcTotal; ++i) {
            codegen_->emit(LOAD,  src.addr + i);
            codegen_->emit(STORE, vi.addr  + i);
        }
    }
    else {
                mustBe(Assign, "Ожидалось ':=' в операторе присваивания");
        expression();
        codegen_->emit(STORE, vi.addr);
    }
}

void Parser::ifStatement()
{
                expression();

    int jumpNoAddress = codegen_->reserve();

    mustBe(Then, "Ожидалось 'then' после условия if");
    statementList();

    if(match(Else)) {
                int jumpAddress = codegen_->reserve();
        codegen_->emitAt(jumpNoAddress, JUMP_NO,
                         codegen_->getCurrentAddress());
        statementList();
        codegen_->emitAt(jumpAddress, JUMP,
                         codegen_->getCurrentAddress());
    }
    else {
        codegen_->emitAt(jumpNoAddress, JUMP_NO,
                         codegen_->getCurrentAddress());
    }

    mustBe(Fi, "Ожидалось 'fi' для закрытия if");
}

void Parser::whileStatement()
{
        int conditionAddress = codegen_->getCurrentAddress();
    expression();     int jumpNoAddress = codegen_->reserve();

    mustBe(Do, "Ожидалось 'do' после условия while");

        LoopInfo li;
    li.continueAddr = conditionAddress;
    loops_.push(li);

    statementList();
    mustBe(Od, "Ожидалось 'od' для закрытия while");

        codegen_->emit(JUMP, conditionAddress);
    int afterLoop = codegen_->getCurrentAddress();
    codegen_->emitAt(jumpNoAddress, JUMP_NO, afterLoop);

        for(int a : loops_.top().breakJumps)
        codegen_->emitAt(a, JUMP, afterLoop);
    loops_.pop();
}

void Parser::forStatement()
{
    mustBe(LBracket, "Ожидалась '(' после 'for'");

        if(!see(Semicolon)) {
        DataType ft = T_INT;
        bool hasTypeDecl = false;
        if(see(Int) || see(Bool)) {
            ft = parseType();
            hasTypeDecl = true;
        }
        if(!see(Id)) {
            reportError("Ожидался идентификатор в инициализаторе for");
        }
        else {
            std::wstring n = current().getStr(); next();
            if(hasTypeDecl) declareVar(n, ft);
            else if(!varExists(n))
                reportError("Необъявленная переменная '"
                    +std::string(n.begin(),n.end())
                    +"' в инициализаторе for");
            mustBe(Assign, "Ожидалось ':=' в инициализаторе for");
            expression();
            codegen_->emit(STORE, getVar(n).addr);
        }
    }
    mustBe(Semicolon, "Ожидалась ';' после инициализатора for");

        int condAddr = codegen_->getCurrentAddress();
    expression();
    int jmpOut  = codegen_->reserve();     int jmpBody = codegen_->reserve();     mustBe(Semicolon, "Ожидалась ';' после условия for");

        int stepAddr = codegen_->getCurrentAddress();
    if(!see(RBracket)) {
        if(!see(Id)) {
            reportError("Ожидался идентификатор в шаге for");
        }
        else {
            std::wstring n = current().getStr(); next();
            if(!varExists(n))
                reportError("Необъявленная переменная '"
                    +std::string(n.begin(),n.end())+"' в шаге for");
            mustBe(Assign, "Ожидалось ':=' в шаге for");
            expression();
            codegen_->emit(STORE, getVar(n).addr);
        }
    }
    codegen_->emit(JUMP, condAddr);
    mustBe(RBracket, "Ожидалась ')' после шага for");
    mustBe(Do,       "Ожидалось 'do' после for(...)");

        int bodyAddr = codegen_->getCurrentAddress();
    codegen_->emitAt(jmpOut,  JUMP_NO, 0);
    codegen_->emitAt(jmpBody, JUMP,    bodyAddr);

    LoopInfo li;
    li.continueAddr = stepAddr;
    loops_.push(li);

    statementList();
    mustBe(Od, "Ожидалось 'od' для закрытия for");

    codegen_->emit(JUMP, stepAddr);
    int afterLoop = codegen_->getCurrentAddress();
    codegen_->emitAt(jmpOut, JUMP_NO, afterLoop);

    for(int a : loops_.top().breakJumps)
        codegen_->emitAt(a, JUMP, afterLoop);
    loops_.pop();
}

void Parser::switchStatement()
{
    mustBe(LBracket, "Ожидалась '(' после 'switch'");
    expression();     mustBe(RBracket, "Ожидалась ')' после выражения switch");
    mustBe(Begin, "Ожидалось 'begin' после switch(...)");

    LoopInfo swInfo;
    swInfo.continueAddr = -1;     loops_.push(swInfo);

    std::vector<int> breakToEnd; 
                                                        while(see(Case)) {
        next();         codegen_->emit(DUP);
        expression();                               codegen_->emit(COMPARE, 0);                int jmpNext = codegen_->reserve();         mustBe(Colon, "Ожидалось ':' после значения case");

        statementList();

                mustBe(Break,     "Ожидалось 'break' в конце case");
        mustBe(Semicolon, "Ожидалась ';' после 'break'");
        breakToEnd.push_back(codegen_->reserve()); 
                codegen_->emitAt(jmpNext, JUMP_NO, codegen_->getCurrentAddress());
    }

    if(match(Default)) {
        mustBe(Colon, "Ожидалось ':' после 'default'");
        statementList();
    }

    mustBe(EndCase, "Ожидалось 'endcase' для закрытия switch");
    codegen_->emit(POP); 
    int afterSwitch = codegen_->getCurrentAddress();
    for(int a : breakToEnd)
        codegen_->emitAt(a, JUMP, afterSwitch);
    for(int a : loops_.top().breakJumps)
        codegen_->emitAt(a, JUMP, afterSwitch);
    loops_.pop();
}


void Parser::writeStatement()
{
    mustBe(LBracket, "Ожидалась '(' после 'write'");

        bool isArrExpr = false;
    if(see(LCurly))
        isArrExpr = true;
    else if(see(ToVector) || see(Copy)  || see(Shimb) || see(Trans) ||
            see(Reverse)  || see(Sorted)|| see(Resize)|| see(Replace))
        isArrExpr = true;
    else if(see(Id) && varExists(current().getStr())) {
        VarInfo& wvi = getVar(current().getStr());
        if(wvi.isArray || wvi.isMatrix)
            isArrExpr = true;
    }

    if(isArrExpr) {
        if(see(Id) && varExists(current().getStr())) {
            std::wstring wname = current().getStr();
            VarInfo wvi = getVar(wname);
            if((wvi.isArray || wvi.isMatrix)
               && pos_ + 1 < lexems_.size()
               && lexems_[pos_+1]->getType() == LSquare)
            {
                next(); next();                 if(see(Var) && seeNext(Colon)) {
                                        int fromV = std::stoi(current().getStr()); next(); next();
                    if(!see(Var))
                        reportError("Ожидалось целое число после ':' в срезе");
                    else {
                        int toV = std::stoi(current().getStr()); next();
                        mustBe(RSquare, "Ожидалась ']'");
                        VarInfo& svi = getVar(wname);
                        VarInfo sl = sliceToTemp(svi, fromV, toV);
                        emitPrintArray(sl);
                    }
                }
                else if(wvi.isMatrix && see(Var)) {
                                        int rowIdx = std::stoi(current().getStr()); next();
                    mustBe(RSquare, "Ожидалась ']'");
                    VarInfo& svi = getVar(wname);
                    VarInfo row = matRowToTemp(svi, rowIdx);
                    emitPrintArray(row);
                }
                else {
                                        expression();
                    mustBe(RSquare, "Ожидалась ']'");
                    if(match(LSquare)) {
                        int cols = wvi.arraySize2;
                        codegen_->emit(PUSH, cols);
                        codegen_->emit(MULT);
                        expression();
                        mustBe(RSquare, "Ожидалась ']'");
                        codegen_->emit(ADD);
                    }
                    codegen_->emit(BLOAD, wvi.addr);
                    codegen_->emit(PRINT);
                }
            }
            else {
                                VarInfo vi = evalArrayExpr();
                emitPrintArray(vi);
            }
        }
        else {
                        VarInfo vi = evalArrayExpr();
            emitPrintArray(vi);
        }
    }
    else {
                expression();
        codegen_->emit(PRINT);
    }

    mustBe(RBracket, "Ожидалась ')' в write(...)");
}

void Parser::readStatement()
{
    mustBe(LBracket, "Ожидалась '(' после 'read'");
    if(!see(Id)) {
        reportError("Ожидался идентификатор в read()");
    }
    else {
        std::wstring name = current().getStr();
        next();
        VarInfo& vi = getVar(name);
        if(match(LSquare)) {
            expression();
            mustBe(RSquare, "Ожидалась ']'");
            if(match(LSquare)) {
                int cols = vi.arraySize2;
                codegen_->emit(PUSH, cols);
                codegen_->emit(MULT);
                expression();
                mustBe(RSquare, "Ожидалась ']'");
                codegen_->emit(ADD);
            }
            codegen_->emit(INPUT);
            codegen_->emit(BSTORE, vi.addr);
        }
        else {
            codegen_->emit(INPUT);
            codegen_->emit(STORE, vi.addr);
        }
    }
    mustBe(RBracket, "Ожидалась ')' в read(...)");
}

void Parser::expression()
{
    boolExpression();
}

void Parser::boolExpression()
{
    boolTerm();
    while(see(FullOr) || see(ShortOr)) {
        bool isShort = see(ShortOr);
        next();
        if(isShort) {
                        codegen_->emit(DUP);
            int jR = codegen_->reserve();             int jE = codegen_->reserve();             codegen_->emitAt(jR, JUMP_NO, codegen_->getCurrentAddress());
            codegen_->emit(POP);
            boolTerm();
            codegen_->emit(PUSH, 0);
            codegen_->emit(COMPARE, 1);             codegen_->emitAt(jE, JUMP, codegen_->getCurrentAddress());
        }
        else {
                        boolTerm();
            codegen_->emit(ADD);
            codegen_->emit(PUSH, 0);
            codegen_->emit(COMPARE, 3);         }
    }
}

void Parser::boolTerm()
{
    boolFactor();
    while(see(FullAnd) || see(ShortAnd)) {
        bool isShort = see(ShortAnd);
        next();
        if(isShort) {
                        codegen_->emit(DUP);
            int jSk = codegen_->reserve();             codegen_->emit(POP);
            boolFactor();
            codegen_->emit(PUSH, 0);
            codegen_->emit(COMPARE, 1);
            int jE = codegen_->reserve();
            codegen_->emitAt(jSk, JUMP_NO, codegen_->getCurrentAddress());
            codegen_->emit(POP);
            codegen_->emit(PUSH, 0);
            codegen_->emitAt(jE, JUMP, codegen_->getCurrentAddress());
        }
        else {
                        boolFactor();
            codegen_->emit(MULT);
            codegen_->emit(PUSH, 0);
            codegen_->emit(COMPARE, 1);
        }
    }
}

void Parser::boolFactor()
{
    if(match(Not)) {
        boolPrimary();
                codegen_->emit(INVERT);
        codegen_->emit(PUSH, 1);
        codegen_->emit(ADD);
    }
    else {
        boolPrimary();
    }
}

void Parser::boolPrimary()
{
    if(match(True))  { codegen_->emit(PUSH, 1); return; }
    if(match(False)) { codegen_->emit(PUSH, 0); return; }

    arithmeticExpression();

            if(see(Equal)        || see(NotEqual)    ||
       see(Less)         || see(LessEqual)   ||
       see(Greater)      || see(GreaterEqual))
    {
        LexemType op = current().getType();
        next();
        arithmeticExpression();
        switch(op) {
        case Equal:        codegen_->emit(COMPARE, 0); break;
        case NotEqual:     codegen_->emit(COMPARE, 1); break;
        case Less:         codegen_->emit(COMPARE, 2); break;
        case Greater:      codegen_->emit(COMPARE, 3); break;
        case LessEqual:    codegen_->emit(COMPARE, 4); break;
        case GreaterEqual: codegen_->emit(COMPARE, 5); break;
        default: break;
        }
    }
}

void Parser::arithmeticExpression()
{
    term();
    while(see(Plus) || see(Diff)) {
        bool isPlus = see(Plus);
        next();
        term();
        codegen_->emit(isPlus ? ADD : SUB);
    }
}

void Parser::term()
{
    factor();
    while(see(Mult) || see(Div)) {
        bool isMult = see(Mult);
        next();
        factor();
        codegen_->emit(isMult ? MULT : DIV);
    }
}

void Parser::factor()
{
    if(see(Diff)) {
        next();
        primary();
        codegen_->emit(INVERT);
        return;
    }
        if(see(LBracket) && seeNext(Int)) {
        next(); next();
        mustBe(RBracket, "");
        factor();
        return;
    }
    primary();
}

void Parser::primary()
{
    if(see(Var)) {
                codegen_->emit(PUSH, std::stoi(current().getStr()));
        next();
        return;
    }

    if(match(True))  { codegen_->emit(PUSH, 1); return; }     if(match(False)) { codegen_->emit(PUSH, 0); return; } 
    if(see(Id)) {
                        std::wstring name = current().getStr();
        next();
        if(!varExists(name)) {
            reportError("Использование необъявленной переменной '"
                + std::string(name.begin(), name.end()) + "'");
            codegen_->emit(PUSH, 0);
            return;
        }
        VarInfo& vi = getVar(name);
        if(match(LSquare)) {
            if(see(Var) && seeNext(Colon)) {
                                int from = std::stoi(current().getStr()); next(); next();
                if(!see(Var))
                    reportError("Ожидалось целое число после ':' в срезе");
                else {
                    int to = std::stoi(current().getStr()); next();
                    mustBe(RSquare, "Ожидалась ']'");
                    callSlice(name, from, to);
                }
            }
            else {
                                expression();
                mustBe(RSquare, "Ожидалась ']'");
                if(match(LSquare)) {
                    int cols = vi.arraySize2;
                    if(cols == 0)
                        reportError("Переменная не является матрицей");
                    codegen_->emit(PUSH, cols);
                    codegen_->emit(MULT);
                    expression();
                    mustBe(RSquare, "Ожидалась ']'");
                    codegen_->emit(ADD);
                }
                codegen_->emit(BLOAD, vi.addr);
            }
        }
        else {
            codegen_->emit(LOAD, vi.addr);
        }
        return;
    }

    if(match(LBracket)) {
                expression();
        mustBe(RBracket, "Ожидалась ')'");
        return;
    }

    if(match(Read)) {
                mustBe(LBracket, "Ожидалась '(' после 'read'");
        mustBe(RBracket, "Ожидалась ')' после 'read('");
        codegen_->emit(INPUT);
        return;
    }

        if(match(Find))     { callFind();     return; }
    if(match(Len))      { callLen();      return; }
    if(match(Reverse))  { callReverse();  return; }
    if(match(Resize))   { callResize();   return; }
    if(match(Replace))  { callReplace();  return; }
    if(match(Sorted))   { callSorted();   return; }
    if(match(ToVector)) { callToVector(); return; }
    if(match(Trans))    { callTrans();    return; }
    if(match(Shimb))    { callShimb();    return; }
    if(match(Fill))     { callFill();     return; }
    if(match(Copy))     { callCopy();     return; }

    reportError("Неожиданный токен в выражении");
    next();
}

void Parser::mustBe(LexemType t, const std::string& errMsg)
{
    if(!match(t)) {
        reportError(errMsg.empty()
            ? "Ожидалось " + std::string(1,'\'')
              + std::to_string((int)t) + std::string(1,'\'')
            : errMsg);
    }
}

void Parser::reportError(const std::string& msg)
{
    error_ = true;
    errors_.push_back({msg, (int)pos_});
}

const Lexem& Parser::current() const
{
    if(pos_ >= lexems_.size()) return *lexems_.back();
    return *lexems_[pos_];
}

bool Parser::see(LexemType t)  const { return current().getType() == t; }

bool Parser::seeNext(LexemType t) const
{
    if(pos_ + 1 >= lexems_.size()) return false;
    return lexems_[pos_+1]->getType() == t;
}

void Parser::next()  { if(pos_ < lexems_.size()) ++pos_; }

bool Parser::match(LexemType t)
{
    if(see(t)) { next(); return true; }
    return false;
}

bool Parser::varExists(const std::wstring& n) const
{
    return symbolTable_.count(n) > 0;
}

int Parser::declareVar(const std::wstring& n, DataType t)
{
    if(symbolTable_.count(n)) return symbolTable_.at(n).addr;
    int a = nextAddr_++;
    symbolTable_[n] = VarInfo(a, t);
    return a;
}

int Parser::declareArray(const std::wstring& n, DataType t, int sz)
{
    if(symbolTable_.count(n)) return symbolTable_.at(n).addr;
    int a = nextAddr_; nextAddr_ += sz;
    symbolTable_[n] = VarInfo(a, t, true, false, sz, 0);
    return a;
}

int Parser::declareMatrix(const std::wstring& n, DataType t, int r, int c)
{
    if(symbolTable_.count(n)) return symbolTable_.at(n).addr;
    int a = nextAddr_; nextAddr_ += r * c;
    symbolTable_[n] = VarInfo(a, t, false, true, r, c);
    return a;
}

VarInfo& Parser::getVar(const std::wstring& n)
{
    if(!symbolTable_.count(n)) {
        reportError("Необъявленная переменная '"
            + std::string(n.begin(), n.end()) + "'");
        symbolTable_[n] = VarInfo(nextAddr_++, T_INT);
    }
    return symbolTable_[n];
}

VarInfo Parser::copyToTemp(const VarInfo& src)
{
    int total = src.isMatrix
        ? src.arraySize * src.arraySize2
        : src.arraySize;
    std::wstring nm = anonName(L"tmp");
    int base = nextAddr_; nextAddr_ += total;
    VarInfo res;
    if(src.isMatrix)
        res = VarInfo(base, src.type, false, true,
                      src.arraySize, src.arraySize2);
    else
        res = VarInfo(base, src.type, true, false, src.arraySize, 0);
    symbolTable_[nm] = res;
    for(int i = 0; i < total; ++i) {
        codegen_->emit(LOAD,  src.addr + i);
        codegen_->emit(STORE, base + i);
    }
    return res;
}

VarInfo Parser::matRowToTemp(const VarInfo& mat, int rowIdx)
{
    int cols = mat.arraySize2;
    std::wstring nm = anonName(L"row");
    int base = nextAddr_; nextAddr_ += cols;
    VarInfo res(base, mat.type, true, false, cols, 0);
    symbolTable_[nm] = res;
    for(int c = 0; c < cols; ++c) {
        codegen_->emit(LOAD,  mat.addr + rowIdx * cols + c);
        codegen_->emit(STORE, base + c);
    }
    return res;
}

VarInfo Parser::sliceToTemp(const VarInfo& src, int from, int to)
{
    int cnt = to - from;
    if(cnt <= 0) {
        reportError("Срез: пустой диапазон ["
            + std::to_string(from) + ":" + std::to_string(to) + "]");
        return VarInfo();
    }
    std::wstring nm = anonName(L"slc");
    if(src.isMatrix) {
        if(from < 0 || to > src.arraySize)
            reportError("Срез: индекс строки выходит за границы");
        int cols = src.arraySize2, base = nextAddr_;
        nextAddr_ += cnt * cols;
        VarInfo res(base, src.type, false, true, cnt, cols);
        symbolTable_[nm] = res;
        for(int r = 0; r < cnt; ++r)
            for(int c = 0; c < cols; ++c) {
                codegen_->emit(LOAD,  src.addr + (from+r)*cols + c);
                codegen_->emit(STORE, base + r*cols + c);
            }
        return res;
    }
    else {
        if(from < 0 || to > src.arraySize)
            reportError("Срез: индекс выходит за границы массива");
        int base = nextAddr_; nextAddr_ += cnt;
        VarInfo res(base, src.type, true, false, cnt, 0);
        symbolTable_[nm] = res;
        for(int i = 0; i < cnt; ++i) {
            codegen_->emit(LOAD,  src.addr + from + i);
            codegen_->emit(STORE, base + i);
        }
        return res;
    }
}

void Parser::emitPrintArray(const VarInfo& vi)
{
    int total = vi.isMatrix
        ? vi.arraySize * vi.arraySize2
        : vi.arraySize;
    for(int i = 0; i < total; ++i) {
        codegen_->emit(LOAD,  vi.addr + i);
        codegen_->emit(PRINT);
    }
}

VarInfo Parser::evalArrayExpr(DataType type)
{
    if(see(LCurly)) {
        next();
        if(see(LCurly)) {
                        size_t p = pos_; int cols = 1, d = 1; ++p;
            while(p < lexems_.size() && d > 0) {
                LexemType lt = lexems_[p]->getType();
                if(lt == LCurly) ++d;
                else if(lt == RCurly) { --d; if(d == 0) break; }
                else if(lt == Comma && d == 1) ++cols;
                ++p;
            }
            int rows = 0; p = pos_; d = 0;
            while(p < lexems_.size()) {
                LexemType lt = lexems_[p]->getType();
                if(lt == LCurly)  { ++d; if(d == 1) ++rows; }
                else if(lt == RCurly) { --d; if(d < 0) break; }
                ++p;
            }
            int base = nextAddr_; nextAddr_ += rows * cols;
            VarInfo res(base, type, false, true, rows, cols);
            symbolTable_[anonName(L"lm")] = res;
            for(int r = 0; r < rows; ++r) {
                if(r > 0) mustBe(Comma, "Ожидалась ',' между строками матрицы");
                mustBe(LCurly, "Ожидалась '{' для строки матрицы");
                for(int c = 0; c < cols; ++c) {
                    if(c > 0) mustBe(Comma, "Ожидалась ',' между элементами");
                    expression();
                    codegen_->emit(STORE, base + r*cols + c);
                }
                mustBe(RCurly, "Ожидалась '}'");
            }
            mustBe(RCurly, "Ожидалась '}' после литерала матрицы");
            return res;
        }
        else {
                        size_t p = pos_; int len = 1, depth = 0;
            while(p < lexems_.size()) {
                LexemType lt = lexems_[p]->getType();
                if(lt == LBracket || lt == LCurly || lt == LSquare)   ++depth;
                else if(lt == RBracket || lt == RCurly || lt == RSquare) {
                    if(depth == 0) break; --depth;
                }
                else if(lt == Comma && depth == 0) ++len;
                ++p;
            }
            int base = nextAddr_; nextAddr_ += len;
            VarInfo res(base, type, true, false, len, 0);
            symbolTable_[anonName(L"la")] = res;
            for(int i = 0; i < len; ++i) {
                if(i > 0) mustBe(Comma, "Ожидалась ','");
                expression();
                codegen_->emit(STORE, base + i);
            }
            mustBe(RCurly, "Ожидалась '}'");
            return res;
        }
    }

    if(match(ToVector)) return callToVectorImpl();
    if(match(Copy))     return callCopyImpl();
    if(match(Shimb))    return callShimbImpl();
    if(match(Trans))    return callTransImpl();
    if(match(Reverse))  return callReverseImpl();
    if(match(Sorted))   return callSortedImpl();
    if(match(Resize))   return callResizeImpl();
    if(match(Replace))  return callReplaceImpl();

    if(!see(Id)) {
        reportError("Ожидалось выражение-массив или матрица");
        return VarInfo();
    }
    std::wstring name = current().getStr(); next();
    VarInfo& vi = getVar(name);
    if(match(LSquare)) {
        if(see(Var) && seeNext(Colon)) {
            int from = std::stoi(current().getStr()); next(); next();
            if(!see(Var)) {
                reportError("Ожидалось целое число после ':' в срезе");
                mustBe(RSquare, ""); return VarInfo();
            }
            int to = std::stoi(current().getStr()); next();
            mustBe(RSquare, "Ожидалась ']'");
            return sliceToTemp(vi, from, to);
        }
        else {
            if(!see(Var)) {
                reportError("Ожидался константный целочисленный индекс");
                mustBe(RSquare, ""); return vi;
            }
            int rowIdx = std::stoi(current().getStr()); next();
            mustBe(RSquare, "Ожидалась ']'");
            if(!vi.isMatrix) {
                reportError("'" + std::string(name.begin(),name.end())
                    + "' не является матрицей");
                return vi;
            }
            if(rowIdx < 0 || rowIdx >= vi.arraySize)
                reportError("Индекс строки выходит за границы матрицы");
            return matRowToTemp(vi, rowIdx);
        }
    }
    return vi;
}

VarInfo Parser::callToVectorImpl()
{
    mustBe(LBracket, "Ожидалась '(' после 'tovector'");
    if(!see(Id)) { reportError("Ожидался идентификатор в tovector()"); mustBe(RBracket,""); return VarInfo(); }
    std::wstring name = current().getStr(); next();
    mustBe(RBracket, "Ожидалась ')' в tovector()");
    VarInfo& vi = getVar(name);
    if(!vi.isMatrix) { reportError("tovector(): аргумент должен быть матрицей"); return VarInfo(); }
    int total = vi.arraySize * vi.arraySize2;
    int base = nextAddr_; nextAddr_ += total;
    VarInfo res(base, vi.type, true, false, total, 0);
    symbolTable_[anonName(L"tvec")] = res;
    for(int i = 0; i < total; ++i) { codegen_->emit(LOAD, vi.addr+i); codegen_->emit(STORE, base+i); }
    return res;
}

VarInfo Parser::callCopyImpl()
{
    mustBe(LBracket, "Ожидалась '(' после 'copy'");
    VarInfo src = evalArrayExpr();
    mustBe(RBracket, "Ожидалась ')' в copy()");
    return copyToTemp(src);
}

VarInfo Parser::callReverseImpl()
{
    mustBe(LBracket, "Ожидалась '(' после 'reverse'");
    VarInfo vi = evalArrayExpr();
    mustBe(RBracket, "Ожидалась ')' в reverse()");
    if(vi.isMatrix) {
        int rows=vi.arraySize, cols=vi.arraySize2, tmp=nextAddr_; nextAddr_+=cols;
        for(int l=0,r=rows-1; l<r; ++l,--r) {
            for(int c=0;c<cols;++c){codegen_->emit(LOAD,vi.addr+l*cols+c);codegen_->emit(STORE,tmp+c);}
            for(int c=0;c<cols;++c){codegen_->emit(LOAD,vi.addr+r*cols+c);codegen_->emit(STORE,vi.addr+l*cols+c);}
            for(int c=0;c<cols;++c){codegen_->emit(LOAD,tmp+c);codegen_->emit(STORE,vi.addr+r*cols+c);}
        }
    }
    else {
        int len=vi.arraySize, tmp=nextAddr_++;
        for(int l=0,r=len-1; l<r; ++l,--r) {
            codegen_->emit(LOAD,vi.addr+l); codegen_->emit(STORE,tmp);
            codegen_->emit(LOAD,vi.addr+r); codegen_->emit(STORE,vi.addr+l);
            codegen_->emit(LOAD,tmp);       codegen_->emit(STORE,vi.addr+r);
        }
    }
    return vi;
}

VarInfo Parser::callSortedImpl()
{
    mustBe(LBracket, "Ожидалась '(' после 'sorted'");
    VarInfo vi = evalArrayExpr();
    if(vi.isMatrix) {
        mustBe(Comma, "Ожидалась ',' в sorted(мат,k)");
        if(!see(Var)){reportError("Ожидался индекс ключа в sorted()");mustBe(RBracket,"");return vi;}
        int keyIdx = std::stoi(current().getStr()); next();
        mustBe(RBracket, "Ожидалась ')' в sorted()");
        int rows=vi.arraySize, cols=vi.arraySize2;
        if(keyIdx<0||keyIdx>=cols){reportError("sorted(): индекс ключа выходит за границы");return vi;}
        int tmp=nextAddr_; nextAddr_+=cols;
        for(int i=0;i<rows-1;++i) for(int j=0;j<rows-1-i;++j) {
            codegen_->emit(LOAD,vi.addr+j*cols+keyIdx);
            codegen_->emit(LOAD,vi.addr+(j+1)*cols+keyIdx);
            codegen_->emit(COMPARE,3);
            int jmp=codegen_->reserve();
            for(int c=0;c<cols;++c){codegen_->emit(LOAD,vi.addr+j*cols+c);codegen_->emit(STORE,tmp+c);}
            for(int c=0;c<cols;++c){codegen_->emit(LOAD,vi.addr+(j+1)*cols+c);codegen_->emit(STORE,vi.addr+j*cols+c);}
            for(int c=0;c<cols;++c){codegen_->emit(LOAD,tmp+c);codegen_->emit(STORE,vi.addr+(j+1)*cols+c);}
            codegen_->emitAt(jmp, JUMP_NO, codegen_->getCurrentAddress());
        }
    }
    else {
        mustBe(RBracket, "Ожидалась ')' в sorted()");
        int len=vi.arraySize, tmp=nextAddr_++;
        for(int i=0;i<len-1;++i) for(int j=0;j<len-1-i;++j) {
            codegen_->emit(LOAD,vi.addr+j); codegen_->emit(LOAD,vi.addr+j+1); codegen_->emit(COMPARE,3);
            int jmp=codegen_->reserve();
            codegen_->emit(LOAD,vi.addr+j);   codegen_->emit(STORE,tmp);
            codegen_->emit(LOAD,vi.addr+j+1); codegen_->emit(STORE,vi.addr+j);
            codegen_->emit(LOAD,tmp);          codegen_->emit(STORE,vi.addr+j+1);
            codegen_->emitAt(jmp, JUMP_NO, codegen_->getCurrentAddress());
        }
    }
    return vi;
}

VarInfo Parser::callTransImpl()
{
    mustBe(LBracket, "Ожидалась '(' после 'trans'");
    VarInfo vi = evalArrayExpr();
    mustBe(RBracket, "Ожидалась ')' в trans()");
    if(!vi.isMatrix){reportError("trans(): аргумент должен быть матрицей");return vi;}
    int rows=vi.arraySize, cols=vi.arraySize2;
    if(rows==cols) {
        for(int i=0;i<rows;++i) for(int j=i+1;j<cols;++j) {
            int tmp=nextAddr_++;
            codegen_->emit(LOAD,vi.addr+i*cols+j); codegen_->emit(STORE,tmp);
            codegen_->emit(LOAD,vi.addr+j*cols+i); codegen_->emit(STORE,vi.addr+i*cols+j);
            codegen_->emit(LOAD,tmp);               codegen_->emit(STORE,vi.addr+j*cols+i);
        }
        return vi;
    }
    else {
        int tBase=nextAddr_; nextAddr_+=rows*cols;
        VarInfo res(tBase,vi.type,false,true,cols,rows); symbolTable_[anonName(L"trans")]=res;
        for(int i=0;i<rows;++i) for(int j=0;j<cols;++j)
            { codegen_->emit(LOAD,vi.addr+i*cols+j); codegen_->emit(STORE,tBase+j*rows+i); }
        return res;
    }
}

VarInfo Parser::callShimbImpl()
{
    mustBe(LBracket, "Ожидалась '(' после 'shimb'");
    VarInfo a = evalArrayExpr(); mustBe(Comma, "Ожидалась ',' в shimb()");
    VarInfo b = evalArrayExpr(); mustBe(Comma, "Ожидалась ',' в shimb()");
    bool useMin = true;
    if(match(False)) useMin=true; else if(match(True)) useMin=false;
    else if(see(Var)){useMin=(std::stoi(current().getStr())==0);next();}
    else{reportError("Ожидалось 0/1 или false/true в shimb()");next();}
    mustBe(RBracket, "Ожидалась ')' в shimb()");
    if(!a.isMatrix||!b.isMatrix){reportError("shimb(): оба аргумента должны быть матрицами");return VarInfo();}
    int n=a.arraySize, p=a.arraySize2, m=b.arraySize2;
    if(b.arraySize!=p){reportError("shimb(): несовместимые размеры матриц");return VarInfo();}
    int cBase=nextAddr_; nextAddr_+=n*m;
    VarInfo res(cBase,a.type,false,true,n,m); symbolTable_[anonName(L"shimb")]=res;
    for(int i=0;i<n;++i) for(int j=0;j<m;++j) {
        int acc=nextAddr_++;
        codegen_->emit(LOAD,a.addr+i*p); codegen_->emit(LOAD,b.addr+j); codegen_->emit(ADD); codegen_->emit(STORE,acc);
        for(int k=1;k<p;++k) {
            int cand=nextAddr_++;
            codegen_->emit(LOAD,a.addr+i*p+k); codegen_->emit(LOAD,b.addr+k*m+j); codegen_->emit(ADD); codegen_->emit(STORE,cand);
            codegen_->emit(LOAD,cand); codegen_->emit(LOAD,acc); codegen_->emit(COMPARE,useMin?2:3);
            int jmp=codegen_->reserve();
            codegen_->emit(LOAD,cand); codegen_->emit(STORE,acc);
            codegen_->emitAt(jmp, JUMP_NO, codegen_->getCurrentAddress());
        }
        codegen_->emit(LOAD,acc); codegen_->emit(STORE,cBase+i*m+j);
    }
    return res;
}

VarInfo Parser::callResizeImpl()
{
    mustBe(LBracket, "Ожидалась '(' после 'resize'");
    std::wstring arrName; bool hasName=false;
    if(see(Id) && !(pos_+1<lexems_.size() && lexems_[pos_+1]->getType()==LSquare))
        { arrName=current().getStr(); hasName=true; }
    VarInfo vi = evalArrayExpr();
    mustBe(Comma, "Ожидалась ',' в resize()");
    if(!see(Var)){reportError("Ожидался новый размер в resize()");mustBe(RBracket,"");return vi;}
    int newSize=std::stoi(current().getStr()); next();
    mustBe(Comma, "Ожидалась ',' в resize()");
    if(vi.isMatrix) {
        VarInfo fillVec = evalArrayExpr();
        mustBe(RBracket, "Ожидалась ')' в resize()");
        int cols=vi.arraySize2, oldRows=vi.arraySize, newBase=nextAddr_; nextAddr_+=newSize*cols;
        int cpR=(oldRows<newSize)?oldRows:newSize;
        for(int r=0;r<cpR;++r) for(int c=0;c<cols;++c)
            {codegen_->emit(LOAD,vi.addr+r*cols+c);codegen_->emit(STORE,newBase+r*cols+c);}
        for(int r=oldRows;r<newSize;++r) for(int c=0;c<cols;++c)
            {codegen_->emit(LOAD,fillVec.addr+c);codegen_->emit(STORE,newBase+r*cols+c);}
        VarInfo res(newBase,vi.type,false,true,newSize,cols); symbolTable_[anonName(L"rsz")]=res;
        if(hasName){symbolTable_[arrName].addr=newBase;symbolTable_[arrName].arraySize=newSize;}
        return res;
    }
    else {
        expression(); mustBe(RBracket, "Ожидалась ')' в resize()");
        int fillAddr=nextAddr_++; codegen_->emit(STORE,fillAddr);
        int oldSize=vi.arraySize, newBase=nextAddr_; nextAddr_+=newSize;
        int cp=(oldSize<newSize)?oldSize:newSize;
        for(int i=0;i<cp;++i){codegen_->emit(LOAD,vi.addr+i);codegen_->emit(STORE,newBase+i);}
        for(int i=oldSize;i<newSize;++i){codegen_->emit(LOAD,fillAddr);codegen_->emit(STORE,newBase+i);}
        VarInfo res(newBase,vi.type,true,false,newSize,0); symbolTable_[anonName(L"rsz")]=res;
        if(hasName){symbolTable_[arrName].addr=newBase;symbolTable_[arrName].arraySize=newSize;}
        return res;
    }
}

VarInfo Parser::callReplaceImpl()
{
    mustBe(LBracket, "Ожидалась '(' после 'replace'");
    VarInfo vi = evalArrayExpr(); mustBe(Comma, "Ожидалась ',' в replace()");
    if(vi.isMatrix) {
        int rows=vi.arraySize, cols=vi.arraySize2;
        int oldBase=nextAddr_; nextAddr_+=cols;
        int newBase2=nextAddr_; nextAddr_+=cols;
        auto readVec=[&](int base){
            VarInfo src=evalArrayExpr(vi.type);
            int total=src.isArray?src.arraySize:src.isMatrix?src.arraySize*src.arraySize2:1;
            for(int c=0;c<cols&&c<total;++c){codegen_->emit(LOAD,src.addr+c);codegen_->emit(STORE,base+c);}
        };
        readVec(oldBase); mustBe(Comma,"Ожидалась ','"); readVec(newBase2);
        mustBe(RBracket,"Ожидалась ')' в replace()");
        for(int r=0;r<rows;++r){
            codegen_->emit(PUSH,1);
            for(int c=0;c<cols;++c){codegen_->emit(LOAD,vi.addr+r*cols+c);codegen_->emit(LOAD,oldBase+c);codegen_->emit(COMPARE,0);codegen_->emit(MULT);}
            int jmp=codegen_->reserve();
            for(int c=0;c<cols;++c){codegen_->emit(LOAD,newBase2+c);codegen_->emit(STORE,vi.addr+r*cols+c);}
            codegen_->emitAt(jmp, JUMP_NO, codegen_->getCurrentAddress());
        }
    }
    else {
        expression(); mustBe(Comma,"Ожидалась ','"); expression();
        mustBe(RBracket,"Ожидалась ')' в replace()");
        int len=vi.arraySize, newV=nextAddr_++, oldV=nextAddr_++;
        codegen_->emit(STORE,newV); codegen_->emit(STORE,oldV);
        for(int i=0;i<len;++i){
            codegen_->emit(LOAD,vi.addr+i); codegen_->emit(LOAD,oldV); codegen_->emit(COMPARE,0);
            int jmp=codegen_->reserve();
            codegen_->emit(LOAD,newV); codegen_->emit(STORE,vi.addr+i);
            codegen_->emitAt(jmp, JUMP_NO, codegen_->getCurrentAddress());
        }
    }
    return vi;
}

VarInfo Parser::callFillImpl()
{
    mustBe(LBracket, "Ожидалась '(' после 'fill'");
    std::wstring arrName; bool hasName=false; int rowIdx=-1;
    if(see(Id)){
        arrName=current().getStr(); hasName=true; next();
        if(match(LSquare)){
            if(!see(Var)){reportError("Ожидался целочисленный индекс в fill(мат[i],...)");mustBe(RSquare,"");mustBe(RBracket,"");return VarInfo();}
            rowIdx=std::stoi(current().getStr()); next();
            mustBe(RSquare,"Ожидалась ']'");
        }
    }
    else{reportError("Ожидался идентификатор в fill()");mustBe(RBracket,"");return VarInfo();}
    mustBe(Comma,"Ожидалась ',' в fill()");
    expression();
    mustBe(RBracket,"Ожидалась ')' в fill()");
    VarInfo& vi=getVar(arrName);
    int fillAddr=nextAddr_++; codegen_->emit(STORE,fillAddr);
    if(rowIdx>=0){
        if(!vi.isMatrix){reportError("fill(массив[i],...): не является матрицей");return vi;}
        if(rowIdx>=vi.arraySize) reportError("fill(мат[i],...): индекс строки выходит за границы");
        int cols=vi.arraySize2;
        for(int c=0;c<cols;++c){codegen_->emit(LOAD,fillAddr);codegen_->emit(STORE,vi.addr+rowIdx*cols+c);}
    }
    else{
        int total=vi.isMatrix?vi.arraySize*vi.arraySize2:vi.arraySize;
        for(int i=0;i<total;++i){codegen_->emit(LOAD,fillAddr);codegen_->emit(STORE,vi.addr+i);}
    }
    return vi;
}

void Parser::callToVector(){ VarInfo r=callToVectorImpl(); codegen_->emit(PUSH,r.addr); }
void Parser::callCopy()    { VarInfo r=callCopyImpl();     codegen_->emit(PUSH,r.addr); }
void Parser::callTrans()   { VarInfo r=callTransImpl();    codegen_->emit(PUSH,r.addr); }
void Parser::callShimb()   { VarInfo r=callShimbImpl();    codegen_->emit(PUSH,r.addr); }
void Parser::callReverse() { callReverseImpl(); codegen_->emit(PUSH,0); }
void Parser::callSorted()  { callSortedImpl();  codegen_->emit(PUSH,0); }
void Parser::callResize()  { callResizeImpl();  codegen_->emit(PUSH,0); }
void Parser::callReplace() { callReplaceImpl(); codegen_->emit(PUSH,0); }
void Parser::callFill()    { callFillImpl();    codegen_->emit(PUSH,0); }

void Parser::callSlice(const std::wstring& name, int from, int to)
{
    VarInfo& vi  = getVar(name);
    VarInfo  res = sliceToTemp(vi, from, to);
    codegen_->emit(PUSH, res.addr);
}

void Parser::callFind()
{
    mustBe(LBracket,"Ожидалась '(' после 'find'");
    VarInfo vi=evalArrayExpr(); mustBe(Comma,"Ожидалась ','");
    if(vi.isMatrix){
        int rows=vi.arraySize, cols=vi.arraySize2;
        VarInfo vec=evalArrayExpr(vi.type); mustBe(RBracket,"Ожидалась ')'");
        int vecBase=nextAddr_; nextAddr_+=cols;
        int total=vec.isArray?vec.arraySize:vec.isMatrix?vec.arraySize*vec.arraySize2:1;
        for(int c=0;c<cols&&c<total;++c){codegen_->emit(LOAD,vec.addr+c);codegen_->emit(STORE,vecBase+c);}
        int resA=nextAddr_++; codegen_->emit(PUSH,-1); codegen_->emit(STORE,resA);
        for(int r=0;r<rows;++r){
            codegen_->emit(PUSH,1);
            for(int c=0;c<cols;++c){codegen_->emit(LOAD,vi.addr+r*cols+c);codegen_->emit(LOAD,vecBase+c);codegen_->emit(COMPARE,0);codegen_->emit(MULT);}
            int jmp=codegen_->reserve();
            codegen_->emit(PUSH,r); codegen_->emit(STORE,resA);
            codegen_->emitAt(jmp, JUMP_NO, codegen_->getCurrentAddress());
        }
        codegen_->emit(LOAD,resA);
    }
    else{
        expression(); mustBe(RBracket,"Ожидалась ')'");
        int len=vi.arraySize, resA=nextAddr_++, iA=nextAddr_++, valA=nextAddr_++;
        codegen_->emit(STORE,valA); codegen_->emit(PUSH,-1); codegen_->emit(STORE,resA);
        codegen_->emit(PUSH,0); codegen_->emit(STORE,iA);
        int loopA=codegen_->getCurrentAddress();
        codegen_->emit(LOAD,iA); codegen_->emit(PUSH,len); codegen_->emit(COMPARE,2);
        int jEnd=codegen_->reserve();
        codegen_->emit(LOAD,iA); codegen_->emit(BLOAD,vi.addr); codegen_->emit(LOAD,valA); codegen_->emit(COMPARE,0);
        int jFound=codegen_->reserve();
        codegen_->emit(LOAD,iA); codegen_->emit(PUSH,1); codegen_->emit(ADD); codegen_->emit(STORE,iA);
        codegen_->emit(JUMP,loopA);
        codegen_->emitAt(jFound, JUMP_YES, codegen_->getCurrentAddress());
        codegen_->emit(LOAD,iA); codegen_->emit(STORE,resA);
        codegen_->emitAt(jEnd, JUMP_NO, codegen_->getCurrentAddress());
        codegen_->emit(LOAD,resA);
    }
}

void Parser::callLen()
{
    mustBe(LBracket,"Ожидалась '(' после 'len'");
    int lv=0;
    if(see(Id)&&varExists(current().getStr())){
        std::wstring name=current().getStr();
        VarInfo& vi=getVar(name); next();
        if(match(LSquare)){
            if(see(Var)&&seeNext(Colon)){
                int from=std::stoi(current().getStr());next();next();
                if(!see(Var)) reportError("Ожидалось целое число после ':' в len()");
                else{int to=std::stoi(current().getStr());next();lv=to-from;}
                mustBe(RSquare,"Ожидалась ']'");
            }
            else{
                int d=1;
                while(d>0&&pos_<lexems_.size()){
                    if(see(LSquare))++d; else if(see(RSquare)){--d;if(d==0)break;} next();
                }
                mustBe(RSquare,"Ожидалась ']'");
                lv=vi.isMatrix?vi.arraySize2:0;
                if(!vi.isMatrix) reportError("len(массив[i]): не является матрицей");
            }
        }
        else lv=vi.arraySize;
    }
    else if(see(LCurly)||see(ToVector)||see(Copy)||see(Shimb)||see(Trans)||
            see(Reverse)||see(Sorted)||see(Resize)||see(Replace)){
        VarInfo vi=evalArrayExpr(); lv=vi.arraySize;
    }
    else reportError("Ожидалось выражение-массив в len()");
    mustBe(RBracket,"Ожидалась ')' в len()");
    codegen_->emit(PUSH,lv);
}

bool Parser::tryArrayArithAssign(const VarInfo& dst)
{
    size_t savedPos = pos_;
    if(!see(Id)) return false;
    std::wstring lName = current().getStr();
    if(!varExists(lName)) return false;
    VarInfo leftInfo = getVar(lName);
    if(!leftInfo.isArray && !leftInfo.isMatrix) return false;
    if(pos_+1 >= lexems_.size()) return false;
    LexemType opTok = lexems_[pos_+1]->getType();
    if(opTok!=Plus && opTok!=Diff && opTok!=Mult) return false;
    next(); next();     int total = dst.isMatrix
        ? dst.arraySize * dst.arraySize2
        : dst.arraySize;
    if(see(Id) && varExists(current().getStr())) {
        std::wstring rName = current().getStr(); next();
        VarInfo rInfo = getVar(rName);
        if(rInfo.isArray || rInfo.isMatrix) {
            int rTotal = rInfo.isMatrix
                ? rInfo.arraySize * rInfo.arraySize2
                : rInfo.arraySize;
            if(rTotal != total)
                reportError("Поэлементная арифметика: несовпадение размеров ("
                    +std::to_string(total)+" и "+std::to_string(rTotal)+")");
            for(int i=0;i<total;++i){
                codegen_->emit(LOAD, leftInfo.addr+i);
                codegen_->emit(LOAD, rInfo.addr+i);
                if(opTok==Plus)      codegen_->emit(ADD);
                else if(opTok==Diff) codegen_->emit(SUB);
                else                 codegen_->emit(MULT);
                codegen_->emit(STORE, dst.addr+i);
            }
            return true;
        }
        pos_ = savedPos; return false;
    }
    int scAddr = nextAddr_++; expression(); codegen_->emit(STORE, scAddr);
    for(int i=0;i<total;++i){
        codegen_->emit(LOAD, leftInfo.addr+i);
        codegen_->emit(LOAD, scAddr);
        if(opTok==Plus)      codegen_->emit(ADD);
        else if(opTok==Diff) codegen_->emit(SUB);
        else                 codegen_->emit(MULT);
        codegen_->emit(STORE, dst.addr+i);
    }
    return true;
}
