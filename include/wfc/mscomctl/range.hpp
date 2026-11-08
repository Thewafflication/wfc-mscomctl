// The range of a ProgressBar or a Slider, apart from any window: Min, Max and
// Value, with the rules both controls share. Header only, with no dependency
// on the WFC evaluator or on Windows; the evaluator, the COM objects of the
// library and the tests all use it. Internal to the WFC libraries.
//
// The behaviour is what MSCOMCTL.OCX does, measured under Visual Basic 6
// (evidence/reference/control-probe): Min must stay below Max, and a Value
// outside the range is refused by a ProgressBar (error 380) and brought inside
// it by a Slider.

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

    RangeModel(const double minimum, const double maximum, const double value,
               const bool clamps_value = false)
        : min_(minimum),
          max_(maximum),
          value_(value),
          clamps_value_(clamps_value) {}

    [[nodiscard]] double min() const { return min_; }
    [[nodiscard]] double max() const { return max_; }
    [[nodiscard]] double value() const { return value_; }

    // Min and Max: the range must stay ordered (Min below Max), and Value is
    // brought inside it.
    [[nodiscard]] Outcome set_min(const double minimum) {
        // A ProgressBar does not take a negative Min (a Slider does).
        if (minimum >= max_ || (!clamps_value_ && minimum < 0.0)) {
            return {false, false};
        }
        min_ = minimum;
        return clamp();
    }

    [[nodiscard]] Outcome set_max(const double maximum) {
        if (maximum <= min_) {
            return {false, false};
        }
        max_ = maximum;
        return clamp();
    }

    // Value outside Min and Max is an invalid property value, except that a
    // Slider brings it inside.
    [[nodiscard]] Outcome set_value(double wanted) {
        if (clamps_value_) {
            wanted = std::min(std::max(wanted, min_), max_);
        }
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
    bool clamps_value_;
};

}  // namespace wfc::mscomctl

#endif  // WFC_MSCOMCTL_RANGE_HPP
