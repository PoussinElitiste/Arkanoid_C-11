#pragma once
#include <string>
#include <assert.h>
#include <functional>

namespace Event
{
	constexpr int MAX_PENDING = 16;

	using CallBack = std::function<void()>;
	using MessageId = unsigned int;

	struct Message
	{
		MessageId id;
		CallBack cb;
	};

	// Double Buffer :
	// ---------------
	// ...xxxxxxx... <- MAX_PENDING
	//    ^      ^
	//    |      |
	//   Head   Tail

	template <class T>
		struct RingBuffer
	{
		using Delegate = std::function<void(const T &)>;

		T pending_[MAX_PENDING];
		int head_ = 0;
		int tail_ = 0;

		void registerEvt(const T &message)
		{
			assert((tail_ + 1) % MAX_PENDING != head_);

			// Add to the end of the list.
			pending_[tail_] = message;
			tail_ = (tail_ + 1) % MAX_PENDING;
		}

		void forEach(const Delegate &cb)
		{
			for (int i = head_; i != tail_; i = (i + 1) % MAX_PENDING)
			{
				cb(pending_[i]);
			}
		}

		T &front()
		{
			return pending_[buffer.head_];
		}

		bool isEmpty() const
		{
			return head_ == tail_;
		}

		void incrementHead()
		{
			head_ = (head_ + 1) % MAX_PENDING;
		}
	};

	class Queue
	{
	public:
		void registerEvt(MessageId id, const CallBack &cb)
		{
			buffer.registerEvt(Message{id, cb});
		}

		void update()
		{
			if (buffer.isEmpty()) return;
			buffer.front().cb();
			buffer.incrementHead();
		}

	private:
		RingBuffer<Message> buffer;
	};

	class Broadcast
	{
	public:
		void registerEvt(MessageId id, const CallBack &cb)
		{
			buffer.registerEvt(Message{ id, cb });
		}

		void update()
		{
			buffer.forEach([](const Message & msg)
			{
				msg.cb();
			});
		}

	private:
		RingBuffer<Message> buffer;
	};
}
