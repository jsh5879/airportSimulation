/* airportSimulation.cpp - to simulate an airport landing and takeoff pattern
�й�:
�̸�:
github id:
*/
/*
airport�� 3���� runway1,2,3
airplane�� 4���� ��� queue ���� �ϳ����� ����Ѵ�. runway1�� 2���� ��� queue, runway2�� ���Ͽ� 2���� ��� queue - �ִ� 3��
- arriving airplane�� 4���� ��� queue ���� �ϳ��� Add(id, ����� ���Ͽ� ��� ������ �ð�), ��������� 4���� queue�� ���̴� ���� ����ϰ� ����
-  3���� Ȱ�ַ� ������ ���Ͽ� ���� takeoff queue :  3���� takeoff queue(size�� ���� ����ϰ�) - �ִ� 3��
- �� runway�� �� time slot�� takeoff �Ǵ� landing
- runway3�� �ַ� takeoff�� ���, �� low fuel�� ��쿡�� landing(���������� ����)
- landing ��� queue�� �ִ� ������� low fuel�̸� high priority �ο��ϰ� runway3�� ���, ���� ���� ������ low fuel�� ���� �߻��ÿ� �ٸ� runway�� ���, ��
  time slot���� 3�븸 ���
  - id �ο� ���: landing�� �������� odd ����, takeoff�� even ��ȣ �ο�
- ���
	1) �� time slot�� ���¸� display - 1) �� queue�� ����, 2) ��� �̷� ��� �ð�, 3) ��� ���� ��� �ð�, 4) ������ �ܿ� ��� ���� �ð�, 5) no fuel ������ ����� ���
- �Է�: ���� ���, �� time slot���� �̷� queue�� �����ϴ� ����� ���, ���� queue�� �����ϴ� ����� ���, ���� queue�� �����ϴ� ������� �ܿ� ���� ���� �ð�
*/
#include "templatequeue.h"
struct randomInput {
	int timeUnit;
	int numPlanesTakeOff;
	int numPlanesLanding;
	int* remainingFlyTime;
};
struct LandingPlane{
	int arrivalTime;
int IDofLandingPlane;
int remainingFlyingTime;
};
struct TakeoffPlane {
	int takeoffTime;
	int IDofTakeoffPlane;
};
int generateInputData(struct randomInput *);
int findSmallLandingQueue(Queue<LandingPlane>* landingQueue);
int findSmallTakeoffQueue(Queue<TakeoffPlane>* takeoffQueue);
void main()
{
	int landingTime, takeoffTime;
	//generate input data by a random number
	struct randomInput inputdata[1000];
	static int landingplaneID = 1;//odd ����
	static int takeoffplaneID = 0;//even ����
	if (!generateInputData(inputdata)) {
		cout << " exit" << endl;
		exit;
	}
	cout << "����ó�� �ð� �Է�:"; cin >> landingTime;
	cout << "�̷�ó�� �ð� �Է�:"; cin >> takeoffTime;
	Queue<LandingPlane> landingQueue[4];//odd ����
	Queue<TakeoffPlane> takeoffQueue[3];//even ����

	for (int i = 0; i < 1000; i++) {
		int now = inputdata[i].timeUnit;
		int countLanding = inputdata[i].numPlanesLanding;
		int countTakeoff = inputdata[i].numPlanesTakeOff;
		//queue���� pop-now ������ �ܿ� �ð��� ����� �ͺ��� pop�� �Ѵ�
		while (countLanding) {
			struct LandingPlane landing;
			landing.arrivalTime = now;
			//now ������ ť�� Add�Ǿ� remainingFlyingTime�� 0�� ID�� runway ���� ó��
			while (1) {//runway1,2���� landing service ó��
				int k = findLandingQueue(landingQueue);
				if (k == 0 || k == 1) check runway1 => struct or class ��� ���θ� �����ϴ� ���� �ʿ�
				else check runway2
				landing = landingQueue[k].Pop();
			}
			landing.IDofLandingPlane = landingplaneID++;
			landing.remainingFlyingTime = inputdata[i].remainingFlyTime[countLanding--];
			//���� ��� queue�� ����� ���� ���� ���� find()
			int j = findSmallLandingQueue(landingQueue);
			landingQueue[j].Push(landing);
		}
		while (countTakeoff--) {
			struct TakeoffPlane takeoff;
			takeoff.takeoffTime = now;
			while (1) {//runway1,2,3���� takeoff service ó��
				int k = findLandingQueue(landingQueue);
				if (k == 0 ) check runway1 = > struct or class ��� ���θ� �����ϴ� ���� �ʿ�
				else (k == 1) check runway2
					else check runway3
		takeoff = takeoffQueue[k].Pop();
			}
			takeoff.IDofTakeoffPlane = takeoffplaneID++;
			//�̷� ��� queue�� ����� ���� ���� ���� find()
			int j = findSmallTakeoffQueue(takeoffQueue);
			takeoffQueue[j].Push(takeoff);
		}
	}

	system("pause");
}
