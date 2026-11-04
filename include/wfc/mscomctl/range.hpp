// The range of a ProgressBar or a Slider, apart from any window: Min, Max and
// Value, with the rules both controls share. Header only, with no dependency
// on the WFC evaluator or on Windows; the evaluator, the COM objects of the
// library and the tests all use it. Internal to the WFC libraries.
//
// The behaviour follows the documented controls (there is no reference probe
// of the Microsoft controls yet, see COMPATIBILITY.md).

#ifndef WFC_MSCOMCTL_RANGE_HPP
#define WFC_MSCOMCTL_RANGE_HPP

#include <algorithm>

namespace wfc::mscomctl {

class RangeModel final {
public:
    // The result of an operation that can change Value.
    struct Outcome {
        // False when the property value is invalid (error 380); nothing
        // changed.
        bool valid = true;
        // Value is not what it was.
        bool changed = false;
    };

    RangeModel(const double minimum, const double maximum, const double value)
        : min_(minimum), max_(maximum), value_(value) {}

    [[nodiscard]] double min() const { return min_; }
    [[nodiscard]] double max() const { return max_; }
    [[nodiscard]] double value() const { return value_; }

    // Min and Max: the range must stay ordered, and Value is brought inside
    // it.
    [[nodiscard]] Outcome set_min(const double minimum) {
        if (minimum > max_) {
            return {false, false};
        }
        min_ = minimum;
        return clamp();
    }

    [[nodiscard]] Outcome set_max(const double maximum) {
        if (maximum < min_) {
            return {false, false};
        }
        max_ = maximum;
        return clamp();
    }

    // Value outside Min and Max is an invalid property value.
    [[nodiscard]] Outcome set_value(const double wanted) {
        if (wanted < min_ || wanted > max_) {
            return {false, false};
        }
        const bool changed = wanted != value_;
        value_ = wanted;
        return {true, changed};
    }

    // The user moves the thumb to `target`: it is kept inside the range.
    [[nodiscard]] Outcome move_to(const double target) {
        const double wanted = std::min(std::max(target, min_), max_);
        const bool changed = wanted != value_;
        value_ = wanted;
        return {true, changed};
    }

private:
    [[nodiscard]] Outcome clamp() {
        const double before = value_;
        value_ = std::min(std::max(value_, min_), max_);
        return {true, value_ != before};
    }

    double min_;
    double max_;
    double value_;
};

}  // namespace wfc::mscomctl

#endif  // WFC_MSCOMCTL_RANGE_HPP
