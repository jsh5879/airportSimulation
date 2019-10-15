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
	int timeUnit;//3, 15, 29, 47 등으로 난수로 생성 - 이것이 queue에 Add하는 시간은 아님
	int numPlanesTakeOff;//1<<num <<6 권고함
	int numPlanesLanding;//1<<num<< 6 권고함
	int* remainingFlyTime;//착륙 비행기당 잔여 비행 가능한 시간 - 즉 대기 가능한 시간
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
	int end;//활주로 사용 종료 시간
};
const int MAXWAITINGPLANES = 3;
int generateInputData(struct randomInput*);//착륙비행기, 이륙비행기 가상 객체들을 난수를 사용하여 생성
int findSmallLandingQueue(Queue<LandingPlane>* landingQueue);//4개의 착륙 대기 queue중에서 가장 높은 priority를 찾는다
int findSmallTakeoffQueue(Queue<TakeoffPlane>* takeoffQueue);//3개의 이륙 대기 queue중에서 가장 높은  priority(대기 시간이 가장 긴 것)를 찾는다
void main()
{
	int LandingTime, TakeOffTime;
	struct UseRunway useRunway1[100], useRunway2[100], useRunway3[100];
	//generate input data by a random number
	struct randomInput inputdata[1000];
	static int landingplaneID = 1;//odd 숫자
	static int takeoffplaneID = 0;//even 숫자
	if (!generateInputData(inputdata)) {//이착륙 대기 queue를 생성
		cout << " exit" << endl;
		exit;
	}
	cout << "착륙처리 시간 입력:"; cin >> LandingTime;
	cout << "이륙처리 시간 입력:"; cin >> TakeOffTime;
	Queue<LandingPlane> landingQueue[4];//odd 숫자
	Queue<TakeoffPlane> takeoffQueue[3];//even 숫자

	for (int i = 0; i < 1000; i++) {
		int now = inputdata[i].timeUnit;
		int countLanding = inputdata[i].numPlanesLanding;
		int countTakeoff = inputdata[i].numPlanesTakeOff;
		//queue에서 pop-now 이전에 잔여 시간이 종료된 것부터 pop을 한다
		while (countLanding) {//착륙 대기 queue 처리함
			struct LandingPlane landing;
			landing.arrivalTime = now;
			//now 이전에 큐에 Add되어 remainingFlyingTime이 0인 ID를 runway 서비스 처리
			while (1) {//runway1,2에서 landing service 처리
				bool runwayService;
				do {
					runwayService = false;
					for (int k = 0; k < 3; k++) {
						struct LandingPlane completeLanding;
						completeLanding = landingQueue[k].Top();
						if (now > completeLanding.remainingFlyingTime + completeLanding.arrivalTime) {
							landing = landingQueue[k].Pop();
							//사용하는 runway에 비행기 ID, 사용 시간을 기록
							//비행기 잔여 대기 시간이 0 이하면 runway3을 사용
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
			while (1) {//runway1,2,3에서 takeoff service 처리
				bool runwayService;
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
	}

	system("pause");
}
