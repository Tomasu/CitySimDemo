//
// Created by moose on 30/03/19.
//

#ifndef CITYSIM_BUFFERBUILDER_H
#define CITYSIM_BUFFERBUILDER_H

#include "core/Constants.h"

#include <cstddef>

#include <QtCore/QByteArray>

template<typename T>
class BufferBuilder
{
	public:
		typedef T ItemType;
		static const size_t ItemSize = sizeof(ItemType);

		BufferBuilder() : mCurIdx(0)
		{
		}

		~BufferBuilder()
		{

		}

		T *data() const
		{
			return reinterpret_cast<ItemType *>(mByteArray.data());
		}

		void add(const T &item)
		{
			mByteArray.resize(mByteArray.size() + ItemSize);
			auto *ptr = reinterpret_cast<ItemType *>(mByteArray.data());
			ptr[mCurIdx] = item; mCurIdx++;
		}

		const QByteArray &byteArray() const
		{
			return mByteArray;
		}

	private:
		uint32_t mCurIdx;
		QByteArray mByteArray;
};

using VertexBufferBuilder = BufferBuilder<VTX_TYPE>;
using IndexBufferBuilder = BufferBuilder<IDX_TYPE>;

#endif //CITYSIM_BUFFERBUILDER_H
