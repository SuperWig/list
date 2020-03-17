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
        node_base()
            : prev(this)
            , next(this)
        {
        }
        node_base(node_base* p, node_base* n)
            : prev(p)
            , next(n){}
        [[nodiscard]] node * as_node() { return static_cast<node*>(this); }
        [[nodiscard]] const node* as_node() const { return static_cast<const node*>(this); } 
        node_base* prev;
        node_base* next;
    };
    struct node : node_base
    {
        node(node_base* prev, node_base* next, const T& val)
            : node_base{prev, next}
            , data(val)
        {
        }
        template <typename... Args>
        node(node_base* prev, node_base* next, Args&&... val)
            : node_base{prev, next}
            , data(std::forward<Args>(val)...)
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

    node_base end_{};
    std::size_t size_{};
    template <bool is_const>
    struct iter_t;

public:
    // typedefs
    using value_type             = T;
    using pointer                = T*;
    using const_pointer          = const T*;
    using reference              = T&;
    using const_reference        = const T&;
    using size_type              = std::size_t;
    using difference_type        = std::ptrdiff_t;
    using iterator               = iter_t<false>;
    using const_iterator         = iter_t<true>;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    list() = default;
    list(std::initializer_list<T> init);
    ~list();
    [[nodiscard]] iterator begin() noexcept { return end_.next; }
    [[nodiscard]] const_iterator begin() const noexcept { return end_.next; }
    [[nodiscard]] const_iterator cbegin() const noexcept { return end_.next; }
    [[nodiscard]] iterator end() noexcept { return &end_; }
    [[nodiscard]] const_iterator end() const noexcept { return &end_; }
    [[nodiscard]] const_iterator cend() const noexcept { return &end_; }
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

    void reverse() noexcept;
    void swap(list& other) noexcept;
};

template <class T>
list<T>::list(std::initializer_list<T> init) : size_(0)
{
    for (auto it = init.begin(); it != init.end(); ++it)
    {
        push_back(*it);
    }
}

template <class T>
list<T>::~list()
{
    for (node_base* next = end_.next; next != &end_;)
    {
        node* del = next->as_node();
        next      = next->next;
        delete del;
    }
}

template <typename T>
template <bool is_const>
struct list<T>::iter_t
{
    friend class list;
    using node_base_t       = std::conditional_t<is_const, const node_base*, node_base*>;
    using value_type        = T;
    using reference         = std::conditional_t<is_const, const T&, T&>;
    using pointer           = std::conditional_t<is_const, const T*, T*>;
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type   = std::ptrdiff_t;

    iter_t()                  = default;
    iter_t(const iter_t&)     = default;
    iter_t(iter_t&&) noexcept = default;
    iter_t(node_base_t node)
        : current_(node)
    {
    }
    //implicit conversion to const_iterator
    operator iter_t<true>() const noexcept { return current_; }
    ~iter_t()       = default;
    iter_t& operator=(const iter_t&) = default;
    iter_t& operator=(iter_t&&) noexcept = default;
    [[nodiscard]] reference operator*() { return current_->as_node()->data; }
    [[nodiscard]] pointer operator->() { return &current_->as_node()->data; }
    iter_t& operator++()
    {
        current_ = current_->next;
        return *this;
    }
    iter_t operator++(int)
    {
        const iter_t ret(current_);
        current_->next;
        return ret;
    }
    iter_t& operator--()
    {
        current_ = current_->prev;
        return *this;
    }
    iter_t operator--(int)
    {
        const iter_t ret(current_);
        current_->prev;
        return ret;
    }
    [[nodiscard]] bool operator==(const iter_t& rhs) const { return current_ == rhs.current_; }
    [[nodiscard]] bool operator!=(const iter_t& rhs) const { return current_ != rhs.current_; }

private:
    node_base_t current_;
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

template <class T>
void list<T>::pop_back()
{
    auto temp(end_.prev);
    end_.prev = end_.prev->prev;
    delete_node(temp);
}

template <class T>
void list<T>::pop_front()
{
    auto temp(end_.next);
    end_.next = end_.next->next;
    delete_node(temp);
}

template <class T>
void list<T>::push_back(const T& val)
{
    add_node(end_.prev, &end_, val);
}

template <class T>
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

template <class T>
template <class... Args>
typename list<T>::reference list<T>::emplace_back(Args&&... args)
{
    node* new_node = add_node(end_.prev, &end_, std::forward<Args>(args)...);
    return new_node->data;
}
template <class T>
template <class... Args>
typename list<T>::reference list<T>::emplace_front(Args&&... args)
{
    node* new_node = add_node(&end_, end_.next, std::forward<Args>(args)...);
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
void list<T>::reverse() noexcept
{
    for (node_base* current = &end_;;)
    {
        node_base* next = current->next;
        current->next   = current->prev;
        current->prev   = next;
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

template <class T>
void swap(list<T>& lhs, list<T>& rhs) noexcept(noexcept(lhs.swap(rhs)))
{
    lhs.swap(rhs);
}
}//namespace dpm