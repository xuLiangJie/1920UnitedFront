#include "inventory/Inventory.h"
#include <algorithm>

namespace mud {

Inventory::Inventory() {
}

void Inventory::addItem(const Item& item) {
    items_.push_back(item);
}

bool Inventory::removeItem(const std::string& item_id) {
    auto it = std::find_if(items_.begin(), items_.end(),
        [&item_id](const Item& item) { return item.id == item_id; });

    if (it != items_.end()) {
        items_.erase(it);
        return true;
    }
    return false;
}

bool Inventory::hasItem(const std::string& item_id) const {
    return std::find_if(items_.begin(), items_.end(),
        [&item_id](const Item& item) { return item.id == item_id; }) != items_.end();
}

Item* Inventory::getItem(const std::string& item_id) {
    auto it = std::find_if(items_.begin(), items_.end(),
        [&item_id](const Item& item) { return item.id == item_id; });

    if (it != items_.end()) {
        return &(*it);
    }
    return nullptr;
}

const Item* Inventory::getItem(const std::string& item_id) const {
    auto it = std::find_if(items_.begin(), items_.end(),
        [&item_id](const Item& item) { return item.id == item_id; });

    if (it != items_.end()) {
        return &(*it);
    }
    return nullptr;
}

void Inventory::clear() {
    items_.clear();
}

int Inventory::getTotalValue() const {
    int total = 0;
    for (const auto& item : items_) {
        total += item.value;
    }
    return total;
}

void Inventory::sortByValue(bool ascending) {
    if (ascending) {
        std::sort(items_.begin(), items_.end(),
            [](const Item& a, const Item& b) { return a.value < b.value; });
    } else {
        std::sort(items_.begin(), items_.end(),
            [](const Item& a, const Item& b) { return a.value > b.value; });
    }
}

std::vector<const Item*> Inventory::findItemsByName(const std::string& name) const {
    std::vector<const Item*> results;
    for (const auto& item : items_) {
        if (item.name.find(name) != std::string::npos) {
            results.push_back(&item);
        }
    }
    return results;
}

} // namespace mud
