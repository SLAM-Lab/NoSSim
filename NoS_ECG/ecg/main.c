#include "ecg.h"
#include "annotation.h"
#include "papi.h"
int main(){
  program_start_BA(2, 0);
  ECG_init();
  ECG_execute(".");
  program_end_BA(2, 0);
  return 0;
}
