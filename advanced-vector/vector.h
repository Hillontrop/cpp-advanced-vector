#pragma once
#include <cassert>
#include <cstdlib>
#include <new>
#include <utility>
#include <algorithm>
#include <memory>   // uninitialized_value_construct_n, uninitialized_copy_n, destroy_n, uninitialized_move_n 

template <typename T>
class RawMemory
{
public:
    RawMemory() = default;

    explicit RawMemory(size_t capacity) : buffer_(Allocate(capacity)), capacity_(capacity) {}

    RawMemory(const RawMemory&) = delete;

    RawMemory& operator=(const RawMemory& rhs) = delete;

    // Ïåðåìåùàþùèé êîíñòðóêòîð (RawMemory move constructor)
    RawMemory(RawMemory&& other) noexcept : buffer_(other.buffer_), capacity_(other.capacity_)
    {
        // Îáíóëÿåì ðåñóðñû â other, ÷òîáû äåñòðóêòîð other íå îñâîáîäèë ïàìÿòü
        other.buffer_ = nullptr;
        other.capacity_ = 0;
    }

    // Îïåðàòîð ïðèñâàèâàíèÿ ñ ïåðåíîñîì (RawMemory move assignment operator)
    RawMemory& operator=(RawMemory&& rhs) noexcept
    {
        if (this != &rhs)
        {
            Deallocate(buffer_);
            buffer_ = rhs.buffer_;
            capacity_ = rhs.capacity_;
            rhs.buffer_ = nullptr;
            rhs.capacity_ = 0;
        }
        return *this;
    }

    ~RawMemory()
    {
        Deallocate(buffer_);
    }

    T* operator+(size_t offset) noexcept
    {
        // Ðàçðåøàåòñÿ ïîëó÷àòü àäðåñ ÿ÷åéêè ïàìÿòè, ñëåäóþùåé çà ïîñëåäíèì ýëåìåíòîì ìàññèâà
        assert(offset <= capacity_);
        return buffer_ + offset;
    }

    const T* operator+(size_t offset) const noexcept
    {
        return const_cast<RawMemory&>(*this) + offset;
    }

    const T& operator[](size_t index) const noexcept
    {
        return const_cast<RawMemory&>(*this)[index];
    }

    T& operator[](size_t index) noexcept
    {
        assert(index < capacity_);
        return buffer_[index];
    }

    void Swap(RawMemory& other) noexcept
    {
        std::swap(buffer_, other.buffer_);
        std::swap(capacity_, other.capacity_);
    }

    const T* GetAddress() const noexcept
    {
        return buffer_;
    }

    T* GetAddress() noexcept
    {
        return buffer_;
    }

    size_t Capacity() const
    {
        return capacity_;
    }

private:
    // Âûäåëÿåò ñûðóþ ïàìÿòü ïîä n ýëåìåíòîâ è âîçâðàùàåò óêàçàòåëü íà íå¸
    static T* Allocate(size_t n)
    {
        return n != 0 ? static_cast<T*>(operator new(n * sizeof(T))) : nullptr;
    }

    // Îñâîáîæäàåò ñûðóþ ïàìÿòü, âûäåëåííóþ ðàíåå ïî àäðåñó buf ïðè ïîìîùè Allocate
    static void Deallocate(T* buf) noexcept
    {
        operator delete(buf);
    }

    T* buffer_ = nullptr;
    size_t capacity_ = 0;
};

template <typename T>
class Vector
{
public:
    using iterator = T*;
    using const_iterator = const T*;

    // Êîíñòðóêòîð ïî óìîë÷àíèþ. Èíèöèàëèçèðóåò âåêòîð íóëåâîãî ðàçìåðà è âìåñòèìîñòè.
    // Íå âûáðàñûâàåò èñêëþ÷åíèé.
    // Àëãîðèòìè÷åñêàÿ ñëîæíîñòü: O(1).
    Vector() = default;

    // Êîíñòðóêòîð, êîòîðûé ñîçäà¸ò âåêòîð çàäàííîãî ðàçìåðà.
    // Âìåñòèìîñòü ñîçäàííîãî âåêòîðà ðàâíà åãî ðàçìåðó,
    // à ýëåìåíòû ïðîèíèöèàëèçèðîâàíû çíà÷åíèåì ïî óìîë÷àíèþ äëÿ òèïà T.
    // Àëãîðèòìè÷åñêàÿ ñëîæíîñòü : O(ðàçìåð âåêòîðà).
    explicit Vector(size_t size) : data_(size), size_(size)
    {
        std::uninitialized_value_construct_n(begin(), size);
    }

