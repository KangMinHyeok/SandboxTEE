#include "native_client/src/trusted/xcall/enclave_exception.h"

// TODO

void ExceptionHandler (unsigned int exit_info, void * uc){
  UNREFERENCED_PARAMETER(exit_info);
  UNREFERENCED_PARAMETER(uc);
}

void HandleExternalEvent (int event, void * uc){
  UNREFERENCED_PARAMETER(uc);
  UNREFERENCED_PARAMETER(event);
}

