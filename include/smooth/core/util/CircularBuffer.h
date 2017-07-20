//
// Created by permal on 7/5/17.
//

#pragma once

namespace smooth
{
    namespace core
    {
        namespace util
        {
            template<typename T>
            class ICircularBuffer
            {
                public:
                    virtual ~ICircularBuffer()
                    {
                    }

                    virtual void put(const T& data) = 0;
                    virtual bool get(T& t) = 0;
                    virtual bool is_empty() = 0;
                    virtual bool is_full() = 0;
                    virtual int available_items() = 0;
                    virtual int available_slots() = 0;
                    virtual void clear() = 0;

            };

            template<typename T, int Size>
            class CircularBuffer
                    : public ICircularBuffer<T>
            {
                public:
                    CircularBuffer();

                    virtual ~CircularBuffer()
                    {
                    }

                    void put(const T& data) override;
                    bool get(T& data) override;

                    bool is_empty() override
                    {
                        return count == 0;
                    }

                    bool is_full() override
                    {
                        return count == Size;
                    }

                    int available_items() override
                    {
                        return count;
                    }

                    int available_slots() override
                    {
                        return Size - count;
                    }

                    void clear() override
                    {
                        read_pos = 0;
                        write_pos = 0;
                        count = 0;
                    }

                    CircularBuffer(const CircularBuffer&) = delete;
                    CircularBuffer& operator=(const CircularBuffer&) = delete;

                private:
                    int next_pos(int current)
                    {
                        return (current + 1) % Size;
                    }

                    T data[Size];
                    int read_pos;
                    int write_pos;
                    int count;
            };


            template<typename T, int Size>
            CircularBuffer<T, Size>::CircularBuffer()
                    :    data(),
                         read_pos(0),
                         write_pos(0),
                         count(0)
            {
            }

            template<typename T, int Size>
            void CircularBuffer<T, Size>::put(const T& data)
            {
                this->data[write_pos] = data;

                if (!is_full())
                {
                    ++count;
                }

                // Overwrite data not yet read
                if (is_full() && read_pos == write_pos)
                {
                    read_pos = next_pos(read_pos);
                }

                write_pos = next_pos(write_pos);
            }

            template<typename T, int Size>
            bool CircularBuffer<T, Size>::get(T& d)
            {
                bool res = false;

                if (!is_empty())
                {
                    d = data[read_pos];
                    read_pos = next_pos(read_pos);
                    --count;

                    res = true;
                }

                return res;
            }

        }
    }
}