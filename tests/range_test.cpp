// Checks the behaviour of the range model (wfc/mscomctl/range.hpp): the rules
// for Min, Max and Value of a ProgressBar and a Slider, and the position the
// user moves to.

#include <wfc/mscomctl/range.hpp>

#include <cstdio>

namespace {

int failures = 0;

void expect(const bool condition, const char* what) {
    if (!condition) {
        std::fprintf(stderr, "FAIL: %s\n", what);
        ++failures;
    }
}

}  // namespace

int main() {
    using wfc::mscomctl::RangeModel;

    RangeModel model{0.0, 100.0, 0.0};
    auto outcome = model.set_value(40.0);
    expect(outcome.valid && outcome.changed && model.value() == 40.0,
           "Value inside the range");
    outcome = model.set_value(40.0);
    expect(outcome.valid && !outcome.changed, "the same Value is no change");
    outcome = model.set_value(100.5);
    expect(!outcome.valid && model.value() == 40.0, "Value above Max is 380");
    outcome = model.set_value(-0.5);
    expect(!outcome.valid && model.value() == 40.0, "Value below Min is 380");

    outcome = model.set_max(30.0);
    expect(outcome.valid && outcome.changed && model.value() == 30.0,
           "a narrower range brings Value inside");
    outcome = model.set_min(50.0);
    expect(!outcome.valid && model.min() == 0.0, "Min above Max is 380");
    outcome = model.set_max(-1.0);
    expect(!outcome.valid && model.max() == 30.0, "Max below Min is 380");
    outcome = model.set_min(30.0);
    expect(outcome.valid && !outcome.changed && model.min() == 30.0,
           "Min equal to Max is allowed");
    outcome = model.set_min(10.0);
    expect(outcome.valid && !outcome.changed && model.value() == 30.0,
           "a wider range leaves Value");

    outcome = model.move_to(500.0);
    expect(outcome.valid && !outcome.changed && model.value() == 30.0,
           "a move stops at Max");
    outcome = model.move_to(0.0);
    expect(outcome.changed && model.value() == 10.0, "a move stops at Min");

    if (failures == 0) {
        std::puts("range model: ok");
    }
    return failures == 0 ? 0 : 1;
}