    // Êîïèðóþùèé êîíñòðóêòîð. Ñîçäà¸ò êîïèþ ýëåìåíòîâ èñõîäíîãî âåêòîðà.
    // Èìååò âìåñòèìîñòü, ðàâíóþ ðàçìåðó èñõîäíîãî âåêòîðà,
    // òî åñòü âûäåëÿåò ïàìÿòü áåç çàïàñà.
    // Àëãîðèòìè÷åñêàÿ ñëîæíîñòü: O(ðàçìåð èñõîäíîãî âåêòîðà).
    Vector(const Vector& other) : data_(other.size_), size_(other.size_)
    {
        std::uninitialized_copy_n(other.data_.GetAddress(), other.size_, begin());
    }

    // Äåñòðóêòîð.Ðàçðóøàåò ñîäåðæàùèåñÿ â âåêòîðå ýëåìåíòû è
    // îñâîáîæäàåò çàíèìàåìóþ èìè ïàìÿòü.
    // Àëãîðèòìè÷åñêàÿ ñëîæíîñòü : O(ðàçìåð âåêòîðà).
    ~Vector()
    {
        std::destroy_n(begin(), size_);
    }

    // Êîíñòðóêòîð ïåðåìåùåíèÿ (Vector move constructor)
    Vector(Vector&& other) noexcept : data_(std::move(other.data_)), size_(other.size_)
    {
        // Îáíóëÿåì ðàçìåð ó èñõîäíîãî îáúåêòà, ÷òîáû äåñòðóêòîð íå îñâîáîäèë ïàìÿòü
        other.size_ = 0;
    }

    // Îïåðàòîð ïåðåìåùåíèÿ(Vector move assignment operator)
    Vector& operator=(const Vector& rhs)
    {
        if (this == &rhs)
        {
            return *this;
        }

        if (data_.Capacity() < rhs.size_)
        {
            // Âûäåëÿåì íîâóþ ïàìÿòü, åñëè òåêóùåé åìêîñòè íåäîñòàòî÷íî
            RawMemory<T> new_data(rhs.size_);
            std::uninitialized_copy_n(rhs.data_.GetAddress(), rhs.size_, new_data.GetAddress());
            std::destroy_n(begin(), size_); // Óíè÷òîæàåì ñòàðûå äàííûå
            data_.Swap(new_data); // Ïîìåíÿåì ìåñòàìè áóôåðû, ÷òîáû èñïîëüçîâàòü íîâûå äàííûå
        }
        else
        {
            // Êîïèðóåì ýëåìåíòû èç èñõîäíîãî âåêòîðà â òåêóùèé âåêòîð
            std::copy(rhs.begin(), rhs.begin() + std::min(size_, rhs.size_), begin());  // + Öèêë çàìåíåí íà std::copy

            // Åñëè èñõîäíûé âåêòîð èìååò áîëüøå ýëåìåíòîâ, êîïèðóåì îñòàâøèåñÿ ýëåìåíòû â ñâîáîäíîå ïðîñòðàíñòâî
            if (size_ < rhs.size_)
            {
                std::uninitialized_copy(rhs.data_.GetAddress() + size_, rhs.data_.GetAddress() + rhs.size_, begin() + size_);
            }
            // Åñëè â èñõîäíîì âåêòîðå ìåíüøå ýëåìåíòîâ, óíè÷òîæàåì ëèøíèå ýëåìåíòû â ïðèíèìàþùåì âåêòîðå
            else if (size_ > rhs.size_)
            {
                std::destroy_n(begin() + rhs.size_, size_ - rhs.size_);
            }
        }

        size_ = rhs.size_;
        return *this;
    }

    Vector& operator=(Vector&& rhs) noexcept
    {
        if (this == &rhs)
        {
            return *this;
        }

        Swap(rhs);  // + Èñïîëüçîâàí Swap (êëàññà Vector)

        return *this;
    }

    // Îáìåíèâàòü ñîäåðæèìîå äâóõ îáúåêòîâ Vector
    void Swap(Vector& other) noexcept
    {
        data_.Swap(other.data_);
        std::swap(size_, other.size_);
    }

    // Ìåòîä void Reserve(size_t capacity).Ðåçåðâèðóåò äîñòàòî÷íî ìåñòà,
    // ÷òîáû âìåñòèòü êîëè÷åñòâî ýëåìåíòîâ, ðàâíîå capacity.
    // Åñëè íîâàÿ âìåñòèìîñòü íå ïðåâûøàåò òåêóùóþ, ìåòîä íå äåëàåò íè÷åãî.
    // Àëãîðèòìè÷åñêàÿ ñëîæíîñòü : O(ðàçìåð âåêòîðà).
    void Reserve(size_t new_capacity)
    {
        if (new_capacity <= data_.Capacity())
        {
            return;
        }

        RawMemory<T> new_data(new_capacity);

        UninitializedCopyOrMove(begin(), size_, new_data.GetAddress());  // + Äîïîëíèòåëüíûé ìåòîä äëÿ èíèöèàëèçàöèè

        std::destroy_n(begin(), size_);
        data_.Swap(new_data);
    }

