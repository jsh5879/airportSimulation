/* airportSimulation.cpp - to simulate an airport landing and takeoff pattern
�й�: 201524561
�̸�: �弮ȯ
github id: jsh5879
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
int time = 0;
void generateInputData(randomInput&);//���������, �̷������ ���� ��ü���� ������ ����Ͽ� ����
int findSmallLandingQueue(Queue<LandingPlane>* landingQueue);//4���� ���� ��� queue�߿��� ���� ���� priority�� ã�´�
int findSmallTakeoffQueue(Queue<TakeoffPlane>* takeoffQueue);//3���� �̷� ��� queue�߿��� ���� ����  priority(��� �ð��� ���� �� ��)�� ã�´�
int findRunway(const UseRunway a, const UseRunway b);//���� ��� ���� Ȱ�ַ� ã��
void useRunwayLanding(UseRunway& a, UseRunway& b, int k, int id, int time); // Ȱ�ַ� ���� �Ҵ�
void useRunwayTakeoff(UseRunway& a, UseRunway& b, UseRunway& c, int k, int id, int time); //Ȱ�ַ� �̷� �Ҵ�


int main(void)
{
	int LandingTime, TakeOffTime;//runway�� ���� ó�� �ð�, �̷� ó�� �ð��� ȭ�鿡�� �Է� �޴´�- 60�� ~ 180�� ���̿� �Է� ����
	struct UseRunway useRunway1[1000], useRunway2[1000], useRunway3[1000];//�� Ȱ�ַ��� ��� ���� - [1000]�� 1000�� ������ �߻��� ����
	//generate input data by a random number
	static int indexRunway1 = 0, indexRunway2 = 0, indexRunway3 = 0;//�� Ȱ�ַ��� ��� ������ ���� next �߰��� ��ġ
	struct randomInput inputdata[1000];//������ ����Ͽ� ������ ����
	static int landingplaneID = 1;//odd ����
	static int takeoffplaneID = 0;//even ����
	static int avglandingtime = 0, avglandingcount = 0;
	static int avglandingwaiting = 0, avglandingwaitingcount = 0;
	static int avgtakeoffwaiting = 0, avgtakeoffwaitingcount = 0;
	static int nolandcount = 0;
	for (int i = 0; i < 1000; i++)
		generateInputData(inputdata[i]);//������ plane���� pool�� ����

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
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < landingQueue[i].Size(); j++) {
				if(landingQueue[i]. < 0)
			}
		}
		//ť���� ����ó������ ������� �ͺ��� ó��
		int runwayendtime; //Ȱ�ַ� ����
		/*
		Ȱ�ַ� ��� ������ �����ϴ� �ڵ� �ۼ�
		queue�� �ܿ� ���� �ð� �����ϴ� �ڵ� �ۼ�
		���� ���� ���� ��� ����� �켱 ó�� �ڵ� �ۼ�
		*/



		while (countLanding) {//���� ��� queue ó����
			struct LandingPlane landing;
			landing.arrivalTime = now;
			//now ������ ť�� Add�Ǿ� remainingFlyingTime <60�� ID�� runway ���� ó��
			bool runwayService = false;
			while (!runwayService && (landingQueue[0].Size() + landingQueue[1].Size() + landingQueue[2].Size() + landingQueue[3].Size() > 0 ) ) {//runway1,2���� landing service ó��
				for (int j = 0; j < 4; j++) {
					struct LandingPlane completeLanding;
					completeLanding = landingQueue[j].Top();
					if (landingQueue[j].Size() != 0)
					{
						if (now > completeLanding.remainingFlyingTime + completeLanding.arrivalTime) {

							//����ϴ� runway�� ����� ID, ��� �ð��� ���
							//landing�� ����� ������ Ȱ�ַθ� ���Ѵ�
							int k = findRunway(useRunway1[indexRunway1], useRunway2[indexRunway2]);
							//����� �ܿ� ��� �ð��� 60 ���ϸ� runway3�� ���
							//�ڵ� �ۼ�
							if (k == 1) {
								useRunway1[indexRunway1].takeoff_landing = false;
								useRunway1[indexRunway1].start = completeLanding.arrivalTime;
								useRunway1[indexRunway1].end = completeLanding.arrivalTime + LandingTime;
								useRunway1[indexRunway1].IDPlane = completeLanding.IDofLandingPlane;
								indexRunway1++;
							}
							else if (k == 2) {
								useRunway2[indexRunway2].takeoff_landing = false;
								useRunway2[indexRunway2].start = completeLanding.arrivalTime;
								useRunway2[indexRunway2].end = completeLanding.arrivalTime + LandingTime;
								useRunway2[indexRunway2].IDPlane = completeLanding.IDofLandingPlane;
								indexRunway2++;
							}
							else if ((k == 3) && (completeLanding.remainingFlyingTime <= 60)) {
								useRunway3[indexRunway3].takeoff_landing = false;
								useRunway3[indexRunway3].start = completeLanding.arrivalTime;
								useRunway3[indexRunway3].end = completeLanding.arrivalTime + LandingTime;
								useRunway3[indexRunway3].IDPlane = completeLanding.IDofLandingPlane;
								indexRunway3++;
							}
						}
						}
					}
					runwayService = true;
					//���� ��� queue�� �ִ� plane�� �ܿ� �ð��� ���� pop��
				}
				landing.IDofLandingPlane = landingplaneID;
				landingplaneID += 2;
				landing.remainingFlyingTime = inputdata[i].remainingFlyTime[--countLanding];
				//���� ��� queue�� ����� ���� ���� ���� find()
				int j = findSmallLandingQueue(landingQueue);
				landingQueue[j].Push(landing);
			}
		while (countTakeoff--) {//�̷� ��� queue�� ó��
			struct TakeoffPlane takeoff;
			takeoff.takeoffTime = now;
			//runway3���� takeoff service ó���ϰ� runway1,2�� ���� ���̸� �̷��� ���
			bool runwayService = false;
			//Ȱ�ַ� 3�� ���� ó����
			while (!runwayService && (takeoffQueue[0].count + takeoffQueue[1].count + takeoffQueue[2].count) > 0) {
				runwayService = false;
				for (int k = 0; k < 3; k++) {
					struct TakeoffPlane completeTakeoff;
					completeTakeoff = takeoffQueue[k].Top();
					//����ϴ� runway�� ����� ID, ��� �ð��� ���
					if (takeoffQueue[k].count != 0)
					{
						int result = findRunway(useRunway1[indexRunway1], useRunway2[indexRunway2]);
						if
							// �̷� ó�� �ڵ� �ۼ�
					}
					takeoff.IDofTakeoffPlane = takeoffplaneID;
					takeoffplaneID += 2;
					//�̷� ��� queue�� ����� ���� ���� ���� find()
					int j = findSmallTakeoffQueue(takeoffQueue);
					takeoffQueue[j].Push(takeoff);
				}
			}// ��� �̷� ��� �ð� = 00,  ��� ���� ��� �ð� = 00, ������ �ܿ� ��� ���� �ð� = 00, no fuel ������ ����� ��� == 00�� display
			cout << "�ð��� = " << now << "\t" << "��� �̷� ��� �ð� = " << avgtakeoffwaiting / avgtakeoffwaitingcount << "\t" << "��� ���� ��� �ð� = " << avglandingwaiting / avglandingwaitingcount;
			cout << "\t" << "������ �ܿ� ��� ���� �ð� = " << avglandingtime / avglandingcount << "\t";
			cout << "�������� ����� �� = " << nolandcount << "\n";
		}
		system("pause");
		return 0;
	}

	//���������, �̷������ ���� ��ü���� ������ ����Ͽ� ����
	void generateInputData(randomInput& a){
		a.timeUnit = time;
		time += rand() % 240 + 120;
		a.numPlanesLanding = rand() % 4 + 1;
		a.numPlanesTakeOff = rand() % 4 + 1;
		a.remainingFlyTime = new int[a.numPlanesLanding];
		for (int i = 0; i < a.numPlanesLanding; i++)
			a.remainingFlyTime[i] = rand() % 320 + 360;
	}

	int findRunway(const UseRunway a, const UseRunway b) //��� ���� Ȱ�ַ� ã��
	{
		if (a.end != 0)
			return 1;
		else if (b.end != 0)
			return 2;
		else
			return 3;
	}
	void useRunwayLanding(UseRunway & a, UseRunway & b, int k, int id, int time) // Ȱ�ַ� ���� �Ҵ�
	{
		if (k == 1)
		{
			a.IDPlane = id;
			a.start = time;
			a.end = 0;
			a.takeoff_landing = 1;
		}
		if (k == 2)
		{
			b.IDPlane = id;
			b.start = time;
			b.end = 0;
			b.takeoff_landing = 1;
		}
	}
	void useRunwayTakeoff(UseRunway & a, UseRunway & b, UseRunway & c, int k, int id, int time) //Ȱ�ַ� �̷� �Ҵ�
	{
		if (k == 1)
		{
			a.IDPlane = id;
			a.start = time;
			a.end = 0;
			a.takeoff_landing = 0;
		}
		if (k == 2)
		{
			b.IDPlane = id;
			b.start = time;
			b.end = 0;
			b.takeoff_landing = 0;
		}
		if (k == 3)
		{
			c.IDPlane = id;
			c.start = time;
			c.end = 0;
			c.takeoff_landing = 0;
		}
	}
	int findSmallLandingQueue(const Queue<LandingPlane> * landingQueue)//4���� ���� ��� queue�߿��� ���� ���� priority�� ã�´�
	{
		int min = landingQueue[0].count;
		for (int i = 0; i < 4; i++)
		{
			if (min > landingQueue[i].count)
				min = landingQueue[i].count;
		}
		for (int i = 0; i < 4; i++)
		{
			if (landingQueue[i].count == min)
				return i;
		}
	}
	int findSmallTakeoffQueue(const Queue<TakeoffPlane> * takeoffQueue)//3���� �̷� ��� queue�߿��� ���� ����  priority(��� �ð��� ���� �� ��)�� ã�´�
	{
		int min = takeoffQueue[0].count;
		for (int i = 0; i < 3; i++)
		{
			if (min > takeoffQueue[i].count)
				min = takeoffQueue[i].count;
		}
		for (int i = 0; i < 3; i++)
		{
			if (takeoffQueue[i].count == min)
				return i;
		}
	}