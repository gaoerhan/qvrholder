#include <iostream>
#include <unistd.h>

#include "qvr/inc/QVRServiceClient.h"
#include <android/log.h>

#define LOG_LIB "liblog.so"
#define TAG "QvrHolder"

// global
typedef int (*__android_log_print_fn)(int prio, const char* tag, const char* fmt, ...);
void* pLogDll = NULL;
__android_log_print_fn __log_func = NULL;


const char* status_to_string(QVRSERVICE_CLIENT_STATUS status)
{
    switch (status) {
        case STATUS_DISCONNECTED:
            return "STATUS_DISCONNECTED";
        case STATUS_STATE_CHANGED:
            return "STATUS_STATE_CHANGED";
        case STATUS_SENSOR_ERROR:
            return "STATUS_SENSOR_ERROR";
        case STATUS_MAX:
        default:
            return "STATUS_MAX";
    }
}

//void (*client_status_callback_fn)(void *pCtx,
//                                  QVRSERVICE_CLIENT_STATUS status, uint32_t arg1, uint32_t arg2);
// arg2 : prev vrmode state
// arg1 : new vrmode state
void client_status_callback (void *pCtx, QVRSERVICE_CLIENT_STATUS status, uint32_t arg1, uint32_t arg2)
{
    __log_func(ANDROID_LOG_VERBOSE, TAG, "client status: %s | arg1: %s | arg2: %s", status_to_string(status)
            , QVRServiceClient_StateToName((QVRSERVICE_VRMODE_STATE) arg1)
            , QVRServiceClient_StateToName((QVRSERVICE_VRMODE_STATE) arg2));
}

void load_log_lib()
{
    pLogDll = dlopen( LOG_LIB, RTLD_NOW);
    std::cout << "log: " << pLogDll << std::endl;

    __log_func = (__android_log_print_fn)dlsym(pLogDll, "__android_log_print");
    __log_func(ANDROID_LOG_VERBOSE, TAG, "init logs");
}

void close_log_lib()
{
    if (pLogDll == NULL)
        return ;

    dlclose(pLogDll);
    pLogDll = NULL;
}

void atexit_handler()
{
    __log_func(ANDROID_LOG_ERROR, TAG, "qvr service client exit !!!");
    close_log_lib();
}


//
void init()
{
    auto res = std::atexit(atexit_handler);
    if (res != 0)
    {
        __log_func(ANDROID_LOG_ERROR, TAG, "register aitexit handle failed");
    }
}

int main() {

    init();

    load_log_lib();

    qvrservice_client_helper_t* qvr_client = QVRServiceClient_Create();
    __log_func(ANDROID_LOG_VERBOSE, TAG, "api version: %d", qvr_client->client->api_version);

    QVRSERVICE_VRMODE_STATE vrstate = QVRServiceClient_GetVRMode(qvr_client);
    __log_func(ANDROID_LOG_VERBOSE, TAG, "vrmode state: %s", QVRServiceClient_StateToName(vrstate));

    int32_t res = QVRServiceClient_StartVRMode(qvr_client);
    vrstate = QVRServiceClient_GetVRMode(qvr_client);
    __log_func(ANDROID_LOG_VERBOSE, TAG, "start vrmode res: %d | curr vrmode: %s", res, QVRServiceClient_StateToName(vrstate));

    QVRServiceClient_SetClientStatusCallback(qvr_client, client_status_callback, NULL);

    XrFramePoseQTI* framePos = new XrFramePoseQTI();
    while (true)
    {
        int32_t frameRes = QVRServiceClient_GetFramePose(qvr_client, &framePos);

        if (frameRes < 0) {
            __log_func(ANDROID_LOG_ERROR, TAG, "get qvr frame pos failed !!!");
            break;
        }

//        __log_func(ANDROID_LOG_DEBUG, TAG, "get frame pos res: %d | pos: {%f, %f, %f}", frameRes
//                , framePos->pose.position.x
//                , framePos->pose.position.y
//                , framePos->pose.position.z);

        unsigned int microseconds = 1000 * 1000; // 1 sec
        usleep(microseconds);
    }

    delete framePos;
    close_log_lib();

    return 0;
}