    void Resize(size_t new_size)
    {
        if (new_size > size_)
        {
            if (new_size > data_.Capacity())
            {
                Reserve(new_size);
            }

            std::uninitialized_value_construct_n(begin() + size_, new_size - size_);
        }
        else if (new_size < size_)
        {
            std::destroy_n(begin() + new_size, size_ - new_size);
        }

        size_ = new_size;
    }

    void PushBack(const T& value)
    {
        Emplace(end(), value);
    }

    void PushBack(T&& value)
    {
        Emplace(end(), std::move(value));
    }

    template <typename... Args>
    T& EmplaceBack(Args&&... args)
    {
        return *(Emplace(end(), std::forward<Args>(args)...));
    }

    void PopBack() /* noexcept */
    {
        if (size_ > 0)
        {
            std::destroy_at(end() - 1);
            --size_;
        }
    }

    iterator begin() noexcept
    {
        return data_.GetAddress();
    }

    iterator end() noexcept
    {
        return begin() + size_;
    }

    const_iterator begin() const noexcept
    {
        return data_.GetAddress();
    }

    const_iterator end() const noexcept
    {
        return begin() + size_;
    }

    const_iterator cbegin() const noexcept
    {
        return data_.GetAddress();
    }

    const_iterator cend() const noexcept
    {
        return begin() + size_;
    }

    template <typename... Args>
    iterator Emplace(const_iterator pos, Args&&... args)
    {
        assert(pos >= begin() && pos <= end());

        size_t index = pos - begin();

        if (size_ == data_.Capacity())
        {
            return EmplaceReallocate(index, std::forward<Args>(args)...);
        }
        else
        {
            return EmplaceWithoutReallocate(index, std::forward<Args>(args)...);
        }
    }


    iterator Erase(const_iterator pos)
    {
        assert(pos >= begin() && pos < end());

        size_t index = pos - begin();

        if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>)
        {
            std::move(begin() + index + 1, end(), begin() + index);
        }
        else
        {
            std::copy(begin() + index + 1, end(), begin() + index);
        }

        std::destroy_at(end() - 1);

        --size_;
        return begin() + index;
    }

    iterator Insert(const_iterator pos, const T& value)
    {
        return Emplace(pos, value);
    }

    iterator Insert(const_iterator pos, T&& value)
    {
        return Emplace(pos, std::move(value));
    }

    size_t Size() const noexcept
    {
        return size_;
    }

    size_t Capacity() const noexcept
    {
        return data_.Capacity();
    }

    const T& operator[](size_t index) const noexcept
    {
        return const_cast<Vector&>(*this)[index];
    }

    T& operator[](size_t index) noexcept
    {
        assert(index < size_);
        return data_[index];
    }

private:
    RawMemory<T> data_;
    size_t size_ = 0;

    template <typename... Args>
    iterator EmplaceReallocate(size_t index, Args&&... args)
    {
        size_t new_capacity = (size_ == 0) ? 1 : size_ * 2;
        RawMemory<T> new_data(new_capacity);

        UninitializedCopyOrMove(begin(), index, new_data.GetAddress());  // + Äîïîëíèòåëüíûé ìåòîä äëÿ èíèöèàëèçàöèè
        UninitializedCopyOrMove(begin() + index, size_ - index, new_data.GetAddress() + index + 1);  // + Äîïîëíèòåëüíûé ìåòîä äëÿ èíèöèàëèçàöèè

        new (new_data.GetAddress() + index) T(std::forward<Args>(args)...);

        std::destroy_n(data_.GetAddress(), size_);
        data_.Swap(new_data);
        ++size_;
        return begin() + index;
    }

    template <typename... Args>
    iterator EmplaceWithoutReallocate(size_t index, Args&&... args)
    {
        if (index < size_)
        {
            T temp(std::forward<Args>(args)...);
            std::uninitialized_move_n(end() - 1, 1, end());
            std::move_backward(begin() + index, end() - 1, end());

            *(data_.GetAddress() + index) = std::move(temp);
        }
        else
        {
            new (end()) T(std::forward<Args>(args)...);
        }
        ++size_;
        return begin() + index;
    }

    template <typename InputIt, typename OutputIt>
    static void UninitializedCopyOrMove(InputIt first, size_t count, OutputIt result)
    {
        if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>)
        {
            std::uninitialized_move_n(first, count, result);
        }
        else
        {
            std::uninitialized_copy_n(first, count, result);
        }
    }
};
