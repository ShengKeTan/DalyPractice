#ifndef __LIST_H__
#define __LIST_H__

#include <cstddef>

template <class T>
class list
{
private:
	struct Node
	{
		T data;
		Node* prev;
		Node* next;
		Node(const T& x = T(), Node *p = NULL, 
			Node *n = NULL) : data(x), prev(p), next(n) {} 
	};

public:
	class const_iterator
	{
	public:
		const_iterator() : MyNode(NULL) {}

		const T& operator*() const
		{
			return (*MyNode).data;
		}
		const T* operator->() const
		{
			return &(operator*());
		}

		const_iterator& operator++()
		{
			MyNode = MyNode->next;
			return *this;
		}
		const_iterator& operator--()
		{
			MyNode = MyNode->prev;
			return *this;
		}
		const_iterator& operator++(int)
		{
			const_iterator tmp = *this;
			++(*this);
			return tmp;
		}
		const_iterator& operator--(int)
		{
			const_iterator tmp = *this;
			--(*this);
			return tmp;
		}

		bool operator==(const const_iterator& x) const { return MyNode == x.MyNode; }
		bool operator!=(const const_iterator& x) const { return MyNode != x.MyNode; }
		
	protected:
		Node* MyNode;
		const_iterator(Node *p) : MyNode(p) {}
		friend class list<T>;
	};

	class iterator : public const_iterator
	{
	public:
		iterator() {}

		T& operator*() const
		{
			return const_iterator::MyNode->data;
		}
		T* operator->() const
		{
			return &(const_iterator::MyNode->data);
		}

		iterator& operator++()
		{
			const_iterator::MyNode = const_iterator::MyNode->next;
			return *this;
		}
		iterator& operator--()
		{
			const_iterator::MyNode = const_iterator::MyNode->prev;
			return *this;
		}
		iterator& operator++(int)
		{
			iterator tmp = *this;
			++(*this);
			return tmp;
		}
		iterator operator--(int)
		{
			iterator tmp = *this;
			--(*this);
			return tmp;
		}

	protected:
		iterator(Node *p) : const_iterator(p) {}
		friend class list<T>;
	};

public:
	typedef size_t size_type;

	list()
	{
		_init();
	}
	list(const list& x)
	{
		_init();
		*this = x;
	}
	~list()
	{
		clear();
		delete head;
		delete tail;
	}

	void operator=(const list& x)
	{
		if (this != &x)
		{
			clear();
			for (const_iterator iter = x.begin(); iter != x.end(); ++iter)
			{
				push_back(*iter);
			}
		}
	}

	iterator end() const
	{
		return iterator(tail);
	}
	iterator begin() const
	{
		return iterator(head->next);
	}

	bool empty() const
	{
		return m_size == 0;
	}
	size_type size() const
	{
		return (size_type)m_size;
	}

	void clear()
	{
		while (!empty())
		{
			pop_front();
		}
	}

	T& front()
	{
		return *begin();
	}
	const T& front() const
	{
		return *begin();
	}

	T& back()
	{
		return *--end();
	}
	const T& back() const
	{
		return *--end();
	}

	void push_back(const T& x)
	{
		insert(end(), x);
	}
	void push_front(const T& x)
	{
		insert(begin(), x);
	}

	void pop_front()
	{
		erase(begin());
	}
	void pop_back()
	{
		erase(--end());
	}

	iterator insert(iterator iter, const T& x)
	{
		Node *tmp = iter.MyNode;
		tmp->prev->next = new Node(x, tmp->prev, tmp);
		tmp->prev = tmp->prev->next;

		m_size++;
		return iterator(tmp->prev);
	}

	iterator erase(iterator iter)
	{
		Node *tmp = iter.MyNode;
		iterator ret(tmp->next);
		tmp->prev->next = tmp->next;
		tmp->next->prev = tmp->prev;

		m_size--;
		delete tmp;
		return ret;

	}
	iterator erase(iterator start, iterator end)
	{
		iterator iter = start;
		while (iter != end)
		{
			iter = erase(iter);
		}
		return end;
	}

private:
	int m_size;
	Node* head;
	Node* tail;

	void _init()
	{
		m_size = 0;
		head = new Node;
		tail = new Node;
		head->next = tail;
		tail->prev = head;
	}
};
#endif //__LIST_H
