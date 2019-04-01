//
// Created by moose on 30/03/19.
//

#ifndef CITYSIM_BUFFERBUILDER_H
#define CITYSIM_BUFFERBUILDER_H

#include <QtCore/QByteArray>

template<typename T>
class BufferBuilder
{
	public:
		typedef T ItemType;
		static const size_t ItemSize = sizeof(ItemType);

		BufferBuilder();
		~BufferBuilder();

		void add(const T &item);

		T *data();
		QByteArray &byteArray();

	private:
		uint32_t mCurIdx;
		QByteArray mByteArray;
};

template< typename T >
void BufferBuilder<T>::add(const T &item)
{
	mByteArray.resize(mByteArray.size() + ItemSize);
	auto *ptr = reinterpret_cast<ItemType *>(mByteArray.data());
	ptr[mCurIdx] = item; mCurIdx++;
}

template< typename T >
BufferBuilder<T>::~BufferBuilder()
{

}

template< typename T >
BufferBuilder<T>::BufferBuilder() : mCurIdx(0)
{

}

template< typename T >
T *BufferBuilder<T>::data()
{
	return reinterpret_cast<ItemType *>(mByteArray.data());
}

template< typename T >
QByteArray &BufferBuilder<T>::byteArray()
{
	return mByteArray;
}


#endif //CITYSIM_BUFFERBUILDER_H
