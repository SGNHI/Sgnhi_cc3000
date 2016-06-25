#ifndef SGNHI_CC3000
#define SGNHI_CC3000

#include <SPI.h>
#include <Adafruit_CC3000.h>
#include <ccspi.h>
#include <string.h>
#include <utility/debug.h>
#if ARDUINO < 100
  #include <WProgram.h>
#else
    #include <Arduino.h>
#endif

#define SERVER "veyrobotics.cafe24.com"

#define atInt 1
#define atFloat 2
#define atDouble 3
#define atLong 4


//cc3000 기본 상태 출력 땜시 10부터 시작하는걸로.
#define sgnhi_OK 10 // 서버에 데이터 전송.
#define sgnhi_WAIT 11 // 서버에 전송한지 대기시간이 지나지 않음, 좀더 뒤에 전송 요망.
#define sgnhi_ERROR 12 // 웹사이트에 접속 실패.


#define MACHTYPE(v,t) (t == atInt?*(int*)v:(t == atFloat?*(float*)v:(t == atDouble?*(double*)v:(t == atLong?*(long*)v:0))))

#define REST 600000

//#define DEBUG

#ifdef DEBUG
 #define DEBUG_PRINT(x) Serial.println(x)
#else
 #define DEBUG_PRINT(x)
#endif

//

class dotori {
public:
	dotori(char *sencode);
	void printcode();
	void set(int val);
	void set(float val);
	void set(long val);
	void set(double val);
	//int request(void *val,int type);
	//
	//vvoid setvalue(void *val);
	int argType;
	int chk = 42;
	char* senCode;
	uint32_t value;
private:
	
};



class Sgnhi_CC3000 : public Adafruit_CC3000{
public:
	Sgnhi_CC3000(const char *id,const char *devcode,uint8_t csPin, uint8_t irqPin, uint8_t vbatPin, uint8_t spispeed = SPI_CLOCK_DIVIDER, Print* cc3kPrinter = CC3K_DEFAULT_PRINTER)
	: Adafruit_CC3000(csPin,irqPin,vbatPin,spispeed,cc3kPrinter),ID(id),devCode(devcode){};
	
	int send(dotori mdotori,...);
	void setRest(unsigned long rest);
private:
	const char *ID;
	const char *devCode;
	unsigned long restTime = REST;
	unsigned long sTime;
	int state = 0;
	//IPAddress addr = IPAddress(183,111,174,69);//sgnih.org 비밀번호, 호스트 를 올바로 적어주도록 하자.
	//B76FAE45
};



#endif
