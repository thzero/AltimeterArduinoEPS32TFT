#ifndef _SWA_H
#define _SWA_H

class SimpleRingBuffer {
  public:
    SimpleRingBuffer() { flush(); }

    constexpr size_t count() const { return Count; }

    SimpleRingBuffer &push(Type value) {
      m_buffer[m_head] = value;
      m_head++;
      if (m_head == Count) {
        m_head = 0;
      }
      return *this;
    }

    Type back() const { return m_buffer[m_head]; }
    Type front() const { return at(0); }

    Type at(size_t offset) {
      const auto adjusted_offset
        = m_head > offset ? m_head - offset - 1 : Count + m_head - offset - 1;
      return m_buffer[adjusted_offset];
    }

    SimpleRingBuffer &flush() {
      for (auto &value : m_buffer) {
        value = {};
      }
      m_head = 0;
      return *this;
    }

  private:
    size_t m_head = 0;
    var::Array<Type, Count> m_buffer;
};

template <typename Type, size_t Count>
class SimpleMovingAverage {
  public:
    constexpr size_t count() const { return Count; }

    SimpleMovingAverage &calculate(Type input) {
      sum += input;
      sum -= m_buffer.back();
      m_buffer.push(input);
      return *this;
    }

    Type present_value() const { return sum / Count; }

  private:
    SimpleRingBuffer<Type, Count> m_buffer;
    Type sum = {};
};

#endif