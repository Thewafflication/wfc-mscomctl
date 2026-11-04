// Checks the rules for finding items (wfc/mscomctl/items.hpp): keys without
// regard to case, indexes rounded to the nearest whole number, the errors and
// where Add inserts.

#include <wfc/mscomctl/items.hpp>

#include <cstdio>
#include <string>
#include <vector>

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
    using namespace wfc::mscomctl;

    const std::vector<std::string> keys{"One", "", "tWo", "Three"};
    const auto key_at = [&](const std::size_t i) -> const std::string& {
        return keys[i];
    };

    auto found = find(keys.size(), Selector::key("two"), key_at);
    expect(found.status == Status::ok && found.position == 2,
           "a key is found without regard to case");
    found = find(keys.size(), Selector::key(""), key_at);
    expect(found.status == Status::element_not_found,
           "an empty key finds nothing");
    found = find(keys.size(), Selector::key("four"), key_at);
    expect(found.status == Status::element_not_found, "an unknown key");
    found = find(keys.size(), Selector::index(1), key_at);
    expect(found.status == Status::ok && found.position == 0, "index 1");
    found = find(keys.size(), Selector::index(4), key_at);
    expect(found.status == Status::ok && found.position == 3, "the last index");
    found = find(keys.size(), Selector::index(2.5), key_at);
    expect(found.status == Status::ok && found.position == 1,
           "2.5 rounds to even, 2");
    found = find(keys.size(), Selector::index(3.5), key_at);
    expect(found.status == Status::ok && found.position == 3,
           "3.5 rounds to even, 4");
    found = find(keys.size(), Selector::index(0), key_at);
    expect(found.status == Status::index_out_of_bounds, "index 0");
    found = find(keys.size(), Selector::index(5), key_at);
    expect(found.status == Status::index_out_of_bounds, "index past the end");
    found = find(keys.size(), Selector::invalid(), key_at);
    expect(found.status == Status::type_mismatch, "neither number nor key");

    expect(key_in_use(keys.size(), "THREE", keys.size(), key_at),
           "a key in use");
    expect(!key_in_use(keys.size(), "THREE", 3, key_at),
           "the item itself does not count");
    expect(!key_in_use(keys.size(), "", keys.size(), key_at),
           "an empty key is never in use");
    expect(!key_in_use(keys.size(), "Four", keys.size(), key_at), "a new key");

    auto where = insert_position(4, nullptr);
    expect(where.status == Status::ok && where.position == 4,
           "Add without an index appends");
    double wanted = 1.0;
    where = insert_position(4, &wanted);
    expect(where.status == Status::ok && where.position == 0,
           "Add at index 1 inserts first");
    wanted = 5.0;
    where = insert_position(4, &wanted);
    expect(where.status == Status::ok && where.position == 4,
           "Add at one past the last index appends");
    wanted = 6.0;
    where = insert_position(4, &wanted);
    expect(where.status == Status::index_out_of_bounds, "Add too far");
    wanted = 0.0;
    where = insert_position(4, &wanted);
    expect(where.status == Status::index_out_of_bounds, "Add at index 0");

    expect(error_number(Status::element_not_found) == 35601 &&
               error_number(Status::index_out_of_bounds) == 35600 &&
               error_number(Status::key_not_unique) == 35602 &&
               error_number(Status::type_mismatch) == 13,
           "the error numbers");

    expect(check_panel_count(15) == Status::ok &&
               check_panel_count(16) == Status::panels_full,
           "a status bar holds sixteen panels");
    expect(check_image_insert(false, 0, 3) == Status::ok &&
               check_image_insert(true, 3, 3) == Status::ok &&
               check_image_insert(true, 1, 3) == Status::image_list_bound,
           "a bound ImageList only grows at the end");
    expect(selects_first_added(1) && !selects_first_added(2),
           "the first tab added is selected");
    expect(neighbor_after_removal(2, 4) == 1, "the previous tab is chosen");
    expect(neighbor_after_removal(0, 3) == 1, "the second when the first goes");
    expect(neighbor_after_removal(0, 1) == 1, "none when it was the only tab");
    expect(error_number(Status::panels_full) == 35609 &&
               error_number(Status::image_list_bound) == 35615,
           "the error numbers of the limits");

    if (failures == 0) {
        std::puts("items: ok");
    }
    return failures == 0 ? 0 : 1;
}
