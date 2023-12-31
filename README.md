# Динамический массив (Advanced Vector)

## 1. Описание
Проект **Advanced Vector** представляет собой собственный динамический массив построеный на выделении сырой памяти (аналог **_std::vector_**).

В данном проекте представленно два класса:

* **RawMemory** обеспечивающий управление сырой памятью, используя идиому _RAII_ для выделения и освобождения памяти автоматически при создании и уничтожении объектов;

* **Vector** представляет динамический массив, построенный на основе _RawMemory_. Идиома _RAII_ применяется для эффективного управления ресурсами, гарантируя, что при создании вектора выделяется память, а при уничтожении – она освобождается.

Шаблонные методы позволяют использовать контейнер для любых конструируемых типов, минимизируя копирование значений. Вместо этого часто используется перемещение значений или их инициализация внутри контейнера, что повышает производительность и эффективность использования ресурсов.

Этот проект может быть легко включен в другие проекты благодаря своей универсальности.

## 2. Цель проекта
Цель проекта Advanced Vector заключается в тренировке использования сырой памятью и использовании идиомы **RAII** _(Resource Acquisition Is Initialization)_. Основной задачей является освоение методов управления динамической памятью через классы _RawMemory_ и _Vector_, предоставляющих гибкий и эффективный способ управления ресурсами.

Этот проект предоставляет уникальную практику в работе с памятью напрямую, что дает возможность глубже понять принципы управления ресурсами в языке программирования, а также обучает использованию средств, которые способствуют безопасной и надежной работе с памятью.

## 3. Использование

Для использования **Vector** необходимо создать экземпляр класса, затем можно добавлять, удалять, изменять элементы вектора с помощью методов, таких как _PushBack_, _PopBack_, _Resize_ и других. Класс **RawMemory** используется внутренне и предоставляет базовую функциональность для управления памятью.

## 4.Описание методов

### Шаблонный класс-обертка RawMemory\<T>

* **RawMemory _()_** - конструктор по умолчанию.
* **RawMemory _(size_t capacity)_** - конструктор для выделения сырой памяти с заданным количеством элементов.
* **RawMemory _(RawMemory&& other)_** - конструктор перемещения.
* **RawMemory& operator= _(RawMemory&& rhs)_** - оператор присваивания с перемещением.
* **~RawMemory _()_** - деструктор.
***
* **T\* operator+ _(size_t offset)_** - перегруженный оператор +. возвращающтй указатель на память, смещенную на заданный размер от начала буфера.
* **T& operator[] _(size_t index)_** - перегруженный оператор [], обеспечивающий доступ к элементам по индексу.
***
* **void Swap _(RawMemory& other)_** - меняет местами содержимое двух объектов _RawMemory_.
* **T\* GetAddress _()_** - полчить адрес.
* **size_t Capacity _()_** - получить вместимость.

#### Запрещены _(delete)_
Операция копирования не может быть выполнена, т.к. класс _RawMemory_ не имеет информации о количестве элементов находящихся в сырой памяти.

* **RawMemory _(const RawMemory&)_** - конструктор копирования.
* **RawMemory& operator= _(const RawMemory& rhs)_** - копирующий оператор присваивания.

### Шаблонный класс Vector\<T>
* **Vector _()_** - конструктор по умолчанию. Инициализирует вектор нулевого размера и вместимости.
* **Vector _(size_t size)_** - конструктор создающий вектор заданного размера с инициализацией элементов по умолчанию.
* **Vector _(const Vector& other)_** - конструктор копирования. Создает копию элементов исходного вектора.
* **Vector _(Vector&& other)_** - конструктор перемещения.
* **Vector& operator= _(const Vector& rhs)_** - копирующий оператор присваивания.
* **Vector& operator= _(Vector&& rhs)_** - оператор присваивания с перемещением.
* **~Vector _()_** - деструктор. Разрушает содержащиеся в векторе элементы и освобождает занимаемую ими память.
#### Итераторы:
* **T& operator[] _(size_t index)_** - обеспечивает доступ к элементу _Vector_.
* **T\* begin _()_** - возвращает итератор, указывающий на начало _Vector_.
* **T\* end _()_** - возвращает итератор, указывающий на элемент после последнего элемента _Vector_.
#### Операции:
* **size_t Capacity _()_** - получить вместимость _Vector_.
* **T& EmplaceBack _(Args&&... args)** - создает новый элемент в конце _Vector_, используя переданные аргументы.
* **T\* Emplace _(const T\* pos, Args&&... args)_** - создает новый элемент по указанной позиции, используя переданные аргументы.
* **T\* Erase _(const T\* pos)_** - удаляет элемент из _Vector_ по указанной позиции.
* **T\* Insert _(const T\* pos, const T& value)_** и **T\* Insert _(const T\* pos, T&& value)_** - вставляет элемент в заданную позицию _Vector_.
* **void PopBack _()** - удаляет последний элемент из _Vector_.
* **void PushBack _(const T& value)** и **void PushBack _(T&& value)_** - добавляет элемент в конец _Vector_.
* **void Reserve _(size_t new_capacity)** - резервирует память для заданной вместимости.
* **void Resize _(size_t new_size)** - изменяет размер _Vector_.
* **size_t Size _()_** - получить размер _Vector_.
* **void Swap _(Vector& other)_** - обменивает содержимое двух _Vector_.

