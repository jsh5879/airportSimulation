#include <iostream>
#include <memory>
#include "bag.h"
//Visual Studio2019에서 template 사용시에 bag.h, bag.cpp로 분리하지 않아야 함
using namespace std;
#define MaxSize 100
template <class T>
class Bag {
public:
	Bag(int bagCapacity = 10);
	~Bag();
	bool IsFull();
	virtual int Size() const;
	bool IsEmpty() const;
	virtual T& Pop();
	virtual void Push(const T&);
private:
	T* array;
	int capacity;
	int top;
};
template <class T>
class Queue : public Bag<T>
{
public:
	Queue(int queueCapacity = 10);
	bool IsEmpty();
	T& Front();
	T& Rear();
	void Push(T const& x, int time);
	T& Pop();
	int Size() const;
	T& Top();
private:
	int front;
	int rear;
	int time;
};

template <class T>
Bag<T>::Bag(int bagCapacity) : capacity(bagCapacity)
{
	if (capacity < 1) throw "Capacity must be > 0";
	array = new T[capacity];
	top = -1;
}

template <class T>
Bag<T>::~Bag() { delete[] array; }

template <class T>
void ChangeSizeID(T*& a, const int oldSize, const int newSize)
{
	if (newSize < 0) throw "New length must be >= 0";

	T* temp = new T[newSize];
	int number = oldSize;
	if (oldSize > newSize) number = newSize;
	//copy(a, a + number, temp);
	memcpy(temp, a, number);
	delete[] a;
	a = temp;
}

template <class T>
int Bag<T>::Size() const {
	return top + 1;
}

template <class T>
bool Bag<T>::IsEmpty() const {
	return top < 0;
}

template <class T>
void Bag<T>::Push(const T& x)
{
	if (top == capacity - 1)
		// 현재 버젼은 ordering 상태에서 push한다. non-ordering되게 push가 가능하게 수정
	{
		ChangeSizeID(array, capacity, 2 * capacity);
		capacity *= 2;
	}
	array[++top] = x;
}

template <class T>
T& Bag<T>::Pop()
{
	T retValue;
	if (IsEmpty()) throw "Bag is empty, cannot delete";
	int deletePos = top / 2;
	retValue = array[deletePos];
	// 실습 사항: no ordering상태로 pop되게 수정
	//copy(array + deletePos + 1, array + top + 1, array + deletePos);
	memcpy(array + deletePos, array + deletePos, top + 1);
	top--;
	return retValue;
}

template <class T>
inline bool Bag<T>::IsFull()
{
	if (top == MaxSize - 1) return TRUE;
	else return FALSE;
}

template <class T>
Queue<T>::Queue(int queueCapacity) : capacity(queueCapacity)
{
	queue = new T[capacity];
	front = rear = -1;
}

template <class T>
inline bool  Queue<T>::IsEmpty() { return front == rear; }

template <class T>
inline T& Queue<T>::Front()
{
	if (IsEmpty()) throw "Queue is empty. No front element";
	return queue[(front + 1) % capacity];
}

template <class T>
inline T& Queue<T>::Rear()
{
	if (IsEmpty()) throw "Queue is empty. No rear element";
	return queue[rear];
}

// doubling queue capacity
template <class T>
void Queue<T>::Push(T const& x, int _time)
{
	if ((rear + 1) % capacity == front)
	{
		T* newQueue = new T[2 * capacity];
		// allocate an array with twice the capacity
		int start = (front + 1) % capacity;
		if (start < 2)
			//copy(queue + start, queue + start + capacity - 1, newQueue);
			memcpy(newQueue, queue + start, start + capacity - 1);
		else
		{
			//copy(queue + start, queue + capacity, newQueue);
			//copy(queue, queue + rear + 1, newQueue + capacity - start);
			memcpy(newQueue, queue + start, capacity);
			memcpy(newQueue + capacity - start, queue, rear + 1);
		}
		// switch to new Queue
		front = 2 * capacity - 1;
		rear = capacity - 2;
		capacity *= 2;
		delete[] queue;
		queue = newQueue;
	}
	time = _time;
	rear = (rear + 1) % capacity;
	queue[rear] = x;
}

template <class T>
T & Queue<T>::Pop()
{
	T retValue;
	if (IsEmpty()) throw "Queue is empty. Cannot delelte.";
	retValue = array[front];
	front = (front + 1) % capacity;
	return retValue;
}

template<class T>
inline int Queue<T>::Size() const
{
	return rear - front;
}

template<class T>
inline T& Queue<T>::Top()
{
	int min = queue[(front + 1 % capacity].time;
	int minIndex = (front + 1) % capacity;
	for (int i = 0; i < size(); i++) {
		if (queue[(front + 1 + i) % capacity].time < min) {
			min = queue[(front + 1 + i) % capacity].time;
			minIndex = (front + 1 + i) % capacity;
		}
	}
	return queue[minIndex];
}
