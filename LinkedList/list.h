#pragma once
#include <initializer_list>

template<class T>
class list
{
	template<class>
	friend void swap(list& lhs, list& rhs) noexcept;
	
	using reference = T&;
	using const_reference = const T&;
	using pointer = T*;
	struct node;
	struct node_base
	{
		node_base() : prev(this), next(this) {}
		node_base(node_base* p, node_base* n) : prev(p), next(n) {}
		node* as_node() { return static_cast<node*>(this); }
		const node* as_node() const { return static_cast<const node*>(this); }
		node_base* prev;
		node_base* next;
	};
	struct node : node_base
	{
		node(node_base* prev, node_base* next, const T& val) : node_base{ prev, next }, data(val) {}
		template<typename...Args>
		node(node_base* prev, node_base* next, Args&&... val) : node_base{ prev, next }, data(std::forward<Args>(val)...) {}
		T data;
	};
	template<class ...Args>
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
	list(std::initializer_list<T> init);
	~list();

	struct iterator;
	struct const_iterator;
	iterator begin() noexcept { return end_.next; }
	const_iterator begin() const noexcept { return end_.next; }
	const_iterator cbegin() const noexcept { return end_.next; }
	iterator end() noexcept { return &end_; }
	const_iterator end() const noexcept { return &end_; }
	const_iterator cend() const noexcept { return &end_; }

	struct reverse_iterator;
	struct const_reverse_iterator;
	reverse_iterator rbegin() noexcept { return end_.prev; }
	const_reverse_iterator rbegin() const noexcept { return end_.prev; }
	const_reverse_iterator crbegin() const noexcept { return end_.prev; }
	reverse_iterator rend() noexcept { return &end_; }
	const_reverse_iterator rend() const noexcept { return &end_; }
	const_reverse_iterator crend() const noexcept { return &end_; }

	reference front();
	const_reference front() const;
	reference back();
	const_reference back() const;

	std::size_t size() const noexcept { return size_; }
	[[nodiscard]]
	bool empty() const noexcept { return !size_; }

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

	void reverse() noexcept;
	void swap(list& other) noexcept;
};

template<class T>
list<T>::list(std::initializer_list<T> init) : size_(init.size())
{
	for(auto it = init.begin(); it != init.end(); ++it)
	{
		push_back(*it);
	}
}

template <class T>
list<T>::~list()
{
	for (node_base* next = end_.next; next != &end_; )
	{
		node* del = next->as_node();
		next = next->next;
		delete del;
	}
}

template<typename T>
struct list<T>::iterator
{
	friend class list;
	using self_type = iterator;
	using value_type = T;
	using reference = T&;
	using pointer = T*;
	using iterator_category = std::forward_iterator_tag;
	using difference_type = std::ptrdiff_t;
	iterator() : current_(nullptr) {}
	iterator(const iterator&) = default;
	iterator(iterator&&) noexcept = default;
	iterator(node_base* node) : current_(node) {}
	~iterator() = default;
	iterator& operator=(const iterator&) = default;
	iterator& operator=(iterator&&) noexcept = default;
	reference operator*() { return current_->as_node()->data; }
	pointer operator->() { return &current_->as_node()->data; }
	self_type& operator++() { const self_type cur = current_; current_ = current_->next; return cur; }
	self_type operator++(int) { return current_->next; }
	self_type& operator--() { const self_type cur = current_; current_ = current_->prev; return cur; }
	self_type operator--(int) { return current_->prev; }
	bool operator==(const self_type& rhs) { return current_ == rhs.current_; }
	bool operator!=(const self_type& rhs) { return current_ != rhs.current_; }	
	void swap(iterator&);
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
	using difference_type = std::ptrdiff_t;
	const_iterator() = default;
	const_iterator(const node_base* node) : current_(node) {}
	const_iterator(const const_iterator&) = default;
	const_iterator(const_iterator&&) noexcept = default;
	~const_iterator() = default;
	const_iterator& operator=(const const_iterator&) = default;
	const_iterator& operator=(const_iterator&&) = default;
	reference operator*() { return current_->as_node()->data; }
	pointer operator->() { return &current_->as_node()->data; }
	self_type& operator++() { const self_type cur = current_; current_ = current_->next; return cur; }
	self_type operator++(int) { return current_->next; }
	self_type& operator--() { const self_type cur = current_; current_ = current_->prev; return cur; }
	self_type operator--(int) { return current_->prev; }
	bool operator==(const self_type& rhs) { return current_ == rhs.current_; }
	bool operator!=(const self_type& rhs) { return current_ != rhs.current_; }
	void swap(const_iterator&);
private:
	const node_base* current_;
};

