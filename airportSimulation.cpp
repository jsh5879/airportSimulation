/* airportSimulation.cpp - to simulate an airport landing and takeoff pattern
�й�:
�̸�:
github id:
*/
/*
airport�� 3���� runway1,2,3
airplane�� 4���� ��� queue ���� �ϳ����� ����Ѵ�. runway1�� 2���� ��� queue, runway2�� ���Ͽ� 2���� ��� queue - �� queue�� �ִ� 3����� ��� ����
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
#include "queue.h"
struct randomInput {//������ ����Ͽ� �����Ѵ�
	int timeUnit;//60, 240, 480 ���� �����μ� 120�� ~ 360�� �������� ���� - �̰��� queue�� Add�ϴ� �ð��� �ƴ�
	//timeUnit�� �����̳� �����ϴ� sequence�̸� �� ������120 ~ 360 �� ������ �ǰ���
	int numPlanesTakeOff;//1<<num <<4 �ǰ���
	int numPlanesLanding;//1<<num<< 4 �ǰ���
	int* remainingFlyTime;//���� ������ �ܿ� ���� ������ �ð� - �� ��� ������ �ð� 240 ~ 560�� ���̸� ����
};
struct LandingPlane {
	int arrivalTime;//timeUnit���� ������
	int IDofLandingPlane;
	int remainingFlyingTime;//��� ������ �ð�
};
struct TakeoffPlane {
	int takeoffTime;//timeUnit���� ������
	int IDofTakeoffPlane;
};
struct UseRunway {
	bool takeoff_landing;//������ ���� �±�
	int IDPlane;
	int start;//Ȱ�ַ� ��� ���� �ð�
	int end;//Ȱ�ַ� ��� ���� �ð�, end= -1�̸� ������, end = 0�̸� �����, end >1�̸� ��� ����, �ʱ�ȭ�� ��� array element�� ���Ͽ� -1�� �ʱ�ȭ ����
};
const int MAXWAITINGPLANES = 3;
int generateInputData(struct randomInput*);//���������, �̷������ ���� ��ü���� ������ ����Ͽ� ����
int findSmallLandingQueue(Queue<LandingPlane>* landingQueue);//4���� ���� ��� queue�߿��� ���� ���� priority�� ã�´�
int findSmallTakeoffQueue(Queue<TakeoffPlane>* takeoffQueue);//3���� �̷� ��� queue�߿��� ���� ����  priority(��� �ð��� ���� �� ��)�� ã�´�
void main()
{
	int LandingTime, TakeOffTime;//runway�� ���� ó�� �ð�, �̷� ó�� �ð��� ȭ�鿡�� �Է� �޴´�- 60�� ~ 180�� ���̿� �Է� ����
	struct UseRunway useRunway1[100], useRunway2[100], useRunway3[100];//�� Ȱ�ַ��� ��� ���� - [100]�� 100�� ������ �߻��� ����
	//generate input data by a random number
	static int indexRunway1 = 0, indexRunway2 = 0, indexRunway3 = 0;//�� Ȱ�ַ��� ��� ������ ���� next �߰��� ��ġ
	struct randomInput inputdata[1000];//������ ����Ͽ� ������ ����
	static int landingplaneID = 1;//odd ����
	static int takeoffplaneID = 0;//even ����
	generateInputData(inputdata); {//������ plane���� pool�� ����

	cout << "����ó�� �ð� �Է�:"; cin >> LandingTime;
	cout << "�̷�ó�� �ð� �Է�:"; cin >> TakeOffTime;
	Queue<LandingPlane> landingQueue[4];//odd ����, 2���� Ȱ�ַο� ���Ͽ� 4���� ���� ���queue�� ����, ��� queue1,2���� runway1�� �켱 ���
	Queue<TakeoffPlane> takeoffQueue[3];//even ����, 3���� Ȱ�ַο� ���Ͽ� 3���� �̷� ��� queue�� ���, runway3�� �ַ� �̷������� ���
	//runway3�� �ַ� �̷����̳� runway1,2�� ��� �������̰� ���� ��� �ð� < 60�� ���� ��� ����Ⱑ ���� �� runway3�� ���������� ���
	//runway1,2�� ���� ��� ����Ⱑ ���� ���� �̷������� ���
	for (int i = 0; i < 1000; i++) {//1000���� random pool�� ���Ͽ� �ð����� ó��
		int now = inputdata[i].timeUnit;
		int countLanding = inputdata[i].numPlanesLanding;
		int countTakeoff = inputdata[i].numPlanesTakeOff;
		//queue���� pop-now ������ �ܿ� �ð��� ����� �ͺ��� pop�� �Ѵ�
		while (countLanding) {//���� ��� queue ó����
			struct LandingPlane landing;
			landing.arrivalTime = now;
			//now ������ ť�� Add�Ǿ� remainingFlyingTime <60�� ID�� runway ���� ó��
			while (1) {//runway1,2���� landing service ó��
				bool runwayService;
				do {
					runwayService = false;
					for (int k = 0; k < 3; k++) {
						struct LandingPlane completeLanding;
						completeLanding = landingQueue[k].Top();
						if (now > completeLanding.remainingFlyingTime + completeLanding.arrivalTime) {

							//����ϴ� runway�� ����� ID, ��� �ð��� ���
							//landing�� ����� ������ Ȱ�ַθ� ���Ѵ�
							k = findRunway();
							//����� �ܿ� ��� �ð��� 60 ���ϸ� runway3�� ���
							landing = landingQueue[k].Pop();
							useRunway(k);//k-th runWay�� ��� ó��
							cout << landing;
							runwayService = true;
						}
					}
				} while (!runwayService);//���� ��� queue�� �ִ� plane�� �ܿ� �ð��� ���� pop��
			}
			landing.IDofLandingPlane = landingplaneID++;
			landing.remainingFlyingTime = inputdata[i].remainingFlyTime[countLanding--];
			//���� ��� queue�� ����� ���� ���� ���� find()
			int j = findSmallLandingQueue(landingQueue);
			landingQueue[j].Push(landing);
		}
		while (countTakeoff--) {//�̷� ��� queue�� ó��
			struct TakeoffPlane takeoff;
			takeoff.takeoffTime = now;
			while (1) {//runway3���� takeoff service ó���ϰ� runway1,2�� ���� ���̸� �̷��� ���
				bool runwayService;//Ȱ�ַ� 3�� ���� ó����
				result = checkRunwayOneTwo();
				if (result == 0 || result == 1)//result�� 0, 1�̸� runway0 , runway1�� �̷������� ���
				{
					runway1,2�� ��� �ְ� �̷��� ��� ������ ��쿡 �̷� ó���Ѵ�.
				}
				do {
					runwayService = false;
					for (int k = 0; k < 3; k++) {
						struct LandingPlane completeTakeoff;
						completeTakeoff = takeoffQueue[k].Top();
						if (now > completeTakeoff.remainingFlyingTime + completeTakeoff.arrivalTime) {
							takeoff = takeoffQueue[k].Pop();
							//����ϴ� runway�� ����� ID, ��� �ð��� ���
							cout << takeoff;
							runwayService = true;
						}
					}
				} while (!runwayService);//���� ��� queue�� �ִ� plane�� �ܿ� �ð��� ���� pop��
			}
			takeoff.IDofTakeoffPlane = takeoffplaneID++;
			//�̷� ��� queue�� ����� ���� ���� ���� find()
			int j = findSmallTakeoffQueue(takeoffQueue);
			takeoffQueue[j].Push(takeoff);
		}
	}// ��� �̷� ��� �ð� = 00,  ��� ���� ��� �ð� = 00, ������ �ܿ� ��� ���� �ð� = 00, no fuel ������ ����� ��� == 00�� display

	system("pause");
}
