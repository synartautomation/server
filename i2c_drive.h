#include <iostream>
#include <unistd.h>
#include <wiringPiI2C.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
using namespace std;
float as[]={0,0,0,0};
int chk;
class I2C_drive{

private:

  int *fd;
  int dd;
  size_t _count;
  uint8_t *out;
public:
  union encoder{
    char result[6];
    float val;
  }*value;
  I2C_drive(size_t count,uint8_t *e_addr,uint8_t d_addr)
  {
     fd=(int*)malloc(sizeof(uint8_t)*count);
     out=(uint8_t*)malloc(sizeof(uint8_t)*(count+1));
     out[count]=0;
     value=(I2C_drive::encoder*)malloc(sizeof(union encoder)*count);
     _count=count;
     for(size_t i=0;i<count;i++){
       fd[i] = wiringPiI2CSetup(e_addr[i]);
     }
     dd=wiringPiI2CSetup(d_addr);
  }

  bool check(){
   uint8_t checksum=0;
   for(int i=0;i<_count;i++){
    checksum=value[i].result[0]^value[i].result[1]^value[i].result[2]^value[i].result[3];
    if(checksum!=(uint8_t)value[i].result[4] || isnan(value[i].val))
{
    printf("[CHECKSUM] %x [VALUE] %x [ENCODERVAL] %f [ISNAN] %d \n",checksum,value[i].result[4],value[i].val,isnan(value[i].val));
    return false;
}  
 }
    return true;
  }
  void read_float()
  {
    bool passed=false;
    do{
    for(size_t i=0;i<_count;i++){
        read(fd[i],(void*)&value[i].result,6);
     //~ printf("%x \n",value[i].result[5]);
     cout<<"[LOG] "<<i<<": "<< value[i].val<<endl;
    }
    passed=check();
    }
    while(!passed);
  }

  void clear(){
   //uint8_t c=0;
   //for(size_t i=0;i<_count;i++){
    //open(fd[i]);
    //write(fd[i],&c,2);
    
   //}
  }

  void drive(int *pwm,bool restart=false){
    out[_count]=0;
    for(uint8_t i=0;i<_count;i++){
      if(pwm[i]<0){
        out[_count]|=1<<i;
        out[i]=(uint8_t)(-1*pwm[i]);
      }
      else
      {
       out[i]=(uint8_t)(pwm[i]);
      }
    }    if(restart)out[_count]|=1<<7;
    write(dd,out,_count+1);
  }
};
