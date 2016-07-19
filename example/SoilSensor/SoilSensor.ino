#include <SGN_Arduino_CC3000.h>

Sgnhi_CC3000 dev = Sgnhi_CC3000("사용자 ID","장치코드",10, 7, 5);

dotori soil("센서코드");

void setup(void)
{
  Serial.begin(9600);

  if (!dev.begin())
  {
    Serial.println("Fail");
    while(1);
  }
  
  if (!dev.connectToAP("와이파이이름","와이파이 비밀번호",WLAN_SEC_WPA2)) {
    Serial.println("Fail");
    while(1);
  }

  dev.setRest((unsigned long)1000 * 60 * 60);//업데이트 주기를 설정한다. (1000 = 1초) * 60 * 60 = 1시간 
}

void loop(void)
{
  soil.set(analogRead(A0));//수분센서값을 읽어들인다.
  dev.send(soil);//센서값을 전송한다.
  delay(1000);
}