template<typename T>
struct list<T>::reverse_iterator
{
	friend class list;
	using self_type = reverse_iterator;
	using value_type = T;
	using reference = T & ;
	using pointer = T * ;
	using iterator_category = std::bidirectional_iterator_tag;
	using difference_type = std::ptrdiff_t;
	reverse_iterator() = default;
	reverse_iterator(node_base* node) : current_(node) {}
	reverse_iterator(const reverse_iterator&) = default;
	reverse_iterator(reverse_iterator&&) noexcept = default;
	~reverse_iterator() = default;
	reverse_iterator& operator=(const reverse_iterator&) = default;
	reverse_iterator& operator=(reverse_iterator&&) noexcept = default;
	reference operator*() { return current_->as_node()->data; }
	pointer operator->() { return &current_->as_node()->data; }
	self_type& operator++() { const self_type cur = current_; current_ = current_->prev; return cur; }
	self_type operator++(int) { return current_->prev; }
	self_type& operator--() { const self_type cur = current_; current_ = current_->next; return cur; }
	self_type operator--(int) { return current_->next; }
	bool operator==(const self_type& rhs) { return current_ == rhs.current_; }
	bool operator!=(const self_type& rhs) { return current_ != rhs.current_; }
	void swap(reverse_iterator&);
private:
	node_base* current_;
};
template<typename T>
struct list<T>::const_reverse_iterator
{
	friend class list;
	using self_type = const_reverse_iterator;
	using value_type = T;
	using reference = const T&;
	using pointer = const T*;
	using iterator_category = std::bidirectional_iterator_tag;
	using difference_type = int;
	const_reverse_iterator() = default;
	const_reverse_iterator(const node_base* node) : current_(node) {}
	const_reverse_iterator(const const_reverse_iterator&) = default;
	const_reverse_iterator(const_reverse_iterator&&) noexcept = default;
	~const_reverse_iterator() = default;
	const_reverse_iterator& operator=(const const_reverse_iterator&) = default;
	const_reverse_iterator& operator=(const_reverse_iterator&&) = default;
	reference operator*() { return current_->as_node()->data; }
	pointer operator->() { return &current_->as_node()->data; }
	self_type& operator++() { const self_type cur = current_; current_ = current_->prev; return cur; }
	self_type operator++(int) { return current_->prev; }
	self_type& operator--() { const self_type cur = current_; current_ = current_->next; return cur; }
	self_type operator--(int) { return current_->next; }
	bool operator==(const self_type& rhs) { return current_ == rhs.current_; }
	bool operator!=(const self_type& rhs) { return current_ != rhs.current_; }
	void swap(const_reverse_iterator&);
private:
	const node_base* current_;
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
	add_node(&end_, end_.next, std::move(val));
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
typename list<T>::reference list<T>::emplace_back(Args&& ...args)
{
	node* new_node = add_node(end_.prev, &end_, std::forward<Args>(args)...);
	return new_node->data;
}
template<class T>
template<class ...Args>
typename list<T>::reference list<T>::emplace_front(Args&& ...args)
{
	node* new_node = add_node(&end_, end_.next, std::forward<Args>(args)...);
	return new_node->data;
}
template<class T>
template<class ...Args>
typename list<T>::iterator list<T>::emplace(const_iterator pos, Args&& ...args)
{
	node* new_node = add_node(pos.current_->prev, pos.current_, std::forward<Args>(args)...);
	return new_node;
}

template<class T>
void list<T>::reverse() noexcept
{
	for (node_base* current = &end_; ;)
	{
		node_base* next = current->next;
		current->next = current->prev;
		current->prev = next;
		if (next == &end_)
			break;
		current = next;
	}
}

template <class T>
void list<T>::swap(list& other) noexcept
{
	std::swap(size_, other.size_);
	std::swap(end_, other.end_);
	if (size_ == 0)
	{
		end_.next = end_.prev = end_.as_node();
	}
	else
	{
		end_.next->prev = end_.prev->next = end_.as_node();
	}
	if (other.size_ == 0)
	{
		other.end_.next = other.end_.prev = other.end_.as_node();
	}
	else
	{
		other.end_.next->prev = other.end_.prev->next = other.end_.as_node();
	}
}

template<class T>
void swap(list<T>& lhs, list<T>& rhs) noexcept
{
	lhs.swap(rhs);
}