## Дополнительно
### RAII
Идиома **RAII** _(Resource Acquisition Is Initialization)_ — это принцип в C++ и других языках программирования, основанных на объектно-ориентированном подходе, который связывает жизненный цикл ресурсов с жизненным циклом объектов.

Суть **RAII** заключается в том, что ресурсы, например как динамическая память, выделяются и освобождаются через инициализацию и деинициализацию объектов. При создании объекта выделяются ресурсы, необходимые для его функционирования, а при уничтожении объекта освобождаются эти ресурсы. Это происходит благодаря вызову конструктора при создании объекта и деструктора при его уничтожении. Таким образом, при использовании **RAII** ресурсы управляются автоматически в рамках жизненного цикла объектов.

Преимущества использования **RAII** включают предсказуемость поведения программы и уменьшение вероятности утечек ресурсов. Этот подход также способствует написанию более безопасного и чистого кода, так как программисту не нужно вручную заботиться о правильной очистке выделенных ресурсов, а управление ресурсами происходит автоматически при выходе объекта из области видимости.

### operator new и operator delete
В классе _RawMemory_ используются **operator new** и **operator delete** для выделения и освобождения сырой памяти.

**operator new** и **operator delete** - это более низкоуровневые инструменты, используемые для динамического выделения памяти, по сравнению с операторами **new** - **new[]** и **delete** - **delete[]**.

**operator new** возвращает указатель на первый байт выделенной памяти и может быть использован для выделения блоков памяти переменного размера.
Пример из класса _RawMemory\<T>_:
```cpp
static T* Allocate(size_t n)
{
    return n != 0 ? static_cast<T*>(operator new(n * sizeof(T))) : nullptr;
}
```
В отличие от **new[]**, использование **operator new** напрямую не инициализирует элементы массива, а только выделяет память. Поэтому для массивов предпочтительнее использовать **new[]**, чтобы правильно управлять памятью и вызывать конструкторы/деструкторы элементов массива.

Пример использования **new** - **delete**:
```cpp
int* ptr = new int;  // Выделение памяти под один int
*ptr = 10;  // Присвоение значение этой памяти
delete ptr;  // Освобождение памяти
```
Пример использования **new[]** - **delete[]**:
```cpp
int* arr = new int[10];  // Выделение памяти для массива из 10 int
delete[] arr;  // Освобождение памяти массива
```
Пример использования **operator new** - **operator delete**:
```cpp
int* ptr = static_cast<int*>(operator new(sizeof(int)));  // Выделим память для одного объекта int
*ptr = 10;  // Используем эту память
operator delete(ptr);  // Освобождаем память
```

**operator delete** используется напрямую для освобождения памяти, выделенной **operator new**, и не вызывает деструкторы для объектов в отличии от **new** и **new[]** которые вызывает деструкторы для объектов.

### Работы с непроинициализированной памятью (библиотека _\<memory>_)
Класс _Vector_ использует функции и алгоритмы из библиотеки **\<memory>**. Эти функции позволяют _Vector_ эффективно управлять памятью и объектами, минимизируя издержки на конструирование, копирование и разрушение элементов. Вместо явного вызова конструкторов и деструкторов на каждом этапе, они вызываются только в тех местах, где это действительно необходимо, что повышает производительность и снижает накладные расходы.

#### Конструирование и инициализация
* _**std::uninitialized_copy**_ и _**std::uninitialized_copy_n**_ - Копирование элементов из диапазона в непроинициализированную память.
* _**std::uninitialized_fill**_ и _**std::uninitialized_fill_n**_ - Инициализация памяти указанным значением.
* _**std::uninitialized_default_construct**_ и _**std::uninitialized_default_construct_n**_ - Вызов конструктора по умолчанию для элементов в выделенной памяти.
* _**std::uninitialized_value_construct**_ и _**std::uninitialized_value_construct_n**_ - Конструирование элементов в памяти с вызовом конструктора по умолчанию.
#### Уничтожение и освобождение памяти
* _**std::destroy**_ и _**std::destroy_n**_ - Вызов деструкторов для элементов в диапазоне.
* _**std::destroy_at**_ - Вызов деструктора для конкретного объекта.
* _**std::destroy_if**_ - Уничтожение элементов на основе условия.
* _**std::destroy_uninitialized**_: Уничтожение части выделенной непроинициализированной памяти.
#### Копирование и перемещение
* _**std::uninitialized_copy**_ - Копирование элементов из диапазона в непроинициализированную память.
* _**std::uninitialized_move**_ и _**std::uninitialized_move_n**_ - Перемещение элементов в непроинициализированную память.
* _**std::uninitialized_copy_if**_ - Копирование элементов на основе условия.
#### Смешанные операции
* _**std::uninitialized_relocate**_ - Перемещение элементов из одной памяти в другую.
* _**std::uninitialized_relocate_if**_ - Перемещение элементов на основе условия.
* _**std::uninitialized_construct_at**_ - Конструирование объекта по указанному адресу.
