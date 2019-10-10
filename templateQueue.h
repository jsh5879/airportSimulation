#pragma once
#include <iostream>
#include <memory>
#include "templatebag.h"

template <class T>
class Queue : public Bag<T>
{
public:
	Queue(int queueCapacity = 10);
	bool IsEmpty();
	T& Front();
	T& Rear();
	void Push(T const& x);
	T& Pop();
private:
	int front;
	int rear;
};
