// to use this header, easylogging++.cc must be included to the project. 20240925_jintaeks

#include "KGen.h"
#include "easylogging++.h"

//INITIALIZE_EASYLOGGINGPP // declare once at main() source file. 20240925_jintaeks

#define KLOG            LOG(INFO)
#define KLOG_NAME(a)    LOG(INFO) << VAR_NAME( a ) << "=" << a

// to use this header, easylogging++.cc must be included to the project. 20240925_jintaeks

/** @example

    #include "KLog.h"

    INITIALIZE_EASYLOGGINGPP // declare once at main() source file. 20240925_jintaeks

    void main() {
        // ...
    }
*/