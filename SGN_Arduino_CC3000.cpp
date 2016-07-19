/*
 * CC3000.h class 구현
 */
#include "SGN_Arduino_CC3000.h"

dotori::dotori(char *sencode){
	senCode = sencode;
}

void dotori::printcode(){
	DEBUG_PRINT(senCode);
}

//입력 변수형별 전송 데이터 타입 정리.
void dotori::set(int val){
	void * vo = &val;
	value = *(uint32_t*)vo;
	argType = atInt;
}

void dotori::set(float val){
	void * vo = &val;
	value = *(uint32_t*)vo;
	argType = atFloat;
}

void dotori::set(long val){
	void * vo = &val;
	value = *(uint32_t*)vo;
	argType = atLong;
}

void dotori::set(double val){
	void * vo = &val;
	value = *(uint32_t*)vo;
	argType = atDouble;
}


void Sgnhi_CC3000::setRest(unsigned long rest){
	restTime = rest < REST? REST:rest;
}


int Sgnhi_CC3000::send(dotori mdotori, ...){//iot_up 소스코드 수정해야함 -> 수정완료.
	//return 1;
	//send value code 아래쪽 부터.
	unsigned long now = millis();

	int result = 0;
	if(state != 0){
		if(now <= sTime){
			unsigned long lastTime = 0xffffffff - sTime;
			if((lastTime + now < restTime)){
				return sgnhi_WAIT;
			}
		}else if(now - sTime < restTime){
			return sgnhi_WAIT;
		}
	}

	//여기서 와이파이 모듈의 상태나 네트워크 연결 상태를 확인할 필요가 있음.
	int now_status = (int)getStatus();
	//Serial.println(now_status);//테스트
	if(now_status != 3){//확인후 적절히 동작을 취하도록 함.
		return now_status;//일단 연결 상태가 아니면 ㅂㅂ 하도록 함.
	}
	Adafruit_CC3000_Client client = connectTCP(0xB76FAE45,80);//클라이언트 객체 새로 만들필요 없이 여기서 바로 끝장...!
	//후.. 아다후루츠 cc3000의 fastprint는 char *만 출력가능. 아래 의 패킷 전체를 String 으로 변형 후, 다시 char array로 변형하도록 하거나 아님... sprintf 로문자열로 변경..
	String sgnhi_packet;//스근하이 패킷 생성.
	sgnhi_packet = "GET /iot/iot_up.php?";
	sgnhi_packet += "uid=" + String(ID);
	sgnhi_packet += "&dc=" + String(devCode);
	int cnt = 0;
	va_list vl;
	va_start(vl,mdotori);
	for(dotori m = mdotori;m.chk == 42;m= va_arg(vl,dotori)){
		//uint32_t value = m.value;//어... 이거 없어도 되는거 아닌가? 왜 여기있지..?
		void * vo = &m.value;
		sgnhi_packet += "&sc" + String(cnt) + "=" + String(m.senCode);
		sgnhi_packet += "&sv" + String(cnt) + "=" + String(MACHTYPE(vo,m.argType));
		cnt++;
	}
	va_end(vl);

	sgnhi_packet += " HTTP/1.1\r\n";
	sgnhi_packet += "Host:sgnhi.org \r\n";
	sgnhi_packet += "User-Agent: sgnhi\r\n";
	sgnhi_packet += "Connection: close\r\n\r\n";

	if (client.connected()) {
		DEBUG_PRINT("connected");
		int packetLength = sgnhi_packet.length();
		for(int i = 0;i < packetLength;i++){
			client.write(sgnhi_packet.charAt(i));
			//Serial.write(sgnhi_packet.charAt(i));
			//새로운 char문자열을 만드는것보다는 메모리를 적게 쓰리라 믿음.
		}
		state = 1;
		unsigned long lastRead = millis();
		sgnhi_packet = "";
		while (client.connected() && (millis() - lastRead < 1000)) {
		    while (client.available()) {
		    	//Serial.write(client.read());
			    sgnhi_packet += (char)client.read();
				if(sgnhi_packet.endsWith("SGNHI0")){
			      	result = 1;
				}
				if(sgnhi_packet.endsWith("SGNHI1")){
					result = 0;
				}
		      	lastRead = millis();
		    }
		}
		client.stop();
		sTime = now;
	}
	else {
		//Serial.println(client.status());
		client.stop();
		while(!checkDHCP()){
			delay(100);
		}
  		DEBUG_PRINT("connection failed");
  		DEBUG_PRINT("try to begin");
  		//init();
  		state = 0;
  		return sgnhi_ERROR;
  	}
  	return result == 1?sgnhi_OK:sgnhi_ERROR;
}
