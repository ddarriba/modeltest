#include "jmodeltest.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    int return_val = 0;

    if (argc > 1)
    {
        /* command line */
        modeltest::ModelTest mt;
        std::vector<int> matrices;
        matrices.push_back(1);
        matrices.push_back(2);
        matrices.push_back(5);
        mt.build_instance(31, 4, matrices, std::string(argv[1]), std::string(argv[2]), tree_user_fixed);

        mt.evaluate_models();
    }
    else
    {
        /* launch GUI */
        QApplication a(argc, argv);
        modeltest::jModelTest w;
        w.show();
        return_val = a.exec();
    }

    return return_val;
}
