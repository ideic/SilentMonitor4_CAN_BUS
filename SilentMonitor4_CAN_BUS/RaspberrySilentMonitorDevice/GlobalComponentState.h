#pragma once
#include <algorithm>
#include <vector>
#include <any>
#include <memory>
#include "IRunable.h"
class GlobalComponentState
{
	std::vector<std::shared_ptr<IRunable>> _components;
public:
	void AddComponent(std::shared_ptr<IRunable> comp) {
		_components.push_back(comp);
	}
	bool ComponentsAreStopped() {
		return std::all_of(begin(_components), end(_components), [](std::shared_ptr<IRunable> item) {
			return item->IsRunning();
		});
	}
};


