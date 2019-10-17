/* airportSimulation.cpp - to simulate an airport landing and takeoff pattern
학번:
이름:
github id:
*/
/*
airport에 3개의 runway1,2,3
airplane는 4개의 대기 queue 중의 하나에서 대기한다. runway1에 2개의 대기 queue, runway2에 대하여 2개의 대기 queue - 각 queue는 최대 3대까지 대기 가능
- arriving airplane는 4개의 대기 queue 중의 하나에 Add(id, 연료로 인하여 대기 가능한 시간), 평균적으로 4개의 queue의 길이는 거의 비슷하게 유지
-  3개의 활주로 각각에 대하여 각각 takeoff queue :  3개의 takeoff queue(size는 거의 비슷하게) - 최대 3대
- 각 runway는 각 time slot에 takeoff 또는 landing
- runway3은 주로 takeoff에 사용, 단 low fuel일 경우에는 landing(예외적으로 적용)
- landing 대기 queue에 있는 비행기의 low fuel이면 high priority 부여하고 runway3을 사용, 일정 수준 이하의 low fuel이 동시 발생시에 다른 runway를 사용, 각
  time slot에서 3대만 허용
  - id 부여 방법: landing은 연속적인 odd 숫자, takeoff는 even 번호 부여
- 출력
	1) 각 time slot의 상태를 display - 1) 각 queue의 내용, 2) 평균 이륙 대기 시간, 3) 평균 착륙 대기 시간, 4) 착륙시 잔여 평균 비행 시간, 5) no fuel 상태의 비행기 댓수
- 입력: 난수 사용, 각 time slot에서 이륙 queue에 도달하는 비행기 댓수, 착륙 queue에 도달하는 비행기 댓수, 착륙 queue에 도달하는 비행기의 잔여 비행 가능 시간
*/
#include "queue.h"
struct randomInput {//난수를 사용하여 생성한다
	int timeUnit;//60, 240, 480 등의 난수로서 120초 ~ 360초 간격으로 생성 - 이것이 queue에 Add하는 시간은 아님
	//timeUnit은 난수이나 증가하는 sequence이며 그 간격은120 ~ 360 초 간격을 권고함
	int numPlanesTakeOff;//1<<num <<4 권고함
	int numPlanesLanding;//1<<num<< 4 권고함
	int* remainingFlyTime;//착륙 비행기당 잔여 비행 가능한 시간 - 즉 대기 가능한 시간 240 ~ 560초 사이를 생성
};
struct LandingPlane {
	int arrivalTime;//timeUnit으로 설정됨
	int IDofLandingPlane;
	int remainingFlyingTime;//대기 가능한 시간
};
struct TakeoffPlane {
	int takeoffTime;//timeUnit으로 설정됨
	int IDofTakeoffPlane;
};
struct UseRunway {
	bool takeoff_landing;//이착륙 구분 태그
	int IDPlane;
	int start;//활주로 사용 시작 시간
	int end;//활주로 사용 종료 시간, end= -1이면 비사용중, end = 0이면 사용중, end >1이면 사용 종료, 초기화는 모든 array element에 대하여 -1로 초기화 설정
};
const int MAXWAITINGPLANES = 3;
int generateInputData(struct randomInput*);//착륙비행기, 이륙비행기 가상 객체들을 난수를 사용하여 생성
int findSmallLandingQueue(Queue<LandingPlane>* landingQueue);//4개의 착륙 대기 queue중에서 가장 높은 priority를 찾는다
int findSmallTakeoffQueue(Queue<TakeoffPlane>* takeoffQueue);//3개의 이륙 대기 queue중에서 가장 높은  priority(대기 시간이 가장 긴 것)를 찾는다
void main()
{
	int LandingTime, TakeOffTime;//runway의 착륙 처리 시간, 이륙 처리 시간을 화면에서 입력 받는다- 60초 ~ 180초 사이에 입력 권장
	struct UseRunway useRunway1[100], useRunway2[100], useRunway3[100];//각 활주로의 사용 내역 - [100]은 100건 이착륙 발생을 말함
	//generate input data by a random number
	static int indexRunway1 = 0, indexRunway2 = 0, indexRunway3 = 0;//각 활주로의 사용 내역에 대한 next 추가할 위치
	struct randomInput inputdata[1000];//난수를 사용하여 데이터 생성
	static int landingplaneID = 1;//odd 숫자
	static int takeoffplaneID = 0;//even 숫자
	generateInputData(inputdata); {//이착륙 plane들의 pool을 생성

	cout << "착륙처리 시간 입력:"; cin >> LandingTime;
	cout << "이륙처리 시간 입력:"; cin >> TakeOffTime;
	Queue<LandingPlane> landingQueue[4];//odd 숫자, 2개의 활주로에 대하여 4개의 착륙 대기queue를 만듬, 대기 queue1,2번은 runway1에 우선 사용
	Queue<TakeoffPlane> takeoffQueue[3];//even 숫자, 3개의 활주로에 대하여 3개의 이륙 대기 queue를 사용, runway3을 주로 이륙용으로 사용
	//runway3은 주로 이륙용이나 runway1,2가 모두 착륙중이고 연료 대기 시간 < 60인 착륙 대기 비행기가 있을 때 runway3을 착륙용으로 사용
	//runway1,2에 착륙 대기 비행기가 없을 때는 이륙용으로 사용
	for (int i = 0; i < 1000; i++) {//1000개의 random pool에 대하여 시간별로 처리
		int now = inputdata[i].timeUnit;
		int countLanding = inputdata[i].numPlanesLanding;
		int countTakeoff = inputdata[i].numPlanesTakeOff;
		//queue에서 pop-now 이전에 잔여 시간이 종료된 것부터 pop을 한다
		while (countLanding) {//착륙 대기 queue 처리함
			struct LandingPlane landing;
			landing.arrivalTime = now;
			//now 이전에 큐에 Add되어 remainingFlyingTime <60인 ID를 runway 서비스 처리
			while (1) {//runway1,2에서 landing service 처리
				bool runwayService;
				do {
					runwayService = false;
					for (int k = 0; k < 3; k++) {
						struct LandingPlane completeLanding;
						completeLanding = landingQueue[k].Top();
						if (now > completeLanding.remainingFlyingTime + completeLanding.arrivalTime) {

							//사용하는 runway에 비행기 ID, 사용 시간을 기록
							//landing에 사용이 가능한 활주로를 구한다
							k = findRunway();
							//비행기 잔여 대기 시간이 60 이하면 runway3을 사용
							landing = landingQueue[k].Pop();
							useRunway(k);//k-th runWay를 사용 처리
							cout << landing;
							runwayService = true;
						}
					}
				} while (!runwayService);//착륙 대기 queue에 있는 plane의 잔여 시간을 보고 pop함
			}
			landing.IDofLandingPlane = landingplaneID++;
			landing.remainingFlyingTime = inputdata[i].remainingFlyTime[countLanding--];
			//착륙 대기 queue의 사이즈가 가장 적을 것을 find()
			int j = findSmallLandingQueue(landingQueue);
			landingQueue[j].Push(landing);
		}
		while (countTakeoff--) {//이륙 대기 queue를 처리
			struct TakeoffPlane takeoff;
			takeoff.takeoffTime = now;
			while (1) {//runway3에서 takeoff service 처리하고 runway1,2가 비사용 중이면 이륙에 사용
				bool runwayService;//활주로 3에 대한 처리임
				result = checkRunwayOneTwo();
				if (result == 0 || result == 1)//result가 0, 1이면 runway0 , runway1이 이륙용으로 사용
				{
					runway1,2가 비어 있고 이륙이 사용 가능한 경우에 이륙 처리한다.
				}
				do {
					runwayService = false;
					for (int k = 0; k < 3; k++) {
						struct LandingPlane completeTakeoff;
						completeTakeoff = takeoffQueue[k].Top();
						if (now > completeTakeoff.remainingFlyingTime + completeTakeoff.arrivalTime) {
							takeoff = takeoffQueue[k].Pop();
							//사용하는 runway에 비행기 ID, 사용 시간을 기록
							cout << takeoff;
							runwayService = true;
						}
					}
				} while (!runwayService);//착륙 대기 queue에 있는 plane의 잔여 시간을 보고 pop함
			}
			takeoff.IDofTakeoffPlane = takeoffplaneID++;
			//이륙 대기 queue의 사이즈가 가장 적을 것을 find()
			int j = findSmallTakeoffQueue(takeoffQueue);
			takeoffQueue[j].Push(takeoff);
		}
	}// 평균 이륙 대기 시간 = 00,  평균 착륙 대기 시간 = 00, 착륙시 잔여 평균 비행 시간 = 00, no fuel 상태의 비행기 댓수 == 00을 display

	system("pause");
}
