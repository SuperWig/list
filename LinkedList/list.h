#pragma once
#include <type_traits>
#include <iterator>


template<class T>
class list //TODO define copy and move assign ctor/assign
{
	using reference = T&;
	using const_reference = const T&;
	using pointer = T*;
	struct node;
	struct node_base
	{
		node_base() : prev(this), next(this) {}
		node_base(node_base* p, node_base* n) : prev(p), next(n) {}		
		node* as_node() { return static_cast<node*>(this); }

		node_base* prev;
		node_base* next;
	};
	struct node : node_base
	{
		node(node_base* prev, node_base* next, const T& val) : node_base{ prev, next }, data(val) {}
		node(node_base* prev, node_base* next, T&& val) : node_base{ prev, next }, data(std::move(val)) {}
		template<typename...Args>
		node(node_base* prev, node_base* next, Args&&... val) : node_base{ prev, next }, data(std::forward<Args>(val)...) {}
		T data;
	};
	template<class T>
	node* add_node(node_base* prev, node_base* next, T&& val)
	{
		auto* new_node = new node(prev, next, std::forward<T>(val));
		new_node->next->prev = new_node;
		new_node->prev->next = new_node;
		++size_;
		return new_node;
	}
	template<class T, class ...Args>
	node* add_node(node_base* prev, node_base* next, Args&&... val)
	{
		auto* new_node = new node(prev, next, std::forward<Args>(val)...);
		new_node->next->prev = new_node;
		new_node->prev->next = new_node;
		++size_;
		return new_node;
	}
	void delete_node(node_base* node);

	node_base end_;
	std::size_t	size_;
public:
	list() : end_{}, size_(0) {}
	~list();

	struct iterator;
	struct const_iterator;
	iterator begin() noexcept { return end_.next; }
	const_iterator begin() const noexcept { return end_.next; }
	const_iterator cbegin() const noexcept { return end_.next; }
	iterator end() noexcept { return &end_; }
	const_iterator end() const noexcept { return const_cast<node_base*>(&end_); }
	const_iterator cend() const noexcept { return const_cast<node_base*>(&end_); }

	reference front();
	const_reference front() const;
	reference back();
	const_reference back() const;

	std::size_t size() const noexcept { return size_; }
	[[nodiscard]]
	bool empty() const { return !size_; }

	void push_back(const T& val);
	void push_back(T&& val);
	void push_front(const T& val);
	void push_front(T&& val);
	iterator insert(const_iterator pos, const T& val);
	iterator insert(const_iterator pos, T&& val);

	template<class... Args>
	reference emplace_back(Args&&... args); 
	template<class... Args>
	reference emplace_front(Args&&... args);
	template<class... Args>
	iterator emplace(const_iterator pos, Args&&... args);
};

template<typename T>
struct list<T>::iterator
{
	friend class list;
	using self_type = iterator;
	using value_type = T;
	using reference = T&;
	using pointer = T*;
	using iterator_category = std::bidirectional_iterator_tag;
	using difference_type = int;
	iterator(node_base* node) : current_(node) {}
	reference operator*() { return current_->as_node()->data; }
	pointer operator->() { return &current_->as_node()->data; }
	self_type operator++() { self_type cur = current_; current_ = current_->next; return cur; }
	self_type operator++(int) { return current_->next; }
	bool operator==(const self_type& rhs) { return current_ == rhs.current_; }
	bool operator!=(const self_type& rhs) { return current_ != rhs.current_; }
private:
	node_base* current_;
};
template<typename T>
struct list<T>::const_iterator
{
	friend class list;
	using self_type = const_iterator;
	using value_type = T;
	using reference = const T&;
	using pointer = const T*;
	using iterator_category = std::bidirectional_iterator_tag;
	using difference_type = int;
	const_iterator(node_base* node) : current_(node) {}
	reference operator*() { return current_->as_node()->data; }
	pointer operator->() { return &current_->as_node()->data; }
	self_type operator++() { self_type cur = current_; current_ = current_->next; return cur; }
	self_type operator++(int) { return current_->next; }
	bool operator==(const self_type& rhs) { return current_ == rhs.current_; }
	bool operator!=(const self_type& rhs) { return current_ != rhs.current_; }
private:
	node_base* current_;
};

template <class T>
typename list<T>::reference list<T>::front()
{
	return end_.next->as_node()->data;
}

template <class T>
typename list<T>::const_reference list<T>::front() const
{
	return end_.next->as_node()->data;
}

template <class T>
typename list<T>::reference list<T>::back()
{
	return end_.prev->as_node()->data;
}

template <class T>
typename list<T>::const_reference list<T>::back() const
{
	return end_.prev->as_node()->data;
}

template <class T>
list<T>::~list()
{
	for(node_base* next = end_.next; next != &end_; )
	{
		node* del = static_cast<node*>(next);
		next = next->next;
		delete del;
	}
}

template <class T>
void list<T>::delete_node(node_base* node)
{
	node->prev->next = node->next;
	node->next->prev = node->prev;
	delete node;
	--size_;
}

template<class T>
void list<T>::push_back(const T& val)
{
	add_node(end_.prev, &end_, val);
}

template<class T>
void list<T>::push_back(T&& val)
{
	add_node(end_.prev, &end_, std::move(val));
}

template <class T>
void list<T>::push_front(const T& val)
{
	add_node(&end_, end_.next, val);
}

template <class T>
void list<T>::push_front(T&& val)
{
	add_node(&end, end_.next, val);
}
template <class T>
typename list<T>::iterator list<T>::insert(const_iterator pos, const T& val)
{
	return add_node(pos.current_->prev, pos.current_, std::move(val));
}

template <class T>
typename list<T>::iterator list<T>::insert(const_iterator pos, T&& val)
{
	return add_node(pos.current_->prev, pos.current_, std::forward<T>(val));
}

template<class T>
template<class ...Args>
inline typename list<T>::reference list<T>::emplace_back(Args&& ...args)
{
	node* new_node = add_node(end_.prev, &end_, std::forward<Args>(args)...);
	return new_node->data;
}
template<class T>
template<class ...Args>
inline typename list<T>::reference list<T>::emplace_front(Args&& ...args)
{
	node* new_node = add_node(&end_, end_.next, std::forward<Args>(args)...);
	return new_node->data;
}
template<class T>
template<class ...Args>
inline typename list<T>::iterator list<T>::emplace(list<T>::const_iterator pos, Args && ...args)
{
	node* new_node = add_node(pos.current_->prev, pos.current_, std::forward<Args>(args)...);
	return new_node;
}

