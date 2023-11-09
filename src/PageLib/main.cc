#include "PageLibPreprocessor.h"
#include "Time.h"

int main() {
    PageLibPreprocessor page;
    Time t;
    page.doProcess();
    page.cutRedundantPages();
    page.createXml();
    page.buildInvertIndex();
    std::cout << "建立网页库用时：";
    t.end();
    return 0;
}
