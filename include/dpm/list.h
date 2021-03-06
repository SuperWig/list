#pragma once
#include <initializer_list>

namespace dpm
{
template <class T>
class list
{
    template <class>
    friend void swap(list& lhs, list& rhs) noexcept;

    struct node;
    struct node_base
    {
        node_base() = default;
        node_base(node_base* p, node_base* n) : prev{ p }, next{ n } {}
        [[nodiscard]] node* as_node() { return static_cast<node*>(this); }
        [[nodiscard]] const node* as_node() const { return static_cast<const node*>(this); }
        node_base* prev = this;
        node_base* next = this;
    };
    struct node : node_base
    {
        template <typename... Args>
        node(node_base* prev, node_base* next, Args&&... val) : node_base{ prev, next }, data(std::forward<Args>(val)...)
        {
        }
        T data;
    };
    template <class... Args>
    node* add_node(node_base* prev, node_base* next, Args&&... val)
    {
        auto* new_node       = new node(prev, next, std::forward<Args>(val)...);
        new_node->next->prev = new_node;
        new_node->prev->next = new_node;
        ++size_;
        return new_node;
    }
    void delete_node(node_base* node);

    node_base* end_ = new node_base;
    std::size_t size_{};
    template <bool is_const>
    struct iterator_impl;

public:
    using value_type             = T;
    using pointer                = T*;
    using const_pointer          = const T*;
    using reference              = T&;
    using const_reference        = const T&;
    using size_type              = std::size_t;
    using difference_type        = std::ptrdiff_t;
    using iterator               = iterator_impl<false>;
    using const_iterator         = iterator_impl<true>;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    list() = default;
    list(const list& other);
    list(list&& other) noexcept;
    list(std::initializer_list<T> init);
    ~list();

    // TODO
    list& operator=(const list&);
    list& operator=(list&&);

    [[nodiscard]] iterator begin() noexcept { return end_->next; }
    [[nodiscard]] const_iterator begin() const noexcept { return end_->next; }
    [[nodiscard]] const_iterator cbegin() const noexcept { return end_->next; }
    [[nodiscard]] iterator end() noexcept { return end_; }
    [[nodiscard]] const_iterator end() const noexcept { return end_; }
    [[nodiscard]] const_iterator cend() const noexcept { return end_; }
    [[nodiscard]] reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    [[nodiscard]] const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
    [[nodiscard]] const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }
    [[nodiscard]] reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    [[nodiscard]] const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
    [[nodiscard]] const_reverse_iterator crend() const noexcept { return const_reverse_iterator(begin()); }

    [[nodiscard]] reference front();
    [[nodiscard]] const_reference front() const;
    [[nodiscard]] reference back();
    [[nodiscard]] const_reference back() const;

    [[nodiscard]] std::size_t size() const noexcept { return size_; }
    [[nodiscard]] bool empty() const noexcept { return !size_; }

    void pop_back();
    void pop_front();

    void push_back(const T& val);
    void push_back(T&& val);
    void push_front(const T& val);
    void push_front(T&& val);
    iterator insert(const_iterator pos, const T& val);
    iterator insert(const_iterator pos, T&& val);

    template <class... Args>
    reference emplace_back(Args&&... args);
    template <class... Args>
    reference emplace_front(Args&&... args);
    template <class... Args>
    iterator emplace(const_iterator pos, Args&&... args);

    iterator erase(const_iterator pos);
    iterator erase(const_iterator first, const_iterator last);

    void reverse() noexcept;
    void swap(list& other) noexcept;
};

template <class T>
list<T>::list(const list& other)
{
    for (auto&& val : other)
    {
        push_back(val);
    }
}

template <class T>
list<T>::list(list&& other) noexcept
{
    swap(other);
}

template <class T>
list<T>::list(std::initializer_list<T> init)
{
    for (auto&& val : init)
    {
        push_back(val);
    }
}

template <class T>
list<T>::~list()
{
    for (node_base* next = end_->next; next != end_;)
    {
        node* del = next->as_node();
        next      = next->next;
        delete del;
    }
    delete end_;
}

template <typename T>
template <bool is_const>
struct list<T>::iterator_impl
{
    friend class list;
    using value_type        = T;
    using reference         = std::conditional_t<is_const, const T&, T&>;
    using pointer           = std::conditional_t<is_const, const T*, T*>;
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type   = std::ptrdiff_t;

