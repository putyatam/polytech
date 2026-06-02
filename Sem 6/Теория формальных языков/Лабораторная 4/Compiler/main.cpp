#include "compile_visual.h"
#include "parser.h"
#include <fstream>
#include <QtWidgets/QApplication>
#include <QStyleFactory>



int main(int argc, char* argv[])
{
    if(argc > 1) {
        std::string filePath = argv[1];

        std::wstring buffer;
        ScanResult result;
        Group currentGroup = gNone;
        FSM<std::wstring>* fsm = initTm(buffer, currentGroup, result.lexems, result.errors, result.lexemsRows);


        ScanResult scanRes = tokenize(fsm, filePath);

        if (!scanRes.errors.empty()) {
            std::cerr << "Lexical errors found:" << std::endl;
            for (auto err : scanRes.errors) {
                err->print();
            }
            return EXIT_FAILURE;
        }

        try {
            CodeGen codegen(std::cout);

            Parser p(scanRes.lexems, &codegen);

            p.parse();

            std::cout << "Success: Translation finished." << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "Parser error: " << e.what() << std::endl;
            return EXIT_FAILURE;
        }


        return EXIT_SUCCESS;
    }
    else {
        //QApplication::setStyle(QStyleFactory::create("Fusion"));
        QApplication app(argc, argv);
        compile_visual window;
        window.show();
        return app.exec();
    }
    
}
