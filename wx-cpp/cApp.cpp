#include "cApp.h"


wxIMPLEMENT_APP(cApp);

cApp::cApp() {

}

cApp::~cApp() {

}

bool cApp::OnInit() {
    
#if defined(_WIN32) || defined(WIN32)
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
#endif
    m_frame1 = new cMain();
    m_frame1->Show();

    return true;
}