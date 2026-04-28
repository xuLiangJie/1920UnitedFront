#ifndef SHOP_H
#define SHOP_H

#include "inventory/Inventory.h"
#include "player/Player.h"
#include <string>
#include <vector>
#include <map>

namespace mud {

// 交易结果枚举
enum class TradeResult {
    Success,
    InsufficientGold,
    ItemNotFound,
    InventoryFull,
    InvalidQuantity,
    CannotAfford,
    ItemNotSellable
};

// 商店物品信息
struct ShopItem {
    Item item;
    int buyPrice;      // 玩家购买价格
    int sellPrice;     // 玩家出售价格（通常为 buyPrice 的 50%）
    int stock;         // 库存数量（-1 表示无限）
    bool isSellable;   // 是否可出售给商店

    ShopItem() : buyPrice(0), sellPrice(0), stock(-1), isSellable(true) {}
    ShopItem(const Item& i, int bp, int sp, int s = -1, bool sell = true)
        : item(i), buyPrice(bp), sellPrice(sp), stock(s), isSellable(sell) {}
};

// 交易记录
struct TradeRecord {
    TradeResult result;
    std::string itemName;
    int quantity;
    int price;
    std::string message;

    TradeRecord() : result(TradeResult::Success), quantity(0), price(0) {}
};

// 商店类 - 管理物品买卖
class Shop {
public:
    Shop();
    explicit Shop(const std::string& name);

    // 商店管理
    void addItem(const ShopItem& item);
    bool removeItem(const std::string& item_id);
    ShopItem* getItem(const std::string& item_id);
    const ShopItem* getItem(const std::string& item_id) const;

    // 获取所有商品
    const std::map<std::string, ShopItem>& getItems() const { return items_; }

    // 交易操作
    TradeRecord buyItem(Player& player, const std::string& item_id, int quantity = 1);
    TradeRecord sellItem(Player& player, const std::string& item_id, int quantity = 1);

    // 获取商店信息
    std::string getName() const { return name_; }
    std::string getShopList() const;
    std::string getBuyMessage(const TradeRecord& record) const;
    std::string getSellMessage(const TradeRecord& record) const;

    // 价格修改器（可用于讨价还价系统）
    void setBuyPriceModifier(float modifier) { buyPriceModifier_ = modifier; }
    void setSellPriceModifier(float modifier) { sellPriceModifier_ = modifier; }
    float getBuyPriceModifier() const { return buyPriceModifier_; }
    float getSellPriceModifier() const { return sellPriceModifier_; }

    // 库存管理
    void restock(const std::string& item_id, int quantity);
    int getStock(const std::string& item_id) const;

private:
    std::string name_;
    std::map<std::string, ShopItem> items_;
    float buyPriceModifier_;   // 购买价格修改器（1.0 为原价）
    float sellPriceModifier_;  // 出售价格修改器（1.0 为原价）

    // 计算实际购买价格
    int calculateBuyPrice(const ShopItem& shopItem, int quantity) const;
    // 计算实际出售价格
    int calculateSellPrice(const ShopItem& shopItem, int quantity) const;
};

// 默认商店工厂
class DefaultShopFactory {
public:
    static Shop createGeneralStore();
    static Shop createWeaponShop();
    static Shop createPotionShop();
};

} // namespace mud

#endif // SHOP_H
