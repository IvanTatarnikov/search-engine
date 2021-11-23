#pragma once

#include <map>
#include <vector>
#include <mutex>

template<typename K, typename V>
class ConcurrentMap {
public:
	static_assert(std::is_integral_v<K>, "ConcurrentMap supports only integer keys");

	struct Access {
		std::lock_guard<std::mutex> g;
		V& ref_to_value;
	};

	explicit ConcurrentMap(size_t bucket_count)
		: concurrent_map(std::vector<Item>(bucket_count)) {}

	Access operator[](const K& key) {
		size_t idx = key % concurrent_map.size();
		Item& item = concurrent_map[idx];
		return {std::lock_guard<std::mutex>(item.m), item.map_[key]};
	}

	std::map<K, V> BuildOrdinaryMap() {
		std::map<K, V> result;
		for (auto& item : concurrent_map) {
			std::lock_guard<std::mutex> g(item.m);
			for (const auto& [key, value] : item.map_) {
				result[key] += value;
			}
		}
		return result;
	}

private:
	struct Item {
		std::map<K, V> map_;
		std::mutex m;
	};
	std::vector<Item> concurrent_map;
};