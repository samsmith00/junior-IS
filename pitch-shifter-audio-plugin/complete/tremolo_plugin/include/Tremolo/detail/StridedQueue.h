#pragma once

namespace tremolo::detail {
template <typename T, size_t Size>
class StridedQueue {
public:
  void setStride(size_t newStride) {
    constexpr auto minimumStride = 1uz;

    stride = juce::jmax(minimumStride, newStride);
  }

  [[nodiscard]] size_t size() const noexcept { return stridedElements.size(); }

  T& front() noexcept { return stridedElements.front(); }

  T& at(size_t index) { return stridedElements.at(index); }

  void pushBack(std::span<const T> buffer) {
    const auto availableStridedCount = newElementsCount(buffer.size());

    if (availableStridedCount < stridedElements.size()) {
      // rotate when not overwritten completely
      std::rotate(stridedElements.begin(),
                  stridedElements.begin() + availableStridedCount,
                  stridedElements.end());
    }
    const auto bufferEndIndex =
        elementIndex + (availableStridedCount - 1u) * stride;

    const auto addedCount =
        std::min(availableStridedCount, stridedElements.size());

    for (const auto i : std::views::iota(0u, addedCount)) {
      jassert(0 <= (bufferEndIndex - i * stride));
      jassert((bufferEndIndex - i * stride) < buffer.size());
      jassert(stridedElements.rbegin() + i < stridedElements.rend());

      *(stridedElements.rbegin() + i) = buffer[bufferEndIndex - i * stride];
    }

    elementIndex = (elementIndex + (buffer.size() / stride + 1u) * stride -
                    buffer.size()) %
                   stride;
  }

  void pushBackZeros(size_t zerosCount) {
    // reset elementIndex; the order changed
    elementIndex = 0u;

    const auto availableStridedCount = newElementsCount(zerosCount);
    if (availableStridedCount < stridedElements.size()) {
      std::rotate(stridedElements.begin(),
                  stridedElements.begin() + availableStridedCount,
                  stridedElements.end());
    }
    const auto beginIndex =
        std::max(0, static_cast<int>(stridedElements.size()) -
                        static_cast<int>(availableStridedCount));
    std::fill(stridedElements.begin() + beginIndex, stridedElements.end(),
              T(0));
  }

private:
  [[nodiscard]] size_t newElementsCount(size_t sampleCount) const noexcept {
    const auto lowerBound = sampleCount / stride;

    if (lowerBound * stride + elementIndex < sampleCount) {
      return lowerBound + 1u;
    }

    return lowerBound;
  }

  std::array<T, Size> stridedElements{};
  size_t elementIndex{0u};
  size_t stride{1u};
};
}  // namespace tremolo::detail