    iterator_impl()                         = default;
    iterator_impl(const iterator_impl&)     = default;
    iterator_impl(iterator_impl&&) noexcept = default;
    iterator_impl(node_base* node) : current_(node) {}
    ~iterator_impl() = default;

    // implicit conversion to const_iterator
    operator iterator_impl<true>() const noexcept { return current_; }
    iterator_impl& operator=(const iterator_impl&) = default;
    iterator_impl& operator=(iterator_impl&&) noexcept = default;
    [[nodiscard]] reference operator*() { return current_->as_node()->data; }
    [[nodiscard]] pointer operator->() { return &current_->as_node()->data; }
    iterator_impl& operator++()
    {
        current_ = current_->next;
        return *this;
    }
    iterator_impl operator++(int)
    {
        const iterator_impl ret(current_);
        current_->next;
        return ret;
    }
    iterator_impl& operator--()
    {
        current_ = current_->prev;
        return *this;
    }
    iterator_impl operator--(int)
    {
        const iterator_impl ret(current_);
        current_->prev;
        return ret;
    }
    [[nodiscard]] bool operator==(const iterator_impl& rhs) const { return current_ == rhs.current_; }
    [[nodiscard]] bool operator!=(const iterator_impl& rhs) const { return current_ != rhs.current_; }

private:
    node_base* current_;
};

template <class T>
typename list<T>::reference list<T>::front()
{
    return end_->next->as_node()->data;
}

template <class T>
typename list<T>::const_reference list<T>::front() const
{
    return end_->next->as_node()->data;
}

template <class T>
typename list<T>::reference list<T>::back()
{
    return end_->prev->as_node()->data;
}

template <class T>
typename list<T>::const_reference list<T>::back() const
{
    return end_->prev->as_node()->data;
}

template <class T>
void list<T>::delete_node(node_base* node)
{
    node->prev->next = node->next;
    node->next->prev = node->prev;
    delete node;
    --size_;
}

template <class T>
void list<T>::pop_back()
{
    auto temp(end_->prev);
    end_->prev = end_->prev->prev;
    delete_node(temp);
}

template <class T>
void list<T>::pop_front()
{
    auto temp(end_->next);
    end_->next = end_->next->next;
    delete_node(temp);
}

template <class T>
void list<T>::push_back(const T& val)
{
    add_node(end_->prev, end_, val);
}

template <class T>
void list<T>::push_back(T&& val)
{
    add_node(end_->prev, end_, std::move(val));
}

template <class T>
void list<T>::push_front(const T& val)
{
    add_node(end_, end_->next, val);
}

template <class T>
void list<T>::push_front(T&& val)
{
    add_node(end_, end_->next, std::move(val));
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

template <class T>
template <class... Args>
typename list<T>::reference list<T>::emplace_back(Args&&... args)
{
    node* new_node = add_node(end_->prev, end_, std::forward<Args>(args)...);
    return new_node->data;
}
template <class T>
template <class... Args>
typename list<T>::reference list<T>::emplace_front(Args&&... args)
{
    node* new_node = add_node(end_, end_->next, std::forward<Args>(args)...);
    return new_node->data;
}
template <class T>
template <class... Args>
typename list<T>::iterator list<T>::emplace(const_iterator pos, Args&&... args)
{
    node* new_node = add_node(pos.current_->prev, pos.current_, std::forward<Args>(args)...);
    return new_node;
}

template <class T>
typename list<T>::iterator list<T>::erase(const_iterator pos)
{
    auto next = pos.current_->next;
    delete_node(pos.current_);
    return next;
}

template <class T>
typename list<T>::iterator list<T>::erase(const_iterator first, const_iterator last)
{
    auto n_first = first.current_;
    while (n_first != last.current_)
    {
        auto next = n_first->next;
        delete_node(n_first);
        n_first = next;
    }
    return n_first;
}

template <class T>
void list<T>::reverse() noexcept
{
    for (node_base* current = end_;;)
    {
        node_base* next = current->next;
        current->next   = current->prev;
        current->prev   = next;
        if (next == end_)
            break;
        current = next;
    }
}

template <class T>
void list<T>::swap(list& other) noexcept
{
    std::swap(size_, other.size_);
    std::swap(end_, other.end_);
}

template <class T>
void swap(list<T>& lhs, list<T>& rhs) noexcept(noexcept(lhs.swap(rhs)))
{
    lhs.swap(rhs);
}
} // namespace dpm