#include "shop/Shop.h"
#include <sstream>
#include <algorithm>

namespace mud {

Shop::Shop()
    : name_("Unnamed Shop")
    , buyPriceModifier_(1.0f)
    , sellPriceModifier_(1.0f) {
}

Shop::Shop(const std::string& name)
    : name_(name)
    , buyPriceModifier_(1.0f)
    , sellPriceModifier_(1.0f) {
}

void Shop::addItem(const ShopItem& item) {
    items_[item.item.id] = item;
}

bool Shop::removeItem(const std::string& item_id) {
    return items_.erase(item_id) > 0;
}

ShopItem* Shop::getItem(const std::string& item_id) {
    auto it = items_.find(item_id);
    return it != items_.end() ? &it->second : nullptr;
}

const ShopItem* Shop::getItem(const std::string& item_id) const {
    auto it = items_.find(item_id);
    return it != items_.end() ? &it->second : nullptr;
}

int Shop::calculateBuyPrice(const ShopItem& shopItem, int quantity) const {
    int basePrice = shopItem.buyPrice * quantity;
    return static_cast<int>(basePrice * buyPriceModifier_);
}

int Shop::calculateSellPrice(const ShopItem& shopItem, int quantity) const {
    int basePrice = shopItem.sellPrice * quantity;
    return static_cast<int>(basePrice * sellPriceModifier_);
}

TradeRecord Shop::buyItem(Player& player, const std::string& item_id, int quantity) {
    TradeRecord record;

    // 验证数量
    if (quantity <= 0) {
        record.result = TradeResult::InvalidQuantity;
        record.message = "Invalid quantity.\r\n";
        return record;
    }

    // 查找商品
    auto it = items_.find(item_id);
    if (it == items_.end()) {
        record.result = TradeResult::ItemNotFound;
        record.message = "Item not found in shop.\r\n";
        return record;
    }

    const ShopItem& shopItem = it->second;
    record.itemName = shopItem.item.name;
    record.quantity = quantity;

    // 检查库存
    if (shopItem.stock != -1 && shopItem.stock < quantity) {
        record.result = TradeResult::InventoryFull;
        record.message = "Not enough stock available.\r\n";
        return record;
    }

    // 计算价格
    int totalPrice = calculateBuyPrice(shopItem, quantity);
    record.price = totalPrice;

    // 检查金币
    if (player.getGold() < totalPrice) {
        record.result = TradeResult::InsufficientGold;
        record.message = "You don't have enough gold. Need " + 
                         std::to_string(totalPrice) + "g, have " + 
                         std::to_string(player.getGold()) + "g.\r\n";
        return record;
    }

    // 执行交易
    player.setGold(player.getGold() - totalPrice);
    
    // 添加物品到玩家背包
    for (int i = 0; i < quantity; ++i) {
        player.addItem(shopItem.item);
    }

    // 减少库存
    if (shopItem.stock != -1) {
        items_[item_id].stock -= quantity;
    }

    record.result = TradeResult::Success;
    record.message = "You bought " + std::to_string(quantity) + " x " + 
                     shopItem.item.name + " for " + 
                     std::to_string(totalPrice) + " gold.\r\n";

    return record;
}

TradeRecord Shop::sellItem(Player& player, const std::string& item_id, int quantity) {
    TradeRecord record;

    // 验证数量
    if (quantity <= 0) {
        record.result = TradeResult::InvalidQuantity;
        record.message = "Invalid quantity.\r\n";
        return record;
    }

    // 检查玩家是否有足够物品
    int playerItemCount = 0;
    const Item* playerItem = nullptr;
    
    for (const auto& item : player.getInventory()) {
        if (item.id == item_id) {
            playerItemCount++;
            playerItem = &item;
        }
    }

    if (playerItemCount < quantity) {
        record.result = TradeResult::ItemNotFound;
        record.message = "You don't have enough " + item_id + " to sell.\r\n";
        return record;
    }

    // 查找商店是否接受此物品
    auto it = items_.find(item_id);
    int sellPrice = 0;
    bool isSellable = false;

    if (it != items_.end()) {
        isSellable = it->second.isSellable;
        const ShopItem& shopItem = it->second;
        sellPrice = calculateSellPrice(shopItem, quantity);
        record.itemName = shopItem.item.name;
    } else if (playerItem != nullptr) {
        // 商店没有此商品列表，按基础价值的 50% 回收
        isSellable = true;
        sellPrice = static_cast<int>(playerItem->value * 0.5 * quantity);
        record.itemName = playerItem->name;
    }

    if (!isSellable) {
        record.result = TradeResult::ItemNotSellable;
        record.message = "The shop doesn't accept this item.\r\n";
        return record;
    }

    record.quantity = quantity;
    record.price = sellPrice;

    // 执行交易
    player.setGold(player.getGold() + sellPrice);

    // 从玩家背包移除物品
    int removed = 0;
    auto& inventory = player.getInventory();
    for (auto iter = inventory.begin(); iter != inventory.end() && removed < quantity;) {
        if (iter->id == item_id) {
            iter = inventory.erase(iter);
            removed++;
        } else {
            ++iter;
        }
    }

    record.result = TradeResult::Success;
    record.message = "You sold " + std::to_string(quantity) + " x " + 
                     record.itemName + " for " + 
                     std::to_string(sellPrice) + " gold.\r\n";

    return record;
}

std::string Shop::getShopList() const {
    std::ostringstream oss;
    oss << "=== " << name_ << " ===\r\n";
    oss << "Items for sale:\r\n";
    oss << "--------------------------------\r\n";

    for (const auto& pair : items_) {
        const ShopItem& item = pair.second;
        oss << "- " << item.item.name << " (" << item.item.id << ")\r\n";
        oss << "  " << item.item.description << "\r\n";
        oss << "  Buy: " << item.buyPrice << "g";
        if (buyPriceModifier_ != 1.0f) {
            oss << " -> " << calculateBuyPrice(item, 1) << "g";
        }
        oss << " | Sell: " << item.sellPrice << "g";
        if (item.stock != -1) {
            oss << " [Stock: " << item.stock << "]";
        }
        oss << "\r\n";
    }

    return oss.str();
}

std::string Shop::getBuyMessage(const TradeRecord& record) const {
    return record.message;
}

std::string Shop::getSellMessage(const TradeRecord& record) const {
    return record.message;
}

void Shop::restock(const std::string& item_id, int quantity) {
    auto it = items_.find(item_id);
    if (it != items_.end()) {
        if (it->second.stock != -1) {
            it->second.stock += quantity;
        }
    }
}

int Shop::getStock(const std::string& item_id) const {
    auto it = items_.find(item_id);
    if (it != items_.end()) {
        return it->second.stock;
    }
    return 0;
}

// 默认商店工厂实现
Shop DefaultShopFactory::createGeneralStore() {
    Shop shop("General Store");

    // 基础装备
    shop.addItem(ShopItem(
        Item("sword_iron", "Iron Sword", "A basic iron sword", 50),
        50, 25, 10, true
    ));

    shop.addItem(ShopItem(
        Item("shield_wood", "Wooden Shield", "A simple wooden shield", 30),
        30, 15, 10, true
    ));

    shop.addItem(ShopItem(
        Item("potion_hp_small", "Small Health Potion", "Restores 50 HP", 25),
        25, 12, 20, true
    ));

    shop.addItem(ShopItem(
        Item("potion_mp_small", "Small Mana Potion", "Restores 25 MP", 25),
        25, 12, 20, true
    ));

    shop.addItem(ShopItem(
        Item("torch", "Torch", "Provides light in dark areas", 5),
        5, 2, 50, true
    ));

    return shop;
}

Shop DefaultShopFactory::createWeaponShop() {
    Shop shop("Weapon Shop");

    shop.addItem(ShopItem(
        Item("sword_steel", "Steel Sword", "A sharp steel blade", 150),
        150, 75, 5, true
    ));

    shop.addItem(ShopItem(
        Item("axe_battle", "Battle Axe", "A heavy battle axe", 200),
        200, 100, 3, true
    ));

    shop.addItem(ShopItem(
        Item("dagger", "Dagger", "A quick stabbing weapon", 80),
        80, 40, 8, true
    ));

    shop.addItem(ShopItem(
        Item("bow", "Shortbow", "A hunter's bow", 100),
        100, 50, 5, true
    ));

    shop.addItem(ShopItem(
        Item("arrow", "Arrow", "A sharp arrow", 5),
        5, 2, 100, true
    ));

    return shop;
}

Shop DefaultShopFactory::createPotionShop() {
    Shop shop("Potion Shop");

    shop.addItem(ShopItem(
        Item("potion_hp_small", "Small Health Potion", "Restores 50 HP", 25),
        25, 12, 30, true
    ));

    shop.addItem(ShopItem(
        Item("potion_hp_large", "Large Health Potion", "Restores 150 HP", 60),
        60, 30, 15, true
    ));

    shop.addItem(ShopItem(
        Item("potion_mp_small", "Small Mana Potion", "Restores 25 MP", 25),
        25, 12, 30, true
    ));

    shop.addItem(ShopItem(
        Item("potion_mp_large", "Large Mana Potion", "Restores 75 MP", 60),
        60, 30, 15, true
    ));

    shop.addItem(ShopItem(
        Item("elixir", "Elixir", "Fully restores HP and MP", 200),
        200, 100, 5, true
    ));

    shop.addItem(ShopItem(
        Item("antidote", "Antidote", "Cures poison", 15),
        15, 7, 25, true
    ));

    return shop;
}

} // namespace mud
