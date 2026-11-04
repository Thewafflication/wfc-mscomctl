// The behaviour of the controls of the library when a COM client creates them
// (see wfc/ocx/class_factory.hpp). The rules themselves are the library's
// models, which the WFC evaluator uses too. Windows only; internal to the WFC
// libraries.

#ifndef WFC_MSCOMCTL_COM_BEHAVIOUR_HPP
#define WFC_MSCOMCTL_COM_BEHAVIOUR_HPP

#include <wfc/mscomctl/range.hpp>
#include <wfc/ocx/class_factory.hpp>

#include <memory>
#include <string>
#include <vector>

namespace wfc::mscomctl {

// A ProgressBar (Single properties) or a Slider (Long properties) keeps Min,
// Max and Value in its range model: the range stays ordered, Value is kept
// inside it and a value outside it is error 380. A change of the Value of a
// Slider raises Change. The other properties are stored by the control as for
// any member.
class RangeBehaviour final : public wfc::com::Behaviour {
public:
    RangeBehaviour(const double minimum, const double maximum,
                   const bool slider)
        : model_(minimum, maximum, 0.0), slider_(slider) {}

    HRESULT put(const std::string& member, const Index&, const VARIANT& value,
                long* error, std::vector<std::string>* events) override {
        if (member != "min" && member != "max" && member != "value") {
            return S_FALSE;
        }
        VARIANT number;
        VariantInit(&number);
        if (VariantChangeType(&number, const_cast<VARIANT*>(&value), 0,
                              VT_R8) != S_OK) {
            *error = 13;  // type mismatch
            return E_FAIL;
        }
        // A Slider holds whole numbers.
        const double wanted =
            slider_ ? static_cast<double>(static_cast<long>(number.dblVal))
                    : number.dblVal;
        const auto outcome = member == "min"   ? model_.set_min(wanted)
                             : member == "max" ? model_.set_max(wanted)
                                               : model_.set_value(wanted);
        if (!outcome.valid) {
            *error = 380;  // invalid property value
            return E_FAIL;
        }
        if (slider_ && outcome.changed) {
            events->push_back("Change");
        }
        return S_OK;
    }

    HRESULT get(const std::string& member, const Index&, VARIANT* result,
                long*) override {
        double number = 0.0;
        if (member == "min") {
            number = model_.min();
        } else if (member == "max") {
            number = model_.max();
        } else if (member == "value") {
            number = model_.value();
        } else {
            return S_FALSE;
        }
        if (slider_) {
            result->vt = VT_I4;
            result->lVal = static_cast<long>(number);
        } else {
            result->vt = VT_R4;
            result->fltVal = static_cast<float>(number);
        }
        return S_OK;
    }

private:
    RangeModel model_;
    bool slider_;
};

// The behaviour of the class called `class_name`, none for most of them yet.
inline std::unique_ptr<wfc::com::Behaviour> make_behaviour(
    const char* class_name) {
    if (_stricmp(class_name, "ProgressBar") == 0) {
        return std::make_unique<RangeBehaviour>(0.0, 100.0, false);
    }
    if (_stricmp(class_name, "Slider") == 0) {
        return std::make_unique<RangeBehaviour>(0.0, 10.0, true);
    }
    return nullptr;
}

}  // namespace wfc::mscomctl

#endif  // WFC_MSCOMCTL_COM_BEHAVIOUR_HPP
